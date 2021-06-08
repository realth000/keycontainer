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

    width: parent.width
    height: 50
    background: Rectangle{
        color: bgColor
    }
    indicator: Rectangle{
        id: indiRect
        implicitWidth: 40
        implicitHeight: 20
        anchors.verticalCenter: parent.verticalCenter
        radius: 10
        x: switchItem.leftPadding
        y: indiRect.height / 2 - height / 2
        color: switchItem.checked ? indiCheckedColor : indiUncheckedColor
        border.color: indiBgColor
        Rectangle {
            x: switchItem.checked ? indiRect.width - width : 0
            width: 20
            height: 20
            radius: 10
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

}
