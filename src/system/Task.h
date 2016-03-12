/**
 * @file      Task.h
 * @author    dtuchscherer <your.email@hs-heilbronn.de>
 * @brief     short description...
 * @details   long description...
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

#ifndef TASK_H_
# define TASK_H_

/*******************************************************************************
 * MODULES USED
 *******************************************************************************/
#include "OSControl.h"
/*******************************************************************************
 * DEFINITIONS AND MACROS
 *******************************************************************************/

/*******************************************************************************
 * TYPEDEFS, ENUMERATIONS, CLASSES
 *******************************************************************************/

/**
 * @tparam Priority of the real-time task
 * @tparam PeriodMicro the period in microseconds the task is called.
 */
template< typename Derived, int Priority, int PeriodMicro >
struct RTTask: OSControl
{
    using TaskType = RTTask< Derived, Priority, PeriodMicro >;

    RTTask() noexcept :
    m_task_running(FALSE)
    {
        const boolean created = create_rt_task< TaskType::task_helper >(this, m_task_handle);
    }

    ~RTTask() noexcept
    {
        m_task_running = FALSE;
    }

    /**
     * @brief Helper function that calls the actual rt task,
     * @remark pthread can not handle member functions so we need this little
     * helper.
     * @param[in] we need the object were the actual method to call is. In this
     * case the context is "this" object.
     */
    static void* task_helper(void* context)
    {
        return (static_cast< TaskType* >(context))->task();
    }

    /**
     * @brief Called once before the real-time loop is entered.
     */
    void pre() noexcept
    {
        static_cast< Derived* >(this)->pre();
    }

    /**
     * @brief Periodically called.
     */
    void update() noexcept
    {
        static_cast< Derived* >(this)->update();
    }

    /**
     * @brief Called once after the execution of the real-time task.
     */
    void post() noexcept
    {
        static_cast< Derived* >(this)->post();
    }

    /**
     * @brief
     */
    void* task() noexcept
    {
        m_task_running = TRUE;
        pre();
        rt_task< Priority, PeriodMicro, TaskType >(m_task_running, *this);
        post();
    }

    //! Statically known priority of the real-time task
    static constexpr int m_priority = Priority;

    //! Statically known period in microseconds.
    static constexpr int m_period = PeriodMicro;

    //! The handle of the task
    TaskHandle m_task_handle;

    //! if the loop of the thread will run or not.
    boolean m_task_running;
};

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* TASK_H_ */
