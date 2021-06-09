import QtQuick 2.12

Rectangle {
    property int leftMargin: 15
    property int rightMargin: 15
    property color sepColor: "#4b6876"

    height: 2
    width: parent.width - leftMargin - rightMargin
    color: sepColor
    border.color: sepColor
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.leftMargin: leftMargin
    anchors.rightMargin: rightMargin
}
