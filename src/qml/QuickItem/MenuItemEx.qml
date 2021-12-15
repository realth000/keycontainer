import QtQuick 2.0

ButtonEx {
    bgColor: "#375564"
    textsUncheckedColor: "#f0ffff"
    borderBottom: true
    borderBottomColor: "#f0ffff"
    iconPos: 1
    posToLeft: true
    leftMargin: 15
    anchors.left: parent.left
    anchors.right: parent.right
    height: 60
    checkable: false
    onClicked: {
        parent.visible = false;
    }
}
