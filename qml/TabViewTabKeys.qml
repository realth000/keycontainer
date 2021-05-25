import QtQuick 2.12
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.12
import QtQml 2.12
import QtQml.Models 2.12
import "QuickItem"

Item {
    property string hides: "***********"
    property color bgColor: "transparent"
    property int toolNum: 4
    //背景
    Rectangle{
        width: parent.width
        height: parent.height
        color: bgColor
    }

    // 装操作，预计装4个，放一排
    // 可以圆一点
    Rectangle{
        id: manaToolRect
        width: parent.width
        height: 50
        anchors.top: parent.top
        anchors.left: parent.left
        color: "#282828"
        ButtonEx{
            id: showAccountAllBtnEx
            width: 100
            height: manaToolRect.height
            anchors.left: manaToolRect.left
            anchors.verticalCenter: manaToolRect.verticalCenter
            bgColor: "transparent"
            checkable: true
            texts: showAccountAllBtnEx.checked ? "隐藏密码" : "显示密码"
            textsBold: true
            iconChecked: "qrc:/androidsrc/showAccount_reverse.png"
            iconUnchecked: "qrc:/androidsrc/hideAccount.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                var state =showAccountAllBtnEx.checked
                for(var i=0; i<keysView.model.count; i++){
                    var obj =keysView.model.get(i)
                    obj.showAccount = state
                    obj.showPassword = state
                }
            }
        }
        ButtonEx{
            width: 100
            height: manaToolRect.height
            anchors.horizontalCenter: manaToolRect.horizontalCenter
            anchors.verticalCenter: manaToolRect.verticalCenter
            bgColor: "transparent"
            checkable: true
            texts: self.checked ? "123" : "456"
            textsBold: true
            iconChecked: "qrc:/androidsrc/showAccount_reverse.png"
            iconUnchecked: "qrc:/androidsrc/hideAccount.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                var newState=false
                self.checked ? newState = true : newState = false
                for(var i=0; i<KeyModel.count; i++){
                    KeyModel.get(i).showAccountBtnEx.checked=newState
                }
            }
        }
        ButtonEx{
            width: 100
            height: manaToolRect.height
            anchors.right: manaToolRect.right
            anchors.verticalCenter: manaToolRect.verticalCenter
            bgColor: "transparent"
            checkable: false
            texts: "查询"
            textsBold: true
            iconChecked: "qrc:/androidsrc/showAccount_reverse.png"
            iconUnchecked: "qrc:/androidsrc/hideAccount.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                var newState=false
                self.checked ? newState = true : newState = false
                for(var i=0; i<KeyModel.length; i++){
                    KeyModel.get(i).showAccountBtnEx.checked=newState
                }
            }
        }
    }
    Rectangle{
        id: keysRect
        width: manaToolRect.width
        height: manaToolRect.width
        anchors.top: manaToolRect.bottom
        anchors.bottom: parent.bottom
        color: "#282828"

        ListView {
            id: keysView
            anchors.fill: parent
            spacing: 10
            model: KeyModel {}
            delegate: keyDelegate
            highlight: Rectangle {
                color: "#40403d";
//                border.color: "#375564"
            }
            highlightFollowsCurrentItem:true
            highlightMoveDuration: 70
            focus: true
            displayMarginBeginning:0
            // NOTE: 超出部分自动隐藏
            clip:true

            Component {
                id: keyDelegate
                Item {
                    property alias accountBtn: showAccountBtnEx
                    property alias keyRect: keyItemRect
                    id: keyItem
                    width: keysRect.width
                    height: 60

                    Rectangle{
                        id: keyItemRect
                        anchors.fill: parent
                        color: "transparent"
            //                    border.color: "red"
                        ButtonEx{
                            id: keysCheckBtnEx
                            width: keyItem.height
                            height: keyItem.height
                            anchors.left: keyItemRect.left
                            anchors.verticalCenter: keyItemRect.verticalCenter
                            useTexts: false
                            bgColor: "transparent"
                            bgSelectedColor: "transparent"
                            iconChecked: "qrc:/androidsrc/key.png"
                            iconUnchecked: "qrc:/androidsrc/key_checked.png"
                            iconWidth: keyItem.height*0.5
                            iconHeight: keyItem.height*0.5
                            checked: keyChecked
                            onClicked: {
                                keysCheckBtnEx.checked ? keyChecked=false : keyChecked=true
                            }
                        }
                        Rectangle{
                            height: keyItemRect.height
                            anchors.left: keysCheckBtnEx.right
                            anchors.verticalCenter: keyItemRect.verticalCenter
                            anchors.right: showAccountBtnEx.left
                            color: "transparent"
                            ColumnLayout {
                                anchors.fill: parent
                                spacing: 0
                                Text {
                                    id: discText
                                    text: '<b>'+ disc + '</b>'
                                    font.pixelSize: 16
                                    color: keysView.currentIndex==index ? "#f0ffff" : "#f0ffff"
                                    clip: true
                                }
                                Text {
                                    id: accountText
                                    text: '<b>账户: ' + showAccountBtnEx.checked ? hides : account + '</b>'
                                    font.pixelSize: 15
                                    color: keysView.currentIndex==index ? "#f0ffff" : "#f0ffff"
                                    clip: true
                                }
                                Text {
                                    id: passwordText
                                    text: '<b>密码: ' + showAccountBtnEx.checked ? hides : password + '</b>'
                                    font.pixelSize: 15
                                    color: keysView.currentIndex==index ? "#f0ffff" : "#f0ffff"
                                    clip: true
                                }
                            }
                        }
                        ButtonEx{
                            id: showAccountBtnEx
                            width: keyItem.height
                            height: keyItem.height
                            anchors.right: showPasswordBtnEx.left
                            anchors.verticalCenter: keyItemRect.verticalCenter
                            useTexts: false
                            bgColor: "transparent"
                            bgSelectedColor: "transparent"
                            iconChecked: "qrc:/androidsrc/showAccount_reverse.png"
                            iconUnchecked: "qrc:/androidsrc/hideAccount.png"
                            iconWidth: keyItem.height*0.5
                            iconHeight: keyItem.height*0.5
                            checked: showAccount
                            onClicked: {
                                showAccountBtnEx.checked ? showAccount=false : showAccount=true
                            }
                            onCheckedChanged: {
                                showAccountBtnEx.checked ? accountText.text=account : accountText.text=hides
                            }
                        }
                        ButtonEx{
                            id: showPasswordBtnEx
                            width: keyItem.height
                            height: keyItem.height
                            anchors.right: keyItemRect.right
                            anchors.verticalCenter: keyItemRect.verticalCenter
                            useTexts: false
                            bgColor: "transparent"
                            bgSelectedColor: "transparent"
                            iconChecked: "qrc:/androidsrc/unlocked_reverse.png"
                            iconUnchecked: "qrc:/androidsrc//lock.png"
                            iconWidth: keyItem.height*0.5
                            iconHeight: keyItem.height*0.5
                            checked: showPassword
                            onClicked: {
                                showPasswordBtnEx.checked ? showPassword=false : showPassword=true
                            }
                            onCheckedChanged: {
                                showPasswordBtnEx.checked ? passwordText.text=password : passwordText.text=hides
                            }
                        }
                    }
                    MouseArea{
                        anchors.fill: keyItem
                        onClicked: {
                            if(mouseX < keysCheckBtnEx.width){
                                keysCheckBtnEx.clicked()
                            }
                            else if(mouseX < showAccountBtnEx.x){
                                keysView.currentIndex=index
                            }
                            else if(mouseX < showPasswordBtnEx.x){
                                showAccountBtnEx.clicked()
                            }
                            else if(mouseX < keyItemRect.width){
                                showPasswordBtnEx.clicked()
                            }
                        }
                    }
                }
            }
         }
    }
}
