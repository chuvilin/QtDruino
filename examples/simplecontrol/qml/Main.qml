import QtQuick 1.0
import "QKit"
import Arduino 1.0

QKitApplication {
    id: application

    Arduino {id: arduino}

    QKitPage {
        id: page

        property double servoMin: 0.56
        property double servoMax: 0.7

        focus: true
        toolbar: QKitToolbar {
            id: toolbar

            QKitToolbarBackButton {enabled: false; }
            QKitToolbarMenuButton {onClicked: menu.enabled=true}
        }
        backgroundColor: arduino.lightSensorValue <= 0 ? "#FFFFFF" : arduino.floatToColor(arduino.lightSensorValue)

        Keys.onDownPressed: {
            var d = 1 / 255;
            if (arduino.servoValue - d >= servoMin)
                arduino.servoValue -= d;
            else
                arduino.servoValue = servoMin;
        }
        Keys.onUpPressed: {
            var d = 1 / 255;
            if (arduino.servoValue + d <= servoMax)
                arduino.servoValue += d;
            else
                arduino.servoValue = servoMax;
        }

        QKitMouseArea {
            anchors.fill: parent
            onClicked: arduino.startLightSensorInput();
        }

        QKitButton {
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.height / 2
            width: 2 * parent.width / 3
            backgroundColor: "#000000"
            borderColor: "#000000"
            borderWidth: 2
            onClicked: servoBar.anchors.rightMargin = width - mouseArea.mouseX
            QKitRectangle {
                id: servoBar
                anchors.fill: parent
                anchors.rightMargin: parent.width
                color: "#FFFFFF"
                onWidthChanged: arduino.servoValue = page.servoMin + (page.servoMax - page.servoMin) * width / parent.width
            }
        }

        QKitRow {
            anchors.fill: parent
            anchors.topMargin: parent.height / 2

            QKitButton {
                width: parent.width / 4
                height: parent.height
                backgroundColor: "#3F0000"
                borderColor: "#000000"
                borderWidth: 2
                onClicked: redBar.anchors.topMargin = mouseArea.mouseY
                QKitRectangle {
                    id: redBar
                    anchors.fill: parent
                    anchors.topMargin: parent.height
                    color: "#FF0000"
                    onHeightChanged: arduino.redDiodValue = Math.max(height - 0.2 * parent.height, 0) / (0.8 * parent.height)
                }
            }
            QKitButton {
                width: parent.width / 4
                height: parent.height
                backgroundColor: "#003F00"
                borderColor: "#000000"
                borderWidth: 2
                onClicked: greenBar.anchors.topMargin = mouseArea.mouseY
                QKitRectangle {
                    id: greenBar
                    anchors.fill: parent
                    anchors.topMargin: parent.height
                    color: "#00FF00"
                    onHeightChanged: arduino.greenDiodValue = Math.max(height - 0.2 * parent.height, 0) / (0.8 * parent.height)
                }
            }
            QKitButton {
                width: parent.width / 4
                height: parent.height
                backgroundColor: "#00003F"
                borderColor: "#000000"
                borderWidth: 2
                onClicked: blueBar.anchors.topMargin = mouseArea.mouseY
                QKitRectangle {
                    id: blueBar
                    anchors.fill: parent
                    anchors.topMargin: parent.height
                    color: "#0000FF"
                    onHeightChanged: arduino.blueDiodValue = Math.max(height - 0.2 * parent.height, 0) / (0.8 * parent.height)
                }
            }
            QKitButton {
                width: parent.width / 4
                height: parent.height
                backgroundColor: "#3F3F3F"
                borderColor: "#000000"
                borderWidth: 2
                onClicked: whiteBar.anchors.topMargin = mouseArea.mouseY
                QKitRectangle {
                    id: whiteBar
                    anchors.fill: parent
                    anchors.topMargin: parent.height
                    color: "#FFFFFF"
                    onHeightChanged: arduino.whiteDiodValue = Math.max(height - 0.2 * parent.height, 0) / (0.8 * parent.height)
                }
            }
        }
    }

    QKitMenu {
        id: menu

        QKitMenuElement {
            text: "Back"
            onClicked: menu.state = "closed"
        }
        QKitMenuElement {
            text: "Empty"
        }
        QKitMenuElement {
            text: "Quit"
            onClicked: Qt.quit()
        }
    }

    onFocusChanged: if (focus) page.focus = true

    Keys.onPressed: {
        switch (event.key) {
        case Qt.Key_1:
            menu.enabled = true
            break
        case Qt.Key_2:
            toolbar.enabled = !toolbar.enabled
            break
        default:
            return
        }
        event.accepted = true
    }

    Component.onCompleted: console.log()
}
