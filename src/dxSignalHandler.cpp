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
#include "dxUtils/dxSignalHandler.h"
#include <errno.h>
#include <signal.h>

namespace dx {
bool DxSignalHandler::m_gotExitSignal = false;

DxSignalHandler::DxSignalHandler()
{
}

DxSignalHandler::~DxSignalHandler()
{
}

bool DxSignalHandler::gotExitSignal()
{
    return m_gotExitSignal;
}

void DxSignalHandler::setExitSignal(bool _bExitSignal)
{
    m_gotExitSignal = _bExitSignal;
}

void DxSignalHandler::exitSignalHandler(int _ignored)
{
    (void)_ignored;
    m_gotExitSignal = true;
}

void DxSignalHandler::setupSignalHandlers()
{
    if (signal((int)SIGINT, DxSignalHandler::exitSignalHandler) == SIG_ERR) {
        throw SignalException("!!!!! Error setting up signal handlers !!!!!");
    }
    if (signal((int)SIGTERM, DxSignalHandler::exitSignalHandler) == SIG_ERR) {
        throw SignalException("!!!!! Error setting up signal handlers !!!!!");
    }
}
}
