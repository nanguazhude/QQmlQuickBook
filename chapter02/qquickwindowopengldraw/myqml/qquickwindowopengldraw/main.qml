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

    /*此对象获得QQUickWindow指针*/
   OpenglDrawWindowItem{  }

}/*~Window*/



