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
                    id: b1
                    texts: "保存路径"
                    textsSize: 15
                    useDoubleTexts: true
                    dtests: savePath
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
                    anchors.top: changeInitKeyBtn.bottom
                    onClicked: {
                        aesKeyState = "设置中……";
                        root.importer.changeAESKey();
                    }
                }
                Switch{
                    id: autoChangeAESChB
                    anchors.top: changeAESKeyBtn.bottom
                    anchors.left: parent.left
                    width: parent.width
                    height: 50
                    checked: autoChangeAES
                    background: Rectangle{
                        color: "transparent"
                    }
                    indicator: Rectangle{
                        id: indi
                        implicitWidth: 40
                        implicitHeight: 20
                        radius: 10
                        x: autoChangeAESChB.leftPadding
                        y: parent.height / 2 - height / 2
                        color: autoChangeAESChB.checked ? "#375564" : "transparent"
                        border.color: "#4b6876"
                        Rectangle {
                            x: autoChangeAESChB.checked ? parent.width - width : 0
                            width: 20
                            height: 20
                            radius: 10
                            color: "#4b6876"
                        }
                    }
                    contentItem: Text{
                        text: "每次保存自动更新加密密钥"
                        anchors.left: indi.right
                        anchors.leftMargin: 7
                        anchors.verticalCenter: parent.verticalCenter
                        verticalAlignment: Qt.AlignVCenter
                        font.pixelSize: 15
                        font.bold: true
                        color: "#4b6876"
                    }
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
         selectedDir: workPath
         onChangeSelectedDir: {
             selectedDir = newDir;
             self.savePath = newDir;
             root.importer.setSavePath(newDir);
             console.log("current dir set to", currentDir);
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
