/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_COMMON_H
#define _LIB_DS_COMMON_H

#include <QTimer>
#include <QtMath>
#include <QDebug>
#include <QObject>
#include <QHostInfo>
#include <QDateTime>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QMetaObject>
#include <QStringList>
#include <QHostAddress>

/* Hacks */
#define DS_Joysticks QList<DS::Joystick*>
#define DS_Schedule(time,object,slot) QTimer::singleShot (time, \
    Qt::PreciseTimer, \
    object, slot)

/**
 * This allows us see the name of an enum when logging it (instead of displaying
 * its numerical output). This can be very helpful for anyone that will end up
 * reading the application logs.
 */
// *INDENT-OFF*
#define SMART_ENUM(ENUM) \
    friend Q_DECL_CONSTEXPR const QMetaObject *qt_getEnumMetaObject(ENUM) \
    Q_DECL_NOEXCEPT { return &staticMetaObject; } \
    friend Q_DECL_CONSTEXPR const char *qt_getEnumName(ENUM) \
    Q_DECL_NOEXCEPT { return #ENUM; }
// *INDENT-ON*

/**
 * Ignore sockets with port set to -1
 */
const int DS_DISABLED_PORT = -1;

/**
 * Global socket listener address
 */
const QHostAddress DS_LISTENER = QHostAddress ("0.0.0.0");

/**
 * Global socket options
 */
const QAbstractSocket::BindMode DS_BIND_FLAGS = QAbstractSocket::ShareAddress |
                                                QAbstractSocket::ReuseAddressHint;

/**
 * Returns the location in where application files (e.g. logs) are stored
 */
extern QString DS_FILES_PATH();

/**
 * Returns the location in where the log files are stored
 */
extern QString DS_LOGGER_PATH();

/**
 * Returns the localtion in which robot/protocol log files are stored
 */
extern QString DS_ROBOT_LOGGER_PATH();

/**
 * This function implements our own logger engine. Feel free to enable
 * it if you want!
 *
 * To enable it, write this in your \c main() function:
 * \c qInstallMessageHandler (DS_MESSAGE_HANDLER)
 */
extern void DS_MESSAGE_HANDLER (QtMsgType type,
                                const QMessageLogContext& context,
                                const QString& data);

/**
 * Holds the common data types shared between the \c DriverStation
 * and the \c Protocol classes of this library. This data must be registered
 * inside a QObject class in order to allow full support for QML applications.
 */
class DS : public QObject {
    Q_OBJECT
    Q_ENUMS (Alliance)
    Q_ENUMS (Position)
    Q_ENUMS (SocketType)
    Q_ENUMS (CodeStatus)
    Q_ENUMS (CommStatus)
    Q_ENUMS (ControlMode)
    Q_ENUMS (EnableStatus)
    Q_ENUMS (OperationStatus)

  public:
    /**
     * @brief The ControlMode enum
     * Represents the available control modes of the robot
     */
    enum ControlMode {
        kControlTest,         /**< Dashboard controls individual actuators */
        kControlAutonomous,   /**< Robot uses its programming to move */
        kControlTeleoperated, /**< Driver moves robot */
    };
    SMART_ENUM (ControlMode)

    /**
     * @brief The Alliance enum
     * Represents the team alliance.
     * The robot program may use this information to change its behavior
     * (e.g adjust its sensors, change dashboard values, etc.)
     */
    enum Alliance {
        kAllianceRed,  /**< Set robot to red alliance */
        kAllianceBlue, /**< Set robot to blue alliance */
    };
    SMART_ENUM (Alliance)

    /**
     * @brief The Position enum
     * Represents the different team positions for each alliance.
     * Again, the robot program may use this data to change its behavior.
     */
    enum Position {
        kPosition1, /**< Position 1, regardless of alliance */
        kPosition2, /**< Position 2, regardless of alliance */
        kPosition3, /**< Position 3, regardless of alliance */
    };
    SMART_ENUM (Position)

    /**
     * @brief The CommStatus enum
     * Represents the communication status between the client and the robot or
     * the client and the radio.
     */
    enum CommStatus {
        kCommsWorking, /**< Communications with target have been established */
        kCommsFailing, /**< We don't have a working comms. link with target */
    };
    SMART_ENUM (CommStatus)

    /**
     * @brief The CodeStatus enum
     * Represents the status of the robot code. The DS is only interested if
     * the code is running or not (in order to be able to operate the robot).
     */
    enum CodeStatus {
        kCodeRunning, /**< Robot code is operational */
        kCodeFailing, /**< For some reason, robot does not have code */
    };
    SMART_ENUM (CodeStatus)

    /**
     * @brief The EnableStatus enum
     * Represents the enabled state of the robot.
     */
    enum EnableStatus {
        kEnabled,  /**< Robot is enabled */
        kDisabled, /**< Robot is disabled */
    };
    SMART_ENUM (EnableStatus)

    /**
     * @brief the OperationStatus enum
     * Represents the operation status of the robot.
     */
    enum OperationStatus {
        kNormal,        /**< Robot program operates */
        kEmergencyStop, /**< Robot program is stopped */
    };
    SMART_ENUM (OperationStatus)

    /**
     * @brief the VoltageStatus enum
     * Represents the status of the robot power supply.
     * When the battery level is low or there are too many power spikes,
     * the robot may report a voltage brownout.
     */
    enum VoltageStatus {
        kVoltageNormal,   /**< Robot systems have enough power to operate */
        kVoltageBrownout, /**< Robot systems may fail due to lack of power */
    };
    SMART_ENUM (VoltageStatus)

    /**
     * Represents the socket types that can be used by the DS modules
     */
    enum SocketType {
        kSocketTypeUDP, /**< Represents a UDP socket */
        kSocketTypeTCP, /**< Represents a TCP socket */
    };
    SMART_ENUM (SocketType)

    /**
     * Represents a joystick and its respective properties
     */
    struct Joystick {
        int numAxes = 0;
        int numPOVs = 0;
        int numButtons = 0;
        int realNumAxes = 0;
        int realNumPOVs = 0;
        int realNumButtons = 0;

        int* povs;
        float* axes;
        bool* buttons;
    };

    /**
     * Returns a calculated IP address based on the team address.
     *
     * For example:
     *    - \c getStaticIp ( 10, 3794, 1) would return \c 10.37.94.1
     *    - \c getStaticIp (177, 3794, 2) would return \c 177.37.94.2
     *    - \c getStaticIp ( 10,  118, 3) would return \c 10.01.18.3
     *    - And so on...
     */
    static inline QString getStaticIP (int net, int team, int host) {
        QString string = QString ("%1").arg (team);
        switch (string.length()) {
        case 1:
            string = "00.0" + string;
            break;
        case 2:
            string = "00." + string;
            break;
        case 3:
            string = QString ("0%1.%2%3").arg (string.at (0),
                                               string.at (1),
                                               string.at (2));
            break;
        case 4:
            string = QString ("%1%2.%3%4").arg (string.at (0),
                                                string.at (1),
                                                string.at (2),
                                                string.at (3));
            break;
        }
        return QString ("%1.%2.%3").arg (QString::number (net),
                                         string,
                                         QString::number (host));
    }

    /**
     * Returns the current timezone as a string
     */
    static inline QString timezone() {
        switch (QDateTime::currentDateTime().offsetFromUtc() / 3600) {
        case -11:
            return "BST11BDT";
            break;
        case -10:
            return "HST10HDT";
            break;
        case -9:
            return "AST9ADT";
            break;
        case -8:
            return "PST8PDT";
            break;
        case -7:
            return "MST7MDT";
            break;
        case -6:
            return "CST6CDT";
            break;
        case -5:
            return "EST5EDT";
            break;
        case -4:
            return "AST4ADT";
            break;
        case -3:
            return "GRNLNDST3GRNLNDDT";
            break;
        case -2:
            return "FALKST2FALKDT";
            break;
        case -1:
            return "AZOREST1AZOREDT";
            break;
        case 0:
            return "GMT0BST";
            break;
        case 1:
            return "NFT-1DFT";
            break;
        case 2:
            return "WET-2WET";
            break;
        case 3:
            return "MEST-3MEDT";
            break;
        case 4:
            return "WST-4WDT";
            break;
        case 5:
            return "PAKST-5PAKDT";
            break;
        case 6:
            return "TASHST-6TASHDT";
            break;
        case 7:
            return "THAIST-7THAIDT";
            break;
        case 8:
            return "WAUST-8WAUDT";
            break;
        case 9:
            return "JST-9JSTDT";
            break;
        case 10:
            return "EET-10EETDT";
            break;
        case 11:
            return "MET-11METDT";
            break;
        case 12:
            return "NZST-12NZDT";
            break;
        }

        return "GMT0BST";
    }

    /**
     * Reads the datagrams received by the given UDP \a socket
     */
    static inline QByteArray readSocket (QUdpSocket* socket) {
        QByteArray data;

        if (socket) {
            while (socket->hasPendingDatagrams()) {
                data.resize (socket->pendingDatagramSize());
                socket->readDatagram (data.data(), data.size());
            }
        }

        return data;
    }

    /**
     * Reads the data received by the given TCP \a socket
     */
    static inline QByteArray readSocket (QTcpSocket* socket) {
        if (socket)
            return socket->readAll();

        return QByteArray ("");
    }
};

Q_DECLARE_METATYPE (DS::Alliance)
Q_DECLARE_METATYPE (DS::Position)
Q_DECLARE_METATYPE (DS::SocketType)
Q_DECLARE_METATYPE (DS::CodeStatus)
Q_DECLARE_METATYPE (DS::CommStatus)
Q_DECLARE_METATYPE (DS::ControlMode)
Q_DECLARE_METATYPE (DS::EnableStatus)
Q_DECLARE_METATYPE (DS::OperationStatus)

#endif
