import QtQuick 2.11

Rectangle{
    color: Qt.rgba(0.1,0,0.8,1)
    Rectangle{
        color: Qt.rgba(0.1,0,0.6,1)
        width: parent.width*0.8
        height: parent.height*0.8
        anchors.centerIn: parent
        Rectangle{
            width: parent.width*0.8
            height: parent.height*0.8
            anchors.centerIn: parent
            color: Qt.rgba(0.1,0,0.4,1)
        }/*Rectangle*/
    }/*Rectangle*/
}/*Rectangle*/


