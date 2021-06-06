import QtQuick 2.12
import Qt.labs.folderlistmodel 2.12
import QtQml 2.12

Rectangle {
    id: mainDialog
    property bool files: false
    property bool dirs: true
    readonly property bool dirsFirst: true
//    signal changeCurrentDir(string dir)
    signal changeSelectedDir(string newDir)
    anchors.fill: parent
    visible: false
    color: "transparent"

    FolderListModel{
        property alias folder: model.folder
        id: model
        showFiles: files
        showDirs: dirs
        showDirsFirst: dirsFirst
        showDotAndDotDot: false
        showHidden: false
        folder: "file:///storage/emulated/0"
    }
    Rectangle{
        id: tRect
        height: 60
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: upDirRect.left
        Rectangle{
            id: titleRect
            width: parent.width
            height: 30
            anchors.left: parent.left
            anchors.top: parent.top
            color: "#202020"
            ButtonEx{
                anchors.fill: parent
                checkable: false
                useIcon: false
                posToLeft: true
                texts: "选择目录"
                textsSize: 18
                textsLeftMargin: 10
            }
        }
        Rectangle{
            id: tipRect
            width: parent.width
            height: 30
            anchors.left: parent.left
            anchors.top: titleRect.bottom
            color: "transparent"
            ButtonEx{
                anchors.fill: parent
                checkable: false
                useIcon: false
                posToLeft: true
                texts: model.folder.toString().replace("/storage/emulated/0", "/~").replace("file://", "").replace(new RegExp("/",'g'), "►")
                textsSize: 17
                borderBottom: true
                textsLeftMargin: 10
            }
        }
    }

    Rectangle{
        id: upDirRect
        width: tRect.height
        height: tRect.height
        anchors.top: parent.top
        anchors.right: parent.right
        color: "transparent"
        ButtonEx{
            width: parent.height
            height: parent.height
            checkable: false
            useTexts: false
            posToLeft: true
            leftMargin: 0
            iconUnchecked: "qrc:/androidsrc/arrow_up_30.png"
            onClicked: {
                if(model.parentFolder === "file:///"){
                    return;
                }

                model.folder = model.parentFolder;
                console.log("FileDialogEx:: Move to parent folder:", model.parentFolder);
            }
        }
    }
    Rectangle{
        id: liseViewRect
        width: parent.width
        anchors.top: tRect.bottom
        anchors.left: parent.left
        anchors.bottom: okBtnEx.top
        color: "#282828"
        ListView{
            id: view
            model: model
            delegate: delegate
            clip: true
            highlight: Rectangle{}
            focus: false
            anchors.fill: parent

            Component{
                id: delegate
                Item{
                    id: item
                    width: view.width
                    height: 40
                    ButtonEx{
                        property int index: 0
                        width: parent.width
                        height: parent.height
                        anchors.fill: parent
                        useIcon: false
                        checkable: false
                        posToLeft: true
                        bgSelectedColor: "transparent"
                        bgColor: "#282828"
                        texts: model.fileName
                        textsSize: 18
                        textsUncheckedColor: "#f0ffff"
                        textsLeftMargin: 15
                        borderBottom: true
                        onClicked: {
                            gotoFolder(model.fileName);
                        }
                    }
                }
            }
        }
    }
    ButtonEx{
        id: okBtnEx
        checkable: false
        useIcon: false
        texts: "确定"
        textsSize: 20
        width: parent.width/2
        height: 60
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        bgColor: "#232323"
        onClicked: {
            var d = model.folder;
            mainDialog.changeSelectedDir(d.toString().replace("file://", ""));
            mainDialog.close();
        }
    }
    ButtonEx{
        id: cancelBtnEx
        checkable: false
        useIcon: false
        texts: "取消"
        textsSize: 20
        width: okBtnEx.width
        height: okBtnEx.height
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        bgColor: "#232323"
        onClicked: {
            mainDialog.close();
        }
    }

    onVisibleChanged: {
        if(visible){
            console.log(model.parentFolder);
        }
    }

    function initModel(){

    }

    function uninitModel(){

    }

    function open(){
        mainDialog.initModel();
        if(model.status === Component.Ready){
            view.model=model;
        }
        // 这里加上..
        mainDialog.visible=true;
    }
    function close(){
        mainDialog.visible=false;
        // NOTE: 设置null后再次打开还是null
//        view.model=null;
        mainDialog.uninitModel();
    }
    function gotoFolder(name){
        model.folder += "/" + name
        console.log("folder=", model.folder)
    }
}
