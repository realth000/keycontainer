import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.12
import "QuickItem"

Item {
    id: addTabItem
    property color bgColor: "transparent"
    property int widthMargin: addTabItem.width/10
    property int heightMargin: addTabItem.height/10
    property bool showPassword: false
    property QtObject root
    signal checkInputExistence(string disc, string account, string password)
    signal addNewKeyToView(string disc, string account, string password)
    Rectangle{
        width: parent.width
        height: parent.height
        color: bgColor
    }

    Rectangle{
        id: addToolRect
        width: parent.width
        height: 50
        anchors.top: parent.top
        anchors.left: parent.left
        color: "#282828"
        ButtonEx{
            id: showAccountAllBtnEx
            width: 100
            height: addToolRect.height
            anchors.left: addToolRect.left
            anchors.verticalCenter: addToolRect.verticalCenter
            bgColor: "transparent"
            bgSelectedColor: "transparent"
            checkable: true
            texts: showAccountAllBtnEx.checked ? "隐藏密码" : "显示密码"
            textsBold: true
            iconChecked: "qrc:/androidsrc/showAccount_reverse.png"
            iconUnchecked: "qrc:/androidsrc/hideAccount.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                showPassword=(showPassword ? false : true)
            }
        }
        ButtonEx{
            width: 100
            height: addToolRect.height
            anchors.horizontalCenter: addToolRect.horizontalCenter
            anchors.verticalCenter: addToolRect.verticalCenter
            bgColor: "transparent"
            bgSelectedColor: "transparent"
            checkable: true
            texts: self.checked ? "123" : "456"
            textsBold: true
            iconChecked: "qrc:/androidsrc/showAccount_reverse.png"
            iconUnchecked: "qrc:/androidsrc/hideAccount.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {

            }
        }
        ButtonEx{
            width: 100
            height: addToolRect.height
            anchors.right: addToolRect.right
            anchors.verticalCenter: addToolRect.verticalCenter
            bgColor: "transparent"
            checkable: false
            texts: "保存密码"
            textsBold: true
            iconUnchecked: "qrc:/androidsrc/key_checked.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                checkInput()
            }
        }
    }
    Rectangle{
       id: addKeysRect
       width: addToolRect.width
       height: addToolRect.height
       anchors.top: addToolRect.bottom
       anchors.bottom: parent.bottom
       color: "#282828"
       Rectangle{
           property alias input1: discInput
           id: rect1
           width: Math.max(300, parent.width*0.5)
           height: 30
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.top: parent.top
           anchors.topMargin: 50
           color: "transparent"
           ButtonEx{
               id: b1
               texts: "说明"
               textsUncheckedColor: "#f0ffff"
               iconPos: 1
               bgColor: "transparent"
               checkable: false
               width: 70
               height: 30
               anchors.left: parent.left
               anchors.verticalCenter: parent.verticalCenter
           }

           TextInputEx{
               id: discInput
               height: 30
               anchors.left: b1.right
               anchors.right: parent.right
               anchors.verticalCenter: parent.verticalCenter
           }
       }
       Rectangle{
           property alias input2: accountInput
           id: rect2
           width: Math.max(300, parent.width*0.5)
           height: 30
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.top: rect1.top
           anchors.topMargin: 50
           color: "transparent"
           ButtonEx{
               id: b2
               texts: "账户"
               textsUncheckedColor: "#f0ffff"
               iconPos: 1
               bgColor: "transparent"
               checkable: false
               width: 70
               height: 30
               anchors.left: parent.left
               anchors.verticalCenter: parent.verticalCenter
           }

           TextInputEx{
               id: accountInput
               height: 30
               anchors.left: b2.right
               anchors.right: parent.right
               anchors.verticalCenter: parent.verticalCenter
           }
       }
       Rectangle{
           property alias input3: passwordInput
           id: rect3
           width: Math.max(300, parent.width*0.5)
           height: 30
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.top: rect2.top
           anchors.topMargin: 50
           color: "transparent"
           ButtonEx{
               id: b3
               texts: "密码"
               textsUncheckedColor: "#f0ffff"
               iconPos: 1
               bgColor: "transparent"
               checkable: false
               width: 70
               height: 30
               anchors.left: parent.left
               anchors.verticalCenter: parent.verticalCenter
           }

           TextInputEx{
               id: passwordInput
               height: 30
               anchors.left: b3.right
               anchors.right: parent.right
               anchors.verticalCenter: parent.verticalCenter
               echoMode: showPassword ? "Normal" : "Password"
           }
       }
       Rectangle{
           property alias input4: passwordConfirmInput
           id: rect4
           width: Math.max(300, parent.width*0.5)
           height: 30
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.top: rect3.top
           anchors.topMargin: 50
           color: "transparent"
           ButtonEx{
               id: b4
               texts: "确认密码"
               textsUncheckedColor: "#f0ffff"
               iconPos: 1
               bgColor: "transparent"
               checkable: false
               width: 70
               height: 30
               anchors.right: passwordConfirmInput.left
               anchors.verticalCenter: parent.verticalCenter
               horizontalOffset:-15
           }

           TextInputEx{
               id: passwordConfirmInput
               height: 30
               anchors.left: b4.right
               anchors.right: parent.right
               anchors.verticalCenter: parent.verticalCenter
               echoMode: showPassword ? "Normal" : "Password"
           }
       }
       Label{
           property string textsE : "sadas"
            id: hintLabel
            height: 30
            anchors.left: rect4.left
            anchors.right: rect4.right
            anchors.top: rect4.top
            anchors.topMargin: 50
            color: "#f0ffff"
            visible: false
            Text{
                text: parent.textsE
                color: "#f0ffff"
                anchors.horizontalCenter: parent.horizontalCenter
                font.bold: true
                font.pixelSize: 15
            }
       }
    }
    function checkInput(){
        if(rect1.input1.text == ""){
            hintLabel.visible=true
            hintLabel.textsE="说明为空";
            return;
        }
        else if(rect2.input2.text == ""){
            hintLabel.visible=true
            hintLabel.textsE="账号为空";
            return;
        }
        else if(rect3.input3.text == ""){
            hintLabel.visible=true
            hintLabel.textsE="密码为空";
            return;
        }
        else if(rect4.input4.text == ""){
            hintLabel.visible=true
            hintLabel.textsE="确认密码为空";
            return;
        }
        else if(rect3.input3.text != rect4.input4.text){
            hintLabel.visible=true
            hintLabel.textsE="两次输入密码不一致";
            return;
        }
        hintLabel.visible=false
        addTabItem.checkInputExistence(rect1.input1.text, rect2.input2.text, rect3.input3.text);
    }

    Connections{
        target: addTabItem
        onCheckInputExistence:{
            var result= -1
            result = root.importer.checkExistence(disc, account, password);
            if(result <0){
                hintLabel.visible=true
                hintLabel.textsE="与第"+ (-result)+"个重复"
                return;
            }
            if(result>0){
                hintLabel.visible=true
                addTabItem.addNewKeyToView(disc, account, password)
                hintLabel.textsE="保存完成"
                rect1.input1.clear();
                rect2.input2.clear();
                rect3.input3.clear();
                rect4.input4.clear();
                closeHintTimer.running=true
                return;
            }
            console.log("TabViewTabAddKey: add result ", result)
        }
    }
    Timer{
        id: closeHintTimer
        interval: 5000
        repeat: false
        running: false
        onTriggered: {
            hintLabel.visible=false
        }
    }

    Connections{
        target: addTabItem
        onAddNewKeyToView:{
            root.viewer.model.append(
                JSON.parse(
                   "{\"disc\":\""+disc+"\",
                    \"account\":\""+account+"\",
                    \"password\":\""+disc+"\",
                    \"keyChecked\":false,
                    \"showAccount\":false,
                    \"showPassword\":false}"
                )
            );
        }
    }
}
