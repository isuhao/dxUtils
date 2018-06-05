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

#include <vector>
#include <functional>


namespace dx {

const int SignalPOLLIN    = (1<<1); /* There is data to read.  */
const int SignalPOLLPRI   = (1<<2); /* There is urgent data to read.  */
const int SignalPOLLOUT   = (1<<3); /* Writing now will not block.  */
const int SignalPOLLERR   = (1<<4); /* Error condition.  */
const int SignalPOLLHUP   = (1<<5); /* Hung up.  */
const int SignalPOLLNVAL  = (1<<6); /* Invalid polling request.  */

class Signal{
public:

    Signal();
    Signal(int fd, int pollType = SignalPOLLIN);
    ~Signal();


    int getFd();
    int send();
    bool wait(int timeout);

    static int waitMultiple(std::vector<std::reference_wrapper<Signal> > sigs, int timeout);


private:
    // Copy
    Signal(const Signal& that) = delete;
    // Move
    Signal(const Signal&& that) = delete;
    Signal& operator=(const Signal&) = delete;

    int m_readFd = -1;
    int m_writeFd = -1;
    bool m_isOwnFd = true;
    int m_pollType = SignalPOLLIN;
};

}   // namespace dx
