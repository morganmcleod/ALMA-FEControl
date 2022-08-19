#ifndef MAXIMIZER_H_
#define MAXIMIZER_H_
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

#include <pthread.h>

class Maximizer {
public:
    Maximizer()
      { reset();
        pthread_mutex_init(&lock_m, NULL); }
    
    virtual ~Maximizer()
      { pthread_mutex_destroy(&lock_m); }


    virtual void reset();
    ///< reset all internal state to as if just constructed.

    bool maximize(float &controlStart, float controlStep, 
                  float controlMin, float controlMax, bool directionUp,
                  int maxIter, int timeOut, int interval);
    ///< perform a maximization of the dependent variable by varying the control variable.
    ///< Iterates up to maxIter times.  Returns true if maximum found.
    ///< The control value where the maximum was found is returned in controlStart.
    ///< Returns false if there is an error setting the control variable via setControlValue().
    ///< Returns false if too many iterations or too much elapsed time.  timeOut specified in ms.
    ///< The interval specifies how long to wait between attempts after requesting a dependent value.

    void stopMaximizer()
      { stop_m = true; }
    ///< abort the process.

protected:
    void setDependentValue(float depVal);
    ///< Set the dependent value after one was requested via requestDependentValue().

    virtual bool setControlValue(float control) = 0;
    ///< Derived class must implement.  Called by the maximizer to set the independent control variable.
    ///< If implementation returns false, the maximize function returns false.
    
    virtual bool requestDependentValue(float &depVal) = 0;
    ///< Derived class must implement.  Called by the maximizer to request the value of the dependent variable.
    ///< Implementation may provide a result right away and return true 
    ///< or return false and provide a value later using setDependentValue().

    bool waitForDependentValue(float &depVal, int timeOut, int interval = 20);
    ///< Wait up to timeOut ms for a fresh dependent value to be provided.
    ///< pause interval ms between attempts.
    ///< returns false if error or timeout.
    
private:

    bool getDependentValue(float &depVal);
    ///< Helper function to synchronize maximize() access to the latest dependent value.

    bool controlRangeCheck(const float &control) const
      { return (control >= controlMin_m && control <= controlMax_m); }
    ///< Check that the control value is within range.

    float controlMin_m;         ///< minimum allowed value for the control variable.
    float controlMax_m;         ///< maximum allowed value for the control variable.

    enum searchCases {
        CASE_error,
        CASE_a1,
        CASE_a2,
        CASE_b1,
        CASE_b2,
        CASE_c1,
        CASE_c2
    };
    
    pthread_mutex_t lock_m;     ///< lock sychronizes access to the dependent value inputs below.
    float depVal_m;             ///< the latest dependent value set.
    bool depValReady_m;         ///< true if maximize() can unblock and read the latest value.
    bool stop_m;
};

#endif /*MAXIMIZER_H_*/
