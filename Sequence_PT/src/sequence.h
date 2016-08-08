#include <iostream>
#include <stack>

template<class T>
class sequence {
private:
    struct Node {
        Node(T key) : m_Key(key), m_Left(0), m_Right(0) {}

        ~Node() {
            delete m_Left;
            delete m_Right;
        }

        T& getKey() {
            return m_Key;
        }

        friend std::ostream& operator<<(std::ostream& os, const Node& crArg) {
            if(crArg.m_Left) {
                os << *crArg.m_Left;
            }
            os << crArg.m_Key << " | ";
            if(crArg.m_Right) {
                os << *crArg.m_Right;
            }

            return os;
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

	Iterator begin() {
		return Iterator(m_Root);
	}

	Iterator end() {
		return Iterator(0);
	}

	class ConstIterator {
		// callable by const sequences
	};
	class ReversIterator {

	};
	class ConstReversIterator {

	};

	// constructors
	sequence() : m_Root(0) {}
	sequence(T t) : m_Root(t) {}
	sequence(const sequence& crArg) {
		// copy constructor
	}
	~sequence() {

	}

	// methods

	// operators
	friend sequence<T> operator+(const sequence<T>& crArg1, const sequence<T>& crArg2) {

	}
	friend std::ostream& operator<<(std::ostream& os, const sequence<T>& crArg) {

	}

private:
	Node* m_Root;
};
