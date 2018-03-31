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
#include "dxUtils/dxSignal.h"

#include <cerrno>
#include <cstring>
#include <functional>
#include <vector>

#include <glog/logging.h>
#include <sys/poll.h>
#include <unistd.h>

using namespace std;

DxSignal::DxSignal()
{
    int pipefd[2];
    int retval = pipe(pipefd);
    if (retval < 0) {
        // TODO: print error;
        // thow bexeption
    }
    m_isOwnFd = true;
    m_readFd = pipefd[0]; // pipefd[0] refers to the read end of the pipe.
    m_writeFd = pipefd[1]; // pipefd[1] refers to the write end of the pipe.
}

DxSignal::DxSignal(int fd, int pollType)
{
    m_pollType = pollType;
    m_isOwnFd = false;
    m_writeFd = -1;
    m_readFd = fd;
}

DxSignal::~DxSignal()
{
    if (m_isOwnFd) {
        close(m_readFd);
        close(m_writeFd);
    }
}

int DxSignal::getFd()
{
    return m_readFd;
}

int DxSignal::send()
{
    if (0 > m_writeFd) {
        return -1;
    }
    unsigned char payload[] = { 0xAB };
    int retval = write(m_writeFd, payload, sizeof(payload));
    if (retval < 0) {
        std::array<char, 64> errMsg;
        strerror_r(errno, &errMsg[0], errMsg.size());
        LOG(ERROR) << "Write failed, " << errMsg.data();
        return -1;
    }
    return -1;
}

bool DxSignal::wait(int timeout)
{
    return waitMultiple({ *this }, timeout);
}

int DxSignal::waitMultiple(std::vector<std::reference_wrapper<DxSignal> > sigs, int timeout)
{
    int signalCount = sigs.size();
    struct pollfd ufds[20];

    int index = 0;
    for (auto& item : sigs) {
        ufds[index].fd = item.get().m_readFd;

        ufds[index].events = 0;
        ufds[index].events |= (SignalPOLLIN & item.get().m_pollType) ? POLLIN : 0;
        ufds[index].events |= (SignalPOLLPRI & item.get().m_pollType) ? POLLPRI : 0;
        ufds[index].events |= (SignalPOLLOUT & item.get().m_pollType) ? POLLOUT : 0;
        ufds[index].events |= (SignalPOLLERR & item.get().m_pollType) ? POLLERR : 0;
        ufds[index].events |= (SignalPOLLHUP & item.get().m_pollType) ? POLLHUP : 0;
        ufds[index].events |= (SignalPOLLNVAL & item.get().m_pollType) ? POLLNVAL : 0;

        index++;
    }

    int retval = poll(ufds, signalCount, timeout);

    if (-1 == retval) {
        LOG(ERROR) << "error occurred in poll";
        return -1;
    } else if (0 == retval) {
        //LOG(ERROR) << "timeout occurred";
        return -1;
    } else {
        for (int i = 0; i < signalCount; i++) {
            if (ufds[i].revents & ufds[i].events) {
                if (true == sigs[i].get().m_isOwnFd) {
                    unsigned char payload[20]; // TODO: Remove magic array size
                    retval = read(ufds[i].fd, payload, sizeof(payload));
                    if (0 > retval) {
                        // TODO: Add error handling
                    }
                }
                //LOG_DEBUG("Signal[%d]\n", i);
                return i;
            }
        }
    }

    return -1;
}
