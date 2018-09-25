import QtQuick 2.11
import QtQuick.Window 2.11
import myqml.qsgdrawpoint 1.0

Window {

    visible: true
    width: 640
    height: 480
    title: qsTr("qsgdraw3dpoints")
    id : _window;
    color: Qt.rgba(0.3,0.35,0.35,1)

    Component{
        id : _dynamic_object_componet
        Quick3DPoint{
            property int iCount : 0
            property double xRate : 1;
            property double yRate : 1;
            property double zRate : 1;
            pointSize : 128
            pointColor :  Qt.rgba( 0.3+Math.random()/1.5,0.3+Math.random()/1.5,0.3+Math.random()/1.5,1 )
        }
    }

    Component.onCompleted: {

        for( var i =0; i<256 ; ++i ){
            var obj = _dynamic_object_componet.createObject(_window);
            obj.xRate = Math.random();
            obj.yRate = Math.random();
            obj.zRate = Math.random();
            obj.y = Qt.binding( function(){ return _window.height * this.yRate - this.width/2 ; } )
            obj.x = Qt.binding( function(){ return _window.width  * this.xRate - this.height/2 ; } )
            obj.z = Qt.binding( function(){ return this.zRate; } )
            obj.iCount = i
        }

    }

}

