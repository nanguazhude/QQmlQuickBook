#include "SSTDChartView.hpp"
#include <QtCore/qcoreapplication.h>

namespace sstd{

SSTDChartView::SSTDChartView(QQuickItem *parent):Super(parent){

}

}/*namespace sstd*/

namespace  {

    static inline void registerThis() {
        qmlRegisterType<sstd::SSTDChartView>("myqml.sstdchartview", 1, 0, "SSTDChartView");
    }
    Q_COREAPP_STARTUP_FUNCTION(registerThis)

}/*namespace*/

