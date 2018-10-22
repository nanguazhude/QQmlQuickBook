#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qobject.h>
#include <QtCharts/QtCharts>
#include <QtQml/qqmlengine.h>
namespace QtCharts {
    class DeclarativeChart;
}/**/;

namespace sstd{

    class SSTDChartView : public QObject {
        Q_OBJECT
    private:
        using Super = QObject;
        Q_PROPERTY(QObject * chart READ chart WRITE setChart NOTIFY chartChanged)
    public:
        SSTDChartView(QObject *parent=nullptr);

        QObject * chart() const {
            return mmm_QMLChart;
        }
        void setChart(QObject * arg) {
            if (mmm_QMLChart == arg) {
                return;
            }
            mmm_QMLChart = arg;
            chartChanged();
        }
        Q_SIGNAL void chartChanged();
    public:
        Q_SLOT void seriesAdded(QtCharts::QAbstractSeries *series);
    public:
        static SSTDChartView * qmlAttachedProperties(QObject *);
    private:
        QObject * mmm_QMLChart{nullptr};
        QtCharts::DeclarativeChart * mmm_DeclarativeChart{nullptr};
        QChart * mmm_Chart{nullptr};
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(SSTDChartView)
    };
    QML_DECLARE_TYPEINFO(sstd::SSTDChartView, QML_HAS_ATTACHED_PROPERTIES)

}/**/











