// Usage: a.out a b, where a and b are the integers of which to find the GCD

#include <iostream>
#include <cstdlib>
using namespace std;

// Used if no arguments are given
#define A 35 
#define B 42

// Finds the GCD of the given integers through a loop
int gcdIterative(int a, int b) {
	int gcd = 0;
	int lowest = (a < b ? a : b);
	// Starting at the lowest number, keep counting down and break the loop when a common divisor is found
	for (int i=lowest; i>=1; i--) {
		if (a % i == 0 && b % i == 0) {
			gcd = i;
			break;
		}
	}
	return gcd;
}

// Find the GCD of the given integers recursively
int gcdRecursive(int a, int b) {
	int temp = 0;
	int q = 0;
	int r = 0;
	if (a < b) {
		temp = a;
		a = b;
		b = temp;
	} 

	q = a/b;
	r = a%b;

	if (r==0) return b;
	else return (gcdRecursive(b, r));
}

int main(int argc, char** argv) {
	if (argc==3) {
		cout << "Iterative GCD(" << argv[1] << ", " << argv[2] << ") = " << gcdIterative(atoi(argv[1]), atoi(argv[2])) << "\n";
		cout << "Recursive GCD(" << argv[1] << ", " << argv[2] << ") = " << gcdRecursive(atoi(argv[1]), atoi(argv[2])) << "\n";
	}
	else {
		cout << "Iterative GCD(" << A << ", " << B << ") = " << gcdIterative(A, B) << "\n";
		cout << "Recursive GCD(" << A << ", " << B << ") = " << gcdRecursive(A, B) << "\n";
	}
	return 0;
}
