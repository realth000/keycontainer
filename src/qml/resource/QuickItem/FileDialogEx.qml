import QtQml 2.12
import QtQuick 2.12
import QtQuick.Layouts 1.12
import Qt.labs.folderlistmodel 2.12

Rectangle {
    id: mainDialog
    enum WorkMode {
        SelectDirOnly = 0,
        SelectFileAndDir
    }

    property int workMode: FileDialogEx.WorkMode.SelectFileAndDir
    readonly property bool dirsFirst: true
    property int fontSize: 18
    property ButtonEx selectedFileButtonEx
    property string selectedFilePath

    signal changeSelectedDir(string selectedPath)

    anchors.fill: parent
    visible: false
    color: "transparent"

    FolderListModel {
        property alias folder: model.folder
        id: model
        showFiles: switch (workMode) {
                   case FileDialogEx.WorkMode.SelectDirOnly:
                       return false
                   case FileDialogEx.WorkMode.SelectFileAndDir:
                       return true
                   default:
                       return false
                   }

        showDirs: true
        showDirsFirst: dirsFirst
        showDotAndDotDot: false
        showHidden: false
        folder: "file:///storage/emulated/0"
    }
    Rectangle {
        id: tRect
        height: 60
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: upDirRect.left
        Rectangle {
            id: titleRect
            width: parent.width
            height: 30
            anchors.left: parent.left
            anchors.top: parent.top
            color: "#202020"
            ButtonEx {
                anchors.fill: parent
                checkable: false
                useIcon: false
                posToLeft: true
                texts: "选择目录"
                textsSize: 18
                textsLeftMargin: 10
                enablePressWave: false
            }
        }
        Rectangle {
            id: tipRect
            width: parent.width
            height: 30
            anchors.left: parent.left
            anchors.top: titleRect.bottom
            color: "transparent"
            ButtonEx {
                anchors.fill: parent
                checkable: false
                useIcon: false
                posToLeft: true
                texts: model.folder.toString().replace("/storage/emulated/0", "/~").replace("file://", "").replace(new RegExp("/",'g'), "►")
                textsSize: fontSize
                borderBottom: true
                textsLeftMargin: 10
                borderBottomMargin: 0
                enablePressWave: false
            }
        }
    }

    Rectangle {
        id: upDirRect
        width: tRect.height
        height: tRect.height
        anchors.top: parent.top
        anchors.right: parent.right
        color: "transparent"
        ButtonEx {
            width: parent.height
            height: parent.height
            checkable: false
            useTexts: false
            posToLeft: false
            leftMargin: 0
            iconUnchecked: "qrc:/pic/arrow_up_30.png"
            onClicked: {
                let coparentPath = model.parentFolder.toString()
                if(coparentPath === "file:///" || coparentPath === "" || coparentPath === "file:///storage/emulated"){
                    return;
                }
                model.folder = model.parentFolder;
                console.log("go to parent:", model.folder)
            }
        }
    }
    Rectangle {
        id: liseViewRect
        width: parent.width
        anchors.top: tRect.bottom
        anchors.left: parent.left
        anchors.bottom: okBtnEx.top
        color: "#282828"
        ListView {
            id: view
            model: model
            delegate: delegate
            clip: true
            highlight: Rectangle{}
            focus: false
            anchors.fill: parent

            Component {
                id: delegate
                Item {
                    id: item
                    width: view.width
                    height: 40
                    ButtonEx {
                        id: itemButtonEx
                        property int index: 0
                        width: item.width
                        height: item.height
                        useIcon: false
                        checkable: false
                        posToLeft: true
                        bgSelectedColor: "transparent"
                        bgColor: model.filePath === selectedFilePath ? "#40403d" : "#282828"
                        texts: model.fileName
                        textsSize: fontSize
                        textsUncheckedColor: "#f0ffff"
                        textsLeftMargin: 15
                        borderBottom: true
                        iconUnchecked: model.fileIsDir ? "qrc:/pic/folder.png" : "qrc:/pic/file.png"
                        iconPos: ButtonEx.IconPos.IconLeft
                        leftMargin: borderBottomMargin
                        enablePressWave: false
                        onClicked: {
                            if(model.fileIsDir) {
                                gotoFolder(model.filePath)
                            }
                            else{
                                selectedFileButtonEx = itemButtonEx
                                selectedFilePath = model.filePath
                                console.log("now select file", selectedFilePath,model.filePath === selectedFilePath )
                            }
                        }
                    }
                }
            }
        }
    }
    ButtonEx {
        id: okBtnEx
        checkable: false
        iconUnchecked: "qrc:/pic/yes.png"
        iconPos: ButtonEx.IconPos.IconLeft
        texts: "确定"      
        textsSize: 20
        textsUncheckedColor: "#f0ffff"
        width: parent.width/2
        height: 50
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        bgColor: "#282828"
        onClicked: {
            var d
            switch (workMode) {
            case FileDialogEx.WorkMode.SelectDirOnly:
                d = model.folder
                break
            case FileDialogEx.WorkMode.SelectFileAndDir:
                d = selectedFilePath
                break
            default:
                d = selectedFilePath
            }
            if(Qt.platform.os == "windows") {
                mainDialog.changeSelectedDir(d.toString().replace("file:///", ""));
            }
            else{
                mainDialog.changeSelectedDir(d.toString().replace("file:///", "/"));
            }
            mainDialog.close();
        }
    }
    ButtonEx {
        id: cancelBtnEx
        checkable: false
        iconUnchecked: "qrc:/pic/no.png"
        iconPos: ButtonEx.IconPos.IconLeft
        texts: "取消"
        textsSize: 20
        textsUncheckedColor: "#f0ffff"
        width: okBtnEx.width
        height: okBtnEx.height
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        bgColor: "#282828"
        onClicked: {
            mainDialog.close();
        }
    }

    onVisibleChanged: {
        if(visible){
            console.log(model.parentFolder);
            selectedFilePath = null
            selectedFileButtonEx = null
        }
    }

    function initModel(){

    }

    function uninitModel(){

    }

    function open() {
        mainDialog.initModel();
        if(model.status === Component.Ready){
            view.model=model;
        }
        // 这里加上..
        mainDialog.visible=true;
    }
    function close() {
        mainDialog.visible=false;
        // NOTE: 设置null后再次打开还是null
//        view.model=null;
        mainDialog.uninitModel();
    }

    /*
     * model.folder must start with "file:///"
     * On Android(also linux), path starts with /storage/emulated/0(also /)
     * On Windows, path starts with c:/ or d:/ ...
     * So:
     *   On Android and linux, model.folder = "file://" + folderPath
     *   On Windows, model.folder = "file:///" + folderPath
     */
    function gotoFolder(folderPath) {
        model.folder = ("file:///" + folderPath).replace("file:////", "file:///")
        console.log("folder path =", folderPath);
    }
}
