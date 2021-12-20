import QtQuick 2.10
import QtQuick.Controls 2.10

TabButton {
    id: self
//    property color bgSelectedColor: "#969999"
    property color bgSelectedColor: "#232323"
//    property color bgSelectedColor: "#40403d"
    property color bgColorBegin: "#232323"
    property color bgColorEnd: "#333333"
//    property color textsCheckedColor: "#28292a"
//    property color textsCheckedColor: "#8b98b6"
    property color textsCheckedColor: "#f0ffff"
    property color textsUncheckedColor: "#336666"
    property string iconChecked
    property string iconUnchecked
    property int iconHeight: 30
    property string texts
    property int textPadding: 3
    property int textsSize: 15
    property bool textsBold: true
    checkable: true

    // 这个方框是辅助定位tabButton中图标和文字的，由于anchors的值只能定义为anchors的值，因此如果想把图标和
    // 文字的整体的中心定位在整个tabButton的中心，需要这样一个装着图标和文字的方框来辅助定位
    Rectangle {
        id: assistant1
        // 方框的大小保证刚刚好装满
        width: Math.max(image1.width, text1.contentWidth)
        height: image1.height + text1.contentHeight + textPadding
        color: "transparent"
        border.width: 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    Image {
        id: image1
        source: self.checked ? iconChecked : iconUnchecked
        sourceSize.width: 60
        sourceSize.height: 60
        width: 30
        height: iconHeight
        // 图标顶着方框上边沿中间
        anchors.horizontalCenter: assistant1.horizontalCenter
        anchors.verticalCenter: assistant1.verticalCenter
        anchors.verticalCenterOffset: self.checked ? -assistant1.height/4 : 0
    }
    // NOTE: 使用contentItem: Text会导致icon无法显示
    Text {
        id: text1
        text: qsTr(texts)
        visible: self.checked ? true : false
        opacity: enabled ? 1.0 : 0.3
        color: self.checked ? textsCheckedColor : textsUncheckedColor
        // 文字顶着方框下边沿中间，图标和文字间的padding设置在方框的高度中了
        anchors.horizontalCenter: assistant1.horizontalCenter
        anchors.bottom: assistant1.bottom
        elide: Text.ElideRight
        font.pixelSize: textsSize
        font.bold: textsBold
    }
    background: Rectangle {
//        implicitWidth: self.parent.width
        implicitHeight: self.parent.height
        color: self.checked ? bgSelectedColor : bgColorBegin
        radius: 0
        border.width: 0
    }
}
