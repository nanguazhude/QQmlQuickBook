/*main.qml*/
import QtQuick 2.9
import QtQuick.Window 2.2

Rectangle {
    visible: true;
    width: 640;
    height: 480;
    color: Qt.rgba(0.8,0.8,0.8,1);

    MainText{
        z : 1 ;
        anchors.fill : parent;
    }/*~MainText*/

    MainRectangle {
        z : 0 ;
        anchors.fill : parent;
    }/*~MainRectangle*/

}/*~Window*/



