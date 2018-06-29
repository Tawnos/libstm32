/*
 * circular_buffer.h
 *
 *  Created on: Jun 28, 2018
 *      Author: cmdc0de
 */

#ifndef DARKNET_LIBSTM32_CIRCULAR_BUFFER_H_
#define DARKNET_LIBSTM32_CIRCULAR_BUFFER_H_

namespace cmdc0de {

template <class T>
class CircularBuffer {
public:
	explicit CircularBuffer(T b[]) :
		buf_(b),
		size_(sizeof(buf_)/sizeof(buf_[0])) {
		//empty constructor
	}

	void put(T *items, uint16_t size) {
		for(int i=0;i<size;i++) {
			put(items[i]);
		}
	}

	void put(T item) {
		buf_[head_] = item;
		head_ = (head_ + 1) % size_;
		if(head_ == tail_) {
			tail_ = (tail_ + 1) % size_;
		}
	}

	T get(void) {
		if(empty()) {
			return T();
		}

		//Read data and advance the tail (we now have a free space)
		auto val = buf_[tail_];
		tail_ = (tail_ + 1) % size_;

		return val;
	}

	void reset(void) {
		head_ = tail_;
	}

	bool empty(void) const {
		//if head and tail are equal, we are empty
		return head_ == tail_;
	}

	bool full(void) const {
		//If tail is ahead the head by 1, we are full
		return ((head_ + 1) % size_) == tail_;
	}

	size_t size(void) const {
		return size_ - 1;
	}

private:
	T buf_[];
	size_t head_ = 0;
	size_t tail_ = 0;
	size_t size_;
};

}



#endif /* DARKNET_LIBSTM32_CIRCULAR_BUFFER_H_ */
