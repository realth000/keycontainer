import QtQuick 2.12

Rectangle {
    property int leftMargin: 15
    property int rightMargin: 15

    height: 2
    width: parent.width - leftMargin - rightMargin
    color: "#4b6876"
    border.color: color
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.leftMargin: leftMargin
    anchors.rightMargin: rightMargin
}
