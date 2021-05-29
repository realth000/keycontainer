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
    property color cursorColor: "#f0ffff"
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
        anchors.fill: parent
        color: bgColor
        border.width: borderWidth
        border.color: borderColor
    }
    Component{
        id:cursorEx
        Rectangle{
            width: cursorWidth
            color: textInputEx.activeFocus ? cursorColor : "transparent"
        }

    }
}
