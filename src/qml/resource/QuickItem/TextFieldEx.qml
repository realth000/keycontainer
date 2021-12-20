import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQml 2.10

TextField {
    id: textInputEx
    property color bgColor: "#2d2e30"
    property int borderWidth: 1
    property color borderColor: "#336666"
    property int borderColorAniDuration: 330
    property string passwordCharacterEx: "******"
    property color textsColor: "#f0ffff"
    property bool textsBold: true
    property int textsSize: 15
    property bool textsClip: true
    property int textsLeftPadding: 2
    property color cursorColor: textsColor
    property int cursorWidth: 2
    property bool enableNativeBorder: false
    property int textLeftPadding: 10

    passwordCharacter: passwordCharacterEx
    color: textsColor
    font.bold: textsBold
    font.pixelSize: textsSize
    verticalAlignment: Qt.AlignVCenter
    cursorDelegate: cursorEx
    clip: textsClip
    leftPadding: textLeftPadding
    selectionColor: borderColor
    selectedTextColor: textsColor
    selectByMouse: true
    background:  Rectangle {
        id: tiexInputArea
        anchors.fill: parent
        color: bgColor
        border.width: enableNativeBorder ? borderWidth : 0
        border.color: borderColor
    }
    SeparatorEx {
        visible: !enableNativeBorder
        id: tiexInputBaseline
        height: 1
        anchors.bottom: tiexInputArea.bottom
        sepColor: textInputEx.activeFocus ? "#f0ffff" : borderColor
        leftMargin: 0
        rightMargin: 0
    }

    // cursor settings
    Component {
        id:cursorEx
        Rectangle {
            width: cursorWidth
            color: textInputEx.activeFocus ? cursorColor : "transparent"
        }
    }

    PropertyAnimation {
        id: borderColorAni
        target: tiexInputBaseline
        property: "width"
        from: 0
        to: width - tiexInputBaseline.leftMargin - tiexInputBaseline.rightMargin
        duration: borderColorAniDuration
    }
    onFocusChanged: {
        if(textInputEx.activeFocus){
            borderColorAni.start();
        }
    }
}
