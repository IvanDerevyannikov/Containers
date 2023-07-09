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



template <typename First, typename... Rest>
First getFirstArgument(First first, Rest...) {
	return first;
}

//template<typename Key,typename Value>
//struct Extract_map<Key, Value>
//{
//	static constexpr bool is_extract = true;
//	static const Key& get_key(const Key& key, const Value&)
//	{
//		return key;
//	}
//};
//
//template<typename Key, typename First,typename Second>
//struct Extract_map<Key, std::pair<const First,Second>>
//{
//	static constexpr bool is_extract = std::is_same_v<Key,First>;
//	static const Key& get_key(const std::pair<const First, Second>& key_val )
//	{
//		return key_val.first;
//	}
//};

template <class _Ty>
using rem_ref_cv = std::remove_cv_t<std::remove_reference_t<_Ty>>;


template <class _Key, class... _Args>
struct Extract_map {
	// by default we can't extract the key in the emplace family and must construct a node we might not use
	static constexpr bool is_extract = false;
};

template <class _Key, class _Second>
struct Extract_map<_Key, _Key, _Second> {
	// if we would call the pair(key, value) constructor family, we can use the first parameter as the key
	static constexpr bool is_extract = true;
	static const _Key& get_key(const _Key& _Val, const _Second&) noexcept {
		return _Val;
	}
};

template <class _Key, class _First, class _Second>
struct Extract_map<_Key, std::pair<_First, _Second>> {
	// if we would call the pair(pair<other, other>) constructor family, we can use the pair.first member as the key
	static constexpr bool is_extract = std::is_same_v<_Key, rem_ref_cv<_First>>;
	static const _Key& get_key(const std::pair<_First, _Second>& _Val) {
		return _Val.first;
	}
};

//template <typename First>
//First getFirstArgument(First first) {
//	return first;
//}