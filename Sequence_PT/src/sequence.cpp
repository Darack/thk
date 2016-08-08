#include <iostream>

using namespace std;

template<class T>
class sequence {
public:
	// iterators
	class Iterator {
		// can change content
	};
	class ConstIterator {
		// callable by const sequences
	};
	class ReversIterator {

	};
	class ConstReversIterator {

	};

	// constructors
	sequence() {
		// generate new empty sequence
	}
	sequence(T t) {
		// generate new sequence with t
	}
	sequence(const sequence& crArg) {
		// copy constructor
	}
	~sequence() {

	}

	// methods

	// operators
	friend sequence<T> operator+(const sequence<T>& crArg1, const sequence<T>& crArg2) {

	}
};
