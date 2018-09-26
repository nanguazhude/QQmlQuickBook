
import QtQuick 2.11
import QtQuick.Window 2.11
import myqml.qsgdrawuserender 1.0

Window {

    color: Qt.rgba(0,0,0,1)
    visible: true
    width: 640
    height: 480
    title: qsTr("qsgdrawuserender")

    QuickSimpleTriangle {
        antialiasing : true
        x :      200
        y :      200
        width :  200
        height : 200
        rotation: 0
        opacity : 1
        scale :   1
    }

    QuickSimpleTriangle {
        antialiasing : true
        x :      200
        y :      200
        width :  200
        height : 200
        rotation: 10
        opacity : 0.6
        scale :   0.8
    }

    QuickSimpleTriangle {
        antialiasing : true
        x :      200
        y :      200
        width :  200
        height : 200
        rotation: 40
        opacity : 0.3
        scale :   1.2
    }

    MouseArea{
        acceptedButtons: Qt.AllButtons
        anchors.fill: parent
        onClicked: {
            var a = 0;
            var b = 1;
            var c = a + b;
            console.log("Mouse Clicked!"+c)
        }
    }


}







