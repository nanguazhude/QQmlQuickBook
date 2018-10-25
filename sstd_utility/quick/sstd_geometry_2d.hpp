#pragma once

#include <sstd_memory.hpp>
#include "sstd_quick_library_global.hpp"

#include <QtCore/qrect.h>

namespace sstd {

    namespace private_scene_2d {

        class Basic;
        class _1_SSTD_QUICK_LIBRARY_EXPORT PrivateBasic {
        public:
            virtual ~PrivateBasic();
            Basic * super{ nullptr };
        private:
            SSTD_MEMORY_DEFINE(PrivateBasic)
        };

        class _1_SSTD_QUICK_LIBRARY_EXPORT Basic {
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

    class _1_SSTD_QUICK_LIBRARY_EXPORT Scene2DItemBasic {
    public:

    public:
        virtual ~Scene2DItemBasic();
        virtual QRectF boundingRect() const = 0;
    public:
        SSTD_MEMORY_DEFINE(Scene2DItemBasic)
    };

    class _0_PrivateScene2D;
    class _1_SSTD_QUICK_LIBRARY_EXPORT Scene2D : public private_scene_2d::Basic {
    public:
        virtual ~Scene2D();
        Scene2D();
        void addItem(sstd::unique_ptr<Scene2DItemBasic>);
    protected:
        Scene2D(private_scene_2d::PrivateBasic*);
    private:
        friend class _0_PrivateScene2D;
        using ThisP = _0_PrivateScene2D;
    public:
        SSTD_MEMORY_DEFINE(Scene2D)
    };

}/*namespace sstd*/













