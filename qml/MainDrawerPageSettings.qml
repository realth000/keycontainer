import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import QtQml 2.12
import "QuickItem"

Item {
    property QtObject root
    property string workPath: ""
    property string savePath: "/storage/emulated/0/KeyContainer"
    property string backupPath: "/storage/emulated/0/KeyContainer"
    property bool autoChangeAES: false
    property string aesKeyState: "已设置"
    property color changeInitKeyHintColor: "#f0ffff"
    id: self
    MainTopRect{
        id: mainTopRect
        anchors.top: parent.top
        anchors.right: parent.right
        width: parent.width
    }
    ScrollView{
        id: mainRect
        width: root.width
        anchors.top: mainTopRect.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        background: Rectangle{
            color: "#282828"
        }
        GroupBoxEx{
            id: saveGB
            height: 100
            iconPath: "qrc:/androidsrc/saveKey.png"
            labelText:"保存"
            Rectangle{
                id: saveBox
                anchors.top: parent.top
                anchors.topMargin: 65
                anchors.left: parent.left
                anchors.leftMargin: saveGB.separatorLeftMargin
                anchors.right: parent.right
                anchors.rightMargin: saveGB.separatorRightMargin
                height: 100
                color: "transparent"
                ButtonEx{
                    id: savePathBtnex
                    texts: "保存路径"
                    textsSize: 15
                    useDoubleTexts: true
                    dtests: savePath.replace("/storage/emulated/0", "")
                    iconPos: 1
                    bgColor: "transparent"
                    checkable: false
                    height: 50
                    width: parent.width
                    iconUnchecked: "qrc:/androidsrc/saveFileConfig.png"
                    leftMargin: 15
                    onClicked: {
                        fileDialog.open()
                    }
                }
            }
        }

        GroupBoxEx{
            id: defaultGB
            height: 100
            iconPath: "qrc:/androidsrc/manage.png"
            labelText:"安全"
            anchors.top: saveGB.bottom
            anchors.topMargin: 30
            Rectangle{
                id: defaultBox
                anchors.top: parent.top
                anchors.topMargin: 65
                anchors.left: parent.left
                anchors.leftMargin: defaultGB.separatorLeftMargin
                anchors.right: parent.right
                anchors.rightMargin: defaultGB.separatorRightMargin
                height: 200
                color: "transparent"
                ButtonEx{
                    id: changeInitKeyBtn
                    texts: "修改启动密码"
                    useDoubleTexts: true
                    dtests: "已设置"
                    textsSize: 15
                    iconPos: 1
                    bgColor: "transparent"
                    checkable: false
                    height: 50
                    width: defaultBox.width
                    iconUnchecked: "qrc:/androidsrc/key_checked.png"
                    posToLeft: true
                    leftMargin: 15
                    anchors.top: defaultBox.top
                    onClicked: {
                        if(initPwFolder.opened){
                            initPwFolder.close();
                        }
                        else{
                            initPwFolder.open();
                        }
                    }
                    Connections{
                        target: root.importer
                        onChangeInitKey_wrong_oldPw:{
                            changeInitKeyBtn.dtests = "旧密码错误";
                            changeInitKeyBtn.dtextsUncheckedColor = changeInitKeyHintColor;
                        }
                        onChangeInitKey_success:{
                            changeInitKeyBtn.dtests = "已设置";
                            changeInitKeyBtn.dtextsUncheckedColor = changeInitKeyBtn.textsUncheckedColor;
                            oldPwInput.clear();
                            pwInput.clear();
                            pwConfirmInput.clear();
                            changeInitKeyBtn.clicked();
                        }
                    }
                }
                FolderEx{
                    id: initPwFolder
                    trigger: changeInitKeyBtn
                    anchors.top: changeInitKeyBtn.bottom
                    anchors.left: changeInitKeyBtn.left
                    anchors.right: changeInitKeyBtn.right
                    openHeight: childrenRect.height
                    bgColor: "#232323"
                    Rectangle{
                        height: childrenRect.height
                        width: parent.width
                        anchors.top: parent.top
                        anchors.left: parent.left
                        color: "transparent"
                        Rectangle{
                            id: rect1
                            width: Math.max(300, parent.width*0.5)
                            height: 30
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: 20
                            color: "transparent"
                            ButtonEx{
                                id: b1
                                texts: "旧密码"
                                posToLeft: true
                                useIcon: false
                                checkable: false
                                bgColor: "transparent"
                                width: 70
                                height: 30
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextInputEx{
                                id: oldPwInput
                                height: 30
                                anchors.left: b1.right
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                echoMode: "Password"
                            }
                        }
                        Rectangle{
                            id: rect2
                            width: Math.max(300, parent.width*0.5)
                            height: 30
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: rect1.bottom
                            anchors.topMargin: 20
                            color: "transparent"
                            ButtonEx{
                                id: b2
                                texts: "新密码"
                                posToLeft: true
                                useIcon: false
                                checkable: false
                                bgColor: "transparent"
                                width: 70
                                height: 30
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextInputEx{
                                id: pwInput
                                height: 30
                                anchors.left: b2.right
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                echoMode: "Password"
                            }

                        }
                        Rectangle{
                            id: rect3
                            width: Math.max(300, parent.width*0.3)
                            height: 30
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: rect2.bottom
                            anchors.topMargin: 20
                            color: "transparent"
                            ButtonEx{
                                id: b3
                                texts: "确认密码"
                                posToLeft: true
                                useIcon: false
                                bgColor: "transparent"
                                checkable: false
                                width: 70
                                height: 30
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            TextInputEx{
                                id: pwConfirmInput
                                height: 30
                                anchors.left: b3.right
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                echoMode: "Password"
                            }
                        }
                        ButtonEx{
                            id: checkInputInitKeyBtnex
                            texts: "确定"
                            useIcon: false
                            checkable: false
                            width: 70
                            height: 70
                            anchors.top: rect3.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            bottomMargin: 10
                            bgColor: "transparent"
                            textsSize: 17
                            onClicked: {
                                if(oldPwInput.text === ""){
                                    changeInitKeyBtn.dtests = "旧密码为空";
                                    changeInitKeyBtn.dtextsUncheckedColor = changeInitKeyHintColor;
                                    return;
                                }
                                if(pwInput.text === ""){
                                    changeInitKeyBtn.dtests = "新密码为空";
                                    changeInitKeyBtn.dtextsUncheckedColor = changeInitKeyHintColor;
                                    return;
                                }
                                if(pwConfirmInput.text === ""){
                                    changeInitKeyBtn.dtests = "确认密码为空";
                                    changeInitKeyBtn.dtextsUncheckedColor = changeInitKeyHintColor;
                                    return;
                                }

                                if(pwInput.text != pwConfirmInput.text){
                                    changeInitKeyBtn.dtests = "两次新密码不一致";
                                    changeInitKeyBtn.dtextsUncheckedColor = changeInitKeyHintColor;
                                    return;
                                }
                                root.importer.checkInputInitKey(oldPwInput.text, pwConfirmInput.text);
                            }
                        }

                    }
                }

                ButtonEx{
                    id: changeAESKeyBtn
                    texts: "更新加密密密钥"
                    textsSize: 15
                    useDoubleTexts: true
                    dtests: aesKeyState
                    iconPos: 1
                    bgColor: "transparent"
                    checkable: false
                    height: 50
                    width: defaultBox.width
                    iconUnchecked: "qrc:/androidsrc/changeAESKey.png"
                    posToLeft: true
                    leftMargin: 15
                    anchors.top: initPwFolder.bottom
                    onClicked: {
                        aesKeyState = "设置中……";
                        root.importer.changeAESKey();
                    }
                }
                SwitchEx{
                    id: autoChangeAESChB
                    anchors.top: changeAESKeyBtn.bottom
                    anchors.left: parent.left
                    checked: autoChangeAES
                    texts: "每次保存自动更新加密密钥"
                    onCheckedChanged: {
                        autoChangeAES=checked
                        root.importer.setAutoChangeAES(autoChangeAES)
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        workPath = root.importer.getWorkPath();
        autoChangeAES = root.importer.getAutoChangeAES();
        savePath = root.importer.getSavePath();
        backupPath = root.importer.getBackupPath();
    }
    FileDialogEx {
         id: fileDialog
         onChangeSelectedDir: {
             self.savePath = newDir;
             root.importer.setSavePath(newDir);
             console.log("change selectr dir: ", newDir);
         }
     }
    Connections{
        target: root
        onMsgUpdate:{
            if(msg.indexOf("SETTINGS") !== 0){
                return;
            }
            if(msg.indexOf("SETTINGS_AESKEY_UPDATE_FINISH") === 0){
                aesKeyState="设置完成"
            }
        }
    }
}
