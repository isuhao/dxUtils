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

#include "readerwriterqueue.h"

namespace dx {

template <typename T>
class DxQueue {

public:
    DxQueue(int size)
    {
        queue = new moodycamel::ReaderWriterQueue<T>(size);
    }

    ~DxQueue()
    {
        delete queue;
    }

    // Enqueues a copy of element on the queue.
    // Allocates an additional block of memory if needed.
    // Only fails (returns false) if memory allocation fails.
    bool enqueue(T const& element)
    {

        return queue->enqueue(element);
    }

    // Enqueues a moved copy of element on the queue.
    // Allocates an additional block of memory if needed.
    // Only fails (returns false) if memory allocation fails.
    bool enqueue(T&& element)
    {
        return queue->enqueue(element);
    }

    // Attempts to dequeue an element; if the queue is empty,
    // returns false instead. If the queue has at least one element,
    // moves front to result using operator=, then returns true.
    bool tryDequeue(T& result)
    {
        return queue->try_dequeue(result);
    }

    bool isEmpty()
    {
        return (nullptr == queue->peek());
    }

    // No copy and move for now
    DxQueue() = delete;
    DxQueue(DxQueue&) = delete;
    DxQueue(DxQueue&&) = delete;

private:
    moodycamel::ReaderWriterQueue<T>* queue;
};

}   // namespace dx
