import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQml 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: gbexMainRect
    property string iconPath
    property string labelText
    property color labelColor: "#4b6876"
    property int labelWidth: 200
    property int labelHeight: 60
    property int labelTextSize: 16
    property bool labelTextBold: true
    property int labelLeftMargin: 15
    property int separatorLeftMargin: 15
    property int separatorRightMargin: 15
    property color separatorColor: labelColor
//    property alias container: gbexContainerRect
    width: parent.width
    height: 200
    color: "transparent"
    Rectangle{
        id: gbexLabelRect
        width: parent.width
        height: parent.height
        color: "transparent"
        ButtonEx{
            id: gbexLabel
            width: labelWidth
            height: labelHeight
            checkable: false
            iconUnchecked: iconPath
            iconPos: 1
            bgColor: parent.color
            anchors.left: parent.left
            anchors.leftMargin: labelLeftMargin
            anchors.top: parent.top
            texts: labelText
            textsUncheckedColor: labelColor
            textsSize: labelTextSize
            textsBold: labelTextBold
            posToLeft:true
        }

        SeparatorEx{
            id: gbexSeparater
            width: parent.width - separatorLeftMargin - separatorRightMargin
            height: 1
            anchors.top: gbexLabel.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.leftMargin: separatorLeftMargin
            anchors.rightMargin: separatorRightMargin
            color: separatorColor
            border.color: separatorColor
        }

//        Rectangle{
//            id: gbexContainerRect
//            width: parent.width
//            anchors.top: gbexLabel.bottom
//            anchors.bottom: parent.bottom
//            anchors.horizontalCenter: parent.horizontalCenter
//            color: "transparent"
//            border.color: "red"
//            border.width: 1
//        }
    }

}
