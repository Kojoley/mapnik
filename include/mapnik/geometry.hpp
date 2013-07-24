/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2011 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef MAPNIK_GEOMETRY_HPP
#define MAPNIK_GEOMETRY_HPP

// mapnik
#include <mapnik/vertex_vector.hpp>
#include <mapnik/box2d.hpp>
#include <mapnik/noncopyable.hpp>

// boost
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace mapnik {

enum eGeomType {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3
};

template <typename T, template <typename> class Container=vertex_vector>
class geometry : private mapnik::noncopyable
{
public:
    typedef T coord_type;
    typedef Container<coord_type> container_type;
    typedef typename container_type::value_type value_type;
    typedef typename container_type::size_type size_type;
private:
    container_type cont_;
    eGeomType type_;
    mutable unsigned itr_;
    mutable box2d<double> extent_;
public:

    geometry()
        : type_(Unknown),
          itr_(0),
          extent_()
    {}

    explicit geometry(eGeomType type)
        : type_(type),
          itr_(0),
          extent_()
    {}

    eGeomType type() const
    {
        return type_;
    }

    void set_type(eGeomType type)
    {
        type_ = type;
    }

    container_type const& data() const
    {
        return cont_;
    }

    size_type size() const
    {
        return cont_.size();
    }

    void set_extent(box2d<double> const& box)
    {
        return extent_ = box;
    }

    box2d<double> const& envelope() const
    {
        if (extent_.valid())
        {
            return extent_;
        }
        double x = 0;
        double y = 0;
        rewind(0);
        for (unsigned i=0; i < size(); ++i)
        {
            unsigned cmd = vertex(&x,&y);
            if (cmd == SEG_CLOSE) continue;
            if (i == 0)
            {
                extent_.init(x,y,x,y);
            }
            else
            {
                extent_.expand_to_include(x,y);
            }
        }
        return extent_;
    }

    void push_vertex(coord_type x, coord_type y, CommandType c)
    {
        cont_.push_back(x,y,c);
    }

    void line_to(coord_type x,coord_type y)
    {
        push_vertex(x,y,SEG_LINETO);
    }

    void move_to(coord_type x,coord_type y)
    {
        push_vertex(x,y,SEG_MOVETO);
    }

    void close_path()
    {
        push_vertex(0,0,SEG_CLOSE);
    }

    unsigned vertex(double* x, double* y) const
    {
        return cont_.get_vertex(itr_++,x,y);
    }

    unsigned vertex(std::size_t index, double* x, double* y) const
    {
        return cont_.get_vertex(index, x, y);
    }

    void rewind(unsigned ) const
    {
        itr_=0;
    }
};

typedef geometry<double,vertex_vector> geometry_type;
typedef boost::shared_ptr<geometry_type> geometry_ptr;
typedef boost::ptr_vector<geometry_type> geometry_container;

}

#endif // MAPNIK_GEOMETRY_HPP
