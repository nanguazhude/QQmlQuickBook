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

namespace this_cpp_file {
    using Number = qreal;
    typedef boost::geometry::model::point<
        Number,
        2,
        boost::geometry::cs::cartesian/*笛卡尔坐标系*/
    > Point2D/*点*/;
    typedef boost::geometry::model::box<Point2D> Box2D/*索引矩形*/;
    typedef std::pair<Box2D, sstd::Scene2DItemBasic *> KeyValue/*键值对*/;
    typedef boost::geometry::index::linear<8> Algorithm/*算法*/;
    typedef boost::geometry::index::equal_to<KeyValue> Equal/*比较*/;
    typedef sstd::allocator<KeyValue> Allocator/*内存*/;
    typedef boost::geometry::index::indexable<KeyValue> IndexGetter/*辅助类*/;
    typedef boost::geometry::index::rtree< KeyValue,
        Algorithm,
        IndexGetter,
        Equal,
        Allocator> RTree/*RTree*/;

}/*this_cpp_file*/
using namespace this_cpp_file;

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
        class SetLess : public std::less<void> {
        public:
            bool operator()(sstd::Scene2DItemBasic * a, sstd::Scene2DItemBasic *b) const {
                return a < b;
            }
            bool operator()(const sstd::unique_ptr<sstd::Scene2DItemBasic> & a, sstd::Scene2DItemBasic *b) const {
                return a.get() < b;
            }
            bool operator()(sstd::Scene2DItemBasic * a, const sstd::unique_ptr<sstd::Scene2DItemBasic> &b) const {
                return a < b.get();
            }
            bool operator()(const sstd::unique_ptr<sstd::Scene2DItemBasic> & a, const sstd::unique_ptr<sstd::Scene2DItemBasic> &b) const {
                return a.get() < b.get();
            }
        };
    public:
        RTree mmm_RTree;
        std::map<
            sstd::unique_ptr<sstd::Scene2DItemBasic>,
            this_cpp_file::KeyValue,
            SetLess,
            sstd::allocator< std::pair<const sstd::unique_ptr<sstd::Scene2DItemBasic>,
            this_cpp_file::KeyValue > > > mmm_Items;

        void insert(sstd::unique_ptr<sstd::Scene2DItemBasic> arg) {
            auto[varPos, varIsInsert] = mmm_Items.emplace(std::move(arg), this_cpp_file::KeyValue{});
            if (false == varIsInsert) {
                return;
            }
            auto varData = varPos->first.get();
            {
                const auto varBoundingRect = varData->boundingRect();
                const auto varTopLeft = varBoundingRect.topLeft();
                const auto varBottomRight = varBoundingRect.bottomRight();
                varPos->second = { {{varTopLeft.x(),varTopLeft.y()},
                    {varBottomRight.x(),varBottomRight.y()}},
                    varData };
            }
            mmm_RTree.insert(varPos->second);
        }

        void remove(sstd::Scene2DItemBasic * arg) {
            auto varPos = mmm_Items.find(arg);
            if (varPos == mmm_Items.end()) {
                return;
            }
            mmm_RTree.remove(varPos->second);
            mmm_Items.erase(varPos);
        }

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

    Scene2DItemBasic::~Scene2DItemBasic() {
    }

    void Scene2D::addItem(sstd::unique_ptr<Scene2DItemBasic>item) {
    }

}/*namespace sstd*/















