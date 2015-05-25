#ifndef DEQUE_H_INCLUDED
#define DEQUE_H_INCLUDED

#include <iostream>
#include <cassert>
#include <iterator>

template<class Type>
class Deque{
	Type * buffer = 0;

	template<class T>
	class Iterator: public std::iterator<std::random_access_iterator_tag, std::ptrdiff_t, Type, T*, T&>{
		size_t index;
		const Deque<Type> * deque;

		void change(const typename Iterator::difference_type change_value, size_t &new_index){
			new_index = (static_cast<typename Iterator::difference_type>(new_index + deque->buf_size) + change_value) & (deque->buf_size - 1);
		}

	public:
		Iterator(){}

		Iterator(size_t real_index, const Deque<Type> * deque):index(real_index), deque(deque){}

		Iterator& operator ++ (){
			change(1, index);
			return *this;
		}

		T& operator * (){
			return *getPointer();
		}

		Iterator(const Iterator &it){
			*this = it;
		}

		Iterator& operator = (const Iterator &it){
			index = it.index;
			deque = it.deque;
			return *this;
		}

		bool operator == (const Iterator &it) const{
			return ((index == it.index) && (deque == it.deque));
		}

		bool operator != (const Iterator &it) const{
			return !(operator == (it));
		}

		T* operator -> (){
			return getPointer();
		}

		Iterator operator ++ (int){
			Iterator it = Iterator(index, deque);
			change(1, index);
			return it;
		}

		Iterator& operator -- (){
			change(-1, index);
			return *this;
		}

		Iterator operator -- (int){
			Iterator it = Iterator(index, deque);
			change(-1, index);
			return it;
		}

		Iterator& operator += (const typename Iterator::difference_type &n){
			change(n, index);
			return *this;
		}

		Iterator operator + (const typename Iterator::difference_type &n) const{
			Iterator it = *this;
			return it.operator += (n);
		}

		friend Iterator operator + (const typename Iterator::difference_type &n, const Iterator &it){
			return it + n;
		}

		Iterator& operator -= (const typename Iterator::difference_type &n){
			return operator += (-n);
		}

		Iterator operator - (const typename Iterator::difference_type &n) const{
			return operator + (-n);
		}

		typename Iterator::difference_type operator - (const Iterator &it) const{
			if (deque->_begin == deque->_end)
				return 0;

			assert(deque == it.deque);

			if (deque->_begin.getRealIndex() < deque->_end.getRealIndex())
				return index - it.index;
			else {
				int result = 0;
				if (it.getRealIndex() >= deque->_begin.getRealIndex())
					result += deque->buf_size - it.index;

				if (index >= deque->_begin.getRealIndex())
					result -= deque->buf_size - index;

				if (it.getRealIndex() <= deque->_end.getRealIndex())
					result -= it.index;

				if (index <= deque->_end.getRealIndex())
					result += index;

				return result;
			}

			assert(0);
			return 0;
		}

		T& operator [] (const typename Iterator::difference_type &n){
			return *(*this + n);
		}

		bool operator < (const Iterator &it) const{
			return it - *this > 0;
		}

		bool operator > (const Iterator &it) const{
			return it.operator < (*this);
		}

		bool operator <= (const Iterator &it) const{
			return !(it.operator > (*this));
		}

		bool operator >= (const Iterator &it) const{
			return !(operator < (it));
		}

		Type * getPointer() const{
			return deque->buffer + index;
		}

		size_t getRealIndex() const{
			return index;
		}
	};

	Iterator<Type> _begin, _end;

	size_t buf_size;

	void reallocate(size_t new_size){
		Type * new_buffer = new Type[new_size];

		size_t old_size = size();

		std::move(_begin, _end, new_buffer);

		delete [] buffer;

		buffer = new_buffer;
		buf_size = new_size;

		_end = Iterator<Type>(old_size, this);
		_begin = Iterator<Type>(0, this);
	}

	void updateBuffer(){
		if (size() + 1 == buf_size)
			reallocate(buf_size << 1);

		if (size() * 4 < buf_size && size() != 0)
			reallocate(buf_size >> 1);
	}

	Type getBack() const{
		assert(size());
		return *(end() - 1);
	}

	Type getFront() const{
		assert(size());
		return *(begin());
	}

public:

	typedef Iterator<Type> iterator;
	typedef Iterator<Type const> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	Deque()	: buffer(new Type[2]), _begin(0, this), _end(0, this), buf_size(2){}

	Deque& operator = (const Deque<Type> &deque){
		delete [] buffer;

		buffer = new Type[deque.buf_size];

		buf_size = deque.buf_size;

		_begin = iterator(deque.begin().getRealIndex(), this);
		_end = iterator(deque.end().getRealIndex(), this);

		std::move(deque.begin(), deque.end(), begin());

		return *this;
	}

	Deque(const Deque<Type> &deque){
		*this = deque;
	}

	template<class T> friend class Deque;

	void pushBack(Type value){
		updateBuffer();
		*(_end++) = value;
	}

	void pushFront(Type value){
		updateBuffer();
		*--_begin = value;
	}

	void popBack(){
		assert(size());

		_end--;
		updateBuffer();
	}

	Type back(){
		return getBack();
	}

	const Type back() const{
		return getBack();
	}

	void popFront(){
		assert(size());

		_begin++;
		updateBuffer();
	}

	Type front(){
		return getFront();
	}

	const Type front() const{
		return getFront();
	}

	Type& operator [] (const int &n){
		return *(_begin + n);
	}

	const Type& operator [] (const int &n) const{
		return *(_begin + n);
	}

	iterator begin(){
		return _begin;
	}

	iterator end(){
		return _end;
	}

	const_iterator cbegin() const{
		return const_iterator(_begin.getRealIndex(), this);
	}

	const_iterator cend() const{
		return const_iterator(_end.getRealIndex(), this);
	}

	const_iterator begin() const{
		return cbegin();
	}

	const_iterator end() const{
		return cend();
	}

	reverse_iterator rbegin(){
		return reverse_iterator(end());
	}

	reverse_iterator rend(){
		return reverse_iterator(begin());
	}

	const_reverse_iterator crbegin() const{
		return const_reverse_iterator(cend());
	}

	const_reverse_iterator crend() const{
		return const_reverse_iterator(cbegin());
	}

	const_reverse_iterator rbegin() const{
		return crbegin();
	}

	const_reverse_iterator rend() const{
		return crend();
	}

	size_t size() const{
		return _end - _begin;
	}

	bool empty() const{
		return _end == _begin;
	}

	~Deque(){
	    delete [] buffer;
	}
};


#endif // DEQUE_H_INCLUDED
