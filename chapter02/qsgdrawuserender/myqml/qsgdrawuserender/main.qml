
import QtQuick 2.11
import QtQuick.Window 2.11
import myqml.qsgdrawuserender 1.0

Window {

    visible: true
    width: 640
    height: 480
    title: qsTr("qsgdrawpoint")

    QuickSimpleTriangle {
        x :      200
        y :      200
        width :  200
        height : 200
        rotation: 0
        opacity : 1
        scale :   1
    }

    QuickSimpleTriangle {
        x :      200
        y :      200
        width :  200
        height : 200
        rotation: 10
        opacity : 0.6
        scale :   0.8
    }

    QuickSimpleTriangle {
        x :      200
        y :      200
        width :  200
        height : 200
        rotation: 40
        opacity : 0.3
        scale :   1.2
    }


}







