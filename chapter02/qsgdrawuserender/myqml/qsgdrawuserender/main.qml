
import QtQuick 2.11
import QtQuick.Window 2.11
import myqml.qsgdrawuserender 1.0

Window {

    visible: true
    width: 640
    height: 480
    title: qsTr("qsgdrawpoint")

    QuickSimpleTriangle {
        x :      100
        y :      100
        width :  200
        height : 200
        rotation: 0
    }
    

}







