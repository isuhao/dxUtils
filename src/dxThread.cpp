/*
 * Copyright 2017 Taras Zaporozhets
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include "dxUtils/dxThread.h"
#include "dxUtils/dxHeplers.h"

#include <algorithm>
#include <cstring>
#include <glog/logging.h>
#include <pthread.h>
namespace dx {

Thread::Thread(std::string name)
    : m_state(DxThreadState::ThreadNEW)
{
    size_t maxNameLen = sizeof(m_name) - 1; // one for NULL terminator!
    size_t nameLen = (name.length() > (maxNameLen)) ? maxNameLen : name.length();

    std::copy_n(name.begin(), nameLen, m_name);
    m_name[nameLen] = 0; // NULL terminator
}

Thread::~Thread()
{
}

int Thread::start(std::function<void(void)> func, std::string name)
{
    if ((ThreadNEW == m_state) || (ThreadTERMINATED == m_state)) {
        m_state = ThreadRUNNABLE;
        m_thread = std::thread(func);
        setName(name);
        m_thread.detach();
    } else {
        LOG(ERROR) << "Thread(" << m_name << "): already executing";
        return -1;
    }

    //while (ThreadStopped == m_state){};

    return 0;
}

int Thread::start(std::function<void(void)> func, std::string name, int policy, int priority)
{
    if ((ThreadNEW == m_state) || (ThreadTERMINATED == m_state)) {
        m_state = ThreadRUNNABLE;
        m_thread = std::thread(func);
        setName(name);
        setPriority(policy, priority);
        m_thread.detach();
    } else {
        LOG(ERROR) << "Thread(" << m_name << "): already executing";
        return -1;
    }

    //while (ThreadStopped == m_state){};

    return 0;
}

int Thread::stop()
{
    m_terminationRequest = true;
    m_wakeupSignal.send();
    while (ThreadTERMINATED != m_state) {
    };

    m_terminationRequest = false;
    return 0;
}

int Thread::setName(std::string name)
{
    size_t maxNameLen = sizeof(m_name) - 1; // one for NULL terminator!
    size_t nameLen = (name.length() > (maxNameLen)) ? maxNameLen : name.length();

    std::copy_n(name.begin(), nameLen, m_name);
    m_name[nameLen] = 0; // NULL terminator

    pthread_t pth = m_thread.native_handle();
    return pthread_setname_np(pth, m_name) == 0;
}

int Thread::sleep(std::vector<std::reference_wrapper<Signal> > wakeUpSignals, int timeout)
{
    if (ThreadRUNNABLE != m_state) {
        LOG(ERROR) << "Thread(" << m_name << "): not in runnable state";
    }
    wakeUpSignals.push_back(m_wakeupSignal);
    m_state = ThreadWAITING;
    int retval = Signal::waitMultiple(wakeUpSignals, timeout);
    m_state = ThreadRUNNABLE;
    return retval;
}

bool Thread::isRunning()
{
    bool isTerminared = (ThreadTERMINATED == m_state);
    bool isNew = (ThreadNEW == m_state);

    return !m_terminationRequest && !isNew && !isTerminared;
}

void Thread::setExitState(int status)
{
    UNUSED(status);
    this->m_state = ThreadTERMINATED;
}

int Thread::setPriority(int newPolicy, int newPriority)
{
    sched_param sch;

    pthread_t pth = m_thread.native_handle();

    int policy;
    pthread_getschedparam(pth, &policy, &sch);

    sch.sched_priority = newPriority;
    if (pthread_setschedparam(pth, newPolicy, &sch)) {
        std::array<char, 64> errMsg;
        strerror_r(errno, &errMsg[0], errMsg.size());
        LOG(ERROR) << "Thread(" << m_name << "): Failed to set the scheduling parameters, " << errMsg.data();
        return -1;
    }

    return 0;
}

int Thread::getPriority(int& policy, int& priority)
{
    sched_param sch;

    pthread_t pth = m_thread.native_handle();

    pthread_getschedparam(pth, &policy, &sch);
    priority = sch.sched_priority;

    return 0;
}

}   // namespace dx
