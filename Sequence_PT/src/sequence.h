#include <iostream>
#include <stack>

template<class T>
class sequence {
private:
    struct SmartNode {
        SmartNode(T* key) : m_Key(key), m_Left(0), m_Right(0), m_puiCnt(new unsigned(0)) {}
        SmartNode(SmartNode* left, SmartNode* right) : m_Key(0), m_Left(left), m_Right(right), m_puiCnt(new unsigned(0)) {}
        SmartNode(SmartNode* left, T* key) : m_Key(0), m_Left(left), m_Right(new SmartNode(key)), m_puiCnt(new unsigned(0)) {}
        SmartNode(T* key, SmartNode* right) : m_Key(0), m_Left(new SmartNode(key)), m_Right(right), m_puiCnt(new unsigned(0)) {}

        //SmartNode(const SmartNode& crArg) : m_Key(crArg.m_Key), m_Left(crArg.m_Left), m_Right(crArg.m_Right), m_puiCnt(crArg.m_puiCnt) {
        //	++*m_puiCnt;
        //}

        ~SmartNode() {
            delete m_Key;
            delete m_puiCnt;
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
		friend class sequence;
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

		bool isNull() {
			return (stack.top()->m_Key == 0);
		}

	private:
		SmartNode* getNode() {
			return stack.top();
		}

		void buildStack(SmartNode* elem) {
			while(elem) {
				stack.push(elem);
				elem = elem->m_Left;
			}
		}

		// members
		std::stack<SmartNode*> stack;
	};

	class ConstIterator {
		friend class sequence;
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

		bool isNull() {
			return (stack.top()->m_Key == 0);
		}

	private:
		SmartNode* getNode() {
			return stack.top();
		}

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
			return *stack.top()->m_Key;
		}

		ReversIterator operator++() {
			SmartNode* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Left);
			return *this;
		}

		bool isNull() {
			return (stack.top()->m_Key == 0);
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
			return *stack.top()->m_Key;
		}

		ConstReversIterator operator++() {
			SmartNode* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Left);
			return *this;
		}

		bool isNull() {
			return (stack.top()->m_Key == 0);
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
public:
	sequence() : m_Root(0) {}
	sequence(T arg) {
		//insert(new T(arg));
		m_Root = new SmartNode(new T(arg));
		increaseCounter();
	}
	// copy constructor
	sequence(const sequence& crArg) : m_Root(crArg.m_Root) {
		increaseCounter();
	}
	~sequence() {
		removeNodes();
	}

private:
	sequence(SmartNode* arg) : m_Root(arg) {
		increaseCounter();
	}

	// methods
public:
//	void insert(T* arg) {
//		m_Root = new SmartNode(arg);
//	}

	void printCounter() {
		for(typename sequence<T>::ConstIterator i = cbegin(); i != cend(); ++i) {
			std::cout << (*i.getNode()->m_puiCnt);
			std::cout << ", ";
		}
		std::cout << std::endl;
	}

private:
	void increaseCounter() {
		for(typename sequence<T>::ConstIterator i = cbegin(); i != cend(); ++i) {
			++(*i.getNode()->m_puiCnt);
		}
	}

	void removeNodes() {
		for(typename sequence<T>::Iterator iter = begin(); iter != end();) {
			// decrease counter
			if (--(*iter.getNode()->m_puiCnt) == 0) {
				SmartNode* node2Delete = iter.getNode();
				++iter;
				delete node2Delete;
			} else {
				++iter;
			}
		}
	}

	// operators
public:
	friend sequence<T> operator+(const sequence<T>& crArg1, const sequence<T>& crArg2) {
		sequence<T> res(new SmartNode(crArg1.m_Root, crArg2.m_Root));
		return res;
	}

	friend sequence<T> operator+(const sequence<T>& crArg1, T crArg2) {
		sequence<T> res(new SmartNode(crArg1.m_Root, new T(crArg2)));
		return res;
	}

	friend sequence<T> operator+(T crArg1, const sequence<T>& crArg2) {
		sequence<T> res(new SmartNode(new T(crArg1), crArg2.m_Root));
		return res;
	}

	friend std::ostream& operator<<(std::ostream& os, const sequence<T>& crArg) {
		for(typename sequence<T>::ConstIterator iter = crArg.cbegin(); iter != crArg.cend(); ++iter) {
			if(!iter.isNull()) {
				os << *iter << " ";
			}
		}
		os << std::endl;
		return os;
	}

	sequence<T>& operator=(const sequence& crArg) {
		// no self assignment
		if(crArg.m_Root != m_Root) {
			// delete elements
			removeNodes();
			// switch content
			m_Root = crArg.m_Root;
			increaseCounter();
		}
		return *this;
	}

private:
	SmartNode* m_Root;
};
