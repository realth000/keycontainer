import QtQuick 2.4
import QtQuick.Window 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.12
import "QuickItem"
import TH.QmlImporter 1.0
import QtQml 2.12

ApplicationWindow  {
    property alias importer: mainQmlImporter
    property alias viewer: p1.viewer
    id: mainWindow
    visible: true
    width: usingWidth
    height: usingHeight

    // 背景默认颜色
    property color backgroundColorBegin: "#232323"
    property color backgroundColorEnd: "#333333"
    // tabButton的文字的颜色
    property color mainTabViewTabTextColor: "#f0ffff"
    property color mainTabViewTabTextSelectedColor: "#28292a"
    property color tabButtonSelectedColor: "#969999"
    // tabButton图标的高度
    property int tabBarIconHeight: 30
    // tabButton内部图标和文字之间的间距
    property int tabButtonIconTextPadding: 3

    // 窗口大小
    property int usingWidth: 354
    property int usingHeight: 700

    signal keysString(string keys)

    StackLayout{
        id: swe
        currentIndex: mainDrawer.currentIndex
        anchors.fill: parent
        Label{
            text: "\n\n\n\n      0"
            font.pixelSize: 50
        }
        MainDrawerPageMain{
            id: p1
            root: mainWindow
        }
        Label{
            text: "\n\n\n\n      2"
            font.pixelSize: 50
        }
        Label{
            text: "\n\n\n\n      3"
            font.pixelSize: 50
        }
        MainDrawerPageSettings{
            id: p4
            root: mainWindow
        }
        Label{
            text: "\n\n\n\n      5"
            font.pixelSize: 50
        }
    }

    Drawer{
        parent: p1.mainColLayout
        id: mainDrawer
        width: 0.66 * mainWindow.width
        height: mainWindow.height
        edge: Qt.LeftEdge
        // FIXME: 设置dragMargin将导致margin宽度的区域内drawer下层其他控件（一般视drawer在最顶层，因此指区域内所有其他控件）无法响应MouseEvent
        // 在Event机制中，似乎是drawer优先捕获事件，导致其他控件无法接受事件，无论这个事件是否是拖拽打开drawer
        // 目前bug尚未修复
        // 详情：https://bugreports.qt.io/browse/QTBUG-59141
//        dragMargin: mainWindow.width

        property int currentIndex: 1

        background: Rectangle{
            color: "#2d2f32"
        }

        ColumnLayout{
            id: mainDrawerBEs
            width: mainDrawer.width
            spacing: 0

            ButtonEx{
                id: mainDrawerLogoRect
                checkable: false
                texts: "KeyContainer"
                textsSize: 18
                textsUncheckedColor: "#f0ffff"
                textsBold: true
                bgColor: "#2a2b2c"
                iconChecked: "qrc:/androidsrc/safe_reverse.png"
                iconUnchecked: "qrc:/androidsrc/safe_reverse.png"
                iconWidth: 60
                iconHeight: 60
                iconAntialiasing: true
                // NOTE: 在layout中要用Layout.preferredWidth和Height
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: parent.width*0.75
                Layout.alignment: Qt.AlignCenter

            }
            ButtonEx{
                id: mainDrawerBE1
                bgColor: "#2d2f32"
                iconPos: 1
                texts: "   主页      "
                iconChecked: "qrc:/androidsrc/home_reverse.png"
                iconUnchecked: "qrc:/androidsrc/home.png"
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 50
                checked: true
                onClicked: {
                    mainDrawer.currentIndex=1
                    mainDrawer.close()
                }
            }
            ButtonEx{
                id: mainDrawerBE2
                bgColor: "#2d2f32"
                iconPos: 1
                texts: "   导出      "
                iconChecked: "qrc:/androidsrc/exportKey_reverse.png"
                iconUnchecked: "qrc:/androidsrc/exportKey.png"
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 50
                onClicked: {
                    mainDrawer.currentIndex=2
                    mainDrawer.close()
                }
            }
            ButtonEx{
                id: mainDrawerBE3
                bgColor: "#2d2f32"
                iconPos: 1
                texts: "   导入      "
                iconChecked: "qrc:/androidsrc/importKeys_reverse.png"
                iconUnchecked: "qrc:/androidsrc/importKeys.png"
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 50
                onClicked: {
                    mainDrawer.currentIndex=3
                    mainDrawer.close()
                }
            }
            ButtonEx{
                id: mainDrawerBE4
                bgColor: "#2d2f32"
                iconPos: 1
                texts: "   设置      "
                iconChecked: "qrc:/androidsrc/config_reverse.png"
                iconUnchecked: "qrc:/androidsrc/config.png"
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 50
                onClicked: {
                    mainDrawer.currentIndex=4
                    mainDrawer.close()
                }
            }
            ButtonEx{
                id: mainDrawerBE5
                bgColor: "#2d2f32"
                iconPos: 1
                texts: "   关于      "
                iconChecked: "qrc:/androidsrc/about_reverse.png"
                iconUnchecked: "qrc:/androidsrc/about.png"
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 50
                onClicked: {
                    mainDrawer.currentIndex=5
                    mainDrawer.close()
                }
            }
       }
        ButtonGroup{
            id: mainDrawerBG
            buttons: mainDrawerBEs.children
        }
    }

    QmlImporter{
        id: mainQmlImporter
        onQml_msg_info: {
            console.log(msg)
        }
        onSendKeys: {
            var obj = JSON.parse(keysJsonString);
            p1.syncKeysFromJson(obj);
        }
    }

    Component.onCompleted: {
        mainQmlImporter.initImporter();
    }
}
