#pragma once

#include "../sstd_memory.hpp"
#include <cstddef>
#include <cstdint>
#include <QtCore/qurl.h>
#include <QtCore/qstring.h>
#include "QuickViewWidget.hpp"
#include "QuickViewWindow.hpp"

namespace sstd {

    enum class WindowType : std::size_t {
        QtQuickWindow,
        QtWidget,
    };

    enum class ResizeMode : std::size_t {
        SizeViewToRootObject,
        SizeRootObjectToView,
    };

    enum class LoadState : std::size_t {
        Null,
        Ready,
        Loading,
        Error,
    };

    template <WindowType>
    class RootWindow;

    namespace _private_sstd {

        class _WindowPrivate : public QuickViewWindow {
            Q_OBJECT
        public:
            using QuickViewWindow::QuickViewWindow;
            void setResizeMode(sstd::ResizeMode);
            void load(const QUrl &);
            sstd::LoadState status() const;
            void setTitle(const QString &);
            _WindowPrivate();
        private:
            using Super = QuickViewWindow;
        private:
            SSTD_MEMORY_QOBJECT_DEFINE(_WindowPrivate)
        };

        class _WidgetPrivate :public QuickViewWidget {
            Q_OBJECT
        public:
            using QuickViewWidget::QuickViewWidget;
            void setResizeMode(sstd::ResizeMode);
            void load(const QUrl &);
            sstd::LoadState status() const;
            void setTitle(const QString &);
            _WidgetPrivate();
        private:
            using Super = QuickViewWidget;
        private:
            SSTD_MEMORY_QOBJECT_DEFINE(_WidgetPrivate)
        };
    }

    template <>
    class RootWindow<WindowType::QtWidget> :
        public _private_sstd::_WidgetPrivate {};

    template <>
    class RootWindow<WindowType::QtQuickWindow> :
        public _private_sstd::_WindowPrivate {};

#if 1/*选择使用QQuickWidget还是QQuickView作为显示窗口*/
    using DefaultRoowWindow = RootWindow<sstd::WindowType::QtWidget>;
#else
    using DefaultRoowWindow = RootWindow<sstd::WindowType::QtQuickWindow>;
#endif

}/*namespace sstd*/





















