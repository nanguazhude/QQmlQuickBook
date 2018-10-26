#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qobject.h>
#include "sstd_quick_library_global.hpp"
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>
#include <QtQuick/qquickitem.h>

namespace sstd {

    namespace private_scene_2d {

        class Basic;
        class _1_SSTD_QUICK_LIBRARY_EXPORT PrivateBasic {
            SSTD_DELETE_COPY_ASSIGN(PrivateBasic);
        public:
            virtual ~PrivateBasic();
            Basic * super{ nullptr };
            PrivateBasic();
        private:
            SSTD_MEMORY_DEFINE(PrivateBasic)
        };

        class _1_SSTD_QUICK_LIBRARY_EXPORT Basic {
            SSTD_DELETE_COPY_ASSIGN(Basic);
        public:
            virtual ~Basic();
            Basic(PrivateBasic *);

            inline PrivateBasic * getThisPrivate() {
                return thisp;
            }

            inline const PrivateBasic * getThisPrivate() const {
                return thisp;
            }

        private:
            PrivateBasic * const thisp;
        private:
            SSTD_MEMORY_DEFINE(Basic)
        };

    }/**/

    class Scene2D;
    class _1_SSTD_QUICK_LIBRARY_EXPORT Scene2DItemBasic :
        public QObject,
        public private_scene_2d::Basic {
        Q_OBJECT
    public:
        Q_PROPERTY(QQuickItem * target READ getTarget WRITE setTarget NOTIFY targetChanged)
    public:
        Q_PROPERTY(Scene2D * scene2D READ getScene2D WRITE setScene2D NOTIFY scene2DChanged)
    public:
        virtual ~Scene2DItemBasic();
        virtual QRectF sceneBoundingRect() const;
        virtual std::shared_ptr< const sstd::vector<QPointF> > sceneBoundingPath() const = 0;
    public:
        QQuickItem * getTarget()const;
        void setTarget(QQuickItem *);
        Q_SIGNAL void targetChanged();
    public:
        Scene2D * getScene2D() const;
        void setScene2D(Scene2D *);
        Q_SIGNAL void scene2DChanged();
    public:
        Q_SIGNAL void sceneBoundingRectChanged();
        Q_SIGNAL void sceneBoundingPathChanged();
    private:
        QQuickItem * mmm_target{ nullptr };
        Scene2D * mmm_scene{ nullptr };
    protected:
        virtual void onTargetChanged();
    public:
        SSTD_MEMORY_QOBJECT_DEFINE(Scene2DItemBasic)
    };

    class _0_PrivateScene2D;
    class _1_SSTD_QUICK_LIBRARY_EXPORT Scene2D :
        public QQuickItem,
        public private_scene_2d::Basic {
        Q_OBJECT
    public:
        virtual ~Scene2D();
        Scene2D();
        void addItem( Scene2DItemBasic * );
    protected:
        Scene2D(private_scene_2d::PrivateBasic*);
    private:
        friend class _0_PrivateScene2D;
        using ThisP = _0_PrivateScene2D;
    public:
        SSTD_MEMORY_QOBJECT_DEFINE(Scene2D)
    };

}/*namespace sstd*/













