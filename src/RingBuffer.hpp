#pragma once

#include <atomic>
#include <cstring>
#include <memory>

/**
 * This class implements a generic, thread-safe and lock-free ringbuffer.
 * @author Jonas Schenke (a.k.a. kloppstock)
 */
template <class T> class RingBuffer {
public:
    /**
     * RingBuffer constructor
     * @param highest possible number of elements
     * @throws bad_alloc if a memory error occurs
     */
    RingBuffer(size_t size) : size(size), full(false), data(new T[size])
    {
        head.store(0);
        tail.store(0);
    }

    /**
     * RingBuffer constructor
     * @param another RingBuffer object
     * @throws bad_alloc if a memory error occurs
     */
    RingBuffer(const RingBuffer& other) : size(other.size), full(other.full), data(new T[size])
    {
        head.store(other.head.load());
        tail.store(other.tail.load());
        memcpy(other.data, data, size * sizeof(T));
    }

    /**
     * The assignment operator was removed because it could cause problems in a
     * multithreaded environment!
     */
    RingBuffer& operator=(const RingBuffer& other) = delete;

    /**
     * Getter for size
     * @return size
     */
    size_t getSize() const { return size; }

	/**
	 * Returns the number of elements in use.
	 * @return number of elements
	 */
	size_t getNumberOfElements() const 
	{
		return (full ? size : ((tail.load() - head.load() + size) % size));
	}

    /**
     * Checks if the buffer is empty.
     * @return true if it is empty
     */
    bool isEmpty() const { return ((head.load() == tail.load()) && !full); }

    /**
     * Checks if the buffer is empty.
     * @return true if it is empty
     */
    bool isFull() const { return full; }

    /**
     * Tries to push an element into the RingBuffer.
     * @param element
     * @return true on success
     */
    bool push(T element)
    {
        size_t current_tail = tail.load();
        if (full)
            return false;
        data[current_tail] = element;
        tail.store(increment(current_tail));
		if(head.load() == tail.load())
			full = true;
        return true;
    }

    /**
     * Tries to pup an element from the RingBuffer.
     * @param element reference for storage
     * @return true on success
     */
    bool pop(T& element)
    {
        size_t current_head = head.load();
        if (current_head == tail.load() && !full)
            return false;
		full = false;
        element = data[current_head];
        head.store(increment(current_head));
        return true;
    }

protected:
private:
    size_t size;
    std::unique_ptr<T[]> data;
    std::atomic<size_t> head, tail;
	bool full;

    /**
     * Increments the given value inside the range of the RingBuffer size
     */
    size_t increment(size_t n) const { return (n + 1) % size; }
};

