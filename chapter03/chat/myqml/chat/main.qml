import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.4
import myqml.chat 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    SSTDChat{
        anchors.fill: parent ;
        textEditDelegate: TextArea{
            width: parent.width;
            text: "sfdsafd"
        }

        textFrameDelegate: Rectangle{
            color: Qt.rgba(Math.random()/3+0.5,Math.random()/3+0.5,Math.random()/5+0.5,1)
        }
    }

}











