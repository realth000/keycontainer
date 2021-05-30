import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQml 2.12

Rectangle{
    property QtObject root
    width: 354
    height: 60
    color:"#141414"

    ButtonEx{
        id: tabButton23
        width: 60
        height: 60
        iconWidth: 30
        iconHeight: 30
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.verticalCenter: parent.verticalCenter
        checkable: false
        useTexts: false
        bgColor: "#141414"
        borderColor: "#141414"
        iconUnchecked: "qrc:/androidsrc/menu.png"
        onClicked: {
            mainDrawer.open()
        }
    }
   /* Label{
        id: titleLabel
        width: 200
        height: 60
        anchors.left: tabButton23.right
        anchors.verticalCenter: parent.verticalCenter
        Text{
            text:"KeyContainer"
            color: "#4b6876"
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            font.pixelSize: 23
        }
        Rectangle{
            anchors.fill: parent
            color: "transparent"
        }
    }*/
    ButtonEx{
        id: lockBtnEx
        width: 60
        height: 60
        iconWidth: 30
        iconHeight: 30
        anchors.right: optsBtnEx.left
        anchors.verticalCenter: parent.verticalCenter
        checkable: false
        useTexts: false
        bgColor: "#141414"
        borderColor: "#141414"
        iconUnchecked: "qrc:/androidsrc/lock.png"
        onClicked: {
            Qt.quit()
        }
    }
    ButtonEx{
        id: optsBtnEx
        width: 60
        height: 60
        iconWidth: 30
        iconHeight: 30
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        checkable: false
        useTexts: false
        bgColor: "#141414"
        borderColor: "#141414"
        iconUnchecked: "qrc:/androidsrc/opts.png"
    }
}
