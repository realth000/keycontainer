import QtQuick 2.12
import QtQuick.Controls 2.5
import "QuickItem"
import QtQml 2.12

Rectangle {
    property QtObject root
    id: loginRect
    color: "#232323"
    visible: false
    property bool hint: false
    Rectangle{
        property alias input1: passwordInput
        id: rect1
        width: Math.max(300, parent.width*0.5)
        height: 30
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -parent.height*0.2
        color: "transparent"

        TextInputEx{
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
        root.importer.checkPwd(passwordInput.text);
    }
    function showHint(){
        hint=true;
        passwordInput.clear();
    }
    Connections{
        target: root
        onLoginShowHint:{
            loginRect.showHint();
        }
    }
    Keys.onReleased: {
        if((event.modifiers === Qt.NoModifier) && (event.key === Qt.Key_Enter || event.key === Qt.Key_Return )){
            loginBtnex.clicked();
        }
    }
    onVisibleChanged: {
        visible ? mainDrawer.interactive=false : mainDrawer.interactive=true;
    }

}
