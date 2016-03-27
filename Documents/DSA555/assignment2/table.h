#include <string>
#include <utility>
#include <functional>
#include <iostream> // for testing
using namespace std;
int news = 0;
int deletes = 0;

template <class TYPE>
class Table{
public:
	Table(){}
	virtual bool update(const string& key, const TYPE& value)=0;
	virtual bool remove(const string& key)=0;
	virtual bool find(const string& key, TYPE& value)=0;
	virtual ~Table(){}
};

template <class TYPE>
class SimpleTable:public Table<TYPE>{

	struct Record{
		TYPE data_;
		string key_;
		Record(const string& key, const TYPE& data){
			key_=key;
			data_=data;
		}

	};

	Record** records_;   //the table
	int max_;           //capacity of the array
	int size_;          //current number of records held
	int search(const string& key);
	void sort();
	void QuickSort(int left, int right);
	void grow();
public:
	SimpleTable(int maxExpected);
	SimpleTable(SimpleTable& other);
	SimpleTable(SimpleTable&& other);
	virtual bool update(const string& key, const TYPE& value);
	virtual bool remove(const string& key);
	virtual bool find(const string& key, TYPE& value);
	virtual const SimpleTable& operator=(SimpleTable& other);
	virtual const SimpleTable& operator=(SimpleTable&& other);
	virtual ~SimpleTable();
};

// Binary search; assumes the table is sorted by key
// O(logn)
//returns index of where key is found.
template <class TYPE>
int SimpleTable<TYPE>::search(const string& key){
	int rc=-1;
	int low = 0;
	int high = size_-1;
	int mid = 0; 
	while (low <= high && rc == -1){
		mid = (high+low)/2;	
		if (records_[mid]->key_ < key) {
			low = mid + 1;
		}
		else if (records_[mid]->key_ > key) {
			high = mid - 1;
		}
		else {
			rc=mid;
		}
	}
	return rc;
}

// Wrapper function for quick sort
template <class TYPE>
void SimpleTable<TYPE>::sort() {
	if (size_ > 1)
		QuickSort(0, size_-1);
}

// Quick sort
// O(nlogn) usually, but O(n^2) worst case
template <class TYPE>
void SimpleTable<TYPE>::QuickSort(int left, int right) {
	int pivotPt;
	int mid = (left+right)/2;
	int i = left;
	int j = right-1;
	Record* temp;

	// Sort only if there are more than two items to consider (i.e. size_ > 1)
	if (left < right) {
		// Median of three partitioning
		if (records_[left]->key_ > records_[right]->key_) {
			if (records_[left]->key_ > records_[mid]->key_) {
				if (records_[mid]->key_ > records_[right]->key_)
					pivotPt = mid;
				else
					pivotPt = right;
			}
			else {
				pivotPt = left;
			}
		}
		else {
			if (records_[right]->key_ > records_[mid]->key_) {
				if (records_[mid]->key_ > records_[left]->key_) 
					pivotPt = mid;
				else
					pivotPt = left;
			}
			else {
				pivotPt = right;
			}
		}

		// Move the pivot to the right of the array by swapping it with the rightmost element 
		temp = records_[right];
		records_[right] = records_[pivotPt];
		records_[pivotPt] = temp;

		// Set the pivot key
		string pivot = records_[right]->key_;

		// Go through the array until i and j meet
		while (i < j) {
			// Move i forward until we find a value greater than the pivot
			while (records_[i]->key_ < pivot) i++;
			// Move j back until we find a value less than the pivot
			while (records_[j]->key_ > pivot) j--;
			// If i and j still haven't met, swap their records
			if (i < j) {
				temp = records_[i];
				records_[i] = records_[j];
				records_[j] = temp;
			}
		}

		// Move the pivot record back to where it was, and return the rightmost record to its proper place
		temp = records_[right];
		records_[right] = records_[i];
		records_[i] = temp;

		// Now sort the left and right parts of the array
		QuickSort(left, i-1);
		QuickSort(i+1, right);
	}
}

// O(n), because we copy over each element into a new array
//grow the array by one element
template <class TYPE>
void SimpleTable<TYPE>::grow(){
	Record** newArray=new Record*[max_+1]();
	max_=max_+1;
	for(int i=0;i<size_;i++){
		newArray[i]=records_[i];
	}
	delete [] records_;
	records_=newArray;
}

// Constructor that creates a SimpleTable of max size maxExpected
template <class TYPE>
SimpleTable<TYPE>::SimpleTable(int maxExpected): Table<TYPE>(){
	records_=new Record*[maxExpected]();
	max_=maxExpected;
	size_=0;
}

// Copy constructor
// O(n), because the records are copied one by one
template <class TYPE>
SimpleTable<TYPE>::SimpleTable(SimpleTable<TYPE>& other){
	records_ = new Record*[other.max_]();
	max_=other.max_;
	size_=0;
	for(int i=0;i<other.size_;i++){
		records_[i] = new Record(other.records_[i]->key_, other.records_[i]->data_);
		size_++;	
	}
}

// Move constructor
template <class TYPE>
SimpleTable<TYPE>::SimpleTable(SimpleTable<TYPE>&& other){
	size_=other.size_;
	max_=other.max_;
	records_=other.records_;
	other.records_=nullptr;
	other.size_=0;
	other.max_=0;
}

// Adds a record with the given key and value to the table
// If the record already exists and we are only modifying it, this function is O(logn)
// If it doesn't exist and we have to add it, it is O(n)
template <class TYPE>
bool SimpleTable<TYPE>::update(const string& key, const TYPE& value){
	// Search for the record; O(logn)
	int idx=search(key);
	// If it doesn't exist..
	if(idx==-1){
		// If the array is at its limit, embiggen it
		if(size_ == max_){
			grow();
		}
	
		// All the following code should be O(n), because we are looking through each element in the array sequentially until we find the right spot
		// i will be the index for the table
		int i = 0;
		
		// Move through the array until we reach the place in which we want to put the new record
		while (i < size_ && records_[i]->key_ < key) {
			i++;	
		}

		// Now, if we are not at the end of the array, put in the new record and shift everything else to the right
		if (i < size_) {
			// Save the record that is currently at i
			Record* oldRecord = records_[i];
			Record* temp;
			// Insert the new record to the location at i
			records_[i] = new Record(key, value);
			i++;
			size_++;
			// Put the old record after the new one and shift every other record to the right
			while (i < size_) {
				temp = records_[i];
				records_[i] = oldRecord;
				oldRecord = temp;	
				i++;
			}
		}
		// If we are at the end of the array, just insert the record at the end
		else {
			records_[i] = new Record(key, value);
			size_++;
		}

	}
	// If it does exist, just update it
	else{
		records_[idx]->data_=value;
	}
	// This function will always either add a new record or update an existing one
	return true;
}

// Removes from the table the element with the given key
// O(n) if removal is successful, because we must shift each successive element to the left to fill the empty space
// O(logn) if the record we want to remove isn't in the table, because we still need to search for it
template <class TYPE>
bool SimpleTable<TYPE>::remove(const string& key){
	// Search is O(logn)
	int idx=search(key);
	if(idx!=-1){
		delete records_[idx];
		// Shift every record after the deleted one to the left
		for(int i=idx;i<size_-1;i++){
			records_[i]=records_[i+1];
		}
		size_--;
		return true;
	}
	else{
		return false;
	}
}

// Finds the record with the given key and sets value to its data_
// O(logn), because of the search
template <class TYPE>
bool SimpleTable<TYPE>::find(const string& key, TYPE& value){
	int idx=search(key);
	if(idx==-1)
		return false;
	else{
		value=records_[idx]->data_;
		return true;
	}
}

// Assignment operator
// O(n), because we are 1) deleting each record in the current array and 2) copying each record from the other array
template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(SimpleTable<TYPE>& other){
	// Check for self-assignment
	if(this!=&other){
		// If records exist, delete them
		if(records_){
			// Delete each record
			for(int i=0;i<size_;i++){
				delete records_[i];
				records_[i] = nullptr;
			}
			// Delete the records array
			delete [] records_;
		}
		records_=new Record*[other.max_];
		max_=other.max_;
		size_=0;
		// Copy each one of other's records
		for(int i=0;i<other.size_;i++){
			records_[i] = new Record(other.records_[i]->key_, other.records_[i]->data_);
			size_++;	
		}

	}
	return *this;
}

// Assignment operator, but we steal the other table's stuff
// O(n) if records exist, because we have to delete them
// O(1) otherwise
template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(SimpleTable<TYPE>&& other){
	// Check for self-assignment
	if(this!=&other){
		// If records exist, delete them
		if(records_){
			for(int i=0;i<size_;i++){
				delete records_[i];
				records_[i] = nullptr;
			}
			delete [] records_;
		}
		records_ = other.records_;
		max_ = other.max_;
		size_ = other.size_;
	}
	return *this;
}

// Destructor
// O(n), because we have to delete every record
template <class TYPE>
SimpleTable<TYPE>::~SimpleTable(){
	if(records_){
		for(int i=0;i<size_;i++){
			delete records_[i];
			records_[i] = nullptr;
		}
		delete [] records_;
	}
}

template <class TYPE>
class HashTable:public Table<TYPE>{
	struct Record{
		TYPE data_;
		string key_;
		Record(const string& key, const TYPE& data){
			key_=key;
			data_=data;
		}

	};
	Record** records_;   //the table
	int max_;           //capacity of the array
	int size_;          //current number of records held
	int search(const string& key);
	void grow();
public:
	HashTable(int maxExpected);
	HashTable(HashTable& other);
	HashTable(HashTable&& other);
	virtual bool update(const string& key, const TYPE& value);
	virtual bool remove(const string& key);
	virtual bool find(const string& key, TYPE& value);
	virtual const HashTable& operator=(HashTable& other);
	virtual const HashTable& operator=(HashTable&& other);
	virtual ~HashTable();
};

// Constructor; builds a HashTable with the given max capacity
template <class TYPE>
HashTable<TYPE>::HashTable(int maxExpected): Table<TYPE>(){
	max_ = maxExpected;
	records_ = new Record*[max_]();
	size_ = 0;
}

// Copy constructor
// O(n), because we copy each individual record if it exists
template <class TYPE>
HashTable<TYPE>::HashTable(HashTable<TYPE>& other){
	records_ = new Record*[other.max_]();
	size_ = 0;
	max_ = other.max_;

	// Look at each space in the other array
	for (int i=0; i<max_; i++) {
		// If there is a record there, copy it over
		if (other.records_[i]) {
			records_[i] = new Record(other.records_[i]->key_, other.records_[i]->data_);
			size_++;
		}
	}
}

// Double the array's size
// O(n), because we copy over each element into a new array
template <class TYPE>
void HashTable<TYPE>::grow(){
	Record** newArray=new Record*[max_*2]();
	Record** oldArray = records_;
	int oldMax = max_;
	max_=max_*2;
	records_=newArray;
	size_ = 0;	

	// Put each record into its new properly-modulated spot
	for(int i=0;i<oldMax;i++){
		if (oldArray[i]) {
			// Update is constant, so we can call it
			update(oldArray[i]->key_, oldArray[i]->data_);
			delete oldArray[i];
			oldArray[i] = nullptr;
		}
	}

	delete [] oldArray;
}

// Add a record with the given key and value to the hash table
// This is O(1) if there is no collision and the array doesn't need to be doubled
// O(n) if the array is too small and must grow
// Even without grow(), worst case is O(n) if there is a collision and the cluster is very large
template <class TYPE>
bool HashTable<TYPE>::update(const string& key, const TYPE& value){
	// Get the hash value of the key and its rightful index
	size_t idx = hash<string>()(key) % max_;

	// If adding a new record would put the array over 70% capacity, grow it
	// This is O(n)
	if (size_+1 > (int)(max_*0.7))
		grow();

	// Move through the array until an open spot is found; that is, set idx to the the first index after the cluster
	// O(n) at worst, because we are looking at each element after the index, and the cluster may be almost n elements long
	// O(1) at best
	while (records_[idx] && records_[idx]->key_ != key) {
		idx++;
		// If we reach the end of the array, loop back around
		if (idx >= max_)
			idx = 0;
	}	

	// If we found the record in the array, update its value
	if (records_[idx] && records_[idx]->key_ == key) {
		records_[idx]->data_ = value;
	}
	// If we didn't find it, put it in
	else {
		records_[idx] = new Record(key, value);
		size_++;
	}

	// The function will never fail to update, so it should return true
	return true;
}

// Remove an element from the table
// O(1) best case
// O(n^2) worst case, if the cluster is very long and each element in it is in the wrong place
template <class TYPE>
bool HashTable<TYPE>::remove(const string& key){
	// Get the hash value of the key and its rightful index
	size_t idx = hash<string>()(key) % max_;
	// i and temp will be used to move through the rest of the cluster after removal
	size_t i = 0;
	Record* temp = nullptr;

	// Move through the cluster; stop only when the key is found or when we've left the cluster
	// O(1) best case, O(n) worst
	while (records_[idx] && records_[idx]->key_ != key) {
		idx++;
		// Loop around the array
		if (idx >= max_)
			idx = 0;
	}

	// If we find the record, we have to delete it
	if (records_[idx]) {
		// Delete it
		delete records_[idx];
		records_[idx] = nullptr;
		size_--;

		// Next, we have to go through the rest of the cluster and move every element at an improper index to its right place
		i =  idx + 1;
		if (i >= max_) i = 0;

		// Move through the cluster...
		while (records_[i]) {
			// If an element's current index is not what it's supposed to be, fix it
			if (hash<string>()(records_[i]->key_) % max_ != i) {
			 	// Save the record
				temp = records_[i];
				// Set the index in the array to null, effectively splitting the cluster into two
				records_[i] = nullptr;
				// Add the saved record to its proper place; it will be somewhere in the first of the split clusters 
				// This should be O(1), and at the worst case O(n)
				update(temp->key_, temp->data_);
				// Since the update function will have created a new record, we have to delete the temporary one
				delete temp;
				temp = nullptr;
			}
			// Continue passing through the current cluster
			i++;
			if (i >= max_) i = 0;
		}
	}
	else {
		return false;
	}

	return true;
}

// Find the element in the table with the given key
// O(1) best case
// O(n) worst case, if the cluster is very long
template <class TYPE>
bool HashTable<TYPE>::find(const string& key, TYPE& value){
	// Get the hash value of the key and its rightful index
	size_t idx = hash<string>()(key) % max_;

	// Move through the cluster; stop only when the key is found or when we've left the cluster
	while (records_[idx] && records_[idx]->key_ != key) {
		idx++;
		// Loop around the array
		if (idx >= max_)
			idx = 0;
	}

	// If a record exists at idx, it means we left the loop because it was our record
	if (records_[idx]) {
		// Return its data
		value = records_[idx]->data_;
		return true;
	}
	// If not, we've left the cluster so the record doesn't exist in the table
	else {
		return false;
	}
}

// Assignment operator
// O(n), because we have to delete every record that exists in the table, and also copy every record in the other one
template <class TYPE>
const HashTable<TYPE>& HashTable<TYPE>::operator=(HashTable<TYPE>& other){
	// Check for self-assignment
	if (this != &other) {
		// If we have records, get rid of them
		if (records_) {
			// Go through each space in the array
			for (int i=0; i<max_; i++) {
				// If there's a record there, make it go away
				if (records_[i]) {
					delete records_[i];
					records_[i] = nullptr;
				}
			}
			// Delete the records array
			delete [] records_;
			records_ = nullptr;
		}

		records_ = new Record*[other.max_]();
		size_ = 0;
		max_ = other.max_;

		// If the other table has records, copy them
		if (other.records_) {
			for (int i=0; i<max_; i++) {
				if (other.records_[i]) {
					update(other.records_[i]->key_, other.records_[i]->data_);
				}
			}
		}
	}

	return *this;
}

// Assignment operator
// O(n) if removal of records is necessary
template <class TYPE>
const HashTable<TYPE>& HashTable<TYPE>::operator=(HashTable<TYPE>&& other){
	// Check for self-assignment
	if (this != &other) {
		// If there are records, throw them away
		if (records_) {
			for (int i=0; i<max_; i++) {
				if (records_[i]) {
					delete records_[i];
					records_[i] = nullptr;
				}
			}
			delete [] records_;
			records_ = nullptr;
		}

		records_ = other.records_;
		size_ = other.size_;
		max_ = other.max_;
	}
	return *this;
}

// Destructor
// O(n), because we delete every record
template <class TYPE>
HashTable<TYPE>::~HashTable(){
	// If we have records stop having them
	if (records_) {
		for (int i=0; i<max_; i++) {
			if (records_[i]) {
				delete records_[i];
				records_[i] = nullptr;
			}
		}
		delete [] records_;
	}
}
