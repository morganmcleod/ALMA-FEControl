#ifndef LOOKUPTABLES_H_
#define LOOKUPTABLES_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2006
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

/// \file
/// \brief declares lookup tables of cartridge parameters which may vary with LO frequency.

/// The following band-specific tuning behaviors are supported:
/// band3: 2 pols, 2 SIS/pol, no magnets.  SIS voltage (VJ) and LO PA drain voltage (VD) 
///        are optimized for max output power and lowest noise.
/// band6: 2 pols, 2 SIS/pol, one magnet coil/pol controls both junctions (IMag01, IMag11).
///        LO PA (VD) is adjusted to set target SIS current (IJ).
/// band7: 2 pols, 2 SIS/pol, separate magnets on all junctions.
///        Magnet current is optimized to minimize junction Josephson current with VJ=0.  
///        LO PA (VD) is adjusted to set target SIS current (IJ).
/// band9: 2 pols, 1 SIS/pol, one magnet pol (IMag01, IMag11).
///        Magnet current is optimized to minimize Josephson current with VJ swept +/- 0.2 mV.
///        Output power is measured while sweeping VJ.
///        LO PA (VD) is adjusted to set target SIS current (IJ).

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>

namespace FEConfig {

    /// A ParamTableRow is a 1-D array of float of a fixed size.
    /// The size may be set when constructed or new data assigned.   
    class ParamTableRow : private std::vector<float> {
        typedef std::vector<float> base_type;   ///< implemented in terms of vector<float>.
    public:
        ParamTableRow(unsigned numCols = 1)
          : base_type(numCols, 0.0),
            fWidth_m(5),
            fPrecision_m(3)
            {}
        ///< construct with the number of columns in the row.  All cells are initialized to zero.
        ~ParamTableRow()
          {}   
        void clear()
          { std::fill(begin(), end(), 0.0); }
        ///< Set all contents to zero retaining size.
          
        float get(unsigned col) const
          { if (col >= size()) return 0.0;
            return base_type::operator[](col); }
        ///< get the item at index 'col'.  Return 0 if out of bounds.                
        
        float operator [](unsigned col) const
          { return get(col); }
        ///< another name for get().
        
        bool set(unsigned col, float val)
          { if (col >= size()) return false;
            base_type::operator[](col) = val;
            return true; }
        ///< set the value at index 'col'.  Return false if out of bounds.

        void weightedAdd(double weight, const ParamTableRow &other)
          { for (unsigned col = 0; col < size(); ++col)
                set(col, get(col) * (1.0 - weight) + other.get(col) * weight); }
        ///< set all values to a point between this and other, determined by 'weight',
        ///< which should be between 0.0 and 1.0 (not enforced.)

        int fWidth_m;       ///< field width for streaming float values.
        int fPrecision_m;   ///< precision for streaming float values.

        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 
    };
    
    inline std::ostream &operator << (std::ostream& out, const ParamTableRow &row)
      { row.streamOut(out); return out; }
    ///< stream output for debugging ParamTableRow    
    
    /// A ParamTable is a 2-D table of float values, indexed by a double 'freq' column.
    /// Given an LO frequency 'freq', a ParamTableRow may be retrieved.
    /// Linear interpolation is used if the requested freq is between two indexed rows.
    class ParamTable : private std::map<double, ParamTableRow> {
        typedef std::map<double, ParamTableRow> base_type;  ///< implemented in terms of STL map.

    protected:
        unsigned numCols_m;     ///< holds a fixed number of columns, not including the freq col.
        double minInterpolateInterval_m;
        ///< when interpolating, if the difference between adjacent frequencies is smaller than this, don't interpolate.  Instead pick the nearest value.

    public:
        typedef base_type::iterator iterator;               ///< declare iterator types
        typedef base_type::const_iterator const_iterator;

        int freqWidth_m;       ///< field width for streaming double frequencies in GHz.
        int freqPrecision_m;   ///< precision for streaming double frequencies in GHz.

        ParamTable(unsigned numCols, double minInterpolateInterval = 0.0)
          : numCols_m(numCols),
            minInterpolateInterval_m(minInterpolateInterval),
            freqWidth_m(5),
            freqPrecision_m(1)
            {}
        ///< construct with the number of columns to hold, not including the 'freq' col.
        virtual ~ParamTable()
          {}
        
        void reset(unsigned numCols) 
          { numCols_m = numCols;    
            clear(); }
        ///< discard all contents and resize the number of columns.

        void clear()
          { base_type::clear(); }
        ///< discard all contents, keeping the same number of columns.
       
        bool set(const double &freq, unsigned col, float value);
        ///< set a value at a given 'freq' and 'col'.
        ///< This creates a new row for 'freq' if needed.
        ///< Returns false if 'col' is out of bounds.
        
        float get(const double &freq, unsigned col, bool interpolate = true) const;
        ///< get a value at a given 'freq' and col.
        ///< If 'interpolate' is true, performs linear interpolation between rows if needed,
        ///<   but won't interpolate if the interval is smaller than minInterpolateInterval_m.
        ///< Otherwise returns the value from the nearest row.
        ///< Returns false if table is empty or col is out of bounds.

        bool get(const double &freq, ParamTableRow &target, bool interpolate = true) const;
        ///< get a row of values corresponding to 'freq'.
        ///< If 'interpolate' is true, performs linear interpolation between rows if needed,
        ///<   but won't interpolate if the interval is smaller than minInterpolateInterval_m.
        ///< Otherwise returns the nearest row.
        ///< returns false if the table is empty.

        bool get(const_iterator &it, double &freq, ParamTableRow &target) const;
        ///< get the row of values at location 'it'.  The frequency is placed in 'freq' and the remaining values in 'target'
        ///< returns true if iterator is valid.        
        
        const const_iterator begin() const
          { return base_type::begin(); }
        ///< expose map's const iterator access methods.

        const const_iterator end() const
          { return base_type::end(); }
        ///< expose map's const iterator access methods.
        
        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 
        
    private:
        /// calculate the weight -- a number between 0.0 and 1.0
        /// indicating position of freq between freqLo and freqHi.
        static double calculateWeight(const double &freq,
                                      const double &freqLo, 
                                      const double &freqHi)
          { return (freq - freqLo) / (freqHi - freqLo); }
    };          
    
    inline std::ostream &operator << (std::ostream& out, const ParamTable &table)
      { table.streamOut(out); return out; }
    ///< stream output for debugging ParamTable    
    
    //------------------------------------------------------------------------
    /// lookup table for SIS mixer voltage and current vs LO frequency
    class MixerParams : public ParamTable {
    public:
        /// define parameter names for use with this ParamTable
        enum PARAM_COLS {
            VJ01 = 0,   ///< SIS junction voltage for pol0 sb1
            VJ02,
            VJ11,
            VJ12,
            IJ01,       ///< SIS junction current for pol0 sb1
            IJ02,
            IJ11,
            IJ12,
            NUM_COLS
        };

        MixerParams()
          : ParamTable(NUM_COLS)
            { ParamTable::minInterpolateInterval_m = 0.000001;
              ParamTable::freqPrecision_m = 6; }
        ///< constructor fixes the number of data columns at NUM_COLS.
        ///< Minimum interpolation interval is 1 kHz.
        ~MixerParams()
          {}

        static void normalizePolSb(unsigned &pol, unsigned &sb)
          { if (pol > 1) pol = 0;
            if (sb < 1) sb = 1;
            else if (sb > 2) sb = 2; }
        ///< utility function to force pol and sb into range.
        
        static PARAM_COLS indexVJ(unsigned pol, unsigned sb)
          { normalizePolSb(pol, sb);
            return (PARAM_COLS) (pol * 2 + (sb - 1)); }
        ///< get the parameter name for VJ at the given pol and sb.

        static PARAM_COLS indexIJ(unsigned pol, unsigned sb)
          { normalizePolSb(pol, sb);
            return (PARAM_COLS) (IJ01 + pol * 2 + (sb - 1)); }
        ///< get the parameter name for IJ at the given pol and sb.

        void streamOut(std::ostream& out, bool asIniFileRecord = false) const;
        ///< stream output for debugging
    };

    inline std::ostream &operator << (std::ostream& out, const MixerParams &params)
      { params.streamOut(out); return out; }
    ///< stream output for debugging MixerParams

    //------------------------------------------------------------------------
    /// lookup table for SIS magnet bias parameters
    class MagnetParams : public ParamTable {
    public:
        /// define parameter names for use with this ParamTable
        enum PARAM_COLS {
            IMag01 = 0,     /// SIS magnet current for pol0 sb1
            IMag02,
            IMag11,
            IMag12,
            NUM_COLS
        };

        MagnetParams()
        : ParamTable(NUM_COLS)
        { ParamTable::minInterpolateInterval_m = 0.000001;
          ParamTable::freqPrecision_m = 6; }
        ///< constructor fixes the number of data columns at NUM_COLS.
        ///< Minimum interpolation interval is 1 kHz.
        ~MagnetParams()
          {}

        static PARAM_COLS indexIMag(unsigned pol, unsigned sb)
          { MixerParams::normalizePolSb(pol, sb);
            return (PARAM_COLS) (pol * 2 + (sb - 1)); }
        ///< get the parameter name for IMag at the given pol and sb.

        void streamOut(std::ostream& out) const;
        ///< stream output for debugging
    };

    inline std::ostream &operator << (std::ostream& out, const MagnetParams &params)
      { params.streamOut(out); return out; }
    ///< stream output for debugging MagnetParams
      
    //------------------------------------------------------------------------
    /// lookup table for LNA bias parameters for a single polirization and sideband
    class PreampParams : public ParamTable {
    public:
        /// define parameter names for use with this ParamTable
        enum PARAM_COLS {
            VD1 = 0,    ///< stage 1 drain voltage
            VD2,        ///< stage 2 drain voltage
            VD3,        ///< stage 3 drain voltage
            ID1,        ///< stage 1 drain current
            ID2,        ///< stage 2 drain current
            ID3,        ///< stage 3 drain current
            VG1,        ///< stage 1 gate voltage
            VG2,        ///< stage 2 gate voltage
            VG3,        ///< stage 3 gate voltage
            NUM_COLS
        };

        PreampParams()
          : ParamTable(NUM_COLS)
            {}
        ///< constructor fixes the number of data columns at NUM_COLS. 
        ~PreampParams()
          {}
        
        static void normalizeStage(unsigned &stage)
          { if (stage < 1) stage = 1;
            if (stage > 6) stage = 6; }
        ///< utility function to force pol and sb into range.

        static PARAM_COLS indexVD(unsigned stage)
          { normalizeStage(stage);
            return (PARAM_COLS) ((stage - 1) % 3); }
        ///< get the parameter name for VD at the given stage.

        static PARAM_COLS indexID(unsigned stage)
          { normalizeStage(stage);
            return (PARAM_COLS) ((stage - 1) % 3 + 3); }
        ///< get the parameter name for ID at the given stage.

        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 
    };

    inline std::ostream &operator << (std::ostream& out, const PreampParams &params)
      { params.streamOut(out); return out; }
    ///< stream output for debugging PreampParams
    
    //------------------------------------------------------------------------
    /// lookup table for LO power amp bias parameters
    class PowerAmpParams : public ParamTable {
    public:
        /// define parameter names for use with this ParamTable
        enum PARAM_COLS {
            VD0 = 0,    ///< drain voltage Pol0
            VD1,        ///< drain voltage Pol1
            VG0,        ///< gate voltage Pol0
            VG1,        ///< gate voltage Pol1
            NUM_COLS
        };

        PowerAmpParams()
          : ParamTable(NUM_COLS)
            {}
        ///< constructor fixes the number of data columns at NUM_COLS. 
        ~PowerAmpParams()
          {}

        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 
    };

    inline std::ostream &operator << (std::ostream& out, const PowerAmpParams &params)
      { params.streamOut(out); return out; }
    ///< stream output for debugging PowerAmpParams

    //------------------------------------------------------------------------
    /// lookup table for Cryostat TVO Coefficients
    class TVOCoeffParams : public ParamTable {
    public:
        /// define parameter names for use with this ParamTable
        enum PARAM_COLS {
            Coeff0 = 0,     /// SIS magnet current for pol0 sb1
            Coeff1,
            Coeff2,
            Coeff3,
            Coeff4,
            Coeff5,
            Coeff6,
            NUM_COLS
        };

        TVOCoeffParams()
        : ParamTable(NUM_COLS)
        { ParamTable::minInterpolateInterval_m = 0.000001;
          ParamTable::freqPrecision_m = 6; }
        ///< constructor fixes the number of data columns at NUM_COLS.
        ///< Minimum interpolation interval is 1 kHz.
        ~TVOCoeffParams()
          {}

        void streamOut(std::ostream& out) const;
        ///< stream output for debugging
    };

    inline std::ostream &operator << (std::ostream& out, const TVOCoeffParams &params)
      { params.streamOut(out); return out; }
    ///< stream output for debugging MagnetParams
    
}; // namespace FEConfig

#endif /*LOOKUPTABLES_H_*/
