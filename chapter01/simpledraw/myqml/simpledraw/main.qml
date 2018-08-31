/*main.qml*/
import QtQuick 2.9
import QtQuick.Window 2.2

import myqml.simpledraw 1.0

Window {
    
    visible: true;
    width: 640;
    height: 480;
    title: qsTr("Hellow Word！");
    color: Qt.rgba(0.8,0.8,0.8,0.3);
    minimumHeight: 64;
    minimumWidth: 256;

    Rectangle {
    anchors.fill : parent 
    color : Qt.rgba(0.3,0.3,0.3,0)

    SimpleDraw {
        anchors.left: parent.left ;
        anchors.top : parent.top ;
         width : parent.width/2 
         height : parent.height/2
    }

   SimpleDraw{
        anchors.left: parent.left ;
        anchors.bottom : parent.bottom ;
         width : parent.width/2 
         height : parent.height/2
    }

   SimpleDraw{
        anchors.right : parent.right ;
        anchors.top: parent.top ;
          width : parent.width/2 
         height : parent.height/2
    }

    SimpleDraw{
        anchors.right : parent.right ;
        anchors.bottom: parent.bottom ;
          width : parent.width/2 
         height : parent.height/2
    }

    }

}/*~Window*/



