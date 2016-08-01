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

#include "Maximizer.h"
#include "logger.h"
#include <iomanip>
using namespace std;


void Maximizer::reset() {
    controlMin_m = 0;
    controlMax_m = 0;
    depVal_m = 0;
    depValReady_m = false;
    stop_m = false;
}


bool Maximizer::maximize(float &control, float controlStep, 
                         float controlMin, float controlMax, bool directionUp,
                         int maxIter, int timeOut, int interval) 
{
    stop_m = false;

    controlMin_m = controlMin;
    controlMax_m = controlMax;

    // set the search direction:
    int direction = directionUp ? 1 : -1;
    
    // set iterations = 0
    int iter = 0;
    
    // collect initial data:  A1-A4 are independent values.  
    // P1-P4 are the corresponding dependent values.
    float A1, A2, A3, A4, P1, P2, P3, P4;
    
    // let A1=the first control value:        
    A1 = control;
    if (!controlRangeCheck(A1)) {
        LOG(LM_ERROR) << "Maximizer control value A1 is out of range." << endl;
        return false;
    }   
    // set the first control value:
    if (!setControlValue(A1)) {
        LOG(LM_ERROR) << "Maximizer setControlValue(A1) failed." << endl;
        return false;
    }        
    // get P1=the first dependent value:
    if (!waitForDependentValue(P1, timeOut, interval)) {
        LOG(LM_ERROR) << "Maximizer waitForDependentValue(P1) failed." << endl;
        return false;
    }
    
    // step in the search direction
    control += (controlStep * direction);

    // let A2=the second control value:
    A2 = control;
    if (!controlRangeCheck(A2)) {
        LOG(LM_ERROR) << "Maximizer control value A2 is out of range." << endl;
        return false;
    }   
    // set the second control value:
    if (!setControlValue(A2)) {
        LOG(LM_ERROR) << "Maximizer setControlValue(A2) failed." << endl;
        return false;
    }        
    // get P2=the second dependent value:
    if (!waitForDependentValue(P2, timeOut, interval)) {
        LOG(LM_ERROR) << "Maximizer waitForDependentValue(P2) failed." << endl;
        return false;
    }
    
    // step in the search direction
    control += (controlStep * direction);

    // let A3=the third control value:
    A3 = control;
    if (!controlRangeCheck(A3)) {
        LOG(LM_ERROR) << "Maximizer control value A3 is out of range." << endl;
        return false;
    }   
    // set the third control value:
    if (!setControlValue(A3)) {
        LOG(LM_ERROR) << "Maximizer setControlValue(A3) failed." << endl;
        return false;
    }        
    // get P3=the third dependent value:
    if (!waitForDependentValue(P3, timeOut, interval)) {
        LOG(LM_ERROR) << "Maximizer waitForDependentValue(P3) failed." << endl;
        return false;
    }

    int currentState = CASE_error;
    bool success = false;
    while (!success && !stop_m) {
        // analysis section: analyzes the three dependent values: P1, P2, and P3,
        // and it makes a decision about the next movement.
        // There are six principal cases:
        // case a1, a2, b1, b2, c1, c2. There are also variations in case of high noise.

        LOG(LM_DEBUG) << "Maximizer analysis: A1=" << A1 << " P1=" << P1 
                                       << " A2=" << A2 << " P2=" << P2 
                                       << " A3=" << A3 << " P3=" << A3 << endl;
        
        // increment iterations and test for maximum iterations:
        if (++iter >= maxIter) {
            LOG(LM_ERROR) << "Maximizer failed: too many iterations." << endl;
            return false;            
        }
        //  case 0 non starter:
        //      If P1 = P2 And P2 = P3 Then Something is wrong, Report an error and exit.
        if (P1 == P2 && P2 == P3) {
            LOG(LM_ERROR) << "Maximizer failed: P1 = P2 And P2 = P3." << endl;
            return false;            
        }
        //  analyze case a2:
        //      If P3 >= P2 And P2 >= P1 And search direction is DOWN goto Case_a2
        if (P3 >= P2 && P2 >= P1 && direction == -1)
            currentState = CASE_a2;
        //  analyze case a1:
        //      If P3 >= P2 And P2 >= P1 And search direction is UP goto Case_a1
        else if (P3 >= P2 && P2 >= P1 && direction == 1)
            currentState = CASE_a1;
        //  Analyze case b1:
        //      If P3 <= P2 And P2 <= P1 And search direction is DOWN goto Case_b1
        else if (P3 <= P2 && P2 <= P1 && direction == -1)
            currentState = CASE_b1;
        //  Analyze case b2
        //      If P3 <= P2 And P2 <= P1 And search direction is UP goto Case_b2
        else if (P3 <= P2 && P2 <= P1 && direction == 1)
            currentState = CASE_b2;
        //  Analyze case c1
        //      If P2 >= P3 And P2 >= P1 And search direction is DOWN goto Case_c1
        else if (P2 >= P3 && P2 >= P1 and direction == -1)
            currentState = CASE_c1;
        //  Analyze case c2
        //      If P2 >= P3 And P2 >= P1 And search direction is UP goto Case_c2
        else if (P2 >= P3 && P2 >= P1 and direction == 1)
            currentState = CASE_c2;
        //  Analyze another case a1
        //      If P2 <= P3 And P2 <= P1 And P3 > P1 And search direction is UP goto Case_a1
        else if (P2 <= P3 && P2 <= P1 && P3 > P1 && direction == 1)
            currentState = CASE_a1;
        //  Analyze another case a2
        //      If P2 <= P3 And P2 <= P1 And P3 > P1 And search direction is DOWN goto Case_a2
        else if (P2 <= P3 && P2 <= P1 && P3 > P1 && direction == -1)
            currentState = CASE_a2;
        //  Analyze another case b1
        //      If P2 <= P3 And P2 <= P1 And P3 < P1 And search direction is DOWN goto Case_b1
        else if (P2 <= P3 && P2 <= P1 && P3 < P1 && direction == -1)
            currentState = CASE_b1;
        //  Analyze another case b2
        //      If P2 <= P3 And P2 <= P1 And P3 < P1 And search direction is UP goto Case_b2
        else if (P2 <= P3 && P2 <= P1 && P3 < P1 && direction == 1)
            currentState = CASE_b2;
        else
            currentState = CASE_error;

        switch (currentState) {
            case CASE_a1:
                // Section Case_a1: this section corresponds to the following state:
                // When P3 >= P2 And P2 >= P1 And search direction is UP then
                // set the next search direction to UP
                // (Keep the sense direction since the function is increasing in value)
                direction = 1;
                // step in the search direction
                control += (controlStep * direction);
                // let A4=the control value
                A4 = control;
                if (!controlRangeCheck(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_a1 control value A4 is out of range." << endl;
                    return false;
                }
                // set the control value:
                if (!setControlValue(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_a1 setControlValue(A4) failed." << endl;
                    return false;
                }
                // get P4=the dependent value for A4:
                if (!waitForDependentValue(P4, timeOut, interval)) {
                    LOG(LM_ERROR) << "Maximizer CASE_a1 waitForDependentValue(P4) failed." << endl;
                    return false;
                }
                LOG(LM_DEBUG) << "Maximizer CASE_a1: A4=" << A4 << " P4=" << P4 << endl;
                //  re-assign Values and return to analysis:
                A1 = A2;
                P1 = P2;
                A2 = A3;
                P2 = P3;
                A3 = A4;
                P3 = P4;
                break;    
            
            case CASE_a2:
                // Section Case_a2: this section corresponds to the following state:
                // When P3 >= P2 And P2 >= P1 And search direction is DOWN then
                // set the next search direction to DOWN
                // (Keep the sense direction since the function is increasing in value)
                direction = -1;
                // step in the search direction
                control += (controlStep * direction);
                // let A4=the control value
                A4 = control;
                if (!controlRangeCheck(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_a2 control value A4 is out of range." << endl;
                    return false;
                }
                // set the control value:
                if (!setControlValue(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_a2 setControlValue(A4) failed." << endl;
                    return false;
                }
                // get P4=the dependent value for A4:
                if (!waitForDependentValue(P4, timeOut, interval)) {
                    LOG(LM_ERROR) << "Maximizer CASE_a2 waitForDependentValue(P4) failed." << endl;
                    return false;
                }
                LOG(LM_DEBUG) << "Maximizer CASE_a2: A4=" << A4 << " P4=" << P4 << endl;  
                //  re-assign Values and return to analysis:
                A1 = A2;
                P1 = P2;
                A2 = A3;
                P2 = P3;
                A3 = A4;
                P3 = P4;
                break;    

            case CASE_b1:
                // Section Case_b1: this section corresponds to the following state:
                // When P3 <= P2 And P2 <= P1 And search direction is DOWN then
                // set the next search direction to UP
                // (Reverse the direction towards increasing function value)
                direction = 1;
                //  step in the search direction
                control += (controlStep * direction);
                // let A4=the control value
                A4 = control;
                if (!controlRangeCheck(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_b1 control value A4 is out of range." << endl;
                    return false;
                }
                // set the control value:
                if (!setControlValue(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_b1 setControlValue(A4) failed." << endl;
                    return false;
                }
                // get P4=the dependent value for A4:
                if (!waitForDependentValue(P4, timeOut, interval)) {
                    LOG(LM_ERROR) << "Maximizer CASE_b1 waitForDependentValue(P4) failed." << endl;
                    return false;
                }
                LOG(LM_DEBUG) << "Maximizer CASE_b1: A4=" << A4 << " P4=" << P4 << endl;
                // Re-assign Values return to analysis:
                // Need to swap (A2,P2) with (A1,P1), use (A3,P3) as save registers
                A3 = A1;
                P3 = P1;
                A1 = A2;
                P1 = P2;
                A2 = A3;
                P2 = P3;
                A3 = A4;
                P3 = P4;
                break;

            case CASE_b2:
                // Section Case_b2: this section corresponds to the following state:
                // When P3 <= P2 And P2 <= P1 And search direction is UP then
                // set the next search direction to DOWN
                // (Reverse the direction towards increasing function value)
                direction = -1;
                //  step in the search direction
                control += (controlStep * direction);
                // let A4=the control value
                A4 = control;
                if (!controlRangeCheck(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_b2 control value A4 is out of range." << endl;
                    return false;
                }
                // set the control value:
                if (!setControlValue(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_b2 setControlValue(A4) failed." << endl;
                    return false;
                }
                // get P4=the dependent value for A4:
                if (!waitForDependentValue(P4, timeOut, interval)) {
                    LOG(LM_ERROR) << "Maximizer CASE_b2 waitForDependentValue(P4) failed." << endl;
                    return false;
                }
                LOG(LM_DEBUG) << "Maximizer CASE_b2: A4=" << A4 << " P4=" << P4 << endl;            
                // Re-assign Values return to analysis:
                // Need to swap (A2,P2) with (A1,P1), use (A3,P3) as save registers
                A3 = A1;
                P3 = P1;
                A1 = A2;
                P1 = P2;
                A2 = A3;
                P2 = P3;
                A3 = A4;
                P3 = P4;
                break;
                
            case CASE_c1:
                // Section Case_c1: this section corresponds to the following state:
                // When P2 >= P3 And P2 >= P1 And search direction is DOWN then
                // set the next search direction to DOWN 
                // (The peak is in between P3 and P1, keep the same sense to go up to the peak)
                //  Note that there are two cases below after A4 and P4 are obtained:
                //  1. If P4 < P3 And P4 < P2 Then there was a global minimum at A2, P2
                //  2. Otherwise we were at a local minima.    
                direction = -1;
                //  step in the search direction
                control += (controlStep * direction);
                // let A4=the control value
                A4 = control;
                if (!controlRangeCheck(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_c1 control value A4 is out of range." << endl;
                    return false;
                }
                // set the control value:
                if (!setControlValue(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_c1 setControlValue(A4) failed." << endl;
                    return false;
                }
                // get P4=the dependent value for A4:
                if (!waitForDependentValue(P4, timeOut, interval)) {
                    LOG(LM_ERROR) << "Maximizer CASE_c1 waitForDependentValue(P4) failed." << endl;
                    return false;
                }
                LOG(LM_DEBUG) << "Maximizer CASE_c1: A4=" << A4 << " P4=" << P4 << endl;            
                
                // If P4 < P3 And P4 < P2 Then <Global Maxima at A2>
                if (P4 < P3 && P4 < P2) {
                    LOG(LM_DEBUG) << "Maximizer CASE_c1:1 <Global Maximum at A2>" << endl;            
                    // Go back to A2:         
                    control += controlStep * 2;
                    A2 = control;
                    if (!controlRangeCheck(A2)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c1:1 control value A2 is out of range." << endl;
                        return false;
                    }
                    // set the control value:
                    if (!setControlValue(A2)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c1:1 setControlValue(A2) failed." << endl;
                        return false;
                    }
                    // get P2=the dependent value for A2:
                    if (!waitForDependentValue(P2, timeOut, interval)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c1:1 waitForDependentValue(P2) failed." << endl;
                        return false;
                    }
                    // Optimizer Completed!
                    LOG(LM_DEBUG) << "Maximizer completed!" << endl;            
                    success = true;
                } else {
                    LOG(LM_DEBUG) << "Maximizer CASE_c1:2 <local Maximum at A2>" << endl;            
                    // Re-assign Values
                    A1 = A3;
                    P1 = P3;
                    A2 = A4;
                    P2 = P4;
                    direction = -1;
                    control -= controlStep;
                    // get A3:
                    A3 = control;
                    if (!controlRangeCheck(A3)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c1:2 control value A3 is out of range." << endl;
                        return false;
                    }
                    // set the control value:
                    if (!setControlValue(A3)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c1:2 setControlValue(A3) failed." << endl;
                        return false;
                    }
                    // get P3=the dependent value for A3:
                    if (!waitForDependentValue(P3, timeOut, interval)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c1:2 waitForDependentValue(P3) failed." << endl;
                        return false;
                    }
                    // return to analysis.
                }
                break;                

            case CASE_c2:
                LOG(LM_DEBUG) << "Maximizer CASE_c2" << endl;            
                // Section Case_c2: this section corresponds to the following state:
                // When P2 >= P3 And P2 >= P1 And search direction is UP then
                // set the next search direction to UP 
                // (The peak is in between P3 and P1, keep the same sense to go up to the peak power)
                //  Note that there are two cases below after A4 and P4 are obtained:
                //  1. If P4 < P3 And P4 < P2 Then there was a global minimum at A2, P2
                //  2. Otherwise we were at a local minima.
                direction = 1;
                //  step in the search direction
                control += (controlStep * direction);
                // let A4=the control value
                A4 = control;
                if (!controlRangeCheck(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_c2 control value A4 is out of range." << endl;
                    return false;
                }
                // set the control value:
                if (!setControlValue(A4)) {
                    LOG(LM_ERROR) << "Maximizer CASE_c2 setControlValue(A4) failed." << endl;
                    return false;
                }
                // get P4=the dependent value for A4:
                if (!waitForDependentValue(P4, timeOut, interval)) {
                    LOG(LM_ERROR) << "Maximizer CASE_c2 waitForDependentValue(P4) failed." << endl;
                    return false;
                }
                LOG(LM_DEBUG) << "Maximizer CASE_c2: A4=" << A4 << " P4=" << P4 << endl;

                // If P4 < P3 And P4 < P2 Then <Global Maxima at A2>
                if (P4 < P3 && P4 < P2) {
                    LOG(LM_DEBUG) << "Maximizer CASE_c2:1 <Global Maximum at A2>" << endl;            
                    // Go back to A2:
                    control -= controlStep * 2;
                    A2 = control;
                    if (!controlRangeCheck(A2)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c2:1 control value A2 is out of range." << endl;
                        return false;
                    }
                    // set the control value:
                    if (!setControlValue(A2)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c2:1 setControlValue(A2) failed." << endl;
                        return false;
                    }
                    // get P2=the dependent value for A2:
                    if (!waitForDependentValue(P2, timeOut, interval)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c2:1 waitForDependentValue(P2) failed." << endl;
                        return false;
                    }
                    // Optimizer Completed!
                    LOG(LM_DEBUG) << "Maximizer completed!" << endl;            
                    success = true;
                } else {
                    LOG(LM_DEBUG) << "Maximizer CASE_c2:2 <local Maximum at A2>" << endl;            
                    // Re-assign Values
                    A1 = A3;
                    P1 = P3;
                    A2 = A4;
                    P2 = P4;
                    direction = 1;
                    control -= controlStep;
                    // get A3:
                    A3 = control;
                    if (!controlRangeCheck(A3)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c2:2 control value A3 is out of range." << endl;
                        return false;
                    }
                    // set the control value:
                    if (!setControlValue(A3)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c2:2 setControlValue(A3) failed." << endl;
                        return false;
                    }
                    // get P3=the dependent value for A3:
                    if (!waitForDependentValue(P3, timeOut, interval)) {
                        LOG(LM_ERROR) << "Maximizer CASE_c2:2 waitForDependentValue(P3) failed." << endl;
                        return false;
                    }
                    // return to analysis.
                }
                break;

            case CASE_error:
            default:
                LOG(LM_ERROR) << "Maximizer CASE_error." << endl;
                return false;
        }
    }
    if (stop_m)
        LOG(LM_INFO) << "Maximizer stopped." << endl;
    return success;           
}    

void Maximizer::setDependentValue(float depVal) {
    pthread_mutex_lock(&lock_m);
    depVal_m = depVal; 
    depValReady_m = true;
    pthread_mutex_unlock(&lock_m);
}

bool Maximizer::getDependentValue(float &depVal) {
    bool ret = false;
    if (pthread_mutex_trylock(&lock_m) == 0) {    
        if (depValReady_m) {
            depVal = depVal_m;
            depValReady_m = false;
            ret = true;
        }
        pthread_mutex_unlock(&lock_m);
    }
    return ret;
}

bool Maximizer::waitForDependentValue(float &depVal, int timeOut, int interval) {
    int elapsed = 0;

    if (stop_m)
        return false;

    // clear out any prior values left at the input:
    while (getDependentValue(depVal)) {
        SLEEP(interval);
        elapsed += interval;
    }

    if (stop_m)
        return false;
    
    // request a fresh value:
    if (requestDependentValue(depVal))
        // got a value.  return success.
        return true;
       
    // wait up to timeOut for the value to arrive:
    while (!getDependentValue(depVal)) {
        SLEEP(interval);
        elapsed += interval;
        if (elapsed >= timeOut || stop_m)
            return false;
    }
    return true;
}


