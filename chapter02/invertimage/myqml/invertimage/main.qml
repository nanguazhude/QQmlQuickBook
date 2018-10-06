
import QtQuick 2.11
import QtQuick.Window 2.11


Window {

    color: Qt.rgba(0.2,0.3,0.3,1)
    visible: true
    width: 640
    height: 480
    title: qsTr("invertimage")

    Item{

        width: parent.width
        height: parent.height
        anchors.centerIn: parent

    Image{
        objectName: "imageSource";
        fillMode: Image.PreserveAspectFit;
        width: parent.width * 0.5;
        height: parent.height;
        anchors.left: parent.left;
    }/*Image*/

    Image{
        objectName: "imageTarget";
        fillMode: Image.PreserveAspectFit;
        width: parent.width * 0.5;
        height: parent.height;
        anchors.right: parent.right;
    }/*Image*/

    }/*Item*/

}







