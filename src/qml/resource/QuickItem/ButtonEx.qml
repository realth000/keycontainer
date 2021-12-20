import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQml 2.12
import QtGraphicalEffects 1.12

Button{
    id: self
    enum IconPos{
        IconAbove = 0,
        IconLeft = 1
    }
    property bool borderBottom: false
    property color borderBottomColor: "#4b6876"
    property int borderBottomMargin: 15
    property bool thisIsButtonEx: true
//    property color bgSelectedColor: "#40403d"
    property color bgSelectedColor: "#353535"
    property color bgColor: "#232323"
    property int borderWidth: 0
    property color borderColor: "#375564"
    property color textsCheckedColor: "#f0ffff"
    property color textsUncheckedColor: "#4b6876"
    property bool textsBold: true
    property string iconChecked
    property string iconUnchecked
    property int iconWidth: 30
    property int iconHeight: 30
    property string texts
    property bool useTexts: true
    property bool useIcon: true
    property int iconPos: ButtonEx.IconPos.IconAbove
    property int textsSize: 16
    property int textsLeftMargin: 0
    property bool iconAntialiasing: true
    property int horizontalOffset: 0
    property bool posToLeft: false

    property bool useDoubleTexts: false
    property string dtests
    property color dtextsCheckedColor: textsCheckedColor
    property color dtextsUncheckedColor: textsUncheckedColor
    property int dtextSize: 13
    property bool dtextBold: textsBold
    property int dtextTopMargin: 4
    property int dtextLeftMargin: 4
    property int leftMargin: 0
    property int bottomMargin: 0

    property bool enablePressWave: true
    property double pressWaveLeftArea: 0.0
    property real pressWaveRightArea: 0
    property int pressWaveDuration: 300
    property int pressWaveStartPosX: 0
    property bool pressWaveFinish: false

    checkable: true
    clip: true

    // 这个方框是辅助定位tabButton中图标和文字的，由于anchors的值只能定义为anchors的值，因此如果想把图标和
    // 文字的整体的中心定位在整个tabButton的中心，需要这样一个装着图标和文字的方框来辅助定位
    Rectangle{
        id: assistant1
        // 方框的大小保证刚刚好装满
        width: if(useTexts){
                   if(iconPos==0){
                       if(useIcon){
                            Math.max(image1.width, text1.contentWidth)
                       }
                       else{
                           text1.contentWidth
                       }
                   }
                   else{
                       image1.width + text1.contentWidth
                   }
               }
               else{
                   image1.width
               }

        height: if(useTexts){
                    if(iconPos==0){
                        if(useIcon){
                            image1.height + text1.contentHeight
                        }
                        else{
                            text1.contentHeight
                        }

                    }
                    else{
                        if(useDoubleTexts || posToLeft){
                            if(useDoubleTexts){
                                Math.max(image1.height, text1.contentHeight + dtextTopMargin + text2.contentHeight)
                            }
                            else{
                                Math.max(image1.height, text1.contentHeight)
                            }
                        }
                        else{
                            Math.max(image1.height, text1.contentHeight)
                        }
                    }
                }
                else{
                    image1.height
                }
        color: "transparent"
        border.width: 0
        anchors.horizontalCenter: if(useDoubleTexts || posToLeft){
                                      parent.left
                                  }
                                  else{
                                      iconPos==0 ? parent.horizontalCenter : parent.horizontalCenter
                                  }
        anchors.verticalCenter: self.verticalCenter
        anchors.horizontalCenterOffset: if(useDoubleTexts || posToLeft){
                                            assistant1.width*0.5 + leftMargin
                                        }
                                        else{
                                            if(!useIcon){
                                                if(posToLeft){
                                                    width/2 - parent.width/2;
                                                }
                                                else{
                                                    0
                                                }
                                            }
                                            else{
                                                iconPos==1 && posToLeft ? assistant1.width*0.5-self.width*0.5 : horizontalOffset
                                            }
                                        }
        anchors.verticalCenterOffset: -bottomMargin
    }

    Image{
        id: image1
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
        // 文字顶着方框下边沿中间，图标和文字间的padding设置在方框的高度中了
        anchors.right: iconPos==0 ? assistant1.right : assistant1.right
        anchors.rightMargin: -textsLeftMargin
        anchors.verticalCenter: iconPos==0 ? assistant1.bottom : assistant1.verticalCenter
        anchors.verticalCenterOffset:if(useDoubleTexts){
                                         -dtextSize/2-dtextTopMargin/2
                                     }
                                     else{
                                         if(iconPos==0){
                                             -textsSize/2;
                                         }
                                         else{
                                             0;
                                         }
                                     }
        elide: Text.ElideRight
        visible: useTexts ? true : false
        font.pixelSize: textsSize
        font.bold: textsBold
    }
    Text {
        id: text2
        text: qsTr(dtests)
        opacity: enabled ? 1.0 : 0.3
        color: self.checkable && self.checked ? dtextsCheckedColor : dtextsUncheckedColor
        anchors.top: text1.bottom
        anchors.topMargin: dtextTopMargin
        anchors.left: text1.left
        anchors.leftMargin: dtextLeftMargin
        elide: Text.ElideRight
        visible: useDoubleTexts ? true : false
        font.pixelSize: dtextSize
        font.bold: dtextBold
    }
    SeparatorEx{
        id: borderBottomRect
        anchors.top: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: -1
        visible: borderBottom
        width: parent.width
        color: borderBottomColor
        margin: borderBottomMargin
        height: 1
    }

    background: Rectangle{
        id: bgRect
        width: Math.max(self.width, assistant1.width)
        height: Math.max(self.height, assistant1.height)
        color: self.checked ? bgSelectedColor : bgColor
        radius: 0
        border.width: borderWidth
        border.color: borderColor
    }

    RadialGradient{
        id: pressWaveGradient
        z: -1
//        anchors.horizontalCenter: self.left
        anchors.horizontalCenterOffset: pressWaveStartPosX
        anchors.verticalCenter: self.verticalCenter
        anchors.leftMargin: pressWaveStartPosX * (1-pressWaveLeftArea)
        anchors.rightMargin: (self.width - pressWaveStartPosX)*(1-pressWaveLeftArea)
        anchors.right: self.right
        anchors.left: self.left
//        width: self.width*pressWaveLeftArea*2
        height: self.height
        horizontalRadius: 100
        verticalRadius: 100
        clip: true
        gradient: Gradient{
            GradientStop{position:0.1; color: self.checked ? bgSelectedColor : bgSelectedColor}
            GradientStop{position:0.9; color: self.checked ? bgSelectedColor : bgSelectedColor}
        }
    }

    PropertyAnimation{
        id: pressWaveAnimation
        target: self
        property: "pressWaveLeftArea"
        from: 0.0
        to: 1
        duration: self.pressWaveDuration
        running: false
        onStopped: {
            pressWaveFinish = true
        }
    }

    onPressWaveFinishChanged: {
        if(pressed){
            return
        }
        self.pressWaveLeftArea=0
        pressWaveGradient.visible=false
        pressWaveFinish = false
    }

    onReleased: {
        if(!pressWaveFinish){
           return
        }
        self.pressWaveLeftArea=0
        pressWaveGradient.visible=false
        pressWaveFinish = false
    }

    onPressed: {
        if(!enablePressWave){
            return
        }
        pressWaveStartPosX = pressX;
        pressWaveGradient.visible=true;
        pressWaveAnimation.start();
    }
}
