import QtQuick 2.11
import QtQuick.Window 2.11

DemoBasic {
    title: qsTr("DemoPropertyAnimation")

    Item {
        anchors.fill: parent ;
        id : _id_item ;

        Image{
            source: "image.png";
            id : _id_image;
            width:implicitWidth ;
            height: implicitHeight;
            anchors.horizontalCenter : _id_item.horizontalCenter ;
            Behavior on y {
                PropertyAnimation {
                    duration: 500 ;
                    easing.type: Easing.InOutElastic;
                }
            }
        }/*Image*/

        MouseArea{
            anchors.fill: parent ;
            acceptedButtons: Qt.LeftButton | Qt.RightButton ;
            onClicked : {
                 _id_image.y = mouseY ;
            }
        }/*MouseArea*/

    }/*Item*/

}





