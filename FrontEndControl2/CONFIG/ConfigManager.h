#ifndef CONFIGMANAGER_H_
#define CONFIGMANAGER_H_
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
/// \brief declaration of class FEConfig::ConfigManager. 

#include "ConfigProvider.h"
class FrontEndImpl;
class SignalSourceImpl;

namespace FEConfig {

    /// ConfigManager uses a Configuration object to load operating parameters into
    ///   FrontEndImpl, SignalSourceImpl objects
    class ConfigManager {
    public:
        ConfigManager()
          {}
        ///< construct with a reference to the ConfigProvider to use.
        ~ConfigManager()
          {}

        bool configure(const Configuration &config, FrontEndImpl &target) const;
        ///< Apply the given Configuration to the target FrontEndImpl object.
        ///< returns true if no error.
        
        bool configure(const Configuration &config, SignalSourceImpl &target) const;
        ///< Apply the given Configuration to the target SignalSourceImpl object.
        ///< returns true if no error.

    private:
        ConfigManager(const ConfigManager &other);
        ///< copy construct is private and forbidden.
        ConfigManager &operator =(const ConfigManager &other);
        ///< assignment is private and forbidden.
    
        bool configureCryostat(const CryostatConfig &cryo, FrontEndImpl &target) const;
        ///< helper to apply a CryostatConfig to the target.
        bool configureLPR(const LPRConfig &lpr, FrontEndImpl &target) const;
        ///< helper to apply a LPRConfig to the target.
        bool configureCartAssembly(const CartAssemblyConfig &cartAssy, FrontEndImpl &target) const;
        bool configureCartAssembly(const CartAssemblyConfig &cartAssy, SignalSourceImpl &target) const;
        ///< helpers to apply a CartAssemblyConfig to the target.
        bool configureColdCart(unsigned port, const ColdCartConfig &coldCart, FrontEndImpl &target) const;
        ///< helper to apply a ColdCartConfig to the target.
        bool configureWCA(unsigned port, const WCAConfig &WCA, FrontEndImpl &target) const;
        bool configureWCA(unsigned port, const WCAConfig &wcaConfig, SignalSourceImpl &target) const;
        ///< helpers to apply a WCAConfig to the target.
    };

}; // namespace FEConfig;

#endif /*CONFIGMANAGER_H_*/
