#pragma once

#include <sstd_memory.hpp>
#include <QtCharts/QtCharts>
#include "sstd_declarativechart_p.h"

namespace sstd{

    class SSTDChartView : public QtCharts::DeclarativeChart {
        //Q_OBJECT
    private:
        using Super = QtCharts::DeclarativeChart;
    public:
        SSTDChartView(QQuickItem *parent=nullptr);
    private:
        //SSTD_MEMORY_QOBJECT_DEFINE(SSTDChartView)
    };
    
}/**/











