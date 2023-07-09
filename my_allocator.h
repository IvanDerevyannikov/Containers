#pragma once

#include<utility>


template<typename T>
class My_allocator
{
public:
	My_allocator() {};
	template<typename U>
	My_allocator(const U&) {}

	T* allocate(size_t count)const 
	{
		return reinterpret_cast<T*>(new char[count*sizeof(T)]);
	}
	void deallocate(void* ptr, size_t) 
	{
		delete[] reinterpret_cast<char*>(ptr);

	}
	template<typename ...Args>
	void construct(T* ptr,Args&&... args) 
	{
		new(ptr)T(std::forward<Args>(args)...);
	}
	void destroy(T* ptr)
	{
		ptr->~T();
	}
	template<typename U>
	struct rebind 
	{
		using other = My_allocator<U>;
	};
};