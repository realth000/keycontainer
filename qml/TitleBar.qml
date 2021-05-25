import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.12
//import "QuickItem"

Item {

    property int iconMargin: 0
    property color backgroundColor: "white"
    property int xx
    property int yy
    property int mainWindowX
    property int mainWindowY

    id: titleBar
    width: parent.width
    height: 30
    // 背景
    Rectangle{
        width: parent.width
        height: parent.height
        color: backgroundColor
    }


    MouseArea{
        id: dragArea
        property bool held: false
        width: parent.width
        height: parent.height
        anchors.left: parent.left
        anchors.right: closeBtn.left
        acceptedButtons: Qt.LeftButton
        onPressed: {
            xx = mouseX + mainWindow.x
            yy = mouseY + mainWindow.y
            mainWindowX = mainWindow.x
            mainWindowY = mainWindow.y
        }
        onPositionChanged: {
            mainWindow.setX(mainWindowX + mouseX + mainWindow.x - xx)
            mainWindow.setY(mainWindowY + mouseY + mainWindow.y - yy)
        }
    }

    Image{
        id: iconImage
        width: titleBar.height - 2*iconMargin
        height: titleBar.height - 2*iconMargin
        source: "qrc:/androidsrc/config.png"
        sourceSize.width: 30
        sourceSize.height: 30
    }

    Rectangle{
        id: titleTextRect
        anchors.left: iconImage.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: minimizeBtn.left
        Text{
            text:"title"
            anchors.margins: 2 // Leaving space between text and borders  (optional)
            anchors.left: titleTextRect.left // Centering text
            anchors.verticalCenter: titleTextRect.verticalCenter
//            renderType: Text.NativeRendering // Rendering type (optional)
        }
    }

    Button{
        id:minimizeBtn
        width: titleBar.height
        height: titleBar.height
        anchors.top: titleBar.top
        anchors.right: maximumBtn.left
        text: "1"
        background: Rectangle{
            radius: 0
            color: backgroundColor
        }
        onClicked: {
            mainWindow.showMinimized()
        }
    }

    Button{
        id:maximumBtn
        width: titleBar.height
        height: titleBar.height
        anchors.top: titleBar.top
        anchors.right: closeBtn.left
        text: "2"
        background: Rectangle{
            radius: 0
            color: backgroundColor
        }
        onClicked: {
            mainWindow.visibility===4 ? mainWindow.showNormal() : mainWindow.showMaximized()
        }
    }

    Button{
        id:closeBtn
        width: titleBar.height
        height: titleBar.height
        anchors.top: titleBar.top
        anchors.right: titleBar.right
        text: "3"
        background: Rectangle{
            radius: 0
            color: backgroundColor
        }
        onClicked: {
            Qt.quit()
        }
    }
}
