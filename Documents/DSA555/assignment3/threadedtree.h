#include <iostream>
using namespace std;

template <class T>
class ThreadedTree{
	struct Node{
		T data_;
		Node* left_;
		Node* right_;
		bool leftIsThread_;
		bool rightIsThread_;
		Node(const T& data=T{},Node* left=nullptr, Node* right=nullptr){
			data_=data;
			left_=left;
			right_=right;
			leftIsThread_=true;
			rightIsThread_=true;
		}
	};
	Node* root_;	
	// Could use these for constant runtime for begin() and end()?
	//Node* smallest_;
	//Node* largest_;
	void print(const Node* rt, int lvl) const{
		//I won't test this on the main.  You might find it useful
		//to implement for testing purposes
		//Note, passed in lvl so that you can use indentations based on lvl to 
		//see where a node is within your tree.  if you don't want to use it
		//just alter the prototype.
		if (!rt->leftIsThread_)
			print(rt->left_, lvl+1);
		cout << rt->data_ << " ";
		if (!rt->rightIsThread_)
			print(rt->right_, lvl+1);
	}

public:
	class const_iterator{
	protected:
		Node* curr_;
		const_iterator(Node* c){curr_=c;}
	public:
		const_iterator(){
			curr_=nullptr;
		}
		// Increments the const_iterator
		const_iterator operator++(int){
			// If there is a node, go to its right
			if (curr_) {
				Node* temp = curr_;
				curr_ = curr_->right_;
				// If there is still a node, go as left as possible
				if (curr_)
					// This ensures we don't follow a thread back to the original node
					while (curr_->left_ > temp)
						curr_ = curr_->left_;
				return iterator(temp);
			}	
			else {
				return *this;
			}
		}
		// Decrements the const_iterator
		const_iterator operator--(int){
			// If there is a node, go left 
			if (curr_) {
				Node* temp = curr_;
				curr_ = curr_->left_;
				// Now go all the way to the right
				if (curr_)
					// Make sure we don't go back to the original node via a thread
					while (curr_->right_ > temp)
						curr_ = curr_->right_;
				return iterator(temp);
			}	
			else {
				return *this;
			}
		}
		const T& operator*(){
			return curr_->data_;
		}
		bool operator==(const const_iterator& rhs) const{
			return curr_ == rhs.curr_;
		}
		bool operator!=(const const_iterator& rhs) const{
			return curr_ != rhs.curr_;
		}
		friend class ThreadedTree;
	};
	class iterator:public const_iterator{
		iterator(Node* c):const_iterator(c){}
	public:
		iterator():const_iterator(){}
		const T& operator*(){
			return this->curr_->data_;
		}
		// Increments the iterator
		iterator operator++(int){
			if (const_iterator::curr_) {
				Node* temp = const_iterator::curr_;
				const_iterator::curr_ = const_iterator::curr_->right_;
				if (const_iterator::curr_)
					while (const_iterator::curr_->left_ > temp)
						const_iterator::curr_ = const_iterator::curr_->left_;
				return iterator(temp);
			}	
			else {
				return *this;
			}
		}
		// Decrements the iterator
		iterator operator--(int){
			if (const_iterator::curr_) {
				Node* temp = const_iterator::curr_;
				const_iterator::curr_ = const_iterator::curr_->left_;
				if (const_iterator::curr_)
					while (const_iterator::curr_->right_ < temp)
						const_iterator::curr_ = const_iterator::curr_->right_;
				return iterator(temp);
			}
			else {
				return *this;
			}
		}
		friend class ThreadedTree;
	};

	ThreadedTree(){
		root_=nullptr;
		//smallest_ = largest_ = root_;
	}

	// Inserts a new node with the given data at the proper location, and sets threads accordingly
	iterator insert(const T& data){
		Node* temp = root_;
		Node* nn = nullptr;
		// If the tree is empty, create a root
		if (!root_)
		{
			root_ = new Node(data);
		//	smallest_ = root_;
		//	largest_ = root_;
			return iterator(root_);
		}
		/*
		else if (data < smallest_->data_) {
			nn = new Node(data, nullptr, smallest_);
			nn->rightIsThread_ = true;
			smallest_->left_ = nn;
			smallest_->leftIsThread_ = false;
			smallest_ = nn;
		}
		else if (data > largest_->data_) {
			nn = new Node(data, largest_, nullptr);
			nn->leftIsThread_ = true;
			largest_->right_ = nn;
			largest_->rightIsThread_ = false;
			largest_ = nn;
		}
		*/
		else {
			// While no new node has been created, keep moving through the tree
			while (!nn) {
				// Move on to the left or right subtree
				if (temp->data_ < data && !temp->rightIsThread_) {
					temp = temp->right_;
				}
				else if (temp->data_ > data && !temp->leftIsThread_) {
					temp = temp->left_;
				}
				else {
					// Add the new node either to the right or left, and set its pointers back to its parent and to its parent's threaded link
					if (temp->data_ > data) {
						nn = new Node(data, temp->left_, temp);
						nn->rightIsThread_ = true;
						temp->left_ = nn;
						temp->leftIsThread_ = false;
					}
					else {
						nn = new Node(data, temp, temp->right_);
						nn->leftIsThread_ = true;
						temp->right_= nn;
						temp->rightIsThread_ = false;
					}
				}
			}
			return iterator(nn);
		}
	}
	// Finds a node with the given data, if it exists; otherwise it returns a null pointer
	iterator find(const T& data) const{
		Node* n = root_;
		// Move through the tree, following threads as appropriate
		while (n) {
			if (n->data_ == data)
				return iterator(n);
			else if (n->data_ > data)
				n = n->left_;
			else
				n = n->right_;
		}
		return iterator(nullptr);
	}
	// Returns an iterator to the smallest node
	iterator begin(){
		Node* n = root_;
		while (n && n->left_ && !n->leftIsThread_) n = n->left_;
		return iterator(n);	
		//return iterator(smallest_);
	}
	// Returns an iterator to the null pointer beyond the largest node
	iterator end(){
		Node* n = root_;
		while (n && !n->rightIsThread_) n = n->right_;
		if (n) return iterator(n->right_);
		else return iterator(n);
		//if (!largest_) return iterator(largest_);
		//return iterator(largest_->right_);
	}
	// Returns an iterator to the smallest node
	const_iterator begin()const{
		Node* n = root_;
		while (n && n->left_ && !n->leftIsThread_) n = n->left_;
		return const_iterator(n);	
		//return const_iterator(smallest_);
	}
	// Returns an iterator to the null pointer beyond the largest node
	const_iterator end() const{
		Node* n = root_;
		while (n && !n->rightIsThread_) n = n->right_;
		if (n) return const_iterator(n->right_);
		else return const_iterator(n);
		//if (!largest_) return const_iterator(largest_);
		//return const_iterator(largest_->right_);
	}
	void print() const{
		print(root_,0);
	}
	// Deletes a node and all its subtrees, recursively
	void clear(Node* r) {
		if (r) {
			if (r->left_ && !r->leftIsThread_) clear(r->left_);
			if (r->right_ && !r->rightIsThread_) clear(r->right_);
			delete r;
		}
	}
	// Destructor
	~ThreadedTree(){
		//cout << "Destroying ";
		//print();
		//cout << endl;
		clear(root_);
	}
};
