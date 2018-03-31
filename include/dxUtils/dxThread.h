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
#pragma once


#include "dxSignal.h"

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class DxThread {
public:
    DxThread(std::string name = "none");
    ~DxThread();

    //DxThread() noexcept = default;
    DxThread(DxThread&) = delete;
    DxThread(const DxThread&) = delete;

    int start(std::function<void(void)> func, std::string name = "Unknown");
    int start(std::function<void(void)> func, std::string name, int policy, int priority);
    int stop();

    //    int wakeup()
    //    {
    //        return 0;
    //    }

    //    int sleep(int timeout = -1)
    //    {
    //        sleep({ m_signal }, timeout);
    //        return 0;
    //    }

    int sleep(std::vector<std::reference_wrapper<DxSignal> > wakeUpSignals, int timeout);

    bool isRunning();

    void setExitState(int status);

    // A thread state. A thread can be in one of the following states:
    enum DxThreadState {
        ThreadNEW, // A thread that has not yet started is in this state.
        ThreadRUNNABLE, // A thread executing is in this state.
        ThreadWAITING, // A thread that is waiting indefinitely for data is in this state.
        ThreadTERMINATED, // A thread that has exited is in this state.
    };

private:
    int setName(std::string name);
    int setPriority(int newPolicy, int newPriority);
    int getPriority(int& policy, int& priority);

    char m_name[16];
    DxThreadState m_state;
    bool m_terminationRequest = false;
    std::thread m_thread;
    DxSignal m_wakeupSignal;
};
