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
        anchors.fill: parent ;
        antialiasing: true   ;

        /*********************************************/
        SSTDChartView.chart: _id_chart_view;
        signal  anyDomainUpdate()
        Component.onCompleted: {
            SSTDChartView.anyDomainUpdate.connect( _id_chart_view.anyDomainUpdate );
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

    }/**/

}/*~Window*/



