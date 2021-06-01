import QtQuick 2.12
import QtQml 2.12

TextInput {
    id: textInputEx
    property color bgColor: "transparent"
    property int borderWidth: 1
    property color borderColor: "#4b6876"
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
    Rectangle{
        id: tiexInputArea
        anchors.fill: parent
//        color: "transparent"
        color: "#2d2e30"
        border.width: 0
        z: -1
    }
    Rectangle{
        id: tiexInputBaseline
        height: 1
        anchors.bottom: tiexInputArea.bottom
        anchors.left: tiexInputArea.left
        anchors.right: tiexInputArea.right
        color: textInputEx.activeFocus ? "#f0ffff" : borderColor
        border.color: textInputEx.activeFocus ? "#f0ffff" : borderColor
    }

    Component{
        id:cursorEx
        Rectangle{
            width: cursorWidth
            color: textInputEx.activeFocus ? cursorColor : "transparent"
        }

    }
}
