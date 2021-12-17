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
            Layout.fillWidth: true
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
                texts: "查看"
                iconChecked: "qrc:/pic/viewKey_reverse.png"
                iconUnchecked: "qrc:/pic/viewKey.png"
            }
            TabButtonEx {
                id: tabButton2
                width: mainTabBar.width/3
                height: mainTabBar.height
                texts: "添加"
                iconChecked: "qrc:/pic/addKey_reverse.png"
                iconUnchecked: "qrc:/pic/addKey2.png"
            }

            TabButtonEx {
                id: tabButton3
                width: mainTabBar.width/3
                height: mainTabBar.height
                texts: "备用"
                iconChecked: "qrc:/pic/about_reverse.png"
                iconUnchecked: "qrc:/pic/about2.png"
            }
        }

    }

    function syncKeysFromJson(keys){
        homeTab.syncKeysFromJson(keys);
    }

}
