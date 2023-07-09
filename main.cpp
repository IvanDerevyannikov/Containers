#include <list>
#include "MyList.h"
#include "my_map.h"
#include <iostream>
#include <map>
#include <utility>
struct A {
	A(int val,int val2) : val(val) {
		std::cout << "Construct A" << '\n' << '\n';
	}
	~A() {
		std::cout << "Deleted A" << '\n' << '\n';
	}

	bool operator>(const A& other) const {
		return val > other.val;
	}
	bool operator<(const A& other) const {
		return val < other.val;
	}
	int val;
	bool operator==(const A& other) const {
		return val == other.val;
	}
};


int main()
{
	std::map<int, int> mp{{1, 1}, { 2,1 }, { 3,1 }, { 4,1 }};
	//MyList<int> lst{1, 2, 3, 4, 5, 6, 7, 8, 9};
	//auto lst_it = lst.begin();
	//mp.emplace({ 100, 1 });
	//mp.emplace(10, 1);
	//mp.emplace(-1,2);
	//std::cout << (lst_it != lst.end());
	//std::pair<int, int> par = emp(std::make_pair(2, 3));
	My_map<int, int> mpp;
	const std::pair<int, int> par2 = { -1,-1 };
	for (int i = 0; i < 4000; ++i)
	{
		std::pair<int, int> par = { i,i };
		mpp.insert(std::make_pair(i + 1000, i));
		mpp.insert(std::move(par));
		mpp.emplace(i + 2000, i);
		mpp.emplace(std::make_pair(i + 3000,i));
	}
	//mpp.insert(par2);
	mpp.insert({ std::make_pair(-10,2),std::make_pair(-20,2),std::make_pair(-30,2),std::make_pair(-40,2),std::make_pair(-50,2),std::make_pair(-60,2),std::make_pair(-70,2),std::make_pair(-80,2) });
	auto iter = mpp.begin();
	for(int i=0;i<10;++i)
	{
		std::cout << iter->first << '\n';
		++iter;
	}
	//mpp.emplace(std::make_pair(-1, -1));
	//mpp.emplace(1,1);
	//mpp.emplace(0, 20);

	mpp._depth();

	return 0;
}