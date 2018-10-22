#include "SSTDChartView.hpp"
#include <QtCore/qcoreapplication.h>
#include <QtQml/QtQml>
#include <QtQuick/QtQuick>
#include <private/declarativechart_p.h>
#include <private/qabstractseries_p.h>
#include <private/abstractdomain_p.h>

namespace sstd {

    SSTDChartView::SSTDChartView(QObject *parent) :Super(parent) {

    }

    SSTDChartView * SSTDChartView::qmlAttachedProperties(QObject * obj) {
        auto varAns = sstdNew<SSTDChartView>(obj);
        auto varDeclarativeChart = dynamic_cast<QtCharts::DeclarativeChart *>(obj);
        assert(varDeclarativeChart);
        varAns->mmm_DeclarativeChart = varDeclarativeChart;
        connect(varDeclarativeChart, SIGNAL(seriesAdded(QtCharts::QAbstractSeries *)),
            varAns,SLOT(seriesAdded(QtCharts::QAbstractSeries *)));
        return varAns;
    }

    class GetQAbstractSeriesPrivate : public QtCharts::QAbstractSeries {
    public:
        QtCharts::QAbstractSeriesPrivate * getPrivateData() const {
            return this->d_ptr.get();
        }
    };

    void SSTDChartView::seriesAdded(QtCharts::QAbstractSeries *series) {
        if (mmm_Chart == nullptr) {
            mmm_Chart = series->chart();
        }
        auto varPrivte = static_cast<GetQAbstractSeriesPrivate*>(series)->getPrivateData();
        auto varDoMain = varPrivte->domain();
        connect(varDoMain, &QtCharts::AbstractDomain::updated, []() {
            qDebug() << "xxx";
        });
        assert(mmm_Chart==series->chart());
    }

}/*namespace sstd*/

namespace  {

    static inline void registerThis() {
        qmlRegisterType<sstd::SSTDChartView>("myqml.sstdchartview", 1, 0, "SSTDChartView");
    }
    Q_COREAPP_STARTUP_FUNCTION(registerThis)

}/*namespace*/



