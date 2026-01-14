#ifndef CONFIGPROVIDERXML_H_
#define CONFIGPROVIDERXML_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2019
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

#include "FrontEndConfig.h"
#include <sys/types.h>
#include <stdio.h>
#include <string>
#include <expat.h>
#include <expatmm/expatmm.h>

namespace FEConfig {

class XMLParser : public expatmm::ExpatXMLFileParser {
public:
    XMLParser(std::string filename);

    virtual ~XMLParser()
      {}

    bool getColdCartConfig(ColdCartConfig &target);
    ///< get the ColdCartConfig portion of the specified config for the specified band.
    ///< returns false if that portion is not available or on error.

    bool getWCAConfig(WCAConfig &target);
    ///< get the WCAConfig portion of the specified config for the specified band.
    ///< returns false if that portion is not available or on error.

    bool getCryostatConfig(CryostatConfig &target);
    ///< get the WCAConfig portion of the specified config for the specified band.
    ///< returns false if that portion is not available or on error.

protected:
    virtual void StartElement(const XML_Char *name, const XML_Char **attrs);
    virtual void EndElement(const XML_Char *name);

private:
    void assignPreampParam(unsigned pol, unsigned sis, double freqLO, const std::string &paramName, double value);

    void initAttrStrings() const;

    typedef enum {
        TYPE_UNDEFINED,
        TYPE_COLDCART,
        TYPE_WCA,
        TYPE_CRYOSTAT,
        TYPE_LPR,
    } TargetType_t;

    TargetType_t targetType_m;

    union {
        ColdCartConfig *cca_p;
        WCAConfig *wca_p;
        CryostatConfig *cryostat_p;
    } target_m;
};

} // namespace FEConfig

#endif /*CONFIGPROVIDERXML_H_*/


