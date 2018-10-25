#include "sstd_geometry_2d.hpp"
#include "../sstd/boost/circular_buffer.hpp"
#include "../sstd/boost/bimap.hpp"
#include "../sstd/boost/geometry.hpp"
#include "../sstd/boost/geometry/index/rtree.hpp"

#ifndef getPrivate
#define getPrivate() static_cast< ThisP * >( getThisPrivate() )
#endif

#ifndef getConstPrivate
#define getConstPrivate() static_cast< const ThisP * >( getThisPrivate() )
#endif

namespace sstd {

    namespace private_scene_2d {

        PrivateBasic::~PrivateBasic() {
        }

        Basic::~Basic() {
            delete thisp;
        }

        Basic::Basic(PrivateBasic * arg) : thisp(arg) {
        }


    }/*private_scene_2d*/

    class _0_PrivateScene2D : public private_scene_2d::PrivateBasic {
    public:
    private:
        SSTD_MEMORY_DEFINE(_0_PrivateScene2D)
    };

    Scene2D::Scene2D() :Scene2D(sstdNew<_0_PrivateScene2D>()) {

    }

    Scene2D::Scene2D(private_scene_2d::PrivateBasic* arg) : Basic(arg) {
        getPrivate()->super = this;
    }

    Scene2D::~Scene2D() {

    }

}/*namespace sstd*/















