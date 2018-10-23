#include "TextFrameItem.hpp"
#include <QtCore/qpointer.h>
#include <QtGui/qtextobject.h>

namespace sstd {

    constexpr const char * $p$name() {
        return "\xF1""$name_frame_text_data";
    }

    TextFrameItem::TextFrameItem(QObject * parent) :Super(parent) {

    }

    void TextFrameItem::setTextData(QTextFrame * arg, TextFrameItem*var) {
        if (arg == nullptr) { return; }
        if (var == nullptr) { return; }
        arg->setProperty($p$name(), QVariant::fromValue<QPointer<QObject>>(var));
    }

    TextFrameItem* TextFrameItem::getTextData(QTextFrame *arg) {
        if (arg == nullptr) { return nullptr; }
        const auto varVariant = arg->property($p$name());
        if (false == varVariant.isValid()) { return nullptr; }
        auto varAns = varVariant.value<QPointer<QObject>>().data();
        return sstd_safe_cast<TextFrameItem*>(varAns);
    }

}









