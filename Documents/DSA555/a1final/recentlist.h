#include <iostream>

template <class T>
class RecentList{
	struct Node{
		T data_;
		Node* next_;
		Node* prev_;
		// Constructor; creates a node with the given data and pointers
		Node(const T& data=T{}, Node* next=nullptr, Node* prev=nullptr){
			data_ = data;
			next_ = next;
			prev_ = prev;
		}
	};
	Node* first_;
	Node* last_;
public:
	class const_iterator{
		friend class RecentList;
		Node* curr_;
		const RecentList* myList_;
	protected:
		// Constructor: creates an iterator pointing to the given node in the given list
		const_iterator(Node* curr, const RecentList* myList) {
			curr_ = curr;
			myList_ = myList;
		}
	public:
		// Compares the iterator to a given iterator and returns true if they are the same
		bool operator==(const_iterator right){
			return (myList_ == right.myList_ && curr_ == right.curr_);
		}
		// Compares the iterator to a given iterator and returns false if they are the same
		bool operator!=(const_iterator right){
			return (myList_ == right.myList_ && curr_ != right.curr_);
		}
		const_iterator(){
			curr_ = nullptr;
			myList_ = nullptr;
		}
		// ++i; moves the iterator to the next node and returns it
		const_iterator operator++() {
			curr_ = curr_->next_;
			return *this;
		}
		// --i; moves the iterator to the previous node and returns it
		const_iterator operator--() {
			curr_ = curr_->prev_;
			return *this; 
		}
		//i++; moves the iterator to the next node and returns an iterator to what was the current node
		const_iterator operator++(int){
			Node* temp = curr_;
			curr_ = curr_->next_;
			return const_iterator(temp, myList_);
		}
		//i--; moves the iterator to the previous node and returns an iterator to what was the current node
		const_iterator operator--(int){
			Node* temp = curr_;
			curr_ = curr_->prev_;
			return const_iterator(temp, myList_);
		}
		// Returns the data of the node to which the iterator is pointing
		const T& operator*() const{
			if (curr_) {
				return curr_->data_;
			}
			else {
				// If the iterator isn't pointing at anything, throw an exception
				throw;
			}
		}
	};
	class iterator:public const_iterator{
		friend class RecentList;
		// Constructor calling the parent class's constructor
		iterator(Node* curr, const RecentList* myList) : const_iterator(curr, myList) {
		}
	public:
		iterator() : const_iterator() {
		// ++i; moves the iterator to the next node and returns it
		}
		iterator operator++() {
			const_iterator::curr_ = const_iterator::curr_->next_;
			return *this;
		}
		// --i; moves the iterator to the previous node and returns it
		iterator operator--(){
			const_iterator::curr_ = const_iterator::curr_->prev_;
			return *this;
		}
		//i++; moves the iterator to the next node and returns an iterator to what was the current node
		iterator operator++(int){
			Node* temp = const_iterator::curr_;
			const_iterator::curr_ = const_iterator::curr_->next_;
			return iterator(temp, const_iterator::myList_);
		}
		//i--; moves the iterator to the previous node and returns an iterator to what was the current node
		iterator operator--(int){
			Node* temp = const_iterator::curr_;
			const_iterator::curr_ = const_iterator::curr_->prev_;
			return iterator(temp, const_iterator::myList_);
		}
		// Returns the data of the node to which the iterator is pointing
		const T& operator*() const{
			return const_iterator::operator*();
		}
		// Returns the data of the node to which the iterator is pointing
		T& operator*(){
			if (const_iterator::curr_) {
				return const_iterator::curr_->data_;
			}
			else {
				// If the iterator isn't pointing at anything, throw an exception
				throw;
			}
		}
	};
	// Creates an empty list with sentinels
	RecentList(){
		first_ = new Node(T{}, nullptr, nullptr);
		last_ = new Node (T{}, nullptr, first_);
		first_->next_ = last_; 
	}
	// Returns the first node after the beginning sentinel
	iterator begin(){
		return iterator(first_->next_, this);	
	}
	const_iterator begin() const{
		return const_iterator(first_->next_, this);
	}

	// Returns the ending sentinel
	iterator end(){
		return iterator(last_, this);	
	}
	const_iterator end() const{
		return const_iterator(last_, this);	
	}

	// Inserts a new node (containing the given data) at the start of the list, i.e. after the start sentinel
	void insert(const T& data){
		Node* nn = new Node(data, first_->next_, first_);
		nn->next_->prev_ = nn;
		first_->next_ = nn;
	}

	// Removes a node matching the given data
	void remove(const T& data){
		for (iterator i = begin(); i != end(); i++) {
			if (*i == data) {
				// Link the previous and next nodes to each other before removing this node
				i.curr_->prev_->next_ = i.curr_->next_;
				i.curr_->next_->prev_ = i.curr_->prev_;
				delete i.curr_;
				break;
			}
		}
	}

	// Searches the list for a node matching the given data, and returns an iterator to the node
	const_iterator search(const T& key) const{
		for (const_iterator i = begin(); i != end(); i++) {
			if (*i == key) {
				return i;
			}
		}
		return end();
	}
	
	// Same as above, but moves the node to the front before returing the iterator
	iterator search(const T& key) {
		for (iterator i = begin(); i != end(); i++) {
			if (*i == key) {
				// Link the next and previous nodes to each other
				i.curr_->prev_->next_ = i.curr_->next_;
				i.curr_->next_->prev_ = i.curr_->prev_;
				
				// Link the current node to the prior first node and to the start sentinel
				i.curr_->prev_ = first_;
				i.curr_->next_ = first_->next_;
				i.curr_->next_->prev_ = i.curr_;
				first_->next_ = i.curr_;

				return begin();
			}
		}
		return end();
	}

	// Destructor; deletes all the nodes, including sentinels
	~RecentList(){
		Node* temp = first_;
		while (temp != nullptr) {
			first_ = first_->next_;
			delete temp;
			temp = first_;
		}
		last_ = nullptr;
	}

	// Copy constructor; takes a constant RecentList reference as a parameter and copies all of its nodes
	RecentList(const RecentList& rhs){
		// If the right hand side is not empty...
		if (rhs.first_) {
			// temp moves through the nodes of rhs
			Node* temp = rhs.first_;
			// nn creates new node for this new list
			Node *nn = new Node(temp->data_, nullptr, nullptr);
			first_ = nn;
			while (temp->next_ != NULL) {
				nn->next_ = new Node(temp->next_->data_, nullptr, nn);
				temp = temp->next_;
				nn = nn->next_;
			}
			// nn, at this point, is a copy of rhs's last_ :entinel
			last_ = nn;
		}
	}

	// Move constructor; takes a RecentList reference as a parameter and sets this list to that list
	RecentList(RecentList&& rhs){
		// Move rhs's sentinels and thereby all of rhs's nodes
		first_ = rhs.first_;
		last_ = rhs.last_;
		//rhs.first_ = rhs.last_ = nullptr;
	}
	
	// Assignment operator; takes a constant RecentList reference as a parameter and copies all of its nodes
	RecentList& operator=(const RecentList& rhs){
		// Check for self-assignment
		if (this != &rhs) {
			Node* temp;
			temp = first_;
			// Delete all of this list's nodes
			while (temp != nullptr) {
				first_ = first_->next_;
				delete temp;
				temp = first_;
			}
			// If the given list has nodes, copy them
			if (rhs.first_) {
				// temp moves through the nodes of rhs
				Node* temp = rhs.first_;
				// nn creates new node for this new list
				Node *nn = new Node(temp->data_, nullptr, nullptr);
				first_ = nn;
				while (temp->next_ != NULL) {
						nn->next_ = new Node(temp->next_->data_, nullptr, nn);
						temp = temp->next_;
						nn = nn->next_;
				}
				// nn, at this point, is a copy of rhs's last_ sentinel
				last_ = nn;
			}
		}
		return *this;
	}
		
	// Move operator; moves the given list to this list
	RecentList& operator=(RecentList&& rhs){
		// Check for self-assignment
		if (*this != *rhs) {
			first_ = rhs.first_;
			last_ = rhs.last_;
			rhs.first_ = rhs.last_ = nullptr;
		}
		return *this;
	}
};

