import QtQuick 2.0
import QtQml 2.12

Rectangle {
    id: folderRect
    property QtObject trigger
    property color bgColor: "transparent"
    property int openHeight: 60
    readonly property int initHeight : height
    property bool opened: false
    height: 0
    visible: false
//    width: parent.width
    color: bgColor
    function close(){
        height = 0;
        opened = false;
        visible = false;
    }
    function open(){
        height = openHeight;
        opened = true;
        visible = true;
    }
}
