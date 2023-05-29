#pragma once
#include <iostream>
#include <exception>

template<bool isSorted,typename T,typename U>
struct isConst {
	using type = T;
};

template<typename T, typename U>
struct isConst<true, T, U> {
	using type = U;
};

template<bool isSorted, typename T, typename U>
using isConst_t = typename isConst<isSorted, T, U>::type;

template<typename Iter>
class ReverseListIter {
	Iter iter;
public:
	using referance = typename Iter::type_reference;
	using pointer = typename Iter::type_pointer;

	ReverseListIter(const Iter& iter) :iter(iter) {}
	referance operator*() const 
	{
		return *iter;
	}
	pointer operator->() 
	{
		return &iter.operator*();
	}
	ReverseListIter<Iter>& operator++() 
	{
		if (iter.iter == iter.fake) throw(std::out_of_range("out of range"));
		if (iter.iter->prev == iter.fake)
		{
			iter.iter = iter.fake;
		}
		else {
			--iter;
		}
		return *this;
	}
	ReverseListIter <Iter> operator++(int) 
	{
		ReverseListIter tmp(iter);
		operator++();
		return tmp;
	}
	ReverseListIter<Iter>& operator--() 
	{
		if (iter.iter == iter.fake)
		{
			iter.iter = iter.iter->next;
		}
		else
		{
		  ++iter;
		}
		return *this;
	}
	ReverseListIter<Iter>& operator--(int) 
	{
		ReverseListIter tmp(iter);
		operator--();
		return tmp;
	}
	bool operator ==(const ReverseListIter& other)
	{
		return this->iter == other.iter;
	}
	bool operator!=(const ReverseListIter& other)
	{
		return !operator==(other);
	}
};