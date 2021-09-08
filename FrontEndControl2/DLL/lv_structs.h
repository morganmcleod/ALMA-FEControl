#ifndef LV_STRUCTS_H_
#define LV_STRUCTS_H_
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
*
*/

/************************************************************************
 * declarations for the data structures and utilities for passing
 * data to and from LabVIEW.  
 * LabVIEW clusters of scalarsare passed to C code flat and in-line.
 * The main difference between GCC alignment rules and LabVIEW's is
 * that GCC aligns unsigned characters in the first byte of four
 * whereas LabVIEW allocates only one byte.
 * Therefore these structures contain only an array of unsigned char
 *----------------------------------------------------------------------
 */

#include <iostream>
#include <string.h>		// for memset

//----------------------------------------------------------------------------
// Template functions for getting and setting a variety of types from within a data array:
// 'field' is an index into offsets[] to finde where in data[] to find a value of type T.

template<typename T>
static T LVGet(int field, const unsigned char data[], const int offsets[], const T &) {
    const T *target = reinterpret_cast<const T *>(data + offsets[field]);
    if (target)
        return *target;
    else
        return 0;
}

template<typename T>
static void LVSet(int field, unsigned char data[], const int offsets[], const T &value) {
    T *target = reinterpret_cast<T *>(data + offsets[field]);
    if (target)
        *target = value;
}

inline bool LVGetBool(int field, const unsigned char data[], const int offsets[]) {
    static unsigned char example(0);
    return (LVGet(field, data, offsets, example) != 0);
}

inline void LVSetBool(int field, unsigned char data[], const int offsets[], const bool &_value) {
    unsigned char value = _value ? 1 : 0;
    LVSet(field, data, offsets, value);
}    

inline float LVGetFloat(int field, const unsigned char data[], const int offsets[]) { 
    static float example(0);
    return LVGet(field, data, offsets, example);
}
    
inline void LVSetFloat(int field, unsigned char data[], const int offsets[], const float &value) {
    LVSet(field, data, offsets, value); 
}

inline short LVGetShort(int field, const unsigned char data[], const int offsets[]) {
    static short example(0);
    return LVGet(field, data, offsets, example);
}

inline void LVSetShort(int field, unsigned char data[], const int offsets[], const short &value) {
    LVSet(field, data, offsets, value); 
}

inline unsigned long LVGetULong(int field, const unsigned char data[], const int offsets[]) {
    static unsigned long example(0);
    return LVGet(field, data, offsets, example);
}

inline void LVSetULong(int field, unsigned char data[], const int offsets[], const unsigned long &value) {
    LVSet(field, data, offsets, value);
}

inline unsigned char LVGetByte(int field, const unsigned char data[], const int offsets[]) {
    static unsigned char example(0);
    return LVGet(field, data, offsets, example);
}

inline void LVSetByte(int field, unsigned char data[], const int offsets[], const unsigned char &value) {
    LVSet(field, data, offsets, value); 
}


//----------------------------------------------------------------------------
// Structure to return cartridge state data:

struct CartStateData_t {
    
    enum Field {
        // All fields are 1-byte unsigned char:
        CART_ENABLED, 
        CART_OBSERVING,
        CART_LNA_ENABLED,
        CART_SIS_ENABLED,
        CART_SIS_MAGNET_ENABLED,
        CART_SIS_HEATER_ENABLED,
        CART_HEMT_LED_ENABLED,
        CART_LO_PA_ENABLED,
        CART_PHOTOMIXER_ENABLED,
        CART_ZERO_INTEGRATOR_ENABLED
    };
    
    unsigned char data[10];
    static const int offsets[];

    CartStateData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    bool getBool(Field field) const
      { return LVGetBool(field, data, offsets); }
    
    void setBool(Field field, const bool &value)
      { LVSetBool(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartStateData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartStateData_t    


//----------------------------------------------------------------------------
// Structure to save cartridge health check IF power data:

struct CartIFPowerData_t {
    enum Field {
        // All fields are 4-byte float:
         PHOT0_IF0,
         PHOT0_IF1,
         PHOT0_IF2,
         PHOT0_IF3,     //16
         PHOT15_IF0,
         PHOT15_IF1,
         PHOT15_IF2,
         PHOT15_IF3,    //32
         PCOLD_IF0,
         PCOLD_IF1,
         PCOLD_IF2,
         PCOLD_IF3,     //48
         YFACTOR_IF0,
         YFACTOR_IF1,
         YFACTOR_IF2,
         YFACTOR_IF3    //64
    };

    unsigned char data[64];
    static const int offsets[];

    CartIFPowerData_t()
      { reset(); }

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }

    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartIFPowerData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartStateData_t

//----------------------------------------------------------------------------
// Structure to return cartridge YTO data:

struct CartYTOData_t {
    
    enum Field {
        YTO_COARSE_TUNE,        // 2 byte unsigned short int
        YTO_FREQUENCY,          // 8 byte double
    };
    unsigned char data[10];
    static const int offsets[];

    CartYTOData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    short getCoarseTune() const
      { return LVGetShort(YTO_COARSE_TUNE, data, offsets); }
    
    void setCoarseTune(const short &value) 
      { LVSetShort(YTO_COARSE_TUNE, data, offsets, value); }
    
    double getFrequency() const 
      { return LVGet(YTO_FREQUENCY, data, offsets, double(0)); }
        
    void setFrequency(const double &value) 
      { LVSet(YTO_FREQUENCY, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartYTOData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartYTOData_t    

//----------------------------------------------------------------------------
// Structure to return cartridge Photomixer data:
    
struct CartPhotomixerData_t {
    enum Field {
        PHOTOMIXER_ENABLE,      // 1 byte unsigned char
        PHOTOMIXER_VOLTAGE,     // 4 byte float
        PHOTOMIXER_CURRENT      // 4 byte float
    };
    unsigned char data[9];
    static const int offsets[];

    CartPhotomixerData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    bool getEnable() const 
      { return LVGetBool(PHOTOMIXER_ENABLE, data, offsets); }
    
    void setEnable(const bool &value) 
      { LVSetBool(PHOTOMIXER_ENABLE, data, offsets, value); }
    
    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }
    
    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartPhotomixerData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartPhotomixerData_t    

//----------------------------------------------------------------------------
// Structure to return cartridge PLL data:

struct CartPLLData_t {
    enum Field {
        PLL_LOCK,                       // 1 byte unsigned char
        PLL_UNLOCK_DETECT_LATCH,        // 1 byte unsigned char
        PLL_LOCK_DETECT_VOLTAGE,        // 4 byte float 
        PLL_CORRECTION_VOLTAGE,         // 4 byte float
        PLL_REF_TOTAL_POWER,            // 4 byte float
        PLL_IF_TOTAL_POWER,             // 4 byte float
        PLL_LOOP_BANDWIDTH_SELECT,      // 1 byte unsigned char
        PLL_SIDEBAND_LOCK_POL_SELECT,   // 1 byte unsigned char
        PLL_NULL_LOOP_INTEGRATOR,       // 1 byte unsigned char            
        PLL_ASSEMBLY_TEMP,              // 4 byte float
        PLL_YTO_HEATER_CURRENT          // 4 byte float
    };
    unsigned char data[29];
    static const int offsets[];

    CartPLLData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    bool getBool(Field field) const
      { return LVGetBool(field, data, offsets); }
    
    void setBool(Field field, const bool &value)
      { LVSetBool(field, data, offsets, value); }
    
    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartPLLData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartPLLData_t    

//----------------------------------------------------------------------------
// Structure to return cartridge AMC data:

struct CartAMCData_t {
    enum Field {
        // All fields are 4-byte float:
        AMC_DRAIN_A_VOLTAGE,
        AMC_DRAIN_A_CURRENT,
        AMC_GATE_A_VOLTAGE,
        AMC_DRAIN_B_VOLTAGE,
        AMC_DRAIN_B_CURRENT,
        AMC_GATE_B_VOLTAGE,
        AMC_DRAIN_E_VOLTAGE,
        AMC_DRAIN_E_CURRENT,
        AMC_GATE_E_VOLTAGE,
        AMC_MULTIPLIER_D_CURRENT,
        AMC_SUPPLY_VOLTAGE_5V,
        AMC_MULTIPLIER_D_COUNTS
    };
    unsigned char data[45];
    static const int offsets[];

    CartAMCData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    unsigned char getByte(Field field) const
      { return LVGetByte(field, data, offsets); }
    
    void setByte(Field field, const unsigned char &value)
      { LVSetByte(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartAMCData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartAMCData_t    

//----------------------------------------------------------------------------
// Structure to return cartridge Power Amp data:

struct CartPAData_t {
    enum Field {
        // All fields are 4-byte float:
        PA_POL0_DRAIN_VOLTAGE,
        PA_POL0_DRAIN_CURRENT,
        PA_POL0_GATE_VOLTAGE,
        PA_POL1_DRAIN_VOLTAGE,
        PA_POL1_DRAIN_CURRENT,
        PA_POL1_GATE_VOLTAGE,
        PA_SUPPLY_VOLTAGE_3V,
        PA_SUPPLY_VOLTAGE_5V
    };
    unsigned char data[32];
    static const int offsets[];

    CartPAData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }
    
    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartPAData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartPAData_t    

//----------------------------------------------------------------------------
// Structure to return cartridge Temperature data:

struct CartTempData_t {
    enum Field {
        // All fields are 4-byte float:
        CARTRIDGE_TEMP0,    
        CARTRIDGE_TEMP1,
        CARTRIDGE_TEMP2,
        CARTRIDGE_TEMP3,
        CARTRIDGE_TEMP4,
        CARTRIDGE_TEMP5
    };
    unsigned char data[24];
    static const int offsets[];

    CartTempData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartTempData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartTempData_t    

//----------------------------------------------------------------------------
// Structure to return cartridge SIS data:

struct CartSISData_t {
    enum Field {
        SIS_OPEN_LOOP,      // 1 byte unsigned char
        SIS_VOLTAGE,        // 4 byte float
        SIS_CURRENT,        // 4 byte float
        SIS_MAGNET_VOLTAGE, // 4 byte float
        SIS_MAGNET_CURRENT  // 4 byte float
    };
    unsigned char data[17];
    static const int offsets[];

    CartSISData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    bool getOpenLoop() const 
      { return LVGetBool(SIS_OPEN_LOOP, data, offsets); }
    
    void setOpenLoop(const bool &value) 
      { LVSetBool(SIS_OPEN_LOOP, data, offsets, value); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartSISData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartSISData_t    

//----------------------------------------------------------------------------
// Structure to return cartridge LNA data:

struct CartLNAData_t {
    enum Field {
        LNA_ENABLE,         // 1 byte unsigned char
        LNA1_DRAIN_VOLTAGE, // 4 byte float
        LNA1_DRAIN_CURRENT, // 4 byte float
        LNA1_GATE_VOLTAGE,  // 4 byte float
        LNA2_DRAIN_VOLTAGE, // 4 byte float
        LNA2_DRAIN_CURRENT, // 4 byte float
        LNA2_GATE_VOLTAGE,  // 4 byte float
        LNA3_DRAIN_VOLTAGE, // 4 byte float
        LNA3_DRAIN_CURRENT, // 4 byte float
        LNA3_GATE_VOLTAGE   // 4 byte float (37)
    };        
    unsigned char data[37];
    static const int offsets[];

    CartLNAData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    bool getEnable() const 
      { return LVGetBool(LNA_ENABLE, data, offsets); }
    
    void setEnable(const bool &value) 
      { LVSetBool(LNA_ENABLE, data, offsets, value); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartLNAData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartLNAData_t    

//----------------------------------------------------------------------------
// Structure to return cold cartridge auxiliary data:
    
struct CartAuxData_t {
    enum Field {
        SIS_HEATER_CURRENT, // 4 byte float
        LNA_LED_ENABLE      // 1 byte unsigned char
    };
    unsigned char data[5];
    static const int offsets[];

    CartAuxData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getHeaterCurrent() const 
      { return LVGetFloat(SIS_HEATER_CURRENT, data, offsets); }
        
    void setHeaterCurrent(const float &value) 
      { LVSetFloat(SIS_HEATER_CURRENT, data, offsets, value); } 

    bool getLEDEnable() const 
      { return LVGetBool(LNA_LED_ENABLE, data, offsets); }
    
    void setLEDEnable(const bool &value) 
      { LVSetBool(LNA_LED_ENABLE, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CartAuxData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartAuxData_t    

//----------------------------------------------------------------------------
// Structure to return Power Distribution module data:
    
struct PowerModuleData_t {
    enum Field {
        VOLTAGE_P6V,    // 4 byte float
        CURRENT_P6V,    // 4 byte float
        VOLTAGE_N6V,    // 4 byte float
        CURRENT_N6V,    // 4 byte float
        VOLTAGE_P15V,   // 4 byte float
        CURRENT_P15V,   // 4 byte float
        VOLTAGE_N15V,   // 4 byte float
        CURRENT_N15V,   // 4 byte float
        VOLTAGE_P24V,   // 4 byte float
        CURRENT_P24V,   // 4 byte float
        VOLTAGE_P8V,    // 4 byte float
        CURRENT_P8V,    // 4 byte float
        ENABLE_MODULE   // 1 byte unsigned char (49)
    };
    unsigned char data[49];
    static const int offsets[];

    PowerModuleData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    bool getEnable() const
      { return LVGetBool(ENABLE_MODULE, data, offsets); }
    
    void setEnable(const bool &value) 
      { LVSetBool(ENABLE_MODULE, data, offsets, value); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const PowerModuleData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CartAuxData_t    

//----------------------------------------------------------------------------
// Structure to return IF Switch module data:
    
struct IFSwitchData_t {
    
    enum Field {
        ATTENUATION_POL0SB1,    // 1 byte unsigned char
        ATTENUATION_POL0SB2,    // 1 byte unsigned char
        ATTENUATION_POL1SB1,    // 1 byte unsigned char
        ATTENUATION_POL1SB2,    // 1 byte unsigned char
        TEMPERATURE_POL0SB1,    // 4 byte float
        TEMPERATURE_POL0SB2,    // 4 byte float
        TEMPERATURE_POL1SB1,    // 4 byte float
        TEMPERATURE_POL1SB2,    // 4 byte float (20)
        TEMPSERVO_POL0SB1,      // 1 byte unsigned char
        TEMPSERVO_POL0SB2,      // 1 byte unsigned char
        TEMPSERVO_POL1SB1,      // 1 byte unsigned char
        TEMPSERVO_POL1SB2,      // 1 byte unsigned char
        OBSERVING_BAND          // 1 byte unsigned char (25)
    };
    unsigned char data[25];
    static const int offsets[];

    IFSwitchData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    unsigned char getByte(Field field) const
      { return LVGetByte(field, data, offsets); }
    
    void setByte(Field field, const unsigned char &value)
      { LVSetByte(field, data, offsets, value); }
    
    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const IFSwitchData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging IFSwitchData_t    

//----------------------------------------------------------------------------
// Structure to return Cryostat M&C module data:
    
struct CryostatData_t {
    
    enum Field {
        CRYOSTAT_TEMP_0,        // 4 byte float
        CRYOSTAT_TEMP_1,        // 4 byte float
        CRYOSTAT_TEMP_2,        // 4 byte float
        CRYOSTAT_TEMP_3,        // 4 byte float
        CRYOSTAT_TEMP_4,        // 4 byte float
        CRYOSTAT_TEMP_5,        // 4 byte float
        CRYOSTAT_TEMP_6,        // 4 byte float
        CRYOSTAT_TEMP_7,        // 4 byte float
        CRYOSTAT_TEMP_8,        // 4 byte float
        CRYOSTAT_TEMP_9,        // 4 byte float
        CRYOSTAT_TEMP_10,       // 4 byte float
        CRYOSTAT_TEMP_11,       // 4 byte float
        CRYOSTAT_TEMP_12,       // 4 byte float (52)
        SUPPLY_CURRENT_230V,    // 4 byte float
        VACUUM_CRYOSTAT_PRES,   // 4 byte float
        VACUUM_PORT_PRES,       // 4 byte float (64)
        BACKING_PUMP_ENABLE,    // 1 byte unsigned char
        TURBO_PUMP_ENABLE,      // 1 byte unsigned char
        TURBO_PUMP_STATE,       // 1 byte unsigned char
        TURBO_PUMP_SPEED,       // 1 byte unsigned char
        GATE_VALVE_STATE,       // 1 byte unsigned char
        SOLENOID_VALVE_STATE,   // 1 byte unsigned char      
        VACUUM_GAUGE_ENABLE,    // 1 byte unsigned char
        VACUUM_GAUGE_STATE      // 1 byte unsigned char (72)
    };
    unsigned char data[72];
    static const int offsets[];

    CryostatData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    unsigned char getByte(Field field) const
      { return LVGetByte(field, data, offsets); }
    
    void setByte(Field field, const unsigned char &value)
      { LVSetByte(field, data, offsets, value); }

    bool getBool(Field field) const
      { return LVGetBool(field, data, offsets); }
    
    void setBool(Field field, const bool &value)
      { LVSetBool(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CryostatData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging CryostatData_t    

//----------------------------------------------------------------------------
// Structure to return IF Switch module data:
    
struct LPRData_t {
    
    enum Field {
        OPTICAL_SWITCH_PORT,            // 1 byte unsigned char        
        OPTICAL_SWITCH_SHUTTER,         // 1 byte unsigned char
        OPTICAL_SWITCH_STATE,           // 1 byte unsigned char
        OPTICAL_SWITCH_BUSY,            // 1 byte unsigned char (4)
        LPR_TEMPERATURE_0,              // 4 byte float
        LPR_TEMPERATURE_1,              // 4 byte float
        EDFA_LASER_PUMP_TEMPERATURE,    // 4 byte float
        EDFA_LASER_DRIVE_CURRENT,       // 4 byte float (20)
        EDFA_LASER_PHOTODETECT_CURRENT, // 4 byte float
        EDFA_PHOTODETECT_CURRENT,       // 4 byte float
        EDFA_PHOTODETECT_POWER,         // 4 byte float
        EDFA_MODULATION_INPUT           // 4 byte float (36)
    };
    unsigned char data[36];
    static const int offsets[];

    LPRData_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    unsigned char getByte(Field field) const
      { return LVGetByte(field, data, offsets); }
    
    void setByte(Field field, const unsigned char &value)
      { LVSetByte(field, data, offsets, value); }
    
    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const LPRData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging LPRData_t    

//----------------------------------------------------------------------------
// Structure to return LORTM status data:
    
struct LORTMStatus_t {
    enum Field {
        SYSTEM_STATE,               // 1 byte unsigned char
        SYSTEM_WARNING,             // 1 byte boolean
        SYSTEM_ERROR,               // 1 byte boolean
        OPERATION_PENDING,          // 1 byte boolean (4)
        INTERLOCK_STATUS,           // 1 byte boolean
        SYSTEM_WARNING_CODE,        // 2 byte int
        SYSTEM_ERROR_CODE,          // 2 byte int
        PHASELOCK_LOCKED,           // 1 byte boolean (10)
        PHASELOCK_TUNING_READY,     // 1 byte boolean
        PHASELOCK_RF_INPUT_READY,   // 1 byte boolean
        SELECTED_LASER,             // 1 byte unsigned char
        SELECTED_BAND               // 1 byte unsigned char (14)
    };
    unsigned char data[4];
    static const int offsets[];
    
    LORTMStatus_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    unsigned char getByte(Field field) const
      { return LVGetByte(field, data, offsets); }
    
    void setByte(Field field, const unsigned char &value)
      { LVSetByte(field, data, offsets, value); }

    bool getBool(Field field) const
      { return LVGetBool(field, data, offsets); }
    
    void setBool(Field field, const bool &value)
      { LVSetBool(field, data, offsets, value); }    
    
    short getShort(Field field) const
      { return LVGetShort(field, data, offsets); }
    
    void setShort(Field field, const short &value)
      { LVSetShort(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const LORTMStatus_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging LORTMStatus_t    

//----------------------------------------------------------------------------
// Structure to return LORTM lasers data:
    
struct LORTMLasers_t {
    enum Field {
        LASER_STATUS1,          // 1 byte unsigned char
        LASER_STATUS2,          // 1 byte unsigned char
        LASER_STATUS3,          // 1 byte unsigned char
        LASER_STATUS4,          // 1 byte unsigned char (4)
        LASER_BIAS_MON1,        // 4 byte float
        LASER_BIAS_MON2,        // 4 byte float
        LASER_BIAS_MON3,        // 4 byte float (16)
        LASER_BIAS_MON4,        // 4 byte float 
        LASER_TEMP_MON1,        // 4 byte float
        LASER_TEMP_MON2,        // 4 byte float
        LASER_TEMP_MON3,        // 4 byte float (32)
        LASER_TEMP_MON4,        // 4 byte float 
        LASER_SLOW_CORR_MON     // 4 byte float (40)
    };
    unsigned char data[40];
    static const int offsets[];
    
    LORTMLasers_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    unsigned char getByte(Field field) const
      { return LVGetByte(field, data, offsets); }
    
    void setByte(Field field, const unsigned char &value)
      { LVSetByte(field, data, offsets, value); }
    
    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const LORTMLasers_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging LORTMLasers_t    

//----------------------------------------------------------------------------
// Structure to return LORTM miscellaneous data:
    
struct LORTMOther_t {
    enum Field {
        LASER_TEC_I_MON1,        // 4 byte float
        LASER_TEC_I_MON2,        // 4 byte float
        LASER_TEC_I_MON3,        // 4 byte float
        LASER_TEC_I_MON4,        // 4 byte float (16)
        LASER_POW_MON1,          // 4 byte float
        LASER_POW_MON2,          // 4 byte float
        LASER_POW_MON3,          // 4 byte float
        LASER_POW_MON4,          // 4 byte float (32)
        PHMIX_BIAS_MON1,         // 4 byte float
        PHMIX_BIAS_MON2,         // 4 byte float
        PHMIX_BIAS_MON3,         // 4 byte float
        PHMIX_BIAS_MON4,         // 4 byte float (48)
        RF_AGC_GAIN_MON,         // 4 byte float
        RF_POW_MON_34DB,         // 4 byte float
        EXTERN_THERM_MON,        // 4 byte float 
        INTERN_THERM_MON         // 4 byte float (64)
    };
    unsigned char data[64];
    static const int offsets[];
    
    LORTMOther_t()
      { reset(); } 

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }
    
    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const LORTMOther_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging LORTMOther_t    

//----------------------------------------------------------------------------
// Structure to return FETIM data:

struct FETIMData_t {
    enum Field {
        INTERNAL_TEMP1,         // 4 byte float
        INTERNAL_TEMP2,         // 4 byte float
        INTERNAL_TEMP3,         // 4 byte float
        INTERNAL_TEMP4,         // 4 byte float (16)
        INTERNAL_TEMP5,         // 4 byte float
        EXTERNAL_TEMP1,         // 4 byte float
        EXTERNAL_TEMP2,         // 4 byte float
        AIRFLOW1,               // 4 byte float (32)
        AIRFLOW2,               // 4 byte float
        HE2_PRESSURE,           // 4 byte float
        GLITCH_VALUE,           // 4 byte float (44)
        INTERNAL_TEMP_OOR,      // 1 byte boolean
        EXTERNAL_TEMP1_OOR,     // 1 byte boolean
        EXTERNAL_TEMP2_OOR,     // 1 byte boolean
        AIRFLOW_OOR,            // 1 byte boolean (48)
        HE2_PRESSURE_OOR,       // 1 byte boolean
        SENSOR_SINGLE_FAIL,     // 1 byte boolean
        SENSOR_MULTI_FAIL,      // 1 byte boolean
        GLITCH_COUNTER_TRIG,    // 1 byte boolean (52)
        DELAY_SHUTDOWN_TRIG,    // 1 byte boolean
        FINAL_SHUTDOWN_TRIG,    // 1 byte boolean
        COMP_STATUS,            // 1 byte boolean
        COMP_INTERLOCK_STATUS,  // 1 byte boolean (56)
        COMP_CABLE_STATUS       // 1 byte boolean (57)
    };
    unsigned char data[57];
    static const int offsets[];

    FETIMData_t()
      { reset(); }

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }

    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    bool getBool(Field field) const
      { return LVGetBool(field, data, offsets); }

    void setBool(Field field, const bool &value)
      { LVSetBool(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const FETIMData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging FETIMData_t


//----------------------------------------------------------------------------
// Structure to return compressor M&C data:

struct CompressorData_t {
    enum Field {
        TEMP1,                  // 4 byte float
        TEMP2,                  // 4 byte float
        TEMP3,                  // 4 byte float
        TEMP4,                  // 4 byte float (16)
        RETURN_PRESSURE,        // 4 byte float
        SUPPLY_PRESSURE,        // 4 byte float
        AUX_TRANSDUCER,         // 4 byte float
        TIME_SINCE_POWER_ON,    // 4 byte unsigned long (32)
        TIME_SINCE_POWER_OFF,   // 4 byte unsigned long (36)
        TEMP_ALARM,             // 1 byte boolean
        PRESSURE_ALARM,         // 1 byte boolean
        DRIVE_INDICATOR,        // 1 byte boolean
        ICCU_STATUS,            // 1 byte boolean (40)
        ICCU_CABLE_ERROR,       // 1 byte boolean
        FETIM_STATUS,           // 1 byte boolean
        FETIM_CABLE_ERROR,      // 1 byte boolean
        INTERLOCK_OVERRIDE,     // 1 byte boolean (44)
        FAULT_STATUS            // 1 byte boolean (45)
    };

    unsigned char data[45];
    static const int offsets[];

    CompressorData_t()
      { reset(); }

    void reset()
      { memset(data, 0, sizeof(data)); }

    float getFloat(Field field) const
      { return LVGetFloat(field, data, offsets); }

    void setFloat(Field field, const float &value)
      { LVSetFloat(field, data, offsets, value); }

    unsigned long getULong(Field field) const
      { return LVGetULong(field, data, offsets); }

    void setULong(Field field, const unsigned long &value)
      { LVSetULong(field, data, offsets, value); }

    bool getBool(Field field) const
      { return LVGetBool(field, data, offsets); }

    void setBool(Field field, const bool &value)
      { LVSetBool(field, data, offsets, value); }

    void streamOut(std::ostream& out) const;
    ///< stream output for debugging
};

inline std::ostream &operator << (std::ostream& out, const CompressorData_t &data)
  { data.streamOut(out); return out; }
///< stream output for debugging FETIMData_t


#endif /*LV_STRUCTS_H_*/
