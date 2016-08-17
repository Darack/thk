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

    class Iter {
    protected:
    	Iter(SmartNode* elem) : stack() {
    		buildStack(elem);
    	}
    public:
    	template<class Z>
		friend bool operator!=(const Z& crI1, const Z& crI2) {
			return crI1.stack != crI2.stack;
		}

		T& operator*() {
			return *stack.top()->m_Key;
		}

		Iter operator++() {
			SmartNode* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Right);
			return *this;
		}
		// TODO needed?
		const Iter operator++() const {
			SmartNode* tmp = stack.top();
			stack.pop();
			buildStack(tmp->m_Right);
			return *this;
		}
	protected:
		void buildStack(SmartNode* elem) {
			while (elem) {
				stack.push(elem);
				elem = elem->m_Left;
			}
		}

		T& getNextLeaf() {
			// TODO infinite loop?
			while (stack.top()->m_Key == 0) {
				operator++();
			}
			return *stack.top()->m_Key;
		}
		// TODO needed?
		const T& getNextLeaf() const {
			while (stack.top()->m_Key == 0) {
				operator++();
			}
			return *stack.top()->m_Key;
		}

		std::stack<SmartNode*> stack;
    };

    class InnerIterator : public Iter {
    	friend class sequence;
	private:
    	InnerIterator(SmartNode* elem) : Iter(elem) {}

		SmartNode* getNode() {
			return Iter::stack.top();
		}
	};

	InnerIterator ibegin() {
		return InnerIterator(m_Root);
	}

	InnerIterator iend() {
		return InnerIterator(0);
	}

public:
	// public iterators
	class Iterator : public Iter {
	public:
		Iterator(SmartNode* elem) : Iter(elem) {}

		T& operator*() {
			return Iter::getNextLeaf();
		}
	};

	class ConstIterator : public Iter {
	public:
		ConstIterator(SmartNode* elem) : Iter(elem) {}

		const T& operator*() {
//			if (Iter::isLeaf()) {
//				Iter::operator++();
//			}
//			return *Iter::stack.top()->m_Key;
			return Iter::getNextLeaf();
		}
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
		for(typename sequence<T>::InnerIterator i = ibegin(); i != iend(); ++i) {
			std::cout << (*i.getNode()->m_puiCnt);
			std::cout << ", ";
		}
		std::cout << std::endl;
	}

private:
	void increaseCounter() {
		for(typename sequence<T>::InnerIterator i = ibegin(); i != iend(); ++i) {
			++(*i.getNode()->m_puiCnt);
		}
	}

	void removeNodes() {
		for(typename sequence<T>::InnerIterator iter = ibegin(); iter != iend();) {
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
			os << *iter << " ";
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
