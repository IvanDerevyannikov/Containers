#pragma once
#include<initializer_list>
#include <utility>
#include<functional>
#include "my_allocator.h"
#include "helper.h"

template<typename Key, typename Value, typename Comp = std::less<Key>, typename Alloc = My_allocator<std::pair<const Key, Value> >>
class My_map {

public:

	struct Node;
	template<bool isCnst>
	class Common_Iterator;
	using iterator = Common_Iterator<false>;
	using const_iterator = Common_Iterator<true>;

public:

	using value_type = std::pair<const Key, Value>;
	using Node_alloc = typename Alloc::template rebind<Node>::other;

	struct BaseNode
	{
		BaseNode() = default;
		BaseNode(bool is_black) :is_black(is_black), left(nullptr), right(nullptr),parent(nullptr) {};

		BaseNode* left;
		BaseNode* right;
		BaseNode* parent;
		bool is_black;
	};

	struct Node :BaseNode
	{
		Node() = default;
		Node(const BaseNode& base_node,const value_type& key_val) :BaseNode(base_node),key_val(key_val) {}

		Node(const BaseNode& base_node, const Key key , Value value) : BaseNode(base_node), key_val(std::make_pair(key,value)) {}
		
		std::pair<Key, Value> key_val;
	};


	My_map() :m_alloc(), m_comp()
	{
		m_sheet.is_black = true;
		m_sheet.parent = nullptr;
		m_head =&m_sheet ;
		m_head->parent = nullptr;
	}

	explicit My_map(const Comp& comp, const Alloc& alloc = Alloc()) :m_comp(comp), m_alloc(alloc)
	{
		m_sheet.is_black = true;
		m_sheet.parent = nullptr;
		m_head = &m_sheet;
	}

	explicit My_map(const Alloc& alloc) :m_alloc(alloc)
	{
		m_sheet.is_black = true;
		m_sheet.parent = nullptr;
		m_head = &m_sheet;
	}

	template<typename Iter>
	My_map(Iter first, Iter last, const Comp& comp, const Alloc& alloc=Alloc());

	template<typename Iter>
	My_map(Iter first, Iter last,const Alloc& alloc=Alloc());

	My_map(std::initializer_list<value_type> init, const Comp& comp = Comp(), 
				const Alloc& alloc = Alloc());

	My_map(std::initializer_list<std::pair<const Key,Value>> init, const Alloc&);

	My_map(const My_map& other);
	My_map(const My_map& other, const Alloc& alloc);
	My_map(My_map&& other) noexcept;
	My_map(My_map&& other, const Alloc& alloc);

	~My_map() 
	{
		clear();
	}

	My_map& operator=(const My_map& other);
	My_map& operator=(My_map&& other);

	Value& operator[](const Key& key);
	Value& operator[](Key&& key);
	Value& at(const Key& key);
	const Value& at(const Key& key) const;

	iterator begin();
	iterator end();

	bool empty();
	std::pair<iterator,bool> insert(const value_type& value);
	std::pair<iterator, bool> insert(value_type&& value);
	template<typename InputIt>
	void insert(InputIt first, InputIt last);
	void insert(std::initializer_list<value_type> i_list);

	template<typename ...Args>
	std::pair<iterator, bool> emplace(Args&&... args);

	iterator erase(iterator pos);
	iterator erase(const_iterator pos);
	iterator erase(iterator first, iterator last);
	iterator erase(const_iterator first,const_iterator last);
	size_t erase(const Key& key);

	void clear();
	void depth_l(int height, BaseNode* node);
	void depth_r(int height, BaseNode* node);
	void _depth()
	{
		depth_l(0, m_head);
		//depth_r(0, m_head);
	}
	iterator find(const Key& key);
	const_iterator find(const Key& key) const;

	size_t count(const Key& key);

public:
	template<bool isCnst>
	class Common_Iterator
	{
	public:
		using self_type = Common_Iterator;
		using self_type_reference = Common_Iterator&;
		Common_Iterator() = default;
		Common_Iterator(BaseNode* iter) :m_iter(iter) {}
		Common_Iterator& operator=(const Common_Iterator& other)
		{
			this->m_iter = other.m_iter;
			return *this;
		}

		isConst_t<isCnst, std::pair<Key, Value>&, const std::pair<Key, Value>&> 
			operator*()
		{
			return static_cast<Node*>(m_iter)->key_val;
		}
		isConst_t<isCnst, std::pair<Key, Value>*, const std::pair<Key, Value>*>
			operator->()
		{
			return &operator*();
		}
		self_type_reference operator++()
		{
			m_iter = find_next_node(m_iter);
			return *this;
		}
		self_type_reference operator++(int) 
		{
			auto tmp = Common_Iterator(m_iter);
			++(*this);
			return tmp;
		}
		self_type_reference operator--()
		{
			m_iter = find_prev_node(m_iter);
			return *this;
		}
		self_type_reference operator--(int)
		{
			auto tmp = Common_Iterator(m_iter);
			--(*this);
			return tmp;
		}
		bool operator==(const Common_Iterator& other) const
		{
			return this->m_iter == other.m_iter;
		}
		bool operator!=(const Common_Iterator& other) const
		{
			return !(*this == other);
		}
	private:
		BaseNode* m_iter;

		BaseNode* find_minimum(BaseNode* node)
		{
			while (node->left!=nullptr)
			{
				node = node->left;
			}
			return node;
		}
		BaseNode* find_maximum(BaseNode* node)
		{
			while (node->right != nullptr)
			{
				node = node->right;
			}
			return node;
		}

		BaseNode* find_next_node(BaseNode* node)
		{
			if (node->right != nullptr)
			{
				return find_minimum(node->right);
			}
			else
			{
				BaseNode* parent = node->parent;
				while(parent!=nullptr && node==parent->right)
				{
					node = parent;
					parent = parent->parent;
				}
				return parent;
			}
		}
		BaseNode* find_prev_node(BaseNode* node)
		{
			if (node->left != nullptr)
			{
				return find_maximum(node->left);
			}
			else
			{
				BaseNode* parent = node->parent;
				while (parent != nullptr && node == parent->left)
				{
					node = parent;
					parent = parent->parent;
				}
				return parent;
			}
		}
	};

private:
	BaseNode* m_head;
	BaseNode m_sheet=BaseNode(true);
	Comp m_comp;
	Node_alloc m_alloc;

	template<typename ...Args>
	BaseNode* create_node(Args&&...arg);
	BaseNode* create_node( const value_type& value);
	BaseNode* find_min_node(BaseNode* node);
	BaseNode* find_max_node(BaseNode* node);
	BaseNode* find_node(const Key& key);
	BaseNode* copy_nodes(BaseNode* node);
	void copy_map(BaseNode* my_node, BaseNode* other_node);

	bool is_left_child(BaseNode* child, BaseNode* parent);
	BaseNode* find_grandparent(BaseNode* node);
	BaseNode* find_uncle(BaseNode* node);
	BaseNode* find_brother(BaseNode* node);

	void insert_rebalance_tree(BaseNode* node);
	void insert_case_1(BaseNode* node);
	void insert_case_2(BaseNode* node);
	void erase_rebalance_tree(BaseNode* parent,BaseNode* brother);

	void rotate_nodes(BaseNode* child, BaseNode* parent);
	bool rotate_case_2(BaseNode* node);
	void erase_rotate_case_2_1_1(BaseNode* child, BaseNode* parent);
	void erase_rotate_case_2_2_1(BaseNode* brother, BaseNode* parent);
	BaseNode* find_red_child(BaseNode* node);

	template <typename ...Args>
	void insert_node(Args&&... args);

	void erase_case_1(BaseNode* node);
	void erase_case_2(BaseNode* node);
	void erase_case_3(BaseNode* node,BaseNode* replaceable_node);
	void erase_head(BaseNode* node);

	void delete_node(BaseNode* node);
	void clear_nodes(BaseNode* node);

	//function for test
	void check_parent(BaseNode* node);
};



//
//
//
//
//Implementation
//
//
//
//

//Constructors
template<typename Key, typename Value,typename Comp, typename Alloc>
My_map<Key, Value, Comp,Alloc>::My_map(std::initializer_list<std::pair<const Key, Value>> init, 
								const Comp& comp, const Alloc& alloc):m_comp(comp),m_alloc(alloc)
{
	m_sheet.is_black = true;
	m_sheet.parent = nullptr;
	m_head = &m_sheet;
	this->insert(init);
}


// copy constuctors
template<typename Key, typename Value, typename Comp, typename Alloc>
My_map<Key, Value, Comp, Alloc>::My_map(const My_map& other)
{
	this->m_head = copy_nodes(other.m_head);
	this->m_sheet = other.m_sheet;
	this->m_head->parent = &m_sheet;
	this->m_comp = other.m_comp;
	this->m_alloc = other.m_alloc;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
My_map<Key, Value, Comp, Alloc>::My_map(My_map&& other) noexcept
{
	this->m_head = other.m_head;
	this->m_comp = other.m_comp;
	this->m_alloc = other.m_alloc;

	other.m_head = &other.m_sheet;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
My_map<Key, Value, Comp, Alloc>& My_map<Key, Value, Comp, Alloc>::operator=(const My_map& other)
{
	if (this->m_head == other.m_head)
	{
		return *this;
	}
	if (this->empty())
	{
		this->m_head = copy_nodes(other.m_head);
		this->m_sheet = other.m_sheet;
		this->m_head->parent = &this->m_sheet;
		this->m_comp = other.m_comp;
		this->m_alloc = other.m_alloc;
		return *this;
	}
	copy_map(this->m_head, other.m_head);
	return *this;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
My_map<Key, Value, Comp, Alloc>& My_map<Key, Value, Comp, Alloc>::operator=(My_map&& other)
{
	clear();
	this->m_head = other.m_head;
	this->m_sheet = other.m_sheet;
	this->m_comp = other.m_comp;
	this->m_alloc = other.m_alloc;
	other.m_head = &other.m_sheet;
	return *this;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
Value& My_map<Key, Value, Comp, Alloc>::operator[](const Key& key)
{
	BaseNode* tmp = m_head;
	while (static_cast<Node*>(tmp)->key_val.first != key)
	{
		if (static_cast<Node*>(tmp)->key_val.first > key)
		{
			if (tmp->left == nullptr)
			{
				tmp->left = m_alloc.allocate(1);
				m_alloc.construct(static_cast<Node*>(tmp->left), BaseNode(true), std::pair<const Key, Value>(key, Value()));
				tmp->left->parent = tmp;
				insert_rebalance_tree(tmp->left);
			}
			tmp = tmp->left;
		}
		else if (static_cast<Node*>(tmp)->key_val.first < key)
		{
			if (tmp->right == nullptr)
			{
				tmp->right = m_alloc.allocate(1);
				m_alloc.construct(static_cast<Node*>(tmp->right), BaseNode(true), std::pair<const Key, Value>(key, Value()));
				tmp->right->parent = tmp;
				insert_rebalance_tree(tmp->right);
			}
			tmp = tmp->right;
		}
	}
	return static_cast<Node*>(tmp)->key_val.second;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
Value& My_map<Key, Value, Comp, Alloc>::operator[](Key&& key)
{
	BaseNode* tmp = m_head;
	while (static_cast<Node*>(tmp)->key_val.first != key)
	{
		if (static_cast<Node*>(tmp)->key_val.first > key)
		{
			if (tmp->left == nullptr)
			{
				tmp->left = m_alloc.allocate(1);
				m_alloc.construct(static_cast<Node*>(tmp->left), BaseNode(true), std::pair<const Key, Value>(key, Value()));
				tmp->left->parent = tmp;
				insert_rebalance_tree(tmp->left);
			}
			tmp = tmp->left;
		}
		else if (static_cast<Node*>(tmp)->key_val.first < key)
		{
			//std::cout << static_cast<Node*>(tmp)->key_val.first << ' ' << key << '\n';
			if (tmp->right == nullptr)
			{
				tmp->right = m_alloc.allocate(1);
				m_alloc.construct(static_cast<Node*>(tmp->right), BaseNode(true), std::pair<const Key, Value>(key, Value()));
				tmp->right->parent = tmp;
				insert_rebalance_tree(tmp->right);
			}
			tmp = tmp->right;
		}
	}
	return static_cast<Node*>(tmp)->key_val.second;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
Value& My_map<Key, Value, Comp, Alloc>::at(const Key& key)
{
	auto iter = find(key);
	if (iter == end()) 
	{
		throw std::out_of_range("don't Know this key");
	}
	return iter->second;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::iterator My_map<Key, Value, Comp, Alloc>::begin()
{
	//return Common_Iterator<true>();
	return	iterator(find_min_node(m_head));
}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::iterator My_map<Key, Value, Comp, Alloc>::end()
{
	//return Common_Iterator<true>();
	return iterator(&m_sheet);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
bool My_map<Key, Value, Comp, Alloc>::empty() 
{
	return m_head == &m_sheet;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::clear()
{
	if (m_head == &m_sheet)
	{
		return;
	}
	clear_nodes(m_head);
	m_head = &m_sheet;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
std::pair<typename My_map<Key, Value, Comp, Alloc>::iterator, bool> 
My_map<Key, Value, Comp, Alloc>::insert(const value_type& value)
{
	return emplace(value);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
std::pair<typename My_map<Key, Value, Comp, Alloc>::iterator, bool> My_map<Key, Value, Comp, Alloc>::insert(value_type&& value)
{
	return emplace(value);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
template<typename InputIt>
void My_map<Key, Value, Comp, Alloc>::insert(InputIt first, InputIt last)
{
	while (first != last)
	{
		this->insert_node(*first);
		++first;
	}
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::insert(std::initializer_list<value_type> init)
{
	this->insert(init.begin(), init.end());
}

template<typename Key, typename Value, typename Comp, typename Alloc>
template<typename ...Args>
std::pair<typename My_map<Key, Value, Comp, Alloc>::iterator, bool> My_map<Key, Value, Comp, Alloc>::emplace(Args&& ...args)
{
	Key key;
	if constexpr (Extract_map<Key, rem_ref_cv<Args>...>::is_extract)
	{
		key = Extract_map<Key, rem_ref_cv<Args>...>::get_key(args...);

		if (m_head == &m_sheet)
		{
			m_head = m_alloc.allocate(1);
			m_alloc.construct(static_cast<Node*>(m_head), BaseNode(true), args...);
			m_head->left = nullptr;
			m_head->right = nullptr;
			m_head->parent = &m_sheet;
			return std::make_pair(iterator(m_head), true);
		}
		BaseNode* parent = m_head;
		while (true)
		{
			if (static_cast<Node*>(parent)->key_val.first == key)
			{
				return std::make_pair(iterator(parent), false);
			}
			if (m_comp(static_cast<Node*>(parent)->key_val.first, key))
			{
				if (parent->right == nullptr)
				{
					parent->right = create_node(args...);
					parent->right->parent = parent;
					insert_rebalance_tree(parent->right);
					return std::make_pair(parent->right, true);
				}
				else
				{
					parent = parent->right;
				}
			}
			else
			{
				if (parent->left == nullptr)
				{
					parent->left = create_node(args...);
					parent->left->parent = parent;
					insert_rebalance_tree(parent->left);
					return std::make_pair(parent->left, true);
				}
				else
				{
					parent = parent->left;
				}
			}
		}

	}
}

template<typename Key, typename Value, typename Comp, typename Alloc>
size_t My_map<Key, Value, Comp, Alloc>::erase(const Key& key)
{
	BaseNode* node = find_node(key);
	if (node == &m_sheet || node==nullptr)
	{
		return -1;
	}
	delete_node(node);
	return 1;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
inline void My_map<Key, Value, Comp, Alloc>::depth_l(int height, BaseNode* node)
{
	if (node == nullptr) 
	{
		std::cout << height+1 << '\n';
		return;
	}
	if (node->is_black) 
	{
		depth_l(height + 1, node->left);
		depth_l(height + 1, node->right);
	}
	else
	{
		depth_l(height, node->left);
		depth_l(height, node->right);
	}
}

template<typename Key, typename Value, typename Comp, typename Alloc>
inline void My_map<Key, Value, Comp, Alloc>::depth_r(int height, BaseNode* node)
{
	if (node == nullptr)
	{
		std::cout << height << '\n';
		return;
	}

	//depth(height + 1, node->left);
	depth_r(height + 1, node->right);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::iterator 
My_map<Key, Value, Comp, Alloc>::find(const Key& key)
{
	return iterator(find_node(key));
}

template<typename Key, typename Value, typename Comp, typename Alloc>
size_t My_map<Key, Value, Comp, Alloc>::count(const Key& key)
{
	if (find(key) != end())
	{
		return 1;
	}
	return 0;
}


///
///
///
///
///private methods
///
///
/// 
/// 
/// 
template<typename Key, typename Value, typename Comp, typename Alloc>
template<typename ...Args>
typename My_map<Key, Value, Comp, Alloc>::BaseNode* My_map<Key, Value, Comp, Alloc>::create_node(Args && ...arg)
{
	BaseNode* node = m_alloc.allocate(1);
	m_alloc.construct(static_cast<Node*>(node), BaseNode(false), arg...);
	node->left = nullptr;
	node->right = nullptr;
	node->is_black = false;
	return node;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::BaseNode* My_map<Key, Value, Comp, Alloc>::create_node(const value_type& value)
{
	BaseNode* node = m_alloc.allocate(1);
	m_alloc.construct(static_cast<Node*>(node), BaseNode(false), value);
	node->left = nullptr;
	node->right = nullptr;
	node->is_black = false;
	return node;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
inline bool My_map<Key, Value, Comp, Alloc>::is_left_child(BaseNode* child, BaseNode* parent)
{
	if (child == parent->left)
	{
		return true;
	}
	return false;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::BaseNode*
My_map<Key, Value, Comp, Alloc>::find_grandparent(BaseNode* node)
{
	if (node->parent!=nullptr && node->parent->parent!=nullptr)
	{
		return node->parent->parent;
	}
	return nullptr;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::BaseNode*
My_map<Key, Value, Comp, Alloc>::find_uncle(BaseNode* node)
{
	BaseNode* grandparent = find_grandparent(node);
	if (grandparent == nullptr) throw 1;
	if (is_left_child(node->parent,grandparent))
	{
		return grandparent->right;
	}
	else if (!is_left_child(node->parent,grandparent))
	{
		return grandparent->left;
	}
	return nullptr;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::BaseNode* My_map<Key, Value, Comp, Alloc>::find_brother(BaseNode* node)
{
	if (is_left_child(node, node->parent))
	{
		return node->parent->right;
	}
	else
	{
		return node->parent->left;
	}
	return nullptr;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::rotate_nodes(BaseNode* child,BaseNode* parent)
{
	BaseNode* grandparent = find_grandparent(child);
	auto rotate = [&](BaseNode* child, BaseNode* parent)
	{
		child->parent = grandparent;
		parent->parent = child;
		if (is_left_child(child, parent))
		{
			parent->left = child->right;
			if (child->right != nullptr)
			{
				child->right->parent = parent;
			}
			child->right = parent;
		}
		else
		{
			parent->right = child->left;
			if (child->left != nullptr)
			{
				child->left->parent = parent;
			}
			child->left = parent;
		}
	};

	if (grandparent == &m_sheet)
	{
		rotate(child, parent);
		m_head = child;
	}
	else if (is_left_child(parent,grandparent))
	{
		grandparent->left = child;
		rotate(child, parent);
	}
	else
	{
		grandparent->right = child;
		rotate(child, parent);
	}
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::insert_case_1(BaseNode* node)
{
	if (node == m_head)
	{
		node->is_black = true;
		return;
	}

	BaseNode* grandparent = find_grandparent(node);
	grandparent->is_black = false;
	grandparent->left->is_black = !grandparent->left->is_black;
	grandparent->right->is_black = !grandparent->right->is_black;

	if (grandparent == m_head)
	{
		grandparent->is_black = true;
		return;
	}
	insert_rebalance_tree(grandparent);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::insert_case_2(BaseNode* node)
{
	BaseNode* grandparent = find_grandparent(node);
	if (rotate_case_2(node))
	{
		rotate_nodes(node, node->parent);
		rotate_nodes(node, grandparent);
		node->is_black = true;
	}
	else 
	{
		rotate_nodes(node->parent, grandparent);
		node->parent->is_black = true;
	}
	grandparent->is_black = false;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::erase_rebalance_tree(BaseNode* parent,BaseNode* brother)
{
	auto node_has_red_child = [](BaseNode* node)->bool
	{
		if ((node->left && !node->left->is_black) || (node->right && !node->right->is_black))
		{
			return true;
		}
		return false;
	};

	if (!parent->is_black)
	{
		if (brother == nullptr || (brother->is_black && node_has_red_child(brother)))
		{
			erase_rotate_case_2_1_1(brother, parent);
		}
		else
		{
			parent->is_black = true;
			brother->is_black = false;
		}
	}
	else
	{
		if (brother->is_black && !node_has_red_child(brother))
		{
			if (parent == m_head)
			{
				brother->is_black = false;
				return;
			}
			brother->is_black = false;
			erase_rebalance_tree(parent->parent, find_brother(parent));
		}
		else if (brother->is_black && node_has_red_child(brother))
		{
			erase_rotate_case_2_2_1(brother,parent);
		}
		else
		{
			auto get_brother_child = [&](BaseNode* brother, BaseNode* parent) -> BaseNode*
			{
				return is_left_child(brother, parent) ? brother->right : brother->left;
			};
			brother->is_black = true;
			parent->is_black = false;
			BaseNode* brother_child = get_brother_child(brother, parent);
			rotate_nodes(brother, parent);
			erase_rebalance_tree(parent, brother_child);
		}
	}
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::insert_rebalance_tree(BaseNode* node)
{
	if (node == nullptr || node->parent == nullptr || node->parent->is_black == true)
	{
		return;
	}

	BaseNode* uncle = find_uncle(node);

	if(uncle==nullptr || uncle->is_black == true)
	{
		insert_case_2(node);
	}
	else if (uncle->is_black == false)
	{
		insert_case_1(node);
	}
}

template<typename Key, typename Value, typename Comp, typename Alloc>
template<typename ... Args>
void My_map<Key, Value, Comp, Alloc>::insert_node(Args && ...args)
{
	Key key;
	if constexpr (Extract_map<Key, rem_ref_cv<Args>...>::is_extract)
	{
		key = Extract_map<Key, rem_ref_cv<Args>...>::get_key(args...);
	}

	if (m_head == &m_sheet)
	{
		m_head = m_alloc.allocate(1);
		m_alloc.construct(static_cast<Node*>(m_head), BaseNode(true), args...);
		m_head->left = nullptr;
		m_head->right = nullptr;
		m_head->parent = &m_sheet;
	}
	else
	{
		BaseNode* parent = m_head;
		while (true)
		{
			if (static_cast<Node*>(parent)->key_val.first == key)
			{
				break;
			}
			if (m_comp(static_cast<Node*>(parent)->key_val.first, key))
			{
				if (parent->right == nullptr)
				{
					parent->right = create_node(args...);
					parent->right->parent = parent;
					insert_rebalance_tree(parent->right);
					break;
				}
				else
				{
					parent = parent->right;
				}
			}
			else
			{
				if (parent->left == nullptr)
				{
					parent->left = create_node(args...);
					parent->left->parent = parent;
					insert_rebalance_tree(parent->left);
					break;
				}
				else
				{
					parent = parent->left;
				}
			}
		}
	}

}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::BaseNode* 
My_map<Key, Value, Comp, Alloc>::find_min_node(BaseNode* node)
{
	if (node == nullptr)
	{
		return nullptr;
	}
	if (node->left == nullptr)
	{
		return node;
	}
	find_min_node(node->left);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::BaseNode* My_map<Key, Value, Comp, Alloc>::find_max_node(BaseNode* node)
{
	if (node == nullptr)
	{
		return nullptr;
	}
	if (node->right == nullptr)
	{
		return node;
	}
	find_max_node(node->right);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::erase_case_1(BaseNode* node)
{
	BaseNode* parent = node->parent;
	if (is_left_child(node, parent))
	{
		parent->left = nullptr;
	}
	else
	{
		parent->right = nullptr;
	}
	m_alloc.destroy(static_cast<Node*>(node));
	m_alloc.deallocate(static_cast<Node*>(node), 1);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::erase_case_2(BaseNode* node)
{
	BaseNode* parent = node->parent;

	if (is_left_child(node, parent))
	{
		parent->left = nullptr;
		erase_rebalance_tree(parent, parent->right);
	}
	else
	{
		parent->right = nullptr;
		erase_rebalance_tree(parent, parent->left);
	}
	m_alloc.destroy(static_cast<Node*>(node));
	m_alloc.deallocate(static_cast<Node*>(node), 1);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::erase_case_3(BaseNode* node,BaseNode* replaceable_node)
{
	BaseNode* parent = node->parent;
	if (is_left_child(node, parent))
	{
		parent->left = replaceable_node;
		replaceable_node->parent = parent;
		replaceable_node->is_black = true;
	}
	else
	{
		parent->right = replaceable_node;
		replaceable_node->parent = parent;
		replaceable_node->is_black = true;
	}
	m_alloc.destroy(static_cast<Node*>(node));
	m_alloc.deallocate(static_cast<Node*>(node), 1);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::erase_head(BaseNode* node)
{
	if (node->left == nullptr && node->right == nullptr)
	{
		m_head = &m_sheet;
	}
	else if (node->left == nullptr)
	{
		m_head = node->right;
		node->right->parent = &m_sheet;
	}
	else if (node->right == nullptr)
	{
		m_head = node->left;
		node->left->parent = &m_sheet;
	}
	m_alloc.destroy(static_cast<Node*>(node));
	m_alloc.deallocate(static_cast<Node*>(node), 1);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::delete_node(BaseNode* node)
{
	//BaseNode* parent = node->parent;
	BaseNode* max_left_node = find_max_node(node->left);
	if (max_left_node)
	{
		static_cast<Node*>(node)->key_val = std::move(static_cast<Node*>(max_left_node)->key_val);
		delete_node(max_left_node);
	}
	else
	{
		if (node->parent == &m_sheet)
		{
			erase_head(node);
		}
		else if (!node->is_black && node->left == nullptr && node->right == nullptr)
		{
			erase_case_1(node);
		}
		else if (node->is_black && node->left == nullptr && node->right == nullptr)
		{
			erase_case_2(node);
		}
		else if (node->is_black && (node->left == nullptr|| node->right == nullptr))
		{
			auto find_non_null_node = [](BaseNode* node)
			{
				if (node->left)
				{
					return node->left;
				}
				else
				{
					return node->right;
				}
			};
			erase_case_3(node, find_non_null_node(node));
		}
	}
	//m_alloc.destroy(static_cast<Node*>(node));
	//m_alloc.deallocate(static_cast<Node*>(node), 1);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::clear_nodes(BaseNode* node)
{
	if (node == nullptr) return;
	clear_nodes(node->left);
	clear_nodes(node->right);
	if (is_left_child(node, node->parent))
	{
		node->parent->left = nullptr;
	}
	else 
	{
		node->parent->right = nullptr;
	}
	m_alloc.destroy(static_cast<Node*>(node));
	m_alloc.deallocate(static_cast<Node*>(node), 1);

}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::BaseNode* 
My_map<Key, Value, Comp, Alloc>::find_node(const Key& key)
{
	BaseNode* node = m_head;
	while (node != nullptr)
	{
		if (static_cast<Node*>(node)->key_val.first == key)
		{
			return node;
		}
		else if (m_comp(static_cast<Node*>(node)->key_val.first, key))
		{
			node = node->right;
		}
		else
		{
			node = node->left;
		}
	}
	return &m_sheet;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::BaseNode* 
My_map<Key, Value, Comp, Alloc>::copy_nodes(BaseNode* node)
{
	if (node==nullptr)
	{
		return nullptr;
	}
	BaseNode* copy_node = m_alloc.allocate(1);
	m_alloc.construct(static_cast<Node*>(copy_node), node->is_black,static_cast<Node*>(node)->key_val);
	if (node->left)
	{
		copy_node->left = copy_nodes(node->left);
		copy_node->left->parent = copy_node;
	}
	if (node->right)
	{
		copy_node->right = copy_nodes(node->right);
		copy_node->right->parent = copy_node;
	}
	return copy_node;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::copy_map(BaseNode* my_node, BaseNode* other_node)
{
	if (my_node == nullptr)
	{
		return;
	}
	if (other_node == nullptr)
	{
		if (is_left_child(my_node, my_node->parent))
		{
			my_node->parent->left = nullptr;
		}
		else
		{
			my_node->parent->right = nullptr;
		}
		clear_nodes(my_node);
		return;
		}
	else if (my_node->left == nullptr || my_node->right == nullptr) 
	{
		if (my_node->left == nullptr && other_node->left!=nullptr)
		{
			my_node->left = copy_nodes(other_node->left);
			my_node->left->parent = my_node;
			copy_map(my_node->right, other_node->right);
		}
		if (my_node->right == nullptr&& other_node->right != nullptr)
		{
			my_node->right = copy_nodes(other_node->right);
			my_node->right->parent = my_node;
			copy_map(my_node->left, other_node->left);
		}
		static_cast<Node*>(my_node)->key_val = static_cast<Node*>(other_node)->key_val;
		return;
	}
	static_cast<Node*>(my_node)->key_val = static_cast<Node*>(other_node)->key_val;
	copy_map(my_node->left, other_node->left);
	copy_map(my_node->right, other_node->right);
}

template<typename Key, typename Value, typename Comp, typename Alloc>
bool My_map<Key, Value, Comp, Alloc>::rotate_case_2(BaseNode* node)
{
	BaseNode* grandparent = find_grandparent(node);
	if (is_left_child(node->parent, grandparent) && !is_left_child(node, node->parent))
	{
		return true;
	}
	if (!is_left_child(node->parent, grandparent) && is_left_child(node, node->parent))
	{
		return true;
	}
	return false;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::erase_rotate_case_2_1_1(BaseNode* child, BaseNode* parent)
{
	BaseNode* red_child = find_red_child(child);
	if ((is_left_child(child, parent) && is_left_child(red_child, child))
		|| (!is_left_child(child, parent) && !is_left_child(red_child, child)))
	{
		rotate_nodes(child, parent);
		parent->is_black = true;
		red_child->is_black = true;
		child->is_black = false;
	}
	else 
	{
		rotate_nodes(red_child, child);
		rotate_nodes(red_child, parent);
		parent->is_black = true;
	}
}

template<typename Key, typename Value, typename Comp, typename Alloc>
 void My_map<Key, Value, Comp, Alloc>::erase_rotate_case_2_2_1(BaseNode* brother, BaseNode* parent)
{
	BaseNode* red_child = find_red_child(brother);
	if ((is_left_child(brother, parent) && is_left_child(red_child, brother))
		|| (!is_left_child(brother, parent) && !is_left_child(red_child, brother)))
	{
		rotate_nodes(brother, parent);
		red_child->is_black = true;
	}
	else
	{
		rotate_nodes(red_child, brother);
		rotate_nodes(red_child, parent);
		red_child->is_black = true;
	}
}

template<typename Key, typename Value, typename Comp, typename Alloc>
typename My_map<Key, Value, Comp, Alloc>::BaseNode* My_map<Key, Value, Comp, Alloc>::find_red_child(BaseNode* node)
{
	if (node->left && !node->left->is_black)
	{
		return node->left;
	}
	else if(node->right && !node->right->is_black)
	{
		return node->right;
	}
	return nullptr;
}

template<typename Key, typename Value, typename Comp, typename Alloc>
void My_map<Key, Value, Comp, Alloc>::check_parent(BaseNode* node)
{
	if (node->left == nullptr || node->right == nullptr) return;
	if (node->left->parent != node) throw 1;
	if (node->right->parent != node)
	{
		throw 2;
	}

	if (!node->is_black && !node->left->is_black)
	{
		throw 3;
	}
	if (!node->is_black && !node->right->is_black)
	{
		throw 4;
	}
	check_parent(node->left);
	check_parent(node->right);
}