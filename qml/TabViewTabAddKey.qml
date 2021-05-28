import QtQuick 2.0
import QtQuick.Layouts 1.12
import "QuickItem"

Item {
    id: addTabItem
//    anchors.fill: parent
    property color bgColor: "transparent"
    property int widthMargin: addTabItem.width/10
    property int heightMargin: addTabItem.height/10
    Rectangle{
        width: parent.width
        height: parent.height
        color: bgColor
    }

    Rectangle{
        id: addToolRect
        width: parent.width
        height: 50
        anchors.top: parent.top
        anchors.left: parent.left
        color: "#282828"
        ButtonEx{
            id: showAccountAllBtnEx
            width: 100
            height: addToolRect.height
            anchors.left: addToolRect.left
            anchors.verticalCenter: addToolRect.verticalCenter
            bgColor: "transparent"
            checkable: true
            texts: showAccountAllBtnEx.checked ? "隐藏密码" : "显示密码"
            textsBold: true
            iconChecked: "qrc:/androidsrc/showAccount_reverse.png"
            iconUnchecked: "qrc:/androidsrc/hideAccount.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                var state =showAccountAllBtnEx.checked
                for(var i=0; i<keysView.model.count; i++){
                    var obj =keysView.model.get(i)
                    obj.showAccount = state
                    obj.showPassword = state
                }
            }
        }
        ButtonEx{
            width: 100
            height: addToolRect.height
            anchors.horizontalCenter: addToolRect.horizontalCenter
            anchors.verticalCenter: addToolRect.verticalCenter
            bgColor: "transparent"
            checkable: true
            texts: self.checked ? "123" : "456"
            textsBold: true
            iconChecked: "qrc:/androidsrc/showAccount_reverse.png"
            iconUnchecked: "qrc:/androidsrc/hideAccount.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                var newState=false
                self.checked ? newState = true : newState = false
                for(var i=0; i<KeyModel.count; i++){
                    KeyModel.get(i).showAccountBtnEx.checked=newState
                }
            }
        }
        ButtonEx{
            width: 100
            height: addToolRect.height
            anchors.right: addToolRect.right
            anchors.verticalCenter: addToolRect.verticalCenter
            bgColor: "transparent"
            checkable: false
            texts: "查询"
            textsBold: true
            iconChecked: "qrc:/androidsrc/showAccount_reverse.png"
            iconUnchecked: "qrc:/androidsrc/hideAccount.png"
            iconPos: 1
            iconWidth: 30
            iconHeight: 30
            onClicked: {
                var newState=false
                self.checked ? newState = true : newState = false
                for(var i=0; i<KeyModel.length; i++){
                    KeyModel.get(i).showAccountBtnEx.checked=newState
                }
            }
        }
    }
    Rectangle{
       id: addKeysRect
       width: addToolRect.width
       height: addToolRect.height
       anchors.top: addToolRect.bottom
       anchors.bottom: parent.bottom
       color: "#282828"
       GridLayout{
           anchors.top: parent.top
//           anchors.topMargin: Math.min(10, parent.height*0.2)
           anchors.bottom: parent.bottom
           anchors.bottomMargin: parent.height*0.2
           anchors.left: parent.left
           anchors.leftMargin: Math.min(20, parent.width*0.1)
           anchors.right: parent.right
           anchors.rightMargin: Math.min(20, parent.width*0.1)
           rows: 4
           rowSpacing: 0
           columns: 2
           ButtonEx{
               Layout.preferredWidth: 70
               Layout.preferredHeight: 40
               Layout.row: 0
               Layout.column: 0
           }

           TextInputEx{
               id: discInput
               Layout.preferredWidth: 300
               Layout.preferredHeight: 40
               Layout.row: 0
               Layout.column: 1
               Layout.alignment: Qt.AlignCenter
           }
           TextInputEx{
               id: accountInput
               Layout.preferredWidth: discInput.width
               Layout.preferredHeight: discInput.height
               Layout.row: 1
               Layout.column: 1
               Layout.alignment: Qt.AlignCenter
               width: discInput.width
               height: discInput.height
           }
           TextInputEx{
               id: passwordInput
               Layout.preferredWidth: discInput.width
               Layout.preferredHeight: discInput.height
               Layout.row: 2
               Layout.column: 1
               Layout.alignment: Qt.AlignCenter
               echoMode: TextInput.Password
           }
           TextInputEx{
               id: passwordConfirmInput
               Layout.preferredWidth: discInput.width
               Layout.preferredHeight: discInput.height
               Layout.row: 3
               Layout.column: 1
               Layout.alignment: Qt.AlignCenter
               echoMode: TextInput.Password
           }
       }
       /*
       TextInput{
           id: discInput
           height: 30
           anchors.left: addKeysRect.left
           anchors.leftMargin: Math.max(widthMargin, 30)
           anchors.right: addKeysRect.right
           anchors.rightMargin: anchors.leftMargin
           anchors.top: addKeysRect.top
           anchors.topMargin: Math.min(heightMargin, 100)
           Rectangle{
               anchors.fill: parent
               color:"red"
           }
       }
       TextInput{
           id: accountInput
           height: 30
           anchors.left: discInput.left
           anchors.leftMargin: discInput.leftMargin
           anchors.right: discInput.right
           anchors.top: discInput.bottom
           anchors.topMargin: 40
           Rectangle{
               anchors.fill: parent
               color:"blue"
           }
       }
       TextInput{
           id: passwordInput
           height: 30
           anchors.left: discInput.left
           anchors.leftMargin: discInput.leftMargin
           anchors.right: discInput.right
           anchors.top: accountInput.bottom
           anchors.topMargin: 40
           echoMode: TextInput.Password
           passwordCharacter: "❄❄❄❄❄❄❄"
           Rectangle{
               anchors.fill: parent
               color:"green"
           }
       }
       TextInput{
           id: passwordConfirmInput
           height: 30
           anchors.left: discInput.left
           anchors.leftMargin: discInput.leftMargin
           anchors.right: discInput.right
           anchors.top: passwordInput.bottom
           anchors.topMargin: 40
           echoMode: TextInput.Password
           passwordCharacter: "❄❄❄❄❄❄❄"
           Rectangle{
               anchors.fill: parent
               color:"yellow"
           }
       }
*/

    }
}
