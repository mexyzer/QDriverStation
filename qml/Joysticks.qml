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

import QtQuick 2.0
import QtQuick.Layouts 1.0

import "widgets"
import "globals.js" as Globals

Item {
    id: joysticks

    //
    // Represents the selected joystick
    //
    property int currentJoystick: 0

    //
    // Regenerate the button, axis and POV indicators for the selected joystick
    //
    function regenerateControls() {
        axes.model = 0
        povs.model = 0
        buttons.model = 0

        if (DriverStation.joystickCount() > currentJoystick) {
            axes.model = DriverStation.getNumAxes (currentJoystick)
            povs.model = DriverStation.getNumPOVs (currentJoystick)
            buttons.model = DriverStation.getNumButtons (currentJoystick)
        }
    }

    //
    // Updates the joystick list and re-generates the controls
    //
    function updateControls() {
        listView.model = 0
        listView.model = QJoysticks.deviceNames()

        if (QJoysticks.count() > 0) {
            joysticks.currentJoystick = 0
            joysticks.regenerateControls()

            widgets.opacity = 1
            noProblemo.opacity = 0
        }

        else {
            axes.model = 0
            povs.model = 0
            buttons.model = 0
            widgets.opacity = 0
            noProblemo.opacity = 1
        }
    }

    //
    // Calculates the minimum width for the left tab
    //
    function getMinimumWidth() {
        return width;
    }

    //
    // Used for determining height of axis and button indicators
    //
    function getWidgetHeight (input) {
        return joysticks.height * 0.7 / input
    }

    //
    // Regenerate the UI when a joystick is removed or attached and
    // update DriverStation joysticks
    //
    Connections {
        target: QJoysticks
        onCountChanged: {
            /* Reset UI controls */
            updateControls()

            /* Register joysticks with DS */
            DriverStation.resetJoysticks()
            for (var i = 0; i < QJoysticks.count(); ++i) {
                DriverStation.registerJoystick (QJoysticks.getNumAxes (i),
                                                QJoysticks.getNumButtons (i),
                                                QJoysticks.getNumPOVs (i))
            }
        }

        /* Send joystick input data to DS */
        onPovChanged: {
            DriverStation.updatePOV (js,
                                     pov,
                                     QJoysticks.isBlacklisted (currentJoystick) ? 0 : angle)
        }
        onAxisChanged: {
            DriverStation.updateAxis (js,
                                      axis,
                                      QJoysticks.isBlacklisted (currentJoystick) ? 0 : value)
        }
        onButtonChanged: {
            DriverStation.updateButton (js,
                                        button,
                                        QJoysticks.isBlacklisted (currentJoystick) ? false : pressed)
        }
    }

    Connections {
        target: DriverStation
        onJoystickCountChanged: regenerateControls()
    }

    //
    // Regenerate the UI controls when the user selects another joystick
    //
    onCurrentJoystickChanged: joysticks.regenerateControls()

    //
    // Initialize the QJoysticks system and call updateControls(), which will
    // help us in the case that the virtual joystick is enabled
    //
    Component.onCompleted: {
        QJoysticks.setSortJoysticksByBlacklistState (true)
        joysticks.updateControls()
    }

    //
    // The "No Joysticks? No Problem" widget
    //
    ColumnLayout {
        opacity: 1
        id: noProblemo
        visible: opacity > 0
        anchors.fill: parent
        spacing: Globals.spacing

        Behavior on opacity {
            NumberAnimation {
                duration: Globals.slowAnimation
            }
        }

        Icon {
            size: Globals.scale (64)
            name: icons.fa_gamepad
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            size: large
            font.bold: true
            text: qsTr ("No Joysticks? No Problem!")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            size: medium
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr ("You can also use the virtual joystick!")
        }

        Button {
            onClicked: window.showVirtualJoystickWindow()
            text: qsTr ("Virtual Joystick Settings") + "..."
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    //
    // All the magic happens here
    //
    RowLayout {
        opacity: 0
        id: widgets
        visible: opacity > 0
        anchors.fill: parent
        spacing: Globals.spacing

        Behavior on opacity {
            NumberAnimation {
                duration: Globals.slowAnimation
            }
        }

        //
        // The joystick selector
        //
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Globals.spacing
            Layout.minimumWidth: visible ? Globals.scale (112) : 0

            //
            // The "USB Devices" label
            //
            Label {
                font.bold: true
                text: qsTr ("USB Devices")
            }

            //
            // The list, with a custom control for each joystick
            //
            ListView {
                id: listView
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: joysticks.height * 0.8

                delegate: JoystickItem {
                    jsIndex: index
                    jsName: modelData
                    currentJS: currentJoystick

                    onSelected: {
                        currentJoystick = jsIndex
                        regenerateControls()
                    }

                    onBlacklistedChanged: {
                        var blacklisted = !QJoysticks.isBlacklisted (jsIndex)
                        QJoysticks.setBlacklisted (jsIndex, blacklisted)
                        updateControls()
                    }
                }
            }

            //
            // Vertical spacer
            //
            Item {
                Layout.fillHeight: true
            }
        }

        //
        // Axis indicators
        //
        ColumnLayout {
            visible: axes.model > 0
            spacing: Globals.spacing

            //
            // The "Axes" label
            //
            Label {
                font.bold: true
                text: qsTr ("Axes")
            }

            //
            // Dynamic list of progressbars for each joystick
            //
            ColumnLayout {
                id: axesCol
                Layout.fillHeight: true
                spacing: Globals.scale (1)

                Repeater {
                    id: axes
                    delegate: Progressbar {
                        id: progressbar
                        minimumValue: 0
                        maximumValue: 200
                        width: Globals.scale (54)
                        text: qsTr ("Axis") + " " + index
                        height: getWidgetHeight (axes.model)
                        barColor: QJoysticks.isBlacklisted (currentJoystick) ?
                                      Globals.Colors.IndicatorError :
                                      Globals.Colors.HighlightColor

                        Component.onCompleted: value = maximumValue / 2

                        Connections {
                            target: QJoysticks
                            onAxisChanged: {
                                if (joysticks.currentJoystick === js
                                        && index === axis)
                                    progressbar.value = (value + 1) * 100
                            }
                        }
                    }
                }
            }

            //
            // "Pushes" the rest of the controls together
            //
            Item {
                Layout.fillHeight: true
            }
        }

        //
        // Button indicators
        //
        ColumnLayout {
            spacing: Globals.spacing
            visible: buttons.model > 0
            Layout.minimumWidth: grid.implicitWidth + Globals.spacing

            //
            // The Buttons label...
            //
            Label {
                font.bold: true
                text: qsTr ("Buttons")
            }

            //
            // A dynamic table of button indicators
            //
            GridLayout {
                id: grid
                rows: 6
                flow: GridLayout.TopToBottom
                rowSpacing: Globals.scale (1)
                columnSpacing: Globals.scale (1)

                Repeater {
                    id: buttons

                    //
                    // You did not expect a button to be represented with a
                    // progressbar? Neither did I first wrote this, but this
                    // is by far the easiest and simplest way to center the
                    // label on a 'LED'.
                    // The minimum and maximum values are set to 0 and 1
                    // respectively, to disguise a progressbar into a checkbox.
                    //
                    delegate: Progressbar {
                        id: button
                        minimumValue: 0
                        maximumValue: 1

                        text: index
                        width: Globals.scale (28)
                        height: getWidgetHeight (axes.model)

                        Connections {
                            target: QJoysticks
                            onButtonChanged: {
                                if (joysticks.currentJoystick === js
                                        && button === index)
                                    value = pressed ? 1 : 0
                            }
                        }
                    }
                }
            }

            //
            // "Pushes" the rest of the controls together
            //
            Item {
                Layout.fillHeight: true
            }
        }

        //
        // POVs indicators
        //
        ColumnLayout {
            visible: povs.model > 0
            spacing: Globals.spacing

            //
            // The POVs label
            //
            Label {
                font.bold: true
                text: qsTr ("POVs")
            }

            //
            // A dynamic list of spinboxes for each POV/hat
            //
            ColumnLayout {
                spacing: Globals.scale (1)

                Repeater {
                    id: povs
                    delegate: Spinbox {
                        id: spinbox
                        enabled: false
                        minimumValue: 0
                        maximumValue: 360
                        width: Globals.scale (48)

                        Connections {
                            target: QJoysticks
                            onPovChanged: {
                                if (joysticks.currentJoystick === js
                                        && pov === index)
                                    spinbox.value = angle
                            }
                        }
                    }
                }
            }

            //
            // "Pushes" the rest of the controls together
            //
            Item {
                Layout.fillHeight: true
            }
        }
    }
}
