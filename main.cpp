#include <list>
#include <iostream>
#include "MyList.h"


struct A {
	A(int val) :val(val) {
	
	}
	~A() {
		std::cout << "Deleted A" << '\n';
	}

	int val;
};


int main()
{
	std::list<int> lst1{ 10 };
	std::list<int> lst2{ -1,4,3,2,1 };
	lst2.sort([](int& i, int& j) { return i > j; });
	//lst2.splice(lst2.begin(),std::move(lst1),lst1.begin(),--lst1.end());
	//lst2.erase(lst2.end());
	for (auto& i : lst2) { std::cout << i << ' '; }
	std::cout << '\n';
	//for (auto& i : lst1) { std::cout << i << ' '; }
	//std::cout << '\n';

	MyList<int> me{1,2,3,4,5};
	MyList<int> me2{6,7,8,9,10};
	//me2.remove_if([](int n) { return n > 0; });
	//me2.splice(me2.begin(),std::move(me),me.begin(),me.end());
	auto iterator = me2.crbegin();
	//std::cout << *iterator;
	for (iterator; iterator!=me2.crend();++iterator) 
	{
		std::cout << *iterator << ' '; 
	}
	return 0;
}