/*main.qml*/
import QtQuick 2.9
import QtQuick.Window 2.2

import myqml.simpledraw 1.0

Window {
    id : _id_window
    visible: true;
    width: 640;
    height: 480;
    title: qsTr("QQuickWindowOpenGLDraw");
    color: Qt.rgba(0.8,0.8,0.8,0.3);
    minimumHeight: 64;
    minimumWidth: 256;

    /*此对象获得QQUickWindow指针*/
    OpenglDrawWindowItem{

        anchors.fill: parent;

        Timer{
            interval: 50 ; running: true; repeat: true
            onTriggered: {
                parent.updateValue<100?
                            parent.updateValue+= 1 :
                            parent.updateValue-= 1 ;
            }
        }


        Text {
            text: qsTr("名字很重要");
            color: Qt.rgba(Math.random()/2.5,Math.random()/5,Math.random()/5,1);
            font.pixelSize: Math.max(_id_window.width,_id_window.height)/5 - 12 ;
            verticalAlignment : Text.AlignVCenter;
            horizontalAlignment : Text.AlignHCenter;
            anchors.centerIn: parent
        }

    }


}/*~Window*/



