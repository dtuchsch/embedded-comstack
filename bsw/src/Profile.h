/**
 * @file      Profile.h
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     Profiling function calls
 * @details   Measures the time between begin and end of a function.
 * @version   1.0
 * @copyright Copyright (c) 2015, dtuchscherer.
 *            All rights reserved.
 *
 *            Redistributions and use in source and binary forms, with
 *            or without modifications, are permitted provided that the
 *            following conditions are met: Redistributions of source code must
 *            retain the above copyright notice, this list of conditions and the
 *            following disclaimer.
 *
 *            Redistributions in binary form must reproduce the above copyright
 *            notice, this list of conditions and the following disclaimer in
 *            the documentation and/or other materials provided with the
 *            distribution.
 *
 *            Neither the name of the Heilbronn University nor the name of its
 *            contributors may be used to endorse or promote products derived
 *            from this software without specific prior written permission.
 *
 *            THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS “AS IS”
 *            AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *            TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *            PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS
 *            OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *            SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *            LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *            USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *            AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *            LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *            ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *            POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PROFILE_H_
# define PROFILE_H_

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/
 
// uses C++11 STL std::chrono to profile execution times.
#include <chrono>

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

/**
 * @brief Profile for determination of time difference between two timepoints.
 */
class Profile
{
public:

    Profile() noexcept = default;

    /**
     * @brief Start / begin the time measurement.
     */
    void start() noexcept
    {
    	m_t1 = std::chrono::high_resolution_clock::now();
    }

    /**
     * @brief Takes the second timepoint and calculates the difference in 
     * seconds between two timepoints.
     * @return the difference in seconds as floating point
     */
    std::chrono::duration< double > end() noexcept
    {
    	m_t2 = std::chrono::high_resolution_clock::now();
    	// difference in time 
    	std::chrono::duration< double > delta = m_t2 - m_t1;
    	// delta t is returned
    	return delta;
    }

private:
    //! timepoint one / start
    std::chrono::high_resolution_clock::time_point m_t1;
    
    //! timepoint two / end
    std::chrono::high_resolution_clock::time_point m_t2;
};
/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* PROFILE_H_ */
