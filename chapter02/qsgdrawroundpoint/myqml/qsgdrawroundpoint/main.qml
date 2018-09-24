import QtQuick 2.11
import QtQuick.Window 2.11
import myqml.qsgdrawroundpoint 1.0

Window {

    visible: true
    width: 640
    height: 480
    title: qsTr("qsgdrawlines")
    id : _window;

    Component{
        id : _dynamic_object_componet
        QuickRoundPoint{
            x : parent.width*Math.random()
            y : parent.height*Math.random()
            z : Math.random()
            pointSize:  128
            pointColor: Qt.rgba( Math.random(),Math.random(),Math.random(),1 )
        }
    }

    Component.onCompleted: {
        for( var i =0; i<100 ; ++i ){
            _dynamic_object_componet.createObject(_window);
        }
    }

}/*Window*/



































