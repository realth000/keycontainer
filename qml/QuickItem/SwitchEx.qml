import QtQuick 2.0
import QtQuick.Controls 2.5

Switch {
    id: switchItem
    property string texts
    property color bgColor: "transparent"
    property color indiBgColor: "#4b6876"
    property color indiCheckedColor: "#375564"
    property color indiUncheckedColor: "transparent"
    property bool borderBottom: false
    property color borderBottomColor: "#4b6876"
    property int switchHeadPosAniDuration: 100

    width: parent.width
    height: 50
    background: Rectangle{
        color: bgColor
    }
    indicator: Rectangle{
        id: indiRect
        implicitWidth: 40
        implicitHeight: width/2
        anchors.verticalCenter: parent.verticalCenter
        radius: 10
        x: switchItem.leftPadding
        y: indiRect.height / 2 - height / 2
        color: switchItem.checked ? indiCheckedColor : indiUncheckedColor
        border.color: indiBgColor
        Rectangle {
            id: switchHead
//            x: switchItem.checked ? indiRect.width - width : 0
            width: indiRect.height
            height: width
            radius: width/2
            color: indiBgColor
        }
    }
    contentItem: Text{
        text: texts
        anchors.left: indiRect.right
        anchors.leftMargin: 7
        anchors.verticalCenter: indiRect.verticalCenter
        verticalAlignment: Qt.AlignVCenter
        font.pixelSize: 15
        font.bold: true
        color: indiBgColor
    }

    SeparatorEx{
        id: borderBottomRect
        anchors.top: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: -1
        visible: borderBottom
        width: parent.width
        color: borderBottomColor
        height: 1
    }
    PropertyAnimation{
        id: headPosAni
        target: switchHead
        property: "x"
        from: switchHead.x
        to: switchItem.checked ? 0 : indiRect.width - switchHead.width
        duration: switchHeadPosAniDuration
    }
    onCheckedChanged: {
        headPosAni.start();
        console.log("swith ani form ", switchHead.x, "to ", switchItem.checked ? indiRect.width - switchHead.width : 0)
    }
}
