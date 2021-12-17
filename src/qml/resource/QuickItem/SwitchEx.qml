import QtQuick 2.0
import QtQuick.Controls 2.5

Rectangle {
    id: switchItem
    property string texts
    property color backgroundColor: "transparent"
    property color indicatorBackgroundColor: "#f0ffff"
    property color indicatorCheckedColor: "#336666"
    property color indicatorUncheckedColor: "transparent"
    property color borderColor: indicatorBackgroundColor
    property int indicatorWidth: 60
    property int indicatorHeight: 25
    property bool borderBottom: false
    property color borderBottomColor: "#f0ffff"
    property int switchHeadPosAniDuration: 100
    property bool checked: false
    property bool checkable: true
    width: parent.width
    height: 50
    color: backgroundColor

    Rectangle{
        id: indiRect
        width: indicatorWidth
        height: indicatorHeight
        anchors.right: parent.right
        radius: 10
        x: 0
        y: indiRect.height / 2 - height / 2
        color: switchItem.checked ? indicatorCheckedColor : indicatorUncheckedColor
        border.color: borderColor
        Rectangle {
            id: switchHead
            width: indiRect.height
            height: width
            radius: width/2
            color: checkable ? indicatorBackgroundColor : "#c1c7d0";
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                switchItem.checked = !switchItem.checked
            }
        }
    }
    Text{
        text: texts
        anchors.left: parent.left
        anchors.leftMargin: 7
        anchors.verticalCenter: indiRect.verticalCenter
        verticalAlignment: Qt.AlignRight
        font.pixelSize: 15
        font.bold: true
        color: indicatorBackgroundColor
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
    }
}
