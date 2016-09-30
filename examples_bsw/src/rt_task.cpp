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
constexpr int RT_PERIOD_US = 1000;

// create a class that declares and defines the methods pre(), update() and
// post(). 
// 1. Method pre() is called by the base class RTTask before the real-time
// loop is started. 
// 2. Then, the task will execute the real-time task periodically at a given 
// rate specified.
// 3. 
class MyRTTask : public RTTask< MyRTTask, RT_TASK_PRIO, RT_PERIOD_US >
{
public:

    /**
     * @brief pre conditions to execute.
     */
    void pre() noexcept;
    
    /**
     * @brief periodic update
     */
    void update() noexcept;
    
    /**
     * @brief post-conditions after task execution.
     */
    void post() noexcept;
    
private:
};

////////////////////////////////////////////////////////////////////////////////
void MyRTTask::pre() noexcept
{

}

////////////////////////////////////////////////////////////////////////////////
void MyRTTask::update() noexcept
{

}

////////////////////////////////////////////////////////////////////////////////
void MyRTTask::post() noexcept
{

}

////////////////////////////////////////////////////////////////////////////////
int main() noexcept
{
    // create an instance of your concrete real-time task application.
    //MyRTTask rt_task;
    //rt_task.task_entry();
    using namespace std::chrono_literals;
    Test(1ms, 98).update();
}

