import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQml 2.12
import "QuickItem"

Item {
    property alias viewer: homeTab.viewer
    property QtObject root
    property color bgColor: "transparent"
    id: self
    Rectangle{
        anchors.fill: parent
        color: bgColor
    }

    ColumnLayout{
        id: mainColLayout
        width: self.parent.width
        height: self.parent.height
        anchors.fill: parent
        spacing: 0

        MainTopRect{
            id: mainTopRect
            root: root
        }

        StackLayout {
             id: mainTabBarContent
             currentIndex: mainTabBar.currentIndex
             TabViewTabKeys{
                 id: homeTab
                 bgColor:"#333333"
                 root: self.root
             }
             TabViewTabAddKey {
                id: addTab
                bgColor:"#333333"
                root: self.root
             }
             TabViewTab3 {
                 id: activityTab
                 bgColor:"#333333"
             }
         }

        TabBar {
            id:mainTabBar
            width: mainColLayout.width
            height: 70
            // NOTE: 删除tabButton之间的间隙，防止长度取/3除不尽时宽度大小变化
            spacing:0
            TabButtonEx {
                id: tabButton1
                width: mainTabBar.width/3
                height: mainTabBar.height
                texts: "管理"
                iconChecked: "qrc:/androidsrc/manage_reverse.png"
                iconUnchecked: "qrc:/androidsrc/manage.png"
            }
            TabButtonEx {
                id: tabButton2
                width: mainTabBar.width/3
                height: mainTabBar.height
                texts: "添加"
                iconChecked: "qrc:/androidsrc/addKey_reverse.png"
                iconUnchecked: "qrc:/androidsrc/addKey.png"
            }

            TabButtonEx {
                id: tabButton3
                width: mainTabBar.width/3
                height: mainTabBar.height
                texts: "备用"
                iconChecked: "qrc:/androidsrc/about_reverse.png"
                iconUnchecked: "qrc:/androidsrc/about.png"
            }
        }

    }

    function syncKeysFromJson(keys){
        homeTab.syncKeysFromJson(keys);
    }

}
