import QtQuick 2.0

TextInput {
    property color bgColor: "transparent"
    property int borderWidth: 1
    property color borderColor: "#4b6876"
    property string passwordCharacterEx: "❄❄❄❄❄❄❄"
    passwordCharacter: passwordCharacterEx
    Rectangle{
        anchors.fill: parent
        color: bgColor
        border.width: borderWidth
        border.color: borderColor
    }
}
