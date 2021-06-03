import QtQuick 2.12
import Qt.labs.folderlistmodel 2.12
import QtQml 2.12

Rectangle {
    id: mainDialog
    property string currentDir
    property string selectedDir
    property bool files: true
    property bool dirs: true
    readonly property bool dirsFirst: true
    signal changeCurrentDir(string dir)
    signal changeSelectedDir(string newDir)
    anchors.fill: parent
    visible: false
    color: "transparent"

    FolderListModel{
        id: model
        showFiles: files
        showDirs: dirs
        showDirsFirst: dirsFirst
        showDotAndDotDot: false
        showHidden: false
        folder: "file://" + currentDir
    }
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
            texts: currentDir
            textsSize: 17
            borderBottom: true
            textsLeftMargin: 10
        }
    }
    Rectangle{
        id: liseViewRect
        width: parent.width
        anchors.top: tipRect.bottom
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
                            mainDialog.changeCurrentDir(model.fileName);
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

    Connections{
        target: mainDialog
        onChangeCurrentDir:{
            currentDir = currentDir + "/" + dir;
        }
    }
    onVisibleChanged: {
        if(visible){
            currentDir = selectedDir;
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
}
