import QtQuick 2.11

Rectangle{
    objectName: "rect0"
    id : rect0
    property int xValue : 12 ;
    color: Qt.rgba(0.8,0.8,0.8,1)
    Rectangle{
        objectName: "rect1"
        id : rect1 ;
        x : xValue ;
        color: Qt.rgba(0.3,0.8,0.1,1)
        width: 64
        height: 64
        Rectangle{
            objectName: "rect2"
            id : rect2 ;
            x : xValue ;
            color: Qt.rgba(0.8,0.5,0.1,1)
            width: 32
            height: 32
            Text {
                text : parent.objectName
                anchors.centerIn: parent
            }
        }/*rect2*/
        Text {
            text : parent.objectName
            anchors.centerIn: parent
        }
    }/*rect1*/
    Text {
        text : parent.objectName
        anchors.centerIn: parent
    }
}/*rect0*/









