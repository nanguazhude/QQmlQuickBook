/*main.qml*/
import QtQuick 2.9

Rectangle {
    visible: true;
    width: 640;
    height: 480;
    color: Qt.rgba(1,0,0,1);
    Rectangle{
        x : parent.width/2     ;
        y : parent.height/2    ;
        width: parent.width/2  ;
        height: parent.height/2;
        color: Qt.rgba(0,1,0,1);
        Rectangle{
            id : _id_2
            x : 0
            y : 0
            width: parent.width/2;
            height: parent.height/2;
            color: Qt.rgba(0,0,1,1);
            Component.onCompleted: {
                console.log( "id2 : global : " + _id_2.mapToGlobal(0,0) );
            }
        }
    }
}/*~Window*/



