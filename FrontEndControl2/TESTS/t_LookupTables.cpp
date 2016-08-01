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

#include "../CONFIG/LookupTables.h"
using namespace FEConfig;
using namespace std;

int main(int, char*[]) {
    {
        ParamTableRow row(4);
        cout << "ParamTableRow(4): " << row << endl;
        
        MixerParams params;
        cout << "MixerParams(): " << params << endl;
        
        double freq;
        float bias;
        for (int i = 0; i < 10; ++i) {
            freq = 100 + (i * 10);
            bias = i * 10;
            params.set(freq, MixerParams::VJ01, bias);
            params.set(freq, MixerParams::VJ02, bias + 2.5);
            params.set(freq, MixerParams::VJ11, bias + 5);
            params.set(freq, MixerParams::VJ12, bias + 7.5);
        }
        params.set(99.0, MixerParams::VJ01, 0.5);           // to test minInterpolateInterval = 1kHz.
        params.set(99.000001, MixerParams::VJ01, 0.6);      //
        params.set(99.000003, MixerParams::VJ01, 0.9);      //

        cout << params << endl;
        
        cout << endl << "This test should return the first row of the table:" << endl;
        freq = 0;
        cout << "MixerParams get(" << freq << ") ";
        if (!params.get(freq, row))
             cout << "failed." << endl;
        else
             cout << "= " << setprecision(7) << freq << ": " << row << endl;

        cout << endl << "This test should interpolate between the 100 and 110 rows:" << endl;
        freq = 105;
        cout << "MixerParams get(" << freq << ") ";
        if (!params.get(freq, row))
             cout << "failed." << endl;
        else
             cout << "= " << setprecision(7) << freq << ": " << row << endl;

        cout << endl << "This test should interpolate between the 100 and 110 rows:" << endl;
        freq = 109;
        cout << "MixerParams get(" << freq << ") ";
        if (!params.get(freq, row))
             cout << "failed." << endl;
        else
             cout << "= " << setprecision(7) << freq << ": " << row << endl;

        cout << endl << "This test should return the last row of the table:" << endl;
        freq = 300;
        cout << "MixerParams get(" << freq << ") ";
        if (!params.get(freq, row))
             cout << "failed." << endl;
        else
             cout << "= " << setprecision(7) << freq << ": " << row << endl;

        cout << endl << "This lookup falls within the minInterpolateInterval and should return the row for 99.0:" << endl;
        freq = 99.0000004;
        cout << setprecision(7) << "MixerParams get(" << freq << ") ";
        if (!params.get(freq, row))
             cout << "failed." << endl;
        else
             cout << "= " << setprecision(7) << freq << ": " << row << endl;

        cout << endl << "This lookup falls within the minInterpolateInterval and should return the row for 99.000001:" << endl;
        freq = 99.0000005;
        cout << setprecision(7) << "MixerParams get(" << freq << ") ";
        if (!params.get(freq, row))
             cout << "failed." << endl;
        else
             cout << "= " << setprecision(7) << freq << ": " << row << endl;

        cout << endl << "This test should interpolate between 99.000001 and 99.000003:" << endl;
        freq = 99.0000015;
        cout << setprecision(7) << "MixerParams get(" << freq << ") ";
        if (!params.get(freq, row))
             cout << "failed." << endl;
        else
             cout << "= " << setprecision(7) << freq << ": " << row << endl;

        cout << endl << "This test should return the row for 100.0:" << endl;
        freq = 101;
        cout << setprecision(7) << "MixerParams get(" << freq << ", interpolate=false) ";
        if (!params.get(freq, row, false))
             cout << "failed." << endl;
        else
             cout << "= " << setprecision(7) << freq << ": " << row << endl;

        cout << endl << "This test should return the row for 110.0:" << endl;
        freq = 105;
        cout << "MixerParams get(" << freq << ", interpolate=false) ";
        if (!params.get(freq, row, false))
             cout << "failed." << endl;
        else
             cout << "= " << setprecision(7) << freq << ": " << row << endl;

        cout << endl << "This test should return the row for 110.0:" << endl;
        freq = 109;
        cout << "MixerParams get(" << freq << ", interpolate=false) ";
        if (!params.get(freq, row, false))
             cout << "failed." << endl;
        else
             cout << "= " << setprecision(7) << freq << ": " << row << endl;
    }
    return 0;
}
