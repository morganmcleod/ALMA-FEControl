/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2011
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


#include "FEICDataBase.h"
#include "logger.h"
#include "stringConvert.h"
using namespace std;

int main(int argc, char *argv[]) {
    FEICDataBase::ID_T id, hid;
    FEICDataBase::IDList_T list;
    bool valid(false);

    LOG(LM_INFO) << "t_FEICDataBase(" << argc << ", " << argv[1] << ")" << endl;

    FEICDataBase db;
    //("sql.cv.nrao.edu", "na_feic", "qSfUO65a", "alma_feic");

    if (argc < 2) {

        db.getFrontEndList(0, list);

        LOG(LM_INFO) << "t_FEICDataBase: pass the front end serial number as the first argument." << endl;
        return -1;
    }

    unsigned serialNum = numericPortion<unsigned>(argv[1]);

    LOG(LM_INFO) << "serialNum='" << serialNum << "'" << endl;


//    string TS, ESN, docs, description;
//    id = db.getFrontEndRecord(0, serialNum, TS, ESN, docs, description);
//    valid = id.valid();
//    LOG(LM_INFO) << id << " SN=" << serialNum << " TS='" << TS << "' ESN='" << ESN << "' docs='" << docs << "' desc='" << description << "'" << endl;

//    id = db.getConfigId(0, serialNum);

 //   valid = db.isValidConfigId(id);
    if (valid) {

//        hid = db.insertCPDSMonitorData(id, FEICDataBase::DS_HEALTH_CHECK, 6, 241.0, 1,2,3,4,5,6,7,8,9,10,11,12);

//        db.updateFrontEndESN(id, "A0A0A0A012121212");

//        hid = db.insertIfSwitchTemps(id, FEICDataBase::DS_HEALTH_CHECK, 21.23, 100.45, 44.56, 66.789);

//        hid = db.insertLPRMonitorData(id, FEICDataBase::DS_HEALTH_CHECK, 10, 20 , 30 , 40, 50 , 60 , 70 , 80);


//        hid = db.insertCryostatTemps(id, FEICDataBase::DS_HEALTH_CHECK, 10, 20 , 30 , 40, 50 , 60 , 70 , 80, 90, 100, 110, 120, 130);

//        db.createTestDataHeader(id, FEICDataBase::TD_SOFTWARETEST, FEICDataBase::DS_HEALTH_CHECK, 0, "Added \"notes\'");
//        hid = db.findTestDataHeader(id, FEICDataBase::TD_SOFTWARETEST, FEICDataBase::DS_HEALTH_CHECK, 0);
    }
    return 0;
}
