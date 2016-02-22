// Searches for a possible candidate for a majority element
// Parameters: integer array containing items to look through, a reference to an int which will be used to return the candidate (if any), and the size of the array
// Returns true if a possible candidate is found and false otherwise
bool candidate(int array[], int& candidate2, int sz) {
	// Index for the new array to be used for the next recursive call
	int j = 0;
	// Size of the new array to be used for the next recursive call
	int newsize = 0;
	// The last element of the current array
	int lastElement = 0;
	// Whether a candidate has been found or not
	bool test = false;

	// Create a new array to be filled with the matching numbers; at most, it will need to be half (rounded down) the size of the original array
	int* newarray = new int[sz/2];

	// If the array has an odd number of elements, save the last one in lastElement
	if (sz % 2 != 0) lastElement = array[sz-1];

	// Move through the array by twos
	for (int i=0; i<sz; i+=2) {
		// If there are at least two elements remaining, and the current element equals the next, add the number to the new array
		if (i+1 < sz && array[i] == array[i+1]) {
			newarray[j++] = array[i];
			newsize++;
		}
	}

	// If the new array is empty, it means that there are no candidates other than potentially lastElement
	if (newsize == 0) test = false;
	// If there are elements in the new array, pass it to the function
	else test = candidate(newarray, candidate2, newsize);

	// We have test and potentially lastElement, so we are done with the array
	delete [] newarray;

	// If further recursive calls didn't find anything, and the original array had an odd number of elements, then lastElement is a candidate
	if (!test && sz % 2 != 0) {
		candidate2 = lastElement;
		return true;
	}
	// Otherwise, return test, which is the result of calling this function with the new array 
	else {
		return test;
	}
}

// Looks for a majority element in the given array
// Parameters: a constant integer array in which to search, an integer reference to be used to return the majority element, and the size of the array
bool majorityElement(const int arr[], int& majority, int sz) {
	// Assume there is no majority element
	bool isMajority = false;
	// Special cases for an empty or one-element array
	if (sz == 0) {
		return false;
	}
	if (sz == 1) {
		majority = arr[0];
		return true;
	} 

	// Create a duplicate of arr
	int* array = new int[sz];

	// Will hold the candidate
	int potential = 0;
	// Will be used to count instances of potential in the array
	int count = 0;

	// Copy the constant array over to the new array
	for (int i=0; i<sz; i++) {
		array[i] = arr[i];
	}
	
	// Look for a candidate; if one is found, check if it's a majority element
	if (candidate(array, potential, sz)) {
		for (int i=0; i<sz; i++) {
			if (arr[i] == potential) count++;
		}	
		// If it's more than half the array, it is a majority element
		if (count > sz/2) {
			majority = potential;
			isMajority = true;
		}
	}
	delete [] array;
	return isMajority;
}
