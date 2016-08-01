/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2007
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
*/

#include "LookupTables.h"
using namespace std;

namespace FEConfig {
    
void ParamTableRow::streamOut(std::ostream& out) const {
    const_iterator it;
    out << fixed << setw(fWidth_m) << setprecision(fPrecision_m);
    for (it = begin(); it != end(); ++it)
        out << *it << " ";
}

bool ParamTable::set(const double &freq, unsigned col, float value) {
    // check for out of bounds col:
    if (col >= numCols_m)
        return false;
    // do we already have a row for 'freq'?
    iterator it = find(freq);
    if (it != end())
        // yes.  Assign the value:
        it -> second.set(col, value);
    else {
        // no.  Create a new row and assign:
        ParamTableRow newRow(numCols_m);
        newRow.set(col, value);
        base_type::operator[](freq) = newRow;
    }
    return true;
}

float ParamTable::get(const double &freq, unsigned col, bool interpolate) const {
    ParamTableRow temp(numCols_m);
    if (get(freq, temp, interpolate))
        return temp[col];
    else
        return 0;
}

bool ParamTable::get(const double &freq, ParamTableRow &target, bool interpolate) const {
    target.clear();
    // check for nothing to return:
    if (base_type::empty())
        return false; 

    // find the first item whose key is not less than freq:
    const_iterator it(base_type::lower_bound(freq));
    double foundFreq(0.0);

    // check for past the last element:
    if (it == base_type::end()) {
        // back up one and return the last element:
        --it;
        foundFreq = it -> first;
        target = it -> second;
        return true;
    }
    // check for freq <= the first element
    if (it == base_type::begin()) {
        // return the first element:
        foundFreq = it -> first;
        target = it -> second;
        return true;
    }
    // check for exact frequency match:
    double freqHi = it -> first;
    if (freqHi == freq) {
        foundFreq = it -> first;
        target = it -> second;
        return true;
    }
    // back up by one and calculate the weighting between the lower and upper elements:
    const_iterator it2 = it;
    --it;
    double freqLo = it -> first;
    double weight = calculateWeight(freq, freqLo, freqHi);

    // check for interval smaller than minInterpolateInterval_m:
    if ((freqHi - freqLo) <= minInterpolateInterval_m)
        interpolate = false;

    // if not interpolating, return one or the other:
    if (!interpolate) {
        if (weight >= 0.5) {
            // return the upper one:
            foundFreq = freqHi;
            target = it2 -> second;
        } else {
            // return the lower one:
            foundFreq = freqLo;
            target = it -> second;
        }

    // interpolate:
    } else {
        // don't assign foundFreq;
        target = it -> second;
        target.weightedAdd(weight, it2 -> second);
    }
    return true;
}

bool ParamTable::get(const_iterator &it, double &freq, ParamTableRow &target) const {
    freq = 0;
    target.clear();
    // check for nothing to return:
    if (it == base_type::end() || base_type::empty())
        return false;

    freq = it -> first;
    target = it -> second;
    return true;
}

void ParamTable::streamOut(std::ostream& out) const {
    const_iterator it;
    for (it = base_type::begin(); it != base_type::end(); ++it) {
        out << fixed << setw(freqWidth_m) << setprecision(freqPrecision_m) << it -> first
            << ": " << it -> second << std::endl;
    }
}

void MixerParams::streamOut(std::ostream& out) const {
    out << "FreqLO: VJ01 VJ02 VJ11 VJ12 IJ01 IJ02 IJ11 IJ12" << std::endl;
    ParamTable::streamOut(out);
}

void MagnetParams::streamOut(std::ostream& out) const {
    out << "FreqLO: IMag01 IMag02 IMag11 IMag12" << std::endl;
    ParamTable::streamOut(out); 
}

void PreampParams::streamOut(std::ostream& out) const {
    out << "FreqLO: VD1 VD2 VD3 ID1 ID2 ID3 VG1 VG2 VG3" << std::endl;
    ParamTable::streamOut(out); 
} 

void PowerAmpParams::streamOut(std::ostream& out) const {
    out << "FreqLO: VD0 VD1 VG0 VG1" << std::endl;
    ParamTable::streamOut(out); 
} 
        
}; //namespace FEConfig

