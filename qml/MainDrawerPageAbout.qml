import QtQuick 2.12
import QtQuick.Controls 2.5
import "QuickItem"

ScrollView {
    id: mainRect
    background: Rectangle{
        color: "#232323"
    }

    Rectangle{
        id: appNameRect
        width: parent.width
        height: 100
        anchors.left: parent.left
        color: "transparent"
        Label{
            id: appNameLabel
            width: parent.width
            height: 50
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Qt.AlignHCenter
            text: "Key Container"
            color: "#afc8d2"
            font.pixelSize: 29
            font.bold: true
        }
    }
    Rectangle{
        id: platformRect
        width: parent.width
        height: 20
        color: "transparent"
        anchors.top: appNameRect.bottom
        anchors.left: parent.left
        Label{
            width: 150
            height: 20
            x: appNameLabel.contentWidth
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Qt.AlignRight
            text: mainWindow.importer.getPlatform()
            color: "#f0ffff"
            font.pixelSize: 16
        }
    }
    Rectangle{
        id: appVersionRect
        width: 200
        height: 20
        color: "transparent"
        anchors.top: platformRect.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 150
        Label{
            width: appNameLabel.contentWidth
            height: 20
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Qt.AlignLeft
            text: "软件版本: " + "<b>" + mainWindow.importer.getAppVersion() + "</b>"
            color: "#f0ffff"
            font.pixelSize: 16
        }
    }
    Rectangle{
        id: buildTimeRect
        width: appVersionRect.width
        height: 20
        color: "transparent"
        anchors.top: appVersionRect.bottom
        anchors.left: parent.left
        anchors.leftMargin: 150
        Label{
            width: appNameLabel.contentWidth
            height: 20
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Qt.AlignLeft
            text: "构建时间: " + mainWindow.importer.getBuildTime()
            color: "#f0ffff"
            font.pixelSize: 16
        }
    }
    Rectangle{
        id: buildInfoRect
        width: buildTimeRect.width
        height: 200
        color: "transparent"
        anchors.top: buildTimeRect.bottom
        anchors.left: parent.left
        anchors.leftMargin: 150
        Label{
            id: lb
            width: 80
            height: 20
            anchors.left: parent.left
            anchors.top: parent.top
            horizontalAlignment: Qt.AlignLeft
            text: "构建信息: "
            color: "#f0ffff"
            font.pixelSize: 16
        }
        Label{
            height: 20
            anchors.left: lb.right
            anchors.top: parent.top
            anchors.right: parent.right
            horizontalAlignment: Qt.AlignLeft
            text: "C++ " + mainWindow.importer.getCPPVersion() + "\n"
                    + mainWindow.importer.getQtVerionString() + "\n"
                    + mainWindow.importer.getCompilerInfo() + "\n"
                    + "Qt-AES bricke@github"
            color: "#f0ffff"
            font.pixelSize: 16
        }
    }


}
