import QtQuick 2.12
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.12
import QtQml 2.12
import QtQml.Models 2.12
import "QuickItem"

Item {
    property QtObject root
    property alias viewer: keysView
    property string hides: "*******"
    property color bgColor: "transparent"
    property int toolNum: 4
    signal saveKeys()
    signal freezeFindSig()
    id: mainTabKeys
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
        color: "#232323"
        ButtonEx{
            id: showAccountAllBtnEx
            width: parent.width/4
            height: manaToolRect.height
            anchors.left: manaToolRect.left
            anchors.verticalCenter: manaToolRect.verticalCenter
            bgColor: "transparent"
            bgSelectedColor: "transparent"
            checkable: true
            texts: showAccountAllBtnEx.checked ? "隐藏" : "显示"
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
            id: saveKeysBtnex
            width: showAccountAllBtnEx.width
            height: manaToolRect.height
            anchors.left: showAccountAllBtnEx.right
            anchors.verticalCenter: manaToolRect.verticalCenter
            bgColor: "transparent"
            checkable: false
            texts: "保存"
            textsBold: true
            iconUnchecked: "qrc:/androidsrc/saveFile.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                mainTabKeys.saveKeys()
            }
        }
        ButtonEx{
            id: delKeysBtnex
            width: showAccountAllBtnEx.width
            height: manaToolRect.height
            anchors.left: saveKeysBtnex.right
            anchors.verticalCenter: manaToolRect.verticalCenter
            bgColor: "transparent"
            checkable: false
            texts: "删除"
            textsBold: true
            iconUnchecked: "qrc:/androidsrc/delKey.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                for(var i=keysView.model.count-1; i>=0; i--){
                    var obj =keysView.model.get(i)
                    if(obj.keyChecked === true){
                        keysView.model.remove(i);
                        root.importer.deleteKey(i);
                    }
                }
                root.importer.syncKeyIndex();
            }
        }
        ButtonEx{
            id: findKeyBtnex
            width: showAccountAllBtnEx.width
            height: manaToolRect.height
            anchors.left: delKeysBtnex.right
            anchors.verticalCenter: manaToolRect.verticalCenter
            bgColor: "transparent"
            checkable: false
            texts: "搜索"
            textsBold: true
            iconUnchecked: "qrc:/androidsrc/findKey.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                findKeyRect.visible = true;
                showAccountAllBtnEx.visible = false;
                saveKeysBtnex.visible = false;
                delKeysBtnex.visible = false;
                findKeyBtnex.visible = false;
            }
        }
    }
    Rectangle{
        id: findKeyRect
        visible: false
        anchors.fill: manaToolRect
        color: manaToolRect.color
        TextInputEx{
            id: findKeyKeywordInput
            height: findKeyRect.height*0.65
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: findBackwardBtnex.left
            onTextChanged: {
                root.importer.changeFindText(text);
            }
        }

        ButtonEx{
            id: findBackwardBtnex
            width: height
            height: findKeyRect.height
            anchors.top: parent.top
            anchors.right: findForwardBtnex.left
            useTexts: false
            checkable: false
            iconUnchecked: "qrc:/androidsrc/arrow_left_30.png"
            onClicked: {
                mainTabKeys.freezeFindSig();
                root.importer.findPreviousKey();
            }
        }
        ButtonEx{
            id: findForwardBtnex
            width: height
            height: findKeyRect.height
            anchors.top: parent.top
            anchors.right: closeFindBtnex.left
            useTexts: false
            checkable: false
            iconUnchecked: "qrc:/androidsrc/arrow_right_30.png"
            onClicked: {
                mainTabKeys.freezeFindSig();
                root.importer.findNextKey();
            }
        }
        ButtonEx{
            id: closeFindBtnex
            width: height
            height: findKeyRect.height
            anchors.top: parent.top
            anchors.right: parent.right
            useTexts: false
            checkable: false
            iconUnchecked: "qrc:/androidsrc/config3.png"
            onClicked: {
                findKeyMenu.visible = true;
            }
        }
    }

    MenuEx{
        id: findKeyMenu
        trigger: closeFindBtnex
        width: 150
        optsCount: 5
        MenuItemEx{
            id: i1
            texts: "计数"
            anchors.top: parent.top
            textsLeftMargin: 5
            iconUnchecked: "qrc:/androidsrc/count2.png"
            onClicked: {
            }
        }

        SwitchEx{
            id: i2
            texts: "全词匹配"
            height: 60
            bgColor: i1.bgColor
            anchors.top: i1.bottom
            indiBgColor: "#f0ffff"
            indiCheckedColor: "#4b6876"
            indiUncheckedColor: "#404040"
            borderBottom: true
            borderBottomColor: "#f0ffff"
            onCheckedChanged: {
                root.importer.setFindAllWord(checked);
                checked || i3.checked ? i4.checkable=false : i4.checkable=true;
            }
        }
        SwitchEx{
            id: i3
            texts: "区分大小写"
            height: 60
            bgColor: i1.bgColor
            anchors.top: i2.bottom
            indiBgColor: "#f0ffff"
            indiCheckedColor: "#4b6876"
            indiUncheckedColor: "#404040"
            borderBottom: true
            borderBottomColor: "#f0ffff"
            onCheckedChanged: {
                root.importer.setFindCaseSen(checked);
                checked || i2.checked ? i4.checkable=false : i4.checkable=true;
            }
        }
        SwitchEx{
            id: i4
            texts: "正则表达式"
            height: 60
            bgColor: i1.bgColor
            anchors.top: i3.bottom
            indiBgColor: "#f0ffff"
            indiCheckedColor: "#4b6876"
            indiUncheckedColor: "#404040"
            borderBottom: true
            borderBottomColor: "#f0ffff"
            onCheckedChanged: {
                root.importer.setFindUseReg(checked);
                if(checked){
                    i2.checkable=false;
                    i3.checkable=false;
                }
                else{
                    i2.checkable=true;
                    i3.checkable=true;
                }
            }
        }
        MenuItemEx{
            id: i5
            texts: "退出搜索"
            textsLeftMargin: 10
            iconUnchecked: "qrc:/androidsrc/close.png"
            borderBottom: false
            anchors.top: i4.bottom
            onClicked: {
                findKeyRect.visible=false;
                showAccountAllBtnEx.visible = true;
                saveKeysBtnex.visible = true;
                delKeysBtnex.visible = true;
                findKeyBtnex.visible = true;
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
            model: KeyModel {}
            delegate: keyDelegate
            highlight: Rectangle {
                color: "#40403d";
            }
            highlightFollowsCurrentItem:true
            highlightMoveDuration: 70
            focus: true
            displayMarginBeginning:0
            // NOTE: 超出部分自动隐藏
            clip:true
            spacing: 5

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
                            id: rrect
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
                                    Layout.preferredWidth: rrect.width
                                    font.pixelSize: 16
                                    color: keysView.currentIndex==index ? "#f0ffff" : "#f0ffff"
                                    clip: true
                                    font.bold: true
                                }
                                Text {
                                    id: accountText
                                    text: '<b>账户: ' + showAccountBtnEx.checked ? hides : account + '</b>'
                                    Layout.preferredWidth: rrect.width
                                    font.pixelSize: 15
                                    color: keysView.currentIndex==index ? "#f0ffff" : "#f0ffff"
                                    clip: true
                                    font.bold: true
                                }
                                Text {
                                    id: passwordText
                                    text: '<b>密码: ' + showAccountBtnEx.checked ? hides : password + '</b>'
                                    Layout.preferredWidth: rrect.width
                                    font.pixelSize: 15
                                    color: keysView.currentIndex==index ? "#f0ffff" : "#f0ffff"
                                    clip: true
                                    font.bold: true
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
            onCurrentIndexChanged: {
                root.importer.updateFindPos(currentIndex);
            }
         }
    }
    Component.onCompleted: {
        keysView.currentIndex = -1;
    }

    function adaptKeyBeforeSync(key){
         return JSON.parse(JSON.stringify(key).replace(/}$/g, ",\"keyChecked\":false,\"showAccount\":false,\"showPassword\":false}"));
    }

    function syncKeysFromJson(keys){
        console.log(keys.time, keys.keys_count);
        var mod = keysView.model;
        var keys_data = keys.keys_data;
        var keys_count = keys.keys_count;

        for(var i=0; i<keys_count; i++){
            mod.append(adaptKeyBeforeSync(keys_data[i]));
        }
    }

    function freezeFind(){
//        findBackwardBtnex.enabled = false;
//        findForwardBtnex.enabled = false;
//        closeFindBtnex.enabled = false;
//        findKeyMenu.enabled = false;
    }

    Connections{
        target: mainTabKeys
        onSaveKeys:{
            root.importer.saveKeys();
        }
        onFreezeFindSig:{
            mainTabKeys.freezeFind();
        }
    }
    Connections{
        target: root.importer
        onFindKeyAt:{
            keysView.currentIndex = pos;
        }
        onUnfreezeFind:{
            findBackwardBtnex.enabled = true;
            findForwardBtnex.enabled = true;
            closeFindBtnex.enabled = true;
            findKeyMenu.enabled = true;
        }
    }
}
