import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQml 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: gbexMainRect
    property QtObject labelRect: gbexLabel
    property QtObject separator: gbexSeparater
    property QtObject item1: item1
    property QtObject item2: item2
    readonly property int titleHeight: labelHeight + gbexSeparater.height
    property string iconPath
    property string labelText
//    property color labelColor: "#4b6876"
//    property color labelColor: "#336699"
//    property color labelColor: "#336666"
    property color labelColor: "#4b6876"
//    property color labelColor: "#f0ffff"
    property color labelBgColor: "transparent"
    property int labelWidth: 200
    property int labelHeight: 60
    property int labelTextSize: 16
    property bool labelTextBold: true
    property int labelLeftMargin: 15
    property int separatorLeftMargin: labelLeftMargin
    property int separatorRightMargin: labelLeftMargin
    property color separatorColor: labelColor
//    property color backgroundColor: "transparent"
    property color backgroundColor: "#272727"
    width: parent.width
    height: 200
    color: "transparent"
    Rectangle {
        id: gbexBackgroundRect
        width: parent.width
        height: parent.height
        color: backgroundColor
    }
    ButtonEx {
        id: gbexLabel
        width: gbexBackgroundRect.width - 2*labelLeftMargin
        height: labelHeight
        checkable: false
        iconUnchecked: iconPath
        iconPos: 1
        bgColor: labelBgColor
        anchors.left: gbexBackgroundRect.left
        anchors.leftMargin: labelLeftMargin
        anchors.top: gbexBackgroundRect.top
        texts: labelText
        textsUncheckedColor: labelColor
        textsSize: labelTextSize
        textsBold: labelTextBold
        posToLeft:true
        enablePressWave: false
    }

    SeparatorEx {
        id: gbexSeparater
        width: gbexBackgroundRect.width - separatorLeftMargin - separatorRightMargin
        height: 1
        anchors.top: gbexLabel.bottom
        anchors.horizontalCenter: gbexBackgroundRect.horizontalCenter
        anchors.leftMargin: separatorLeftMargin
        anchors.rightMargin: separatorRightMargin
        color: separatorColor
        border.color: separatorColor
    }
}
