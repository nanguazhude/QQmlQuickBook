/*这个模块用于检查编译器是否加载Qt.Chart模块*/
#include <QtCharts/QtCharts>

namespace {


    static void test(){

        QtCharts::QChart varChart;
        QtCharts::QChartView varChartView;

        //add ...
        varChartView.setChart(&varChart);

    }


}/**/








