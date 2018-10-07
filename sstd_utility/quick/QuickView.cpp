#include "QuickView.hpp"

namespace sstd::_private_sstd {

    void _WindowPrivate::load(const QUrl & arg) {
        Super::setSource(arg);
    }

    void _WidgetPrivate::load(const QUrl & arg) {
        Super::setSource(arg);
    }

    void _WindowPrivate::setResizeMode(sstd::ResizeMode arg) {
        switch (arg) {
        case sstd::ResizeMode::SizeViewToRootObject: {
            Super::setResizeMode(Super::SizeViewToRootObject);
            break;
        }
        case sstd::ResizeMode::SizeRootObjectToView: {
            Super::setResizeMode(Super::SizeRootObjectToView);
            break;
        }
        }
    }

    void _WidgetPrivate::setResizeMode(sstd::ResizeMode arg) {
        switch (arg) {
        case sstd::ResizeMode::SizeViewToRootObject: {
            Super::setResizeMode(Super::SizeViewToRootObject);
            break;
        }
        case sstd::ResizeMode::SizeRootObjectToView: {
            Super::setResizeMode(Super::SizeRootObjectToView);
            break;
        }
        }
    }

    _WindowPrivate::_WindowPrivate() {
        this->setResizeMode(sstd::ResizeMode::SizeRootObjectToView);
    }

    _WidgetPrivate::_WidgetPrivate() {
        this->setResizeMode(sstd::ResizeMode::SizeRootObjectToView);
    }

    sstd::LoadState _WidgetPrivate::status() const {
        switch (Super::status()){
        case Super::Null:return sstd::LoadState::Null;
        case Super::Ready:return sstd::LoadState::Ready;
        case Super::Loading:return sstd::LoadState::Loading;
        case Super::Error:return sstd::LoadState::Error;
        }
        return sstd::LoadState::Error;
    }

    sstd::LoadState _WindowPrivate::status() const {
        switch (Super::status()) {
        case Super::Null:return sstd::LoadState::Null;
        case Super::Ready:return sstd::LoadState::Ready;
        case Super::Loading:return sstd::LoadState::Loading;
        case Super::Error:return sstd::LoadState::Error;
        }
        return sstd::LoadState::Error;
    }

    void _WidgetPrivate::setTitle(const QString & arg) {
        Super::setWindowTitle(arg);
    }

    void _WindowPrivate::setTitle(const QString & arg) {
        Super::setTitle(arg);
    }

    void _WindowPrivate::setClearColor(const QColor & arg) {
        Super::setColor(arg);
    }

    void _WidgetPrivate::setClearColor(const QColor & arg) {
        Super::setClearColor(arg);
    }

}/**/




























