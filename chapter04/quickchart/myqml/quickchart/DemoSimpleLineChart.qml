
import QtQuick 2.11
import QtCharts 2.2

DemoBasic{

    width : 640 ;
    height : 512 ;

    ChartView{

        antialiasing:true
        animationOptions : ChartView.SeriesAnimations
        theme :ChartView.ChartThemeDark
        anchors.fill: parent ;

        ValueAxis {
            id: _id_axisX
            min: 0
            max: 5
            tickCount: 5
        }

        ValueAxis {
            id: _id_axisY
            min: 0
            max: 5
        }

        LineSeries {
            name: "LineSeries"
            XYPoint { x: 0; y: 0 }
            XYPoint { x: 1.1; y: 2.1 }
            XYPoint { x: 1.9; y: 3.3 }
            XYPoint { x: 2.1; y: 2.1 }
            XYPoint { x: 2.9; y: 4.9 }
            XYPoint { x: 3.4; y: 3.0 }
            XYPoint { x: 4.1; y: 3.3 }

            axisX:  _id_axisX
            axisY: _id_axisY
        }

        plotAreaColor: Qt.rgba(0,0,0,0)
        backgroundColor: Qt.rgba(0,0,0,0)

    }/*ChartView*/

}/**/












