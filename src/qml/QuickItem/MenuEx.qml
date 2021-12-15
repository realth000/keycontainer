import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQml 2.12

Rectangle {
    id: menuRect
    enum MenuPos{
        UpLeft = 0x00000001,
        UpRight = 0x00000002,
        DownLeft = 0x00000003,
        DownRight = 0x00000004
    }
    property QtObject trigger
    property QtObject fullScreen
    property int pos: MenuEx.MenuPos.DownLeft
    property int optsCount: 4
    width: 100
    height: 60*optsCount
    z: 1
    visible: false;
    onVisibleChanged: {
        if(visible){
            if(pos === MenuEx.MenuPos.DownLeft){
                menuRect.x = trigger.x + trigger.width - width;
                menuRect.y = trigger.y;
            }
            else if(pos === MenuEx.MenuPos.DownRight){
                menuRect.x = trigger.x;
                menuRect.y = trigger.y;
            }
            else if(pos === MenuEx.MenuPos.UpLeft){
                menuRect.x = trigger.x;
                menuRect.y = trigger.y + trigger.height - height;
            }
            else if(pos === MenuEx.MenuPos.UpRight){
                menuRect.x = trigger.x + trigger.width - width;
                menuRect.y = trigger.y + trigger.height - height;
            }
        }
    }
    color: "transparent"
    MouseArea{
        width: mainWindow.width
        height: mainWindow.height
        anchors.left: menuRect.left
        anchors.leftMargin: -menuRect.x
        anchors.top: menuRect.top
        anchors.topMargin: -menuRect.y
        onClicked: {
            menuRect.visible = false;
        }
    }
}
