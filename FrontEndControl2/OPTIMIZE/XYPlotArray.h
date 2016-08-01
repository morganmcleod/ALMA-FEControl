#ifndef XYPLOTARRAY_H_
#define XYPLOTARRAY_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2009
*
*This library is free software; you can redistribute it and/or
*modify it under the terms of the GNU Lesser General Public
*License as published by the Free Software Foundation; either
*version 2.1 of the License, or (at your option) any later version.
*
*This library is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*Lesser General Public License for more details.
*
*You should have received a copy of the GNU Lesser General Public
*License along with this library; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
*
*/

/// \file
/// \brief Define a vector of points for containing various types of X-Y plot data.

#include <vector>
#include "logger.h"

struct XYPlotPoint {
    float X;
    union {
        float Y;
        float Y1;
    };
    float Y2;
    XYPlotPoint(float _X = 0, float _Y1 = 0, float _Y2 = 0)
      : X(_X),
        Y1(_Y1),
        Y2(_Y2)
        {}
};

typedef std::vector<XYPlotPoint> XYPlotArrayBase;

class XYPlotArray : public XYPlotArrayBase {
public:
    XYPlotArray()
      : XYPlotArrayBase()
        {}
    ///< default constructor
    XYPlotArray(size_type n)
      : XYPlotArrayBase(n)
        {}
    ///< construct an array of size n
    XYPlotArray(size_type n, const XYPlotPoint& t)
      : XYPlotArrayBase(n, t)
        {}
    ///< construct with n copies of t
    XYPlotArray(const XYPlotArray& other)
      : XYPlotArrayBase(other)
        {}
    ///< copy constructor
    XYPlotArray& operator=(const XYPlotArray& other) {
        if (this != &other) {
            XYPlotArrayBase::operator =(other);
        }
        return *this;
    }
    ///< assignment operator
    
    void print() const {
        for (const_iterator it = begin(); it != end(); ++it)
            LOG(LM_INFO) << (*it).X << ", " << (*it).Y << ", " << (*it).Y2 << std::endl;
    }
    
};

#endif /*XYPLOTARRAY_H_*/
