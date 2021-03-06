/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "utilities.h"

#include <QTimer>
#include <QClipboard>
#include <QApplication>

#if defined Q_OS_WIN           /* Microsoft Windows Hacks */
#include <pdh.h>
#include <tchar.h>
#include <windows.h>
static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
static SYSTEM_POWER_STATUS power;

#elif defined Q_OS_MAC         /* Mac OSX Hacks */
static const QString CPU_CMD = "bash -c \"ps -A -o %cpu | "
                               "awk '{s+=$1} END {print s}'\"";
static const QString BTY_CMD = "pmset -g batt";
static const QString PWR_CMD = "pmset -g batt";

#elif defined Q_OS_LINUX       /* Linux Hacks */
static const QString CPU_CMD = "bash -c \"grep 'cpu ' /proc/stat | "
                               "awk '{usage=($2+$4)*100/($2+$4+$5)} "
                               "END {print usage}'\"";
static const QString BTY_CMD = "bash -c \"upower -i "
                               "$(upower -e | grep 'BAT') | "
                               "grep -E 'state|to\\ full|percentage'\"";
static const QString PWR_CMD = "bash -c \"upower -i "
                               "$(upower -e | grep 'BAT') | "
                               "grep -E 'state|to\\ full|percentage'\"";

#else                          /* Aww sucks */
#warning Your OS will not be able to get CPU and Battery information
static const QString CPU_CMD = "";
static const QString BTY_CMD = "";
static const QString PWR_CMD = "";
#endif

/**
 * Configures the class and nitializes the CPU querying process under Windows.
 */
Utilities::Utilities() {
    m_cpuUsage = 0;
    m_batteryLevel = 0;
    m_connectedToAC = 0;

    /* Read process data when they finish */
    connect (&m_cpuProcess,           SIGNAL (finished                 (int)),
             this,                      SLOT (readCpuUsageProcess      (int)));
    connect (&m_batteryLevelProcess,  SIGNAL (finished                 (int)),
             this,                      SLOT (readBatteryLevelProcess  (int)));
    connect (&m_connectedToACProcess, SIGNAL (finished                 (int)),
             this,                      SLOT (readConnectedToACProcess (int)));

    /* Configure Windows */
#ifdef Q_OS_WIN
    PdhOpenQuery (0, 0, &cpuQuery);
    PdhAddCounter (cpuQuery,
                   L"\\Processor(_Total)\\% Processor Time",
                   0,
                   &cpuTotal);
    PdhCollectQueryData (cpuQuery);
#endif

    /* Start loop */
    updateCpuUsage();
    updateBatteryLevel();
    updateConnectedToAC();
}

/**
 * Returns the current CPU usage (from 0 to 100)
 */
int Utilities::cpuUsage() {
    m_cpuUsage = abs (m_cpuUsage);

    if (m_cpuUsage <= 100)
        return m_cpuUsage;

    return 0;
}

/**
 * Returns the current battery level (from 0 to 100)
 */
int Utilities::batteryLevel() {
    m_batteryLevel = abs (m_batteryLevel);

    if (m_batteryLevel <= 100)
        return m_batteryLevel;

    return 0;
}

/**
 * Returns \c true if the computer is connected to a power source or the
 * battery is not discharging.
 */
bool Utilities::isConnectedToAC() {
    return m_connectedToAC;
}

/**
 * Copies the given \a data to the system clipboard
 */
void Utilities::copy (const QVariant& data) {
    qApp->clipboard()->setText (data.toString(), QClipboard::Clipboard);
}

/**
 * Queries for the current CPU usage
 */
void Utilities::updateCpuUsage() {
#ifdef Q_OS_WIN
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData (cpuQuery);
    PdhGetFormattedCounterValue (cpuTotal, PDH_FMT_DOUBLE, 0, &counterVal);
    m_cpuUsage = static_cast<int>(counterVal.doubleValue);
#else
    m_cpuProcess.terminate();
    m_cpuProcess.start (CPU_CMD, QIODevice::ReadOnly);
#endif

    QTimer::singleShot (1000,
                        Qt::PreciseTimer,
                        this, SLOT (updateCpuUsage()));
}

/**
 * Queries for the current battery level
 */
void Utilities::updateBatteryLevel() {
#ifdef Q_OS_WIN
    GetSystemPowerStatus (&power);
    m_batteryLevel = static_cast<int> (power.BatteryLifePercent);
#else
    m_batteryLevelProcess.terminate();
    m_batteryLevelProcess.start (BTY_CMD, QIODevice::ReadOnly);
#endif

    QTimer::singleShot (1000,
                        Qt::PreciseTimer,
                        this, SLOT (updateBatteryLevel()));
}

/**
 * Queries for the current AC power source status
 */
void Utilities::updateConnectedToAC() {
#ifdef Q_OS_WIN
    GetSystemPowerStatus (&power);
    m_connectedToAC = (power.ACLineStatus != 0);
#else
    m_connectedToACProcess.terminate();
    m_connectedToACProcess.start (PWR_CMD, QIODevice::ReadOnly);
#endif

    QTimer::singleShot (1000,
                        Qt::PreciseTimer,
                        this, SLOT (updateConnectedToAC()));
}

/**
 * Reads the output of the process launched to get the CPU usage
 */
void Utilities::readCpuUsageProcess (int exit_code) {
    if (exit_code == EXIT_FAILURE)
        return;

#if defined Q_OS_MAC || defined Q_OS_LINUX
    m_cpuUsage = 0;
    m_cpuProcess.terminate();
    QByteArray data = m_cpuProcess.readAll();

    if (!data.isEmpty() && data.length() >= 2) {
        /* Parse the digits of the percentage */
        int t = data.at (0) - '0'; // Tens
        int u = data.at (1) - '0'; // Units

        /* Check if process data is invalid */
        if (t < 0) t = 0;
        if (u < 0) u = 0;

        /* Update information */
        m_cpuUsage = (t * 10) + u;
    }
#endif
}

/**
 * Reads the output of the process launched to get the battery level
 */
void Utilities::readBatteryLevelProcess (int exit_code) {
    if (exit_code == EXIT_FAILURE)
        return;

#if defined Q_OS_MAC || defined Q_OS_LINUX
    m_batteryLevel = 0;
    m_batteryLevelProcess.terminate();
    QByteArray data = m_batteryLevelProcess.readAll();

    if (!data.isEmpty()) {
        /* Parse the digits of the percentage */
        int h = data.at (data.indexOf ("%") - 3) - '0'; // Hundreds
        int t = data.at (data.indexOf ("%") - 2) - '0'; // Tens
        int u = data.at (data.indexOf ("%") - 1) - '0'; // Units

        /* Check if rocess data is invalid */
        if (h < 0) h = 0;
        if (t < 0) t = 0;
        if (u < 0) u = 0;

        /* Update information */
        m_batteryLevel = (h * 100) + (t * 10) + u;
    }
#endif
}

/**
 * Reads the output of the process launched to get the AC power source status
 */
void Utilities::readConnectedToACProcess (int exit_code) {
    if (exit_code == EXIT_FAILURE)
        return;

#if defined Q_OS_MAC || defined Q_OS_LINUX
    m_connectedToAC = false;
    m_connectedToACProcess.terminate();
    QByteArray data = m_connectedToACProcess.readAll();

    if (!data.isEmpty())
        m_connectedToAC = !data.contains ("discharging");
#endif
}

