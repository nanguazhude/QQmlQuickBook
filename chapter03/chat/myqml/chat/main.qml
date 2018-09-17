import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import myqml.chat 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    SSTDChat{

        anchors.fill: parent ;

        textEditDelegate: TextArea{
            text: "sfdsafd"
            style: TextAreaStyle {
                     textColor: "#333"
                     selectionColor: "steelblue"
                     selectedTextColor: "#eee"
                     backgroundColor: "#eee"
                 }
        }

        textFrameDelegate: Rectangle{
            color: Qt.rgba(Math.random()/3+0.5,Math.random()/3+0.5,Math.random()/5+0.5,1)
        }
    }

}











