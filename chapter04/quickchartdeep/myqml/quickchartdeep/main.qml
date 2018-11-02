/*main.qml*/
import QtQuick 2.9
import QtCharts 2.2
import myqml.sstdchartview 1.0

Rectangle {
    visible: true;
    width: 640;
    height: 480;
    color: Qt.rgba(0.8,0.8,0.8,1);

    ChartView {

        id : _id_chart_view  ;
        anchors.centerIn: parent ;
        width : parent.width * 0.8 ;
        height: parent.height * 0.8;
        antialiasing: true   ;
        clip: true ;

        /*********************************************/
        SSTDChartView.chart: _id_chart_view;
        signal anyDomainUpdate()
        Component.onCompleted: {
            _id_chart_view.SSTDChartView.anyDomainUpdate.connect(
                        _id_chart_view.anyDomainUpdate );
            _id_chart_view.SSTDChartView.anyDomainUpdate();
        }
        /*********************************************/

        ScatterSeries {
            id: _id_scatter1
            name: "Scatter1"
            XYPoint { x: 1.5; y: 1.5 }
            XYPoint { x: 1.5; y: 1.6 }
            XYPoint { x: 1.57; y: 1.55 }
            XYPoint { x: 1.8; y: 1.8 }
            XYPoint { x: 1.9; y: 1.6 }
            XYPoint { x: 2.1; y: 1.3 }
            XYPoint { x: 2.5; y: 2.1 }
        }

        Rectangle{
            id : _id_rect
            width: 32 ;
            height: 32 ;
            color: Qt.rgba(1,0,0,1)
            visible: _id_scatter1.axisX.min <= 1.8 ;
        }

        Rectangle{
            id : _id_rect1
            width: 32 ;
            height: 32 ;
            color: Qt.rgba(1,0,0,1)
            visible: _id_scatter1.axisX.min <= 1.5 ;
        }

        onAnyDomainUpdate : {

            var pos = _id_chart_view.mapToPosition( Qt.point(1.8,1.8), _id_scatter1)
            _id_rect.x = pos.x ;
            _id_rect.y = pos.y ;

            var pos = _id_chart_view.mapToPosition( Qt.point(1.5,1.5), _id_scatter1)
            _id_rect1.x = pos.x ;
            _id_rect1.y = pos.y ;

        }

        Timer{
            property int flipState : 0 ;
            interval: 1500;
            running: true;
            repeat: true
            onTriggered: {
                ++flipState;
                switch(flipState){
                case 1:_id_chart_view.scrollLeft(32);break;
                case 2:_id_chart_view.scrollRight(64);break;
                case 3:_id_chart_view.scrollLeft(32);flipState=0;break;
                }
            }
        }

    }/**/

}/*~Window*/



