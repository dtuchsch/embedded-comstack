/**
 * @file      OSControl.h
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     Abstraction interface
 * @details   Abstracting the management of real-time tasks.
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

#ifndef OSCONTROL_H_
# define OSCONTROL_H_

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/

#include <limits>

#include <string.h>

// Timestamps and nanosleep
#include <time.h>

// Accessing the scheduler for the real-time tasks to set priority.
#include <sched.h>

// Memory management for the real-time tasks.
#include <sys/mman.h>

// POSIX threads for send and receive thread.
#include <pthread.h>

#include "Std_Types.h"

/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

struct TaskMutex
{
    pthread_mutex_t m_mutex;
};

struct TaskHandle
{
    pthread_t m_handle;
};

class OSControl
{
public:

    /**
     * @brief Creates a real-time task as pthread.
     * @tparam F is the function called after pthread creation.
     * @param[in] context is a pointer to an object that is used to get access
     * to class methods within the pthread context.
     * @return TRUE if the task has been created, FALSE if the creation was
     * not successful.
     */
    template< void* F(void* context) >
    boolean create_rt_task(void* context, TaskHandle& handle) noexcept
    {
        boolean created = FALSE;

        // This creates the receive thread that waits in a loop for transmitted CAN frames.
        const auto thread_running = pthread_create(&handle.m_handle, NULL_PTR,
                                                   F, context);

        if ( thread_running == 0 )
        {
            created = TRUE;
        }
        else
        {
            created = FALSE;
        }

        return created;
    }

    /**
     * @brief Waits until the thread is terminated.
     */
    boolean close_rt_task(TaskHandle& handle) noexcept
    {
        boolean closed = FALSE;

        const int joined = pthread_join(handle.m_handle, NULL_PTR);

        if ( joined == 0 )
        {
            closed = TRUE;
        }
        else
        {
            closed = FALSE;
        }

        return closed;
    }

    /**
     * @brief The RT Task to call. This will enter a loop as long as the
     * running variable is set to true.
     * @param running
     * @param callee
     */
    template< int Priority, int Period, typename T >
    void rt_task(const boolean& running, T& callee)
    {
        struct timespec t;
        struct sched_param sched_param;
        constexpr int INTERVAL = 1000 * Period; // 1000ns = 1us

        static_assert(INTERVAL <= std::numeric_limits< int >::max(),
                "Interval is too big.");

        sched_param.sched_priority = Priority;

        // Set up the scheduler to round-robin algorithm.
        // Additionally to SCHED_FIFO SCHED_RR time slices
        const int prio_policy_set = sched_setscheduler(0, SCHED_RR,
                                                       &sched_param);
        if ( prio_policy_set == -1 )
        {

        }

        /* Lock memory */
        if ( mlockall(MCL_CURRENT | MCL_FUTURE) == -1 )
        {

        }

        /* Pre-fault our stack */
        stack_prefault();

        // Write the time struct once.
        clock_gettime(CLOCK_MONOTONIC, &t);

        // start after one second
        ++t.tv_sec;

        // we let the task running as long as this reference variable is true.
        while ( running == TRUE )
        {
            // Wait the given interval
            // @remark Use nanosleep for high precision. Uses the high resolution timer.
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

            // The method must always be named like this!
            callee.update();

            // add for the next shot, otherwise clock_nanosleep has no effect
            t.tv_nsec += INTERVAL;

            // if the nanoseconds field exceeds 1s...
            normalize(t);
        }
    }

    /**
     * @brief Initialize a mutex.
     */
    boolean mutex_init(TaskMutex& mutex) noexcept
    {
        boolean init_success = FALSE;

        const auto init = pthread_mutex_init(&mutex.m_mutex, NULL);

        if ( init == 0 )
        {
            init_success = TRUE;
        }
        else
        {
            init_success = FALSE;
        }

        return init_success;
    }

    /**
     * @brief Locks the mutex
     */
    void mutex_lock(TaskMutex& mutex) noexcept
    {
        pthread_mutex_lock(&mutex.m_mutex);
    }

    /**
     * @brief Unlocks the mutex
     */
    void mutex_unlock(TaskMutex& mutex) noexcept
    {
        pthread_mutex_unlock(&mutex.m_mutex);
    }

    /**
     * @brief Destroys the mutex.
     */
    void mutex_destroy(TaskMutex& mutex) noexcept
    {
        pthread_mutex_destroy(&mutex.m_mutex);
    }

    /**
     * @brief tries to allocate memory within a pthread.
     */
    void stack_prefault()
    {
        uint8 stack[8 * 1024];
        memset(stack, 0, 8 * 1024);
    }

private:

    /**
     * @brief Necessary to calculate the time correctly for the next
     * nanosleep. If the field nanoseconds of structure timespec exceeds
     * 1000000000 ns = 1 s we must add this to the field that holds seconds
     * and reset the nanoseconds field.
     * @param[in] t the time structure we want to adjust.
     */
    void normalize(struct timespec& t) noexcept
    {
        constexpr int NSEC_PER_SEC = 1000000000LL;

        // Normalize the time structure to calculate the next shot.
        while ( t.tv_nsec >= NSEC_PER_SEC )
        {
            t.tv_nsec -= NSEC_PER_SEC;
            ++t.tv_sec;
        }
    }
};

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* OSCONTROL_H_ */
