#ifndef _CPPUTIL_DELAYSTATS_H_
#define _CPPUTIL_DELAYSTATS_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2025
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
*/

// THis simple template class calculates mean, standard deviation, and max for 
// a vector of any numeric type.
//
// Results are provided in public attributes mean_m, std_m, and max_m
//

#include <cmath>

template <class T>
class delayStats {
public:
    double mean_m;
    double std_m;
    T max_m;

    void calculate(const std::vector<T> &v) {
        double sum = 0;
        max_m = 0;
        auto len = v.size();
        for (auto &each: v) {
            sum += each;
            if (each > max_m)
                max_m = each;
        }
        mean_m = sum / len;        
        double square_sum_of_difference = 0;
        double tmp;
        for (auto &each: v) {
            tmp = each - mean_m;
            square_sum_of_difference += tmp * tmp;
        }
        std_m = std::sqrt(square_sum_of_difference / (len - 1));
    }
};

#endif //_CPPUTIL_DELAYSTATS_H_