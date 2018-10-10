#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qobject.h>

class ContexTest : public QObject {
    Q_OBJECT
private:
    int mmm_YValue = 20;
    Q_PROPERTY(int yValue READ yValue WRITE setYValue NOTIFY yValueChanged)
public:
    int yValue() const {
        return mmm_YValue;
    }
    void setYValue(int a) {
        if (mmm_YValue == a) {
            return;
        }
        mmm_YValue = a;
        yValueChanged();
    }
    Q_SIGNAL void yValueChanged();
private:
    SSTD_MEMORY_QOBJECT_DEFINE(ContexTest)
};











