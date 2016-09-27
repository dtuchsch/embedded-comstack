/**
 * @file      RTTask.h
 * @author    dtuchscherer <daniel.tuchscherer@hs-heilbronn.de>
 * @brief     Real-Time Task abstraction
 * @details   The template class RTTask covers the simple creation of real-time
 *            tasks with pre and post-routines.
 * @version   1.0
 * @edit      27.09.2016
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

#ifndef RTTASK_H_
# define RTTASK_H_

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
 * @tparam Derived A class that holds at least three methods pre(), update() and
 * post().
 * @tparam Priority of the real-time task
 * @tparam PeriodMicro the period in microseconds the task is called.
 */
template< typename Derived, int Priority, int PeriodMicro >
class RTTask: public OSControl
{
public:

    // Get the type for giving it to the template method of OSControl.
    using TaskType = RTTask< Derived, Priority, PeriodMicro >;

    /**
     * @brief Default constructor creating the real-time task.
     */
    RTTask() noexcept :
            m_task_running(FALSE)
    {

    }

    /**
     * @brief Cancels the real-time task.
     */
    ~RTTask() noexcept
    {
        // make sure a thread is not executed after the constructor of RTTask
        // is called.
        m_task_running = FALSE;
    }

    /**
     * @brief Called once before the real-time loop is entered.
     */
    void pre() noexcept
    {
        // CRTP like call.
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
     * @brief This is entering the loop after the thread is created.
     * Setting the priority of this task and the scheduler type.
     * In this loop the update() method is called periodically.
     */
    void* task_entry() noexcept
    {
        m_task_running = TRUE;
        // before we enter the real-time task loop we will call the pre-condition.
        pre();
        // calls the update method cyclically at a given rate.
        rt_task< Priority, PeriodMicro, TaskType >(m_task_running, *this);
        post();
    }

    /**
     * @brief Helper function that calls the actual rt task,
     * @remark pthread can not handle member functions so we need this little
     * helper.
     * @param[in] we need the object were the actual method to call is.
     */
    static void* thread_helper(void* context)
    {
        return (static_cast< TaskType* >(context))->task_entry();
    }

    //! Static priority of the real-time task
    static constexpr int m_priority = Priority;

    //! Static period in microseconds.
    static constexpr int m_period = PeriodMicro;

protected:

    /**
     * @brief Creates an extra thread independent of the current task executed.
     */
    boolean create_thread() noexcept
    {
        return create_rt_thread< TaskType::thread_helper >(this, m_task_handle);
    }

    /**
     * @brief 
     */
    boolean close_thread() noexcept
    {
        return close_rt_thread(m_task_handle);
    }

    //! if the loop of the thread will run or not.
    boolean m_task_running;

    //! The handle of the task
    TaskHandle m_task_handle;

private:

};

template< typename Derived, int Priority, int PeriodMicro >
class RTThread : public RTTask< Derived, Priority, PeriodMicro >
{
public:

    RTThread() noexcept
    {
        const boolean created = this->create_thread();
    }

    ~RTThread() noexcept
    {
        const boolean closed = this->close_thread();
    }
};

/*******************************************************************************
 * EXPORTED VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************/

#endif /* RTTASK_H_ */
