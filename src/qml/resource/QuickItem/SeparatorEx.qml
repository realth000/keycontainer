import QtQuick 2.10

Rectangle {
    property int margin: 15
    property int leftMargin: margin
    property int rightMargin: margin
    property color sepColor: "#4b6876"

    height: 2
    width: parent.width - leftMargin - rightMargin
    color: sepColor
    border.color: sepColor
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.leftMargin: leftMargin
    anchors.rightMargin: rightMargin
}
