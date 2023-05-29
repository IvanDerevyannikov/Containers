#pragma once
#include <initializer_list>
#include <exception>
#include "helper.h"

template <typename T>
class MyList
{
	//class Iterator;
public:
	using value_type = T;
	using reference = value_type&;
	template<bool isCnst>
	class Common_iterator;

	using iterator = Common_iterator<false>;
	using const_iterator = Common_iterator<true>;
	using reverse_iterator = ReverseListIter<iterator>;
	using const_reverse_iterator = ReverseListIter<const_iterator>;

	explicit MyList();
	explicit MyList(std::initializer_list<T> init);
	explicit MyList(size_t count);
	explicit MyList(size_t count, const T& value);

	~MyList()
	{
		clear();
	}

	MyList(const MyList& other);
	MyList(MyList&& other)
		noexcept;

	MyList& operator=(MyList& other);
	MyList& operator=(MyList&& other) 
		noexcept;

	void assign(std::size_t count, const T& value)
	{
		clear();
		*this = MyList<T>::MyList(count, value);
	}

	void assign(const std::initializer_list<T>& ilist) 
	{
		clear();
		*this= MyList<T>::MyList(ilist);
	}

	T& front() 
	{
		if (!listSize) throw std::invalid_argument("Empty list");
		return static_cast<Elem*>(baseEl.next)->val;
	}

	T& back() 
	{
		if (!listSize) throw std::invalid_argument("Empty list");
		return static_cast<Elem*>(baseEl.prev)->val;
	}

	iterator begin()
	{
		return iterator(baseEl.next, &baseEl);
	}


	iterator end()
	{
		return iterator(&baseEl, &baseEl);
	}

	const_iterator cbegin()
	{
		return const_iterator(baseEl.next, &baseEl);
	}

	const_iterator cend()
	{
		return const_iterator(&baseEl, &baseEl);
	}

	reverse_iterator rbegin()
	{
		return reverse_iterator(--end());
	}

	reverse_iterator rend()
	{
		return reverse_iterator(end());
	}

	const_reverse_iterator crbegin()
	{
		return const_reverse_iterator(--cend());
	}

	const_reverse_iterator crend()
	{
		return const_reverse_iterator(cend());
	}

	bool empty() {
		return !listSize;
	}

	size_t size()
	{
		return listSize;
	}

	void clear();

	iterator  insert(iterator pos, const T& value)
	{
		BaseElem* next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
		new(next) Elem(value);
		next->next = pos.iter;
		next->prev = pos.iter->prev;
		pos.iter->prev->next = next;
		pos.iter->prev = next;
		pos.iter = next;
		++listSize;
		return pos;
	}

	iterator insert(iterator pos, T&& value) 
	{
		BaseElem* next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
		new(next) Elem(std::move(value));
		next->next = pos.iter;
		next->prev = pos.iter->prev;
		pos.iter->prev->next = next;
		pos.iter->prev = next;
		pos.iter = next;
		++listSize;
		return pos;
	}

	iterator insert(iterator pos, std::size_t count, const T& value) 
	{
		iterator tmp = pos;
		BaseElem* next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
		new(next) Elem(pos->val);
		next->next = pos->next;
		pos->val = value;
		for (std::size_t i = 1; i < count; ++i) 
		{
			pos.iter->next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
			new(pos.iter->next) Elem(value);
			pos.iter->next->prev = pos.iter;
			++pos;
		}
		pos.iter->next = next;
		next->prev = pos.iter;
		listSize+=count;
		return tmp;
	}

	iterator insert(iterator pos, std::initializer_list<T> ilist)
	{
		iterator tmp = pos;
		BaseElem* next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
		new(next) Elem(pos->val);
		next->next = pos->next;
		const auto inIt = ilist.begin();
		pos->val = *(inIt++);
		for (size_t i = 1; i < ilist.size(); ++i)
		{
			pos.iter->next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
			new(pos.iter->next) Elem(*(inIt++));
			pos.iter->next->prev = pos.iter;
			++pos;
		}
		pos.iter->next = next;
		next->prev = pos.iter;
		listSize += ilist.size();
		return tmp;
	}

	iterator insert(iterator pos,iterator left, iterator right)
	{
		if (left.fake != right.fake) throw std::length_error("other iterators");
		size_t size_iter = 0;
		BaseElem* next = pos.iter;
		BaseElem* tmp = pos.iter->prev;
		while (left != right)
		{
			pos.iter = new Elem(*(left++));
			pos.iter->prev =tmp;
			tmp->next = pos.iter;
			tmp = tmp->next;
			++pos;
			++size_iter;
		}
		
		pos.iter = next;
		pos.iter->prev =tmp ;
		tmp->next = pos.iter;
		listSize +=size_iter;

		return pos;
	}

	template<typename... Args>
	iterator emplace(iterator pos, Args&&... arg)
	{
		auto tmp = pos;
		BaseElem* empl=reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
		new (empl)Elem(T(std::forward<Args>(arg)...));
		empl->prev = pos->next;
		empl->next = pos.operator->();
		pos->prev = empl;
		++listSize;
		return --tmp;
	}

	iterator erase(iterator pos) 
	{
		if (pos.fake != this->begin().fake) throw std::out_of_range("Bad iterator");
		if (pos.iter == pos.fake) throw std::out_of_range("Out of range");
		iterator tmp = pos;
		++tmp;
		tmp.iter->prev = pos.iter->prev;
		pos.iter->prev->next = tmp.iter;
		static_cast<Elem*>(pos.iter)->val.~T();
		delete[] reinterpret_cast<char*>(pos.iter);
		pos = iterator();
		--(this->listSize);
		return tmp;
	}

	iterator erase(iterator first, iterator last)
	{
		if (first.fake != this->begin().fake || last.fake != this->begin().fake ) throw std::out_of_range("Bad iterator");
		iterator tmp = first;
		first.iter->prev->next = last.iter;
		if (last.iter != last.fake) 
		{
			last.iter->prev = tmp->prev;
		}
		else 
		{
			last.iter->prev = last.iter;
		}

		while (first != last) {
			++tmp;
			static_cast<Elem*>(first.operator->())->val.~T();
			delete[] reinterpret_cast<char*>(first.operator->());
			first = tmp;
		}
		return tmp;
	}

	void push_back(const T& value);
	void push_back(T&& value);
	template<typename... Args>
	void emplace_back(Args&&... args);
	void pop_back();

	void push_front(const T& value);
	void push_front(T&& value);
	template <typename... Args>
	void emplace_front(Args&&... args);
	void pop_front();

	void resize(size_t count);
	void resize(size_t count, const T& value);

	void swap(MyList& other) 
		noexcept;

	void reverse() 
		noexcept;

	void splice(iterator pos, MyList&& other);
	void splice(iterator pos, MyList&& other, iterator it);
	void splice(iterator pos, MyList&& other, iterator first, iterator last);

	size_t remove(const T& value);
	template<typename UnaryPredicate>
	size_t remove_if(UnaryPredicate p);

	void merge(MyList& other);
	void merge(MyList&& other);
	template<typename Compare>
	void merge(MyList& other, Compare comp);
	template<typename Compare>
	void merge(MyList&& other, Compare comp);

	void sort();
	template<typename Compare>
	void sort(const Compare& comp);

	bool operator==(const MyList& other);


private:
	struct BaseElem
	{
		BaseElem() = default;
		BaseElem(BaseElem* next, BaseElem* prev = nullptr) :next(next), prev(prev) {}

		BaseElem operator++() 
		{
			return next;
		}

		BaseElem operator--() 
		{
			return prev;
		}
		BaseElem* next, * prev;
	};
	struct Elem :BaseElem
	{
		Elem() :val()
		{
		}
		Elem(const T& val) : val(val)
		{
		}
		Elem(BaseElem* next, BaseElem* prev = nullptr) :BaseElem(next, prev)
		{
		}
		Elem( BaseElem* next, BaseElem* prev, const int& val) :BaseElem(next, prev), val(val)
		{
		}
		~Elem() 
		{
		}

		T val;
	};

	template <bool isCnst>
	class Common_iterator {
	public:
		using self_type = Common_iterator;
		using self_type_reference = Common_iterator&;
		using type = isConst_t<isCnst,T,const T>;
		using type_reference = isConst_t<isCnst, T&, const T&>;
		using type_pointer = isConst_t<isCnst, T*, const T*>;
		Common_iterator()
		{
		}
		Common_iterator(BaseElem* el, BaseElem* fake) :iter(el), fake(fake)
		{
		}
		Common_iterator(const Common_iterator& other) {
			iter = other.iter;
			fake = other.fake;
		}
		type_reference operator*()const {
			if (iter == nullptr) throw std::out_of_range("Bad Iterator");
			if (iter == fake) throw std::out_of_range("Out of range");
			return static_cast<Elem*>(iter)->val;
		}
		isConst_t<isCnst, T*, const T*>  operator->()const 
		{
			if (iter == nullptr) throw std::out_of_range("Bad Iterator");
			if (iter == fake) throw std::out_of_range("Out of range");
			return &static_cast<Elem*>(iter)->val;
		}
		Common_iterator& operator++() 
		{
			if (iter == fake) throw int(1);
			iter = iter->next;
			return *this;
		}
		Common_iterator& operator++(int) 
		{
			if (iter == fake) throw int(1);
			Common_iterator tmp(iter, fake);
			iter = iter->next;
			return tmp;
		}
		Common_iterator& operator--() 
		{
			if (iter == fake->next) throw int(1);
			iter = iter->prev;
			return *this;
		}
		Common_iterator& operator--(int)
		{
			if (iter == fake->next) throw int(1);
			Common_iterator tmp(iter, fake);
			iter = iter->next;
			return tmp;
		}
		Common_iterator& operator=(const Common_iterator& other) 
		{
			this->iter = other.iter;
			this->fake = other.fake;
			return *this;
		}

		bool operator==(const Common_iterator& other) const 
		{
			return iter == other.iter;
		}
		bool operator!=(const Common_iterator& other) const 
		{
			return !((*this) == other);
		}

		BaseElem* iter = nullptr;
		BaseElem* fake = nullptr;
	};


	bool isSorted(iterator first);


	template<typename Pred>
	bool isSorted(iterator first,Pred pred);

	BaseElem* split(BaseElem* head)
	{
		if (head == &baseEl || head->next == &baseEl) return head;
		BaseElem* fast = head;
		BaseElem* slow = head;
		while (fast->next != &baseEl && fast->next->next != &baseEl)
		{
			fast = fast->next->next;
			slow = slow->next;
		}
		fast = slow->next;
		slow->next = &baseEl;
		return fast;

	}
	template <typename Compare>
	BaseElem* mergeNode(BaseElem* left, BaseElem* right, const Compare& comp)
	{
		if (left == &baseEl) return right;
		if (right == &baseEl) return left;

		if (comp(static_cast<Elem*>(left)->val,static_cast<Elem*>(right)->val))
		{
			left->next = mergeNode(left->next, right,comp);
			left->next->prev = left;
			return left;
		}
		else
		{
			right->next = mergeNode(left, right->next,comp);
			right->next->prev = right;
			return right;
		}
	}
	template <typename Compare>
	BaseElem* mergeSort(BaseElem* head, const Compare& comp)
	{
		if (head == &baseEl || head->next == &baseEl) return head;
		BaseElem* right = split(head);

		head = mergeSort(head,comp);
		right = mergeSort(right,comp);

		return mergeNode(head, right,comp);
	}
	
	BaseElem baseEl;
	size_t listSize;
};



//Defenition of methods;
template <typename T>
MyList<T>::MyList<T>() :listSize(0)
{
	baseEl.next = &baseEl;
	baseEl.prev = &baseEl;
}

template <typename T>
MyList<T>::MyList(std::initializer_list<T> init) :listSize(init.size())
{
	if (!listSize)
	{
		baseEl.next = &baseEl;
		baseEl.prev = &baseEl;
	}
	else
	{
		auto inIt = init.begin();
		baseEl.next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
		new(baseEl.next) Elem(*(inIt++));
		BaseElem* dummy = baseEl.next;
		BaseElem* dummyPrev = baseEl.next;
		dummy->prev = &baseEl;
		for (size_t i = 1; i < init.size(); ++i)
		{
			dummy->next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
			dummy = dummy->next;
			new(dummy) Elem(*(inIt++));
			dummy->prev = dummyPrev;
			dummyPrev = dummy;
		}
		dummy->next = &baseEl;
		baseEl.prev = dummy;
	}

}
template <typename T>
MyList<T>::MyList(size_t count) :listSize(count)
{
	if (!listSize)
	{
		baseEl = nullptr;
	}
	else
	{
		baseEl.next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
		new(baseEl.next) Elem();
		BaseElem* dummy = baseEl.next;
		BaseElem* dummyPrev = baseEl.next;
		dummy->prev = &baseEl;
		for (size_t i = 1; i < count; ++i)
		{
			dummy->next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
			dummy = dummy->next;
			new(dummy) Elem();
			dummy->prev = dummyPrev;
			dummyPrev = dummy;
		}
		dummy->next = &baseEl;
		baseEl.prev = dummy;

	}
}

template <typename T>
MyList<T>::MyList(size_t count, const T& value) :listSize(count)
{
	if (!listSize)
	{
		baseEl.next = &baseEl;
		baseEl.prev = &baseEl;
	}
	else
	{
		baseEl.next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
		new(baseEl.next) Elem(value);
		BaseElem* dummy = baseEl.next;
		BaseElem* dummyPrev = baseEl.next;
		dummy->prev = &baseEl;
		for (size_t i = 1; i < count; ++i)
		{
			dummy->next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
			dummy = dummy->next;
			new(dummy) Elem(value);
			dummy->prev = dummyPrev;
			dummyPrev = dummy;
		}
		dummy->next = &baseEl;
		baseEl.prev = dummy;
	}
}

template <typename T>
MyList<T>::MyList(const MyList& other)
{
	if (!other.listSize)
	{
		baseEl.next = &baseEl;
		baseEl.prev = &baseEl;
	}
	else
	{
		iterator otherIter = other.begin();
		baseEl.next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
		new(baseEl.next) Elem(*otherIter++);
		BaseElem* dummy = baseEl.next;
		BaseElem* dummyPrev = baseEl.next;
		for (otherIter; otherIter != other.end(); ++otherIter)
		{
			dummy->next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
			dummy = dummy->next;
			new(dummy) Elem(*otherIter);
			dummy->prev = dummyPrev;
			dummyPrev = dummy;
		}
		dummy->next = &baseEl;
		baseEl.prev = dummy;
	}
	listSize = other.listSize;
}

template <typename T>
MyList<T>::MyList(MyList&& other)
noexcept
{
	this->baseEl = other.baseEl;
	this->listSize = other.listSize;
	other.baseEl.next = nullptr;
	other, baseEl.prev = nullptr;
	other.listSize = 0;
}

template <typename T>
MyList<T>& MyList<T>::operator=( MyList& other)
{
	if (&baseEl == &other.baseEl) return *this;

	if (!other.listSize)
	{
		this->clear();
		baseEl.next = &baseEl;
		baseEl.prev = &baseEl;
	}
	else
	{
		iterator otherIter = other.begin();
		if (!this->listSize)
		{
			baseEl.next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
			new(baseEl.next) Elem(*otherIter++);
			BaseElem* dummy = baseEl.next;
			BaseElem* dummyPrev = baseEl.next;
			for (otherIter; otherIter != other.end(); ++otherIter)
			{
				dummy->next = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
				dummy = dummy->next;
				new(dummy) Elem(*otherIter);
				dummy->prev = dummyPrev;
				dummyPrev = dummy;
			}
			dummy->next = &baseEl;
			baseEl.prev = dummy;
		}
		else
		{
			auto dummy = this->begin();
			while (otherIter != other.end() && dummy != this->end())
			{
				*dummy = *otherIter;
				++dummy;
				++otherIter;
			}
			if (dummy != this->end())
			{
				this->erase(dummy, this->end());
			}
			if (otherIter != other.end()) {
				this->insert(this->end(), otherIter, other.end());
			}
		}
	}
	listSize = other.listSize;
	return *this;
}
template <typename T>
MyList<T>& MyList<T>::operator=(MyList&& other) noexcept
{
	this->clear();
	this->baseEl.next = other.baseEl.next;
	this->baseEl.prev = other.baseEl.prev;
	this->baseEl.prev->next = &this->baseEl;
	this->listSize = other.listSize;
	other.baseEl.next = nullptr;
	other.baseEl.prev = nullptr;
	other.listSize = 0;
	return *this;
}
template <typename T>
void MyList<T>::clear()
{
	if (listSize == 0) return;
	else if (listSize == 1) {
		static_cast<Elem*> (baseEl.next)->val.~T();
		delete[] reinterpret_cast<char*> (baseEl.next);
		listSize = 0;
		baseEl.next = &baseEl;
		baseEl.prev = &baseEl;
		return;
	}
	BaseElem* del = baseEl.next;
	BaseElem* next = baseEl.next->next;
	while (del != &baseEl)
	{
		static_cast<Elem*> (del)->val.~T();
		delete[] reinterpret_cast<char*>(del);
		del = next;
		next = next->next;
	}
	listSize = 0;
	baseEl.next = &baseEl;
	baseEl.prev = &baseEl;
}



template <typename T>
void MyList<T>::push_back(const T& value)
{
	BaseElem* last = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
	new(last) Elem(value);
	last->prev = baseEl.prev;
	last->next = &baseEl;
	baseEl.prev->next = last;
	baseEl.prev = last;
	++listSize;
}

template <typename T>
void MyList<T>::push_back(T&& value) 
{
	BaseElem* last = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
	new(last) Elem(std::move(value));
	last->prev = baseEl.prev;
	last->next = &baseEl;
	baseEl.prev->next = last;
	baseEl.prev = last;
	++listSize;
}

template<typename T>
template<typename... Args>
void MyList<T>::emplace_back(Args&&... args)
{
	BaseElem* last = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
	new(last) Elem(T(std::forward<Args>(args)...));
	last->prev = baseEl.prev;
	last->next = &baseEl;
	baseEl.prev->next = last;
	baseEl.prev = last;
	++listSize;
}

template<typename T>
void  MyList<T>::pop_back()
{
	BaseElem* del = baseEl.prev;
	del->prev->next = del->next;
	del->next->prev = del->prev;
	static_cast<Elem*> (del)->val.~T();
	delete[] reinterpret_cast<char*>(del);
	--listSize;
}

template<typename T>
void MyList<T>::push_front(const T& value)
{
	BaseElem* first = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
	new(first) Elem(value);
	first->prev = &baseEl;
	first->next = baseEl.next;
	baseEl.next->prev = first;
	baseEl.next = first;
	++listSize;
}

template<typename T>
void MyList<T>::push_front(T&& value)
{
	BaseElem* first = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
	new(first) Elem(std::move(value));
	first->prev = &baseEl;
	first->next = baseEl.next;
	baseEl.next->prev = first;
	baseEl.next = first;
	++listSize;
}

template<typename T>
template<typename... Args>
void MyList<T>::emplace_front(Args&&... args)
{
	BaseElem* first = reinterpret_cast<Elem*>(new char[sizeof(Elem)]);
	new(first) Elem(T(std::forward<Args>(args)...));
	first->prev = &baseEl;
	first->next = baseEl.next;
	baseEl.next->prev = first;
	baseEl.next = first;
	++listSize;
}

template<typename T>
void MyList<T>::pop_front()
{
	BaseElem* del = baseEl.next;
	del->prev->next = del->next;
	del->next->prev = del->prev;
	static_cast<Elem*> (del)->val.~T();
	delete[] reinterpret_cast<char*> (del);
	--listSize;
}

template<typename T>
void MyList<T>::resize(size_t count)
{
	if (this->size() >= count)
	{
		while (this->size() != count) 
		{
			this->pop_back();
		}
	}
	else
	{
		while (this->size() != count) 
		{
			this->push_back(std::move(T()));
		}
	}
	listSize = count;
}

template<typename T>
void MyList<T>::resize(size_t count, const T& value)
{
	if (this->size() >= count)
	{
		while (this->size() != count)
		{
			this->pop_back();
		}
	}
	else
	{
		while (this->size() != count)
		{
			this->push_back(value);
		}
	}
	listSize = count;
}

template<typename T>
void MyList<T>::swap(MyList& other) noexcept 
{
	std::swap(baseEl.prev->next, other.baseEl.prev->next);
	std::swap(baseEl.next->prev, other.baseEl.next->prev);
	std::swap(baseEl.next, other.baseEl.next);
	std::swap(baseEl.prev, other.baseEl.prev);
}

template<typename T>
void MyList<T>::reverse()
noexcept
{
	if (listSize)
	{
		BaseElem* copy = &baseEl;
		BaseElem* tmp;
		for (;;)
		{
			tmp = copy->prev;
			copy->prev = copy->next;
			copy->next = tmp;
			if (copy->next == &baseEl) break;
			copy = copy->next;
		}
	}
}

template <typename T>
void MyList<T>::splice(iterator pos, MyList&& other)
{
	other.baseEl.next->prev = pos.iter->prev;
	pos.iter->prev->next = other.baseEl.next;
	other.baseEl.prev->next = pos.iter;
	pos.iter->prev = other.baseEl.prev;

	pos.iter = other.baseEl.next;
	other.baseEl.next = &other.baseEl;
	other.baseEl.prev = &other.baseEl;
	this->listSize += other.listSize;
	other.listSize = 0;
}

template<typename T>
void MyList<T>::splice(iterator pos, MyList&& other, iterator it)
{
	it.iter->prev->next = it->next;
	it->next->prev=it->prev;
	pos.iter->prev->next = it.iter;
	it.iter->prev = pos.iter->prev;
	it.iter->next = pos.iter;
	pos.iter->prev = it.iter;

	pos.iter = it.iter;
	++this->listSize;
	--other.listSize;
}

template<typename T>
void MyList<T>::splice(iterator pos, MyList&& other, iterator first, iterator last)
{
	first.iter->prev->next = last.iter;
	last.iter->next->prev = first.iter->prev;
	pos.iter->prev->next = first.iter;
	first.iter->prev = pos.iter->prev;
	pos.iter->prev = last.iter->prev;
	last.iter->prev->next = pos.iter;

	size_t size = 0;
	while (first.iter != last.iter->prev)
	{
		++first;
		++size;
	}
	this->listSize+=size;
	other.listSize -= size;
}

template<typename T>
size_t MyList<T>::remove(const T& value)
{
	return remove_if([&](const T& el) {return el == value; });
}

template<typename T>
template< typename UnaryPredicate >
size_t MyList<T>::remove_if(UnaryPredicate p)
{
	size_t result = 0;
	BaseElem* iter = baseEl.next;
	while (iter != &baseEl)
	{
		if (p(static_cast<Elem*>(iter)->val))
		{
			BaseElem* deleted = iter;
			iter = iter->next;
			deleted->prev->next = iter;
			iter->prev = deleted->prev;
			static_cast<Elem*>(deleted)->val.~T();
			delete[] reinterpret_cast<char*>(deleted);
			++result;
		}
		else
		{
			iter=iter->next;
		}
	}
	return result;
}

template<typename T>
void MyList<T>::merge(MyList<T>& other)
{
	merge(other, [](T& left, T& right) {return left < right; });
}

template<typename T>
void MyList<T>::merge(MyList<T>&& other)
{
	merge(std::move(other), [](T& left, T& right) {return left < right; });
}

template<typename T>
template<typename Compare>
void MyList<T>::merge(MyList<T>& other,Compare comp)
{
	if (isSorted(this->begin()) != true || isSorted(other.begin()) != true) throw std::invalid_argument("Not sorted list");
	if (&this->baseEl == &other.baseEl) return;
	BaseElem* myIter = baseEl.next;
	BaseElem* otherIter = other.baseEl.next;
	while (myIter != &baseEl && otherIter != &other.baseEl)
	{
		if (comp(static_cast<Elem*>(myIter)->val,static_cast<Elem*>(otherIter)->val)) myIter = myIter->next;
		else
		{
			BaseElem* nextOther = otherIter->next;
			BaseElem* nextMy = myIter;
			BaseElem* prevMy = myIter->prev;
			myIter = otherIter;
			myIter->prev = prevMy;
			myIter->next = nextMy;
			prevMy->next = myIter;
			nextMy->prev = myIter;
			otherIter = nextOther;
		}
	}
	if (myIter == &baseEl)
	{
		BaseElem* prev = myIter->prev;
		BaseElem* start = &baseEl;
		myIter = otherIter;
		myIter->prev = prev;
		prev->next = myIter;
		other.baseEl.prev->next = &baseEl;
	}
	listSize += other.listSize;
	other.baseEl.next = &other.baseEl;
	other.baseEl.prev = &other.baseEl;
	other.listSize = 0;
}

template<typename T>
template<typename Compare>
void MyList<T>::merge(MyList<T>&& other, Compare comp)
{
	if (isSorted(this->begin()) != true || isSorted(other.begin()) != true) throw std::invalid_argument("Not sorted list");
	if (&this->baseEl == &other.baseEl) return;
	auto myIter = baseEl.next;
	auto otherIter = other.baseEl.next;
	while (myIter != &baseEl && otherIter != &other.baseEl)
	{
		if (comp(static_cast<Elem*>(myIter)->val, static_cast<Elem*>(otherIter)->val)) myIter = myIter->next;
		else
		{
			BaseElem* nextOther = otherIter->next;
			BaseElem* nextMy = myIter;
			BaseElem* prevMy = myIter->prev;
			myIter = otherIter;
			myIter->prev = prevMy;
			myIter->next = nextMy;
			prevMy->next = myIter;
			nextMy->prev = myIter;
			otherIter = nextOther;
		}
	}
	if (myIter == &baseEl)
	{
		BaseElem* prev = myIter->prev;
		BaseElem* start = &baseEl;
		myIter = otherIter;
		myIter->prev = prev;
		prev->next = myIter;
		other.baseEl.prev->next = &baseEl;
	}
	listSize += other.listSize;
	other.baseEl.next = &other.baseEl;
	other.baseEl.prev = &other.baseEl;
	other.listSize = 0;
}

template<typename T>
void MyList<T>::sort() 
{
	baseEl.next = mergeSort(baseEl.next, [](T& left, T& right) {return left < right; });
}

template<typename T>
template<typename Compare>
void MyList<T>::sort(const Compare& comp)
{
	baseEl.next = mergeSort(baseEl.next, comp);
}

template<typename T>
bool MyList<T>::operator==(const MyList& other) 
{
	if (this->listSize != other.listSize) return false;
	BaseElem* thisNode = baseEl.next;
	BaseElem* otherNode = other.baseEl.next;
	while (thisNode != &baseEl)
	{
		if (static_cast<Elem*>(thisNode)->val != static_cast<Elem*>(otherNode)->val) return false;
		thisNode = thisNode->next;
		otherNode = otherNode->next;
	}
	return true;
}
template<typename T>
bool MyList<T>::isSorted(iterator first)
{
	while (first.iter->next != first.fake)
	{
		if (static_cast<Elem*>(first.iter)->val > static_cast<Elem*>(first.iter->next)->val) return false;
		++first;
	}
	return true;
}