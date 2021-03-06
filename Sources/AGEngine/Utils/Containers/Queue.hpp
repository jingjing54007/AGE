#pragma once

#include "Config.hpp"

#include <crtdefs.h>
#include <cassert>

namespace AGE
{
	template <class T>
	class Age_Queue
	{
	public:
		Age_Queue()
			: _size(0)
			, _start(0)
			, _end(0)
			, _chunkSize(124)
			, _data(nullptr)
		{}

		~Age_Queue()
		{
			if (_data)
				delete[] _data;
			_data = nullptr;
		}

		Age_Queue(const Age_Queue &o)
		{
			_size = o._size;
			_start = o._start;
			_end = o._end;
			_chunkSize = o._chunkSize;
			release();
			for (auto i = o._start; i < o._end; ++i)
				push(o._data[i]);
			/*_data = std::move(o._data);*/
		}

		Age_Queue(Age_Queue &&o)
		{
			_size = std::move(o._size);
			_start = std::move(o._start);
			_end = std::move(o._end);
			_chunkSize = std::move(o._chunkSize);
			release();
			for (auto i = o._start; i < o._end; ++i)
				push(o._data[i]);
			//_data = std::move(o._data);
		}

		Age_Queue &operator=(const Age_Queue &o)
		{
			_size = std::move(o._size);
			_start = std::move(o._start);
			_end = std::move(o._end);
			_chunkSize = std::move(o._chunkSize);
			release();
			for (auto i = o._start; i < o._end; ++i)
				push(o._data[i]);
			//_data = std::move(o._data);
			return *this;
		}

		std::size_t size() const
		{
			return _end - _start;
		}

		bool empty() const
		{
			return _size == 0 || _end == 0 || _end == _start;
		}

		const T &front() const
		{
			assert(!empty());
			return _data[_start];
		}

		T &front()
		{
			assert(!empty());
			return _data[_start];
		}

		const T &back() const
		{
			assert(!empty());
			return _data[_end - 1];
		}

		T &back()
		{
			assert(!empty());
			return _data[_end - 1];
		}

		void push(const T &e)
		{
			if (_end == _size)
				_allocate();
			_data[_end] = e;
			++_end;
		}

		void push(T &&e)
		{
			if (_end == _size)
				_allocate();
			_data[_end] = e;
			++_end;
		}

		template <typename... Args>
		void emplace(Args &&...args)
		{
			if (_end == _size)
				_allocate();
			_data[_end] = T(args...);
			++_end;
		}

		void pop()
		{
			if (empty())
				return;
			++_start;
			if (_start == _end)
			{
				clear();
			}
		}

		//Reset queue, do not release memory
		void clear()
		{
			_start = 0;
			_end = 0;
		}

		//Release memory
		void release()
		{
			if (_data)
				delete[] _data;
			_start = _end = _size = 0;
			_data = nullptr;
		}

	private:
		void _allocate()
		{
			if (_size == 0)
			{
				_data = new T[_chunkSize]();
				assert(_data != nullptr);
				_size = _chunkSize;
			}
			else
			{
				_size += _chunkSize;
				auto tmp = new T[_size]();
				assert(tmp != nullptr);
				for (std::size_t i = _start; i < _end; ++i)
				{
					tmp[i] = _data[i];
				}
				delete[] _data;
				_data = tmp;
			}
		}

		std::size_t _size;
		std::size_t _start;
		std::size_t _end;
		std::size_t _chunkSize;
		T *_data;
	};
}


#ifdef USE_AGE_CONTAINER
namespace AGE
{
	template<typename T>
	using Queue = AGE::Age_Queue<T>;

	template<typename T>
	static void clearQueue(Queue<T> &q)
	{
		q.clear();
	}

}
#else
	#include <queue>
namespace AGE
{
	template<typename T>
	using Queue = ::std::queue<T>;

	template<typename T>
	static void clearQueue(Queue<T> &q)
	{
		while (!q.empty())
			q.pop();
	}
}
#endif