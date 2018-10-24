// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2015 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2015 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2015 Mateusz Loskot, London, UK.

// This file was modified by Oracle on 2014-2018.
// Modifications copyright (c) 2014-2018 Oracle and/or its affiliates.

// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle
// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_GEOMETRY_HPP
#define BOOST_GEOMETRY_GEOMETRY_HPP

// Shortcut to include all header files

#include <sstd/boost/geometry/core/closure.hpp>
#include <sstd/boost/geometry/core/coordinate_dimension.hpp>
#include <sstd/boost/geometry/core/coordinate_system.hpp>
#include <sstd/boost/geometry/core/coordinate_type.hpp>
#include <sstd/boost/geometry/core/cs.hpp>
#include <sstd/boost/geometry/core/interior_type.hpp>
#include <sstd/boost/geometry/core/point_order.hpp>
#include <sstd/boost/geometry/core/point_type.hpp>
#include <sstd/boost/geometry/core/ring_type.hpp>
#include <sstd/boost/geometry/core/tag.hpp>
#include <sstd/boost/geometry/core/tag_cast.hpp>
#include <sstd/boost/geometry/core/tags.hpp>

// Core algorithms
#include <sstd/boost/geometry/core/access.hpp>
#include <sstd/boost/geometry/core/exterior_ring.hpp>
#include <sstd/boost/geometry/core/interior_rings.hpp>
#include <sstd/boost/geometry/core/radian_access.hpp>
#include <sstd/boost/geometry/core/radius.hpp>
#include <sstd/boost/geometry/core/topological_dimension.hpp>

#include <sstd/boost/geometry/arithmetic/arithmetic.hpp>
#include <sstd/boost/geometry/arithmetic/dot_product.hpp>

#include <sstd/boost/geometry/strategies/strategies.hpp>

#include <sstd/boost/geometry/algorithms/append.hpp>
#include <sstd/boost/geometry/algorithms/area.hpp>
#include <sstd/boost/geometry/algorithms/assign.hpp>
#include <sstd/boost/geometry/algorithms/buffer.hpp>
#include <sstd/boost/geometry/algorithms/centroid.hpp>
#include <sstd/boost/geometry/algorithms/clear.hpp>
#include <sstd/boost/geometry/algorithms/comparable_distance.hpp>
#include <sstd/boost/geometry/algorithms/convert.hpp>
#include <sstd/boost/geometry/algorithms/convex_hull.hpp>
#include <sstd/boost/geometry/algorithms/correct.hpp>
#include <sstd/boost/geometry/algorithms/covered_by.hpp>
#include <sstd/boost/geometry/algorithms/crosses.hpp>
#include <sstd/boost/geometry/algorithms/densify.hpp>
#include <sstd/boost/geometry/algorithms/difference.hpp>
#include <sstd/boost/geometry/algorithms/disjoint.hpp>
#include <sstd/boost/geometry/algorithms/distance.hpp>
#include <sstd/boost/geometry/algorithms/envelope.hpp>
#include <sstd/boost/geometry/algorithms/equals.hpp>
#include <sstd/boost/geometry/algorithms/expand.hpp>
#include <sstd/boost/geometry/algorithms/for_each.hpp>
#include <sstd/boost/geometry/algorithms/intersection.hpp>
#include <sstd/boost/geometry/algorithms/intersects.hpp>
#include <sstd/boost/geometry/algorithms/is_empty.hpp>
#include <sstd/boost/geometry/algorithms/is_simple.hpp>
#include <sstd/boost/geometry/algorithms/is_valid.hpp>
#include <sstd/boost/geometry/algorithms/length.hpp>
#include <sstd/boost/geometry/algorithms/make.hpp>
#include <sstd/boost/geometry/algorithms/num_geometries.hpp>
#include <sstd/boost/geometry/algorithms/num_interior_rings.hpp>
#include <sstd/boost/geometry/algorithms/num_points.hpp>
#include <sstd/boost/geometry/algorithms/num_segments.hpp>
#include <sstd/boost/geometry/algorithms/overlaps.hpp>
#include <sstd/boost/geometry/algorithms/perimeter.hpp>
#include <sstd/boost/geometry/algorithms/relate.hpp>
#include <sstd/boost/geometry/algorithms/relation.hpp>
#include <sstd/boost/geometry/algorithms/remove_spikes.hpp>
#include <sstd/boost/geometry/algorithms/reverse.hpp>
#include <sstd/boost/geometry/algorithms/simplify.hpp>
#include <sstd/boost/geometry/algorithms/sym_difference.hpp>
#include <sstd/boost/geometry/algorithms/touches.hpp>
#include <sstd/boost/geometry/algorithms/transform.hpp>
#include <sstd/boost/geometry/algorithms/union.hpp>
#include <sstd/boost/geometry/algorithms/unique.hpp>
#include <sstd/boost/geometry/algorithms/within.hpp>

// check includes all concepts
#include <sstd/boost/geometry/geometries/concepts/check.hpp>

#include <sstd/boost/geometry/srs/srs.hpp>

#include <sstd/boost/geometry/util/for_each_coordinate.hpp>
#include <sstd/boost/geometry/util/math.hpp>
#include <sstd/boost/geometry/util/select_coordinate_type.hpp>
#include <sstd/boost/geometry/util/select_most_precise.hpp>

#include <sstd/boost/geometry/views/box_view.hpp>
#include <sstd/boost/geometry/views/closeable_view.hpp>
#include <sstd/boost/geometry/views/identity_view.hpp>
#include <sstd/boost/geometry/views/reversible_view.hpp>
#include <sstd/boost/geometry/views/segment_view.hpp>

#include <sstd/boost/geometry/io/io.hpp>
#include <sstd/boost/geometry/io/dsv/write.hpp>
#include <sstd/boost/geometry/io/svg/svg_mapper.hpp>
#include <sstd/boost/geometry/io/svg/write.hpp>
#include <sstd/boost/geometry/io/wkt/read.hpp>
#include <sstd/boost/geometry/io/wkt/write.hpp>

#endif // BOOST_GEOMETRY_GEOMETRY_HPP
