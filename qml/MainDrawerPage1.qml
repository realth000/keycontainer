import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import "QuickItem"

Item {
    id: self
    ColumnLayout{
        id: mainColLayout
        width: self.parent.width
        height: self.parent.height
        anchors.fill: parent
        spacing: 0
        Rectangle{
            id: mainTopRect
            Layout.preferredWidth: self.parent.width
            Layout.preferredHeight: 60
            color:"#141414"

            ButtonEx{
                id: tabButton23
                width: 60
                height: 60
                checkable: false
                useTexts: false
                bgColor: "#141414"
                borderColor: "#141414"
                iconChecked: "qrc:/androidsrc/menu.png"
                iconUnchecked: "qrc:/androidsrc/menu.png"
                onClicked: {
                    mainDrawer.open()
                }
            }
        }

        StackLayout {
             id: mainTabBarContent
             currentIndex: mainTabBar.currentIndex
             TabViewTabKeys{
                 id: homeTab
                 bgColor:"#333333"
             }
             TabViewTab2 {
                id: discoverTab
                bgColor:"#333333"
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
                iconChecked: "qrc:/androidsrc/config_reverse.png"
                iconUnchecked: "qrc:/androidsrc/config.png"
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

}
