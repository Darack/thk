#include <iostream>
#include <stack>

template<class T>
class sequence {
private:
    struct SmartNode {
        SmartNode(SmartNode* previous) : m_previous(previous), m_puiCnt(new unsigned(0)) {}

        virtual ~SmartNode() {
            delete m_puiCnt;
        }

    public:
        virtual SmartNode* getLeft() {
        	return 0;
        }
        virtual SmartNode* getRight() {
        	return 0;
        }
        virtual T* getKey() {
        	return 0;
        }
        virtual SmartNode& operator=(const T& crArg) {}
        unsigned& getCounter() {
        	return *m_puiCnt;
        }
    private:
        SmartNode* m_previous;
        unsigned* m_puiCnt;
    };

    struct LeafNode : SmartNode {

    	LeafNode(SmartNode* previous, T* key) : SmartNode(previous), m_Key(key) {}

    	~LeafNode() {
    		delete m_Key;
    	}

        T* getKey() {
        	return m_Key;
        }

        SmartNode& operator=(const T& crArg) {
        	if (*m_Key!=crArg) {
        		if (SmartNode::getCounter() == 0) {
        			*m_Key = crArg;
        		} else {
        			//???
        		}
        	}
        	return *this;
        }

        T* m_Key;
    };

    struct InnerNode : SmartNode {

    	InnerNode(SmartNode* previous, SmartNode* left, SmartNode* right) : SmartNode(previous), m_Left(left), m_Right(right) {}
    	InnerNode(SmartNode* previous, SmartNode* left, T* key) : SmartNode(previous), m_Left(left), m_Right(new LeafNode(previous, key)) {}
    	InnerNode(SmartNode* previous, T* key, SmartNode* right) : SmartNode(previous), m_Left(new LeafNode(key)), m_Right(right) {}
    	InnerNode(SmartNode* previous, T* key) : SmartNode(previous), m_Left(new LeafNode(previous, key)), m_Right(0) {}

    	~InnerNode() {}

        SmartNode* getLeft() {
        	return m_Left;
        }
        SmartNode* getRight() {
        	return m_Right;
        }

        SmartNode* m_Left;
        SmartNode* m_Right;
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
    	Iter operator++() {
			SmartNode* tmp = stack.top();
			stack.pop();
			buildStack(tmp->getRight());
			return *this;
		}
		SmartNode& operator*() {
			return getNextLeaf();
		}
	protected:
		virtual void buildStack(SmartNode* elem) {
			while (elem) {
				stack.push(elem);
				elem = elem->getLeft();
			}
		}
		// TODO infinite loop?
		SmartNode& getNextLeaf() {
			while (stack.top()->getKey() == 0) {
				operator++();
			}
			return *stack.top();
		}

		std::stack<SmartNode*> stack;
    };

    class RevIter {
    protected:
    	RevIter(SmartNode* elem) : stack() {
    		buildStack(elem);
    	}
    public:
		RevIter operator++() {
			SmartNode* tmp = stack.top();
			stack.pop();
			buildStack(tmp->getLeft());
			return *this;
		}
		SmartNode& operator*() {
			return getNextLeaf();
		}
	protected:
		virtual void buildStack(SmartNode* elem) {
			while (elem) {
				stack.push(elem);
				elem = elem->getRight();
			}
		}
		SmartNode& getNextLeaf() {
			while (stack.top()->getKey() == 0) {
				operator++();
			}
			return *stack.top();
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

		T& operator*() {
			return *Iter::stack.top()->getKey();
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

		SmartNode& operator*() {
			return Iter::getNextLeaf();
		}
	};

	class ConstIterator : public Iter {
	public:
		ConstIterator(SmartNode* elem) : Iter(elem) {}

		SmartNode& operator*() {
			return Iter::getNextLeaf();
		}
	};

	class ReversIterator : public RevIter {
	public:
		ReversIterator(SmartNode* elem) : RevIter(elem) {}

		SmartNode& operator*() {
			return RevIter::getNextLeaf();
		}
	};

	class ConstReversIterator : public RevIter {
	public:
		ConstReversIterator(SmartNode* elem) : RevIter(elem) {}

		SmartNode& operator*() {
			return RevIter::getNextLeaf();
		}
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
		m_Root = new LeafNode(0, new T(arg));
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
			std::cout << (i.getNode()->getCounter());
			std::cout << ", ";
		}
		std::cout << std::endl;
	}

private:
	void increaseCounter() {
		for(typename sequence<T>::InnerIterator i = ibegin(); i != iend(); ++i) {
			++(i.getNode()->getCounter());
		}
	}

	void removeNodes() {
		for(typename sequence<T>::InnerIterator iter = ibegin(); iter != iend();) {
			// decrease counter
			if (--(iter.getNode()->getCounter()) == 0) {
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
		sequence<T> res(new InnerNode(0, crArg1.m_Root, crArg2.m_Root));
		return res;
	}

	friend sequence<T> operator+(const sequence<T>& crArg1, T crArg2) {
		sequence<T> res(new InnerNode(0, crArg1.m_Root, new T(crArg2)));
		return res;
	}

	friend sequence<T> operator+(T crArg1, const sequence<T>& crArg2) {
		sequence<T> res(new InnerNode(0, new T(crArg1), crArg2.m_Root));
		return res;
	}

	friend std::ostream& operator<<(std::ostream& os, const sequence<T>& crArg) {
		for(typename sequence<T>::ConstIterator iter = crArg.cbegin(); iter != crArg.cend(); ++iter) {
			os << *((*iter).getKey()) << " ";
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
