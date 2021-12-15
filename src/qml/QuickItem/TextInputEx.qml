import QtQuick 2.12
import QtQml 2.12

TextInput {
    id: textInputEx
    property color bgColor: "transparent"
    property int borderWidth: 1
    property color borderColor: "#4b6876"
    property int borderColorAniDuration: 330
    property string passwordCharacterEx: "******"
    property color textsColor: "#f0ffff"
    property bool textsBold: true
    property int textsSize: 15
    property bool textsClip: true
    property int textsLeftPadding: 2
    property color cursorColor: textsColor
    property int cursorWidth: 2
    passwordCharacter: passwordCharacterEx
    color: textsColor
    font.bold: textsBold
    font.pixelSize: textsSize
    verticalAlignment: Qt.AlignVCenter
    cursorDelegate: cursorEx
    clip: textsClip
    leftPadding: 2
    selectionColor: borderColor
    selectByMouse: true
    Rectangle{
        id: tiexInputArea
        anchors.fill: parent
        color: "#2d2e30"
        border.width: 0
        z: -1
    }
    SeparatorEx{
        id: tiexInputBaseline
        height: 1
        anchors.bottom: tiexInputArea.bottom
        sepColor: textInputEx.activeFocus ? "#f0ffff" : borderColor
        leftMargin: 0
        rightMargin: 0
    }

    Component{
        id:cursorEx
        Rectangle{
            width: cursorWidth
            color: textInputEx.activeFocus ? cursorColor : "transparent"
        }
    }
    PropertyAnimation{
        id: borderColorAni
        target: tiexInputBaseline
        property: "width"
        from: 0
        to: width - tiexInputBaseline.leftMargin - tiexInputBaseline.rightMargin
        duration: borderColorAniDuration
    }
    onFocusChanged: {
        borderColorAni.start();
    }
}
