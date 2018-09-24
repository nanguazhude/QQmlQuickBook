import QtQuick 2.11
import QtQuick.Window 2.11
import myqml.qsgdrawpoint 1.0

Window {

    visible: true
    width: 640
    height: 480
    title: qsTr("qsgdrawpoint")

    QuickPoint{
        x : 100
        y : 100
        pointSize : 64;
        pointColor: Qt.rgba(0.3,0.4,0.5,1.0)
    }

    Rectangle{
        antialiasing : true
        x :            200
        y :            200
        width:         64
        height:        64
        radius:        32
        color:         Qt.rgba(0.5,0.3,0.2,1)
        border.width:  0
    }

}







