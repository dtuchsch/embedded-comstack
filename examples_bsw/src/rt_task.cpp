// This example shows how to use real-time tasks under Linux.
#include <chrono>

// header to include to get access to the class
#include "RTTask.h"

// configuration stuff. this normally should be placed into a configuration header
// and source file.
// the real-time task shall run with highest priority possible.
constexpr int RT_TASK_PRIO = 98;

// the sample time of the real-time thread shall be 1000 microseconds which 
// equals 1 millisecond.
constexpr int RT_PERIOD_US = 1000000;

// create a class that declares and defines the methods pre(), update() and
// post(). 
// 1. Method pre() is called by the base class RTTask before the real-time
// loop is started. 
// 2. Then, the task will execute the real-time task periodically at a given 
// rate specified.
class MyRTTask : public RTTask< MyRTTask, RT_TASK_PRIO, RT_PERIOD_US >
{
public:

    /**
     * @brief pre conditions to execute.
     */
    AR::boolean pre() noexcept;
    
    /**
     * @brief periodic update
     */
    AR::boolean update() noexcept;
    
    /**
     * @brief post-conditions after task execution.
     */
    void post() noexcept;
    
private:

};

////////////////////////////////////////////////////////////////////////////////
AR::boolean MyRTTask::pre() noexcept
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
AR::boolean MyRTTask::update() noexcept
{
    std::cout << "RT TASK CALLED\n";
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void MyRTTask::post() noexcept
{

}

////////////////////////////////////////////////////////////////////////////////
int main() noexcept
{
    // create an instance of your concrete real-time task application.
    MyRTTask rt_task;
    // this will execute the pre-condition. if the pre-condition is fulfilled
    // the task is executed periodically.
    rt_task.task_entry();
}

