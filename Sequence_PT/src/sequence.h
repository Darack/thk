#include <iostream>
#include <stack>

template<class T>
class sequence {
private:
    struct Node {
        Node(T key) : m_Key(key), m_Left(0), m_Right(0) {}

        // todo reference counting
        ~Node() {
            delete m_Left;
            delete m_Right;
        }

        T& getKey() {
            return m_Key;
        }

        T m_Key;
        Node* m_Left;
        Node* m_Right;
    };

public:
	// iterators
	class Iterator {
	public:
		Iterator(Node* elem) : stack() {
			buildStack(elem);
		}

		friend bool operator!=(const Iterator& crI1, const Iterator& crI2) {
			return crI1.stack != crI2.stack;
		}

		T& operator*() {
			return stack.top()->m_Key;
		}

		Iterator operator++() {
			Node* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Right);

			return *this;
		}

	private:
		void buildStack(Node* elem) {
			while(elem) {
				stack.push(elem);
				elem = elem->m_Left;
			}
		}

		// members
		std::stack<Node*> stack;
	};

	class ConstIterator {
	public:
		ConstIterator(Node* elem) : stack() {
			buildStack(elem);
		}

		friend bool operator!=(const ConstIterator& crI1, const ConstIterator& crI2) {
			return crI1.stack != crI2.stack;
		}

		const T& operator*() {
			return stack.top()->m_Key;
		}

		ConstIterator operator++() {
			Node* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Right);

			return *this;
		}

	private:
		void buildStack(Node* elem) {
			while(elem) {
				stack.push(elem);
				elem = elem->m_Left;
			}
		}

		// members
		std::stack<Node*> stack;
	};

	class ReversIterator {
	public:
		ReversIterator(Node* elem) : stack() {
			buildStack(elem);
		}

		friend bool operator!=(const ReversIterator& crI1, const ReversIterator& crI2) {
			return crI1.stack != crI2.stack;
		}

		T& operator*() {
			return stack.top()->m_Key;
		}

		ReversIterator operator++() {
			Node* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Left);

			return *this;
		}

	private:
		void buildStack(Node* elem) {
			while(elem) {
				stack.push(elem);
				elem = elem->m_Right;
			}
		}

		// members
		std::stack<Node*> stack;
	};

	class ConstReversIterator {
	public:
		ConstReversIterator(Node* elem) : stack() {
			buildStack(elem);
		}

		friend bool operator!=(const ConstReversIterator& crI1, const ConstReversIterator& crI2) {
			return crI1.stack != crI2.stack;
		}

		const T& operator*() {
			return stack.top()->m_Key;
		}

		ConstReversIterator operator++() {
			Node* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Left);

			return *this;
		}

	private:
		void buildStack(Node* elem) {
			while(elem) {
				stack.push(elem);
				elem = elem->m_Right;
			}
		}

		// members
		std::stack<Node*> stack;
	};

	Iterator begin() {
		return Iterator(m_Root);
	}

	Iterator end() {
		return Iterator(0);
	}

	ConstIterator cbegin() const {
		return ConstIterator(m_Root);
	}

	ConstIterator cend() const {
		return ConstIterator(0);
	}

	ReversIterator rbegin() {
		return ReversIterator(m_Root);
	}

	ReversIterator rend() {
		return ReversIterator(0);
	}

	ConstReversIterator crbegin() const {
		return ConstReversIterator(m_Root);
	}

	ConstReversIterator crend() const {
		return ConstReversIterator(0);
	}

	// constructors
	sequence() : m_Root(0) {}
	sequence(T arg) {
		//m_Root = new Node(arg);
		insert(arg);
	}
	sequence(const sequence& crArg) {
		// copy constructor
	}
	~sequence() {

	}

	// methods
	void insert(T arg) {
		m_Root = new Node(arg);
	}

	// operators
	friend sequence<T> operator+(const sequence<T>& crArg1, const sequence<T>& crArg2) {

	}

	friend std::ostream& operator<<(std::ostream& os, const sequence<T>& crArg) {
		for(sequence<T>::ConstIterator i = crArg.cbegin(); i != crArg.cend(); ++i) {
			os << *i << " ";
		}
		os << std::endl;
		return os;
	}

	sequence<T>& operator=(const sequence& crArg) {

	}

private:
	Node* m_Root;
};
