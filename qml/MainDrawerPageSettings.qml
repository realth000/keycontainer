import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQml 2.12
import "QuickItem"

Item {
    property QtObject root

    MainTopRect{
        id: mainTopRect
        anchors.top: parent.top
        anchors.right: parent.right
        root: root
    }
    Rectangle{
        id: mainRect
        width: root.width
        anchors.top: mainTopRect.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        color: "#282828"
        GroupBox{
            id: saveGB
            width: parent.width
            height: 150
            anchors.top: mainRect.top
            anchors.left: mainRect.left
            anchors.right: mainRect.right
            title:"asdsadasd"
            label: Label{
                width: 200
                height: 40
                color: "red"
//                Text{
////                    text: "保存"
//                    color: "red"
//                    font.bold: true
//                    font.pixelSize: 15
//                }
            }
        }
        GroupBox{
            id: startupGB
            height: 150
            anchors.top: saveGB.bottom
            anchors.left: mainRect.left
            anchors.right: mainRect.right
            title: "启动"


        }
        GroupBox{
            id: securityGB
            height: 150
            anchors.top: startupGB.bottom
            anchors.left: mainRect.left
            anchors.right: mainRect.right
            title:"安全"
        }
    }
}
