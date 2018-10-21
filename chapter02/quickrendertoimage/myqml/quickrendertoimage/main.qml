
import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.0

Rectangle{
    color: Qt.rgba(0.5,0.5,0.5,1);

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent ;

        RowLayout {
        width: parent.width;

            Button {
                Layout.preferredWidth: 128
                id: button
                text: qsTr("选择源")
            }

            TextField {
                Layout.fillWidth:true
                id: textField
                text: qsTr(".qml")
            }
        }

        RowLayout {
width: parent.width;
            Button {
                Layout.preferredWidth: 128
                id: button1
                text: qsTr("保存图像名称")
            }

            TextField {
                Layout.fillWidth:true
                id: textField1
                text: qsTr(".png")
            }
        }

        Button {
            Layout.fillWidth:true
            width: parent.width;
            id: button2
            x: 196
            y: 324
            text: qsTr("执行")
        }
    }



}/*Rectangle*/













