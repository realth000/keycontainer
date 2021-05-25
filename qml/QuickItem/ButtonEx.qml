import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQml 2.12

Button{
    id: self
    QtObject{
        enum IconPos{
            IconAbove = 0x00000000,
            IconLeft = 0x00000001
        }
    }
    property bool thisIsButtonEx: true
    property color bgSelectedColor: "#40403d"
    property color bgColor: "#232323"
//    property color bgColorEnd: "#333333"
    property int borderWidth: 0
    property color borderColor: "#375564"
//    property color textsCheckedColor: "#28292a"
    property color textsCheckedColor: "#f0ffff"
    property color textsUncheckedColor: "#4b6876"
    property bool textsBold: false
    property string iconChecked
    property string iconUnchecked
    property int iconWidth: 30
    property int iconHeight: 30
    property string texts
    property int textPadding: 3
    property bool useTexts: true
    property int iconPos: 0
    property int textsSize: 13
    property bool iconAntialiasing: false
    checkable: true

    // 这个方框是辅助定位tabButton中图标和文字的，由于anchors的值只能定义为anchors的值，因此如果想把图标和
    // 文字的整体的中心定位在整个tabButton的中心，需要这样一个装着图标和文字的方框来辅助定位
    Rectangle{
        id: assistant1
        // 方框的大小保证刚刚好装满
//        width: useTexts ? Math.max(image1.width, text1.contentWidth) : image1.width
        width: if(useTexts){
                   if(iconPos==0){
                       Math.max(image1.width, text1.contentWidth)
                   }
                   else{
                       image1.width + text1.contentWidth
                   }
               }
               else{
                   image1.width
               }

//        height: useTexts ? image1.height + text1.contentHeight + textPadding : image1.height
        height: if(useTexts){
                    if(iconPos==0){
                        image1.height + text1.contentHeight
                    }
                    else{
                        Math.max(image1.height, text1.contentHeight)
                    }
                }
                else{
                    image1.height
                }

        color: "transparent"
        border.width: 0
        anchors.horizontalCenter: iconPos==0 ? parent.horizontalCenter : parent.horizontalCenter
        anchors.verticalCenter: self.verticalCenter
    }

    Image{
        id: image1
//        source: self.checked ? iconChecked : iconUnchecked
        source: self.checked ? iconChecked :iconUnchecked
        sourceSize.width: 60
        sourceSize.height: 60
        width: iconWidth
        height: iconHeight
        // 图标顶着方框上边沿中间
        anchors.horizontalCenter: iconPos==0 ? assistant1.horizontalCenter : assistant1.left
        anchors.horizontalCenterOffset: iconPos==0 ? 0 : iconWidth/2
        anchors.top: iconPos==0 ? assistant1.top : assistant1.top
        antialiasing: iconAntialiasing
    }
    // NOTE: 使用contentItem: Text会导致icon无法显示
    Text {
        id: text1
        text: qsTr(texts)
        opacity: enabled ? 1.0 : 0.3
        color: self.checkable && self.checked ? textsCheckedColor : textsUncheckedColor
//        color: textsCheckedColor
        // 文字顶着方框下边沿中间，图标和文字间的padding设置在方框的高度中了
        anchors.right: iconPos==0 ? assistant1.right : assistant1.right
        anchors.verticalCenter: iconPos==0 ? assistant1.bottom : assistant1.verticalCenter
        anchors.verticalCenterOffset:iconPos==0 ? -textsSize/2 :0
        elide: Text.ElideRight
        visible: useTexts ? true : false
        font.pixelSize: textsSize
        font.bold: textsBold
    }
    background: Rectangle{
        width: Math.max(self.width, assistant1.width)
        height: Math.max(self.height, assistant1.height)
        color: self.checked ? bgSelectedColor : bgColor
        radius: 0
        border.width: borderWidth
        border.color: borderColor
    }
}
