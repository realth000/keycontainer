import QtQuick 2.12
import QtQuick.Controls 2.5
import "QuickItem"
import QtQml 2.12
import QtGraphicalEffects 1.15

Rectangle {
    property QtObject root
    id: loginRect
    color: "#232323"
    visible: false
    property bool hint: false
    Rectangle {
        width: parent.width
        anchors.bottom: rect1.bottom
        anchors.bottomMargin: 70
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        color: "#272727"
        LinearGradient {
            width: parent.width
            height: parent.height
            gradient: Gradient {
                GradientStop{position:0.0; color: "#262626" }
                GradientStop{position:1.0; color: "#292929" }
            }
        }

        Image {
            width: 300
            height: 150
            id: name
            source: "qrc:/pic/IconWide.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle{
        property alias input1: passwordInput
        id: rect1
        width: Math.max(300, parent.width*0.5)
        height: 40
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -parent.height*0.2
        color: "transparent"

        TextFieldEx{
            id: passwordInput
            height: 45
            width: 300
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            echoMode: "Password"
            textsSize: 20
        }
    }

    ButtonEx{
        id: loginBtnex
        useIcon: false
        checkable: false
        texts: "登录"
        textsSize: 20
        width: parent.width
        height: 70
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: rect1.bottom
        anchors.topMargin: 40
        bgColor: "transparent"
        enablePressWave: false
        onClicked: {
            hint = false;
            loginRect.checkPassword();
        }
    }

    Label{
        text: "密码错误"
        color: "#f0ffff"
        font.pixelSize: 18
        font.bold: true
        horizontalAlignment: Qt.AlignHCenter
        background: Rectangle{
            color: "transparent"
        }
        height: 30
        width: 100
        anchors.top: loginBtnex.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        visible: hint
    }

    function checkPassword(){
        mainQmlImporter.checkPwd(passwordInput.text);
    }
    function showHint(){
        hint=true;
        passwordInput.clear();
    }
    Connections{
        target: root
        function onLoginShowHint() {
            loginRect.showHint();
        }
    }
    Keys.onReleased: {
        if((event.modifiers === Qt.NoModifier) && (event.key === Qt.Key_Enter || event.key === Qt.Key_Return )){
            loginBtnex.clicked();
        }
    }

}
