
import QtQuick 2.11
import QtQuick.Window 2.11
import myqml.qsgdrawuserender 1.0

Window {

    visible: true
    width: 640
    height: 480
    title: qsTr("qsgdrawpoint")

    QuickSimpleTriangle {
        x :      300
        y :      300
        width :  200
        height : 200
        rotation: 0
    }

    QuickSimpleTriangle {
        x :      300
        y :      300
        width :  200
        height : 200
        rotation: 10
    }

    QuickSimpleTriangle {
        x :      300
        y :      300
        width :  200
        height : 200
        rotation: 20
    }


}







