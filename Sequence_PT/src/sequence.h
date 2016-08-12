#include <iostream>
#include <stack>

template<class T>
class sequence {
private:
    struct SmartNode {
        SmartNode(T* key) : m_Key(key), m_Left(0), m_Right(0), m_puiCnt(new unsigned(1)) {}
        SmartNode(SmartNode* left, SmartNode* right) : m_Key(0), m_Left(left), m_Right(right), m_puiCnt(new unsigned(0)) {}

        //SmartNode(const SmartNode& crArg) : m_Key(crArg.m_Key), m_Left(crArg.m_Left), m_Right(crArg.m_Right), m_puiCnt(crArg.m_puiCnt) {
        //	++*m_puiCnt;
        //}

        // todo reference counting
        ~SmartNode() {
            delete m_Left;
            delete m_Right;
        }

        T& operator*() const {
        	return *m_Key;
        }

        T* operator->() const {
            return m_Key;
        }

        T* m_Key;
        SmartNode* m_Left;
        SmartNode* m_Right;
        unsigned* m_puiCnt;
    };

public:
	// iterators
	class Iterator {
	public:
		Iterator(SmartNode* elem) : stack() {
			buildStack(elem);
		}

		friend bool operator!=(const Iterator& crI1, const Iterator& crI2) {
			return crI1.stack != crI2.stack;
		}

		T& operator*() {
			return *stack.top()->m_Key;
		}


		Iterator operator++() {
			SmartNode* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Right);

			return *this;
		}

		SmartNode* getNode() {
			return stack.top();
		}

	private:
		void buildStack(SmartNode* elem) {
			while(elem) {
				stack.push(elem);
				elem = elem->m_Left;
			}
		}

		// members
		std::stack<SmartNode*> stack;
	};

	// TODO andere iteratoren ändern
	class ConstIterator {
	public:
		ConstIterator(SmartNode* elem) : stack() {
			buildStack(elem);
		}

		friend bool operator!=(const ConstIterator& crI1, const ConstIterator& crI2) {
			return crI1.stack != crI2.stack;
		}

		const T& operator*() {
			return *stack.top()->m_Key;
		}

		ConstIterator operator++() {
			SmartNode* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Right);

			return *this;
		}

		SmartNode* getNode() {
			return stack.top();
		}

	private:
		void buildStack(SmartNode* elem) {
			while(elem) {
				stack.push(elem);
				elem = elem->m_Left;
			}
		}

		// members
		std::stack<SmartNode*> stack;
	};

	class ReversIterator {
	public:
		ReversIterator(SmartNode* elem) : stack() {
			buildStack(elem);
		}

		friend bool operator!=(const ReversIterator& crI1, const ReversIterator& crI2) {
			return crI1.stack != crI2.stack;
		}

		T& operator*() {
			return stack.top()->m_Key;
		}

		ReversIterator operator++() {
			SmartNode* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Left);

			return *this;
		}

	private:
		void buildStack(SmartNode* elem) {
			while(elem) {
				stack.push(elem);
				elem = elem->m_Right;
			}
		}

		// members
		std::stack<SmartNode*> stack;
	};

	class ConstReversIterator {
	public:
		ConstReversIterator(SmartNode* elem) : stack() {
			buildStack(elem);
		}

		friend bool operator!=(const ConstReversIterator& crI1, const ConstReversIterator& crI2) {
			return crI1.stack != crI2.stack;
		}

		const T& operator*() {
			return stack.top()->m_Key;
		}

		ConstReversIterator operator++() {
			SmartNode* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Left);

			return *this;
		}

	private:
		void buildStack(SmartNode* elem) {
			while(elem) {
				stack.push(elem);
				elem = elem->m_Right;
			}
		}

		// members
		std::stack<SmartNode*> stack;
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
		insert(&arg);
	}

	// copy constructor
	sequence(const sequence& crArg) : m_Root(crArg.m_Root) {
		increaseCount();
	}

	~sequence() {

	}

	// methods
	void insert(T* arg) {
		m_Root = new SmartNode(arg);
	}

	void increaseCount() {
		for(typename sequence<T>::ConstIterator i = cbegin(); i != cend(); ++i) {
			++(*i.getNode()->m_puiCnt);
		}
	}

	// todo remove after test
	SmartNode* getRoot() {
		return m_Root;
	}

	// operators
	friend sequence<T> operator+(const sequence<T>& crArg1, const sequence<T>& crArg2) {
		sequence<T> res(crArg1.m_Root, crArg2.m_Root);
		increaseCount();
		return res;
	}

	friend std::ostream& operator<<(std::ostream& os, const sequence<T>& crArg) {
		for(typename sequence<T>::ConstIterator i = crArg.cbegin(); i != crArg.cend(); ++i) {
			os << *i << " ";
		}
		os << std::endl;
		return os;
	}

	sequence<T>& operator=(const sequence& crArg) {

	}

private:
	SmartNode* m_Root;
};
