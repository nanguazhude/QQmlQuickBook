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

    class AbstractRootWindow {
    public:
        virtual void setResizeMode(sstd::ResizeMode)=0;
        virtual void load(const QUrl &)=0;
        virtual sstd::LoadState status() const=0;
        virtual void setTitle(const QString &)=0;
        virtual void setClearColor(const QColor &)=0;
        virtual void show()=0;
        virtual ~AbstractRootWindow();
    };

    template <WindowType>
    class RootWindow;

    namespace _private_sstd {

        class _WindowPrivate : public QuickViewWindow ,
            public virtual AbstractRootWindow {
            Q_OBJECT
        public:
            using QuickViewWindow::QuickViewWindow;
            void setResizeMode(sstd::ResizeMode) override;
            void load(const QUrl &) override;
            sstd::LoadState status() const override;
            void setTitle(const QString &) override;
            void setClearColor(const QColor &) override;
            void show() override;
            _WindowPrivate();
        private:
            using Super = QuickViewWindow;
        private:
            SSTD_MEMORY_QOBJECT_DEFINE(_WindowPrivate)
        };

        class _WidgetPrivate :public QuickViewWidget,
            public virtual AbstractRootWindow {
            Q_OBJECT
        public:
            using QuickViewWidget::QuickViewWidget;
            void setResizeMode(sstd::ResizeMode) override;
            void load(const QUrl &) override;
            sstd::LoadState status() const override;
            void setTitle(const QString &) override;
            void setClearColor(const QColor &) override;
            void show() override;
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

#if !defined(_DEBUG)/*选择使用QQuickWidget还是QQuickView作为显示窗口*/
    using DefaultRoowWindow = RootWindow<sstd::WindowType::QtWidget>;
#else
    using DefaultRoowWindow = RootWindow<sstd::WindowType::QtQuickWindow>;
#endif

}/*namespace sstd*/





















