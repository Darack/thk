#include <iostream>
#include <stack>

template<class T>
class sequence {
private:
	struct Node {
		Node() : m_puiCnt(new unsigned(0)) {}
		virtual ~Node() {
			delete m_puiCnt;
		}

	public:
		unsigned& getCnt() {
			return *m_puiCnt;
		}
		virtual Node* getLeft() = 0;
		virtual Node* getRight() = 0;
		virtual const T* getKey() const = 0;
		virtual void setKey(const T& key) = 0;

		// only compares leaf nodes
    	friend bool operator==(const Node& crN1, const Node& crN2) {
    		if (crN1.getKey() && crN2.getKey()) {
    			return *crN1.getKey() == *crN2.getKey();
    		}
    		return false;
    	}

	private:
		unsigned* m_puiCnt;
	};

	struct Leaf : Node {
		Leaf(T* key) : m_Key(key) {}
		~Leaf() {
			delete m_Key;
		}

	public:
		Node* getLeft() {
			return 0;
		}
		Node* getRight() {
			return 0;
		}
		const T* getKey() const {
			return m_Key;
		}
		void setKey(const T& key) {
			if (key) {
				*m_Key = key;
			}
		}

	private:
		T* m_Key;
	};

	struct Inner : Node {
		Inner(Node* left, Node* right) : m_Left(left), m_Right(right) {}
		Inner(Node* left, T* key) : m_Left(left), m_Right(new Leaf(key)) {}
		Inner(T* key, Node* right) : m_Left(new Leaf(key)), m_Right(right) {}
		Inner(T* key) : m_Left(new Leaf(key)), m_Right(0) {}
		~Inner() {}

	public:
    	Node* getLeft() {
        	return m_Left;
        }
    	Node* getRight() {
        	return m_Right;
        }
		const T* getKey() const {
			return 0;
		}
		void setKey(const T& key) {
			// do nothing
		}

	private:
    	Node* m_Left;
    	Node* m_Right;
	};

	class Dummy {
		friend class sequence;
    public:
    	Dummy(sequence<T>* sequence, Node* node) : seq(sequence), node2Edit(node) {}
    	~Dummy() {}

    	friend std::ostream& operator<<(std::ostream& os, const Dummy& crArg) {
    		os << Dummy::node2Edit->getKey();
    		return os;
    	}
    	Dummy& operator=(const T& crArg) {
    		// no self assignment
    		if (*node2Edit->getKey() != crArg) {
    			if (node2Edit->getCnt() > 1) {
    				sequence<T> tmp(*seq);
    				seq->removeNodes();
    				tmp.copyNodes(seq);
    				// change copied node
    				for (sequence<T>::InnerIterator iter = (*seq).ibegin(); iter!=(*seq).iend(); ++iter) {
    					if (**iter == *node2Edit) {
    						std::cout << "node found: " << (*iter)->getKey() << std::endl;
    						(*iter)->setKey(crArg);
    					}
    				}
    			} else {
    				// change node directly
    				std::cout << "no self assignment?" << std::endl;
    				node2Edit->setKey(crArg);
    			}
    		}
    		return *this;
    	}

    private:
    	sequence<T>* seq;
    	Node* node2Edit;
    };

	class InnerIterator {
    	friend class sequence;
	private:
    	InnerIterator(Node* elem) : stack() {
    		buildStack(elem);
    	}

    	InnerIterator operator++() {
    		Node* tmp = stack.top();
			stack.pop();
			buildStack(tmp->getRight());
			return *this;
    	}
    	Node* operator*() {
    		return stack.top();
    	}
    	friend bool operator!=(const InnerIterator& crI1, const InnerIterator& crI2) {
    		return crI1.stack!=crI2.stack;
    	}

    	void buildStack(Node* elem) {
    		while(elem) {
    			stack.push(elem);
    			elem = elem->getLeft();
    		}
    	}
    	std::stack<Node*> stack;
	};

	InnerIterator ibegin() {
		return InnerIterator(m_Root);
	}
	InnerIterator iend() {
		return InnerIterator(0);
	}

public:
	// public iterators
	class Iterator {
	public:
		Iterator(Node* elem, sequence* sequence) : stack(), seq(sequence) {
    		buildStack(elem);
    	}
    	friend bool operator!=(const Iterator& crI1, const Iterator& crI2) {
    		return crI1.stack!=crI2.stack;
    	}
    	Iterator operator++() {
    		Node* tmp = stack.top();
			stack.pop();
			buildStack(tmp->getRight());

			// TODO
//    		Node* tmp = stack.top();
//    		buildStack(tmp->getRight());
//    		std::cout << "stack.size()" << stack.size() << std::endl;
//			if (stack.top() == tmp) {
//				stack.pop();
//				std::cout << std::endl << "tmp: " << tmp << std::endl;
//				if (stack.empty())
//					std::cout << "stack.top() == 0" << std::endl;
//				std::cout << "stack.top(): " << stack.top() << std::endl;
//				std::cout << "stack.top()->getRight(): " << stack.top()->getRight() << std::endl;
//				if ( stack.top() && (stack.top()->getRight() == tmp) ) {
//					stack.pop();
//				}
//			}

			return *this;
    	}
    	Dummy operator*() {
    		return Dummy(seq, stack.top());
    	}

	protected:
    	Node* getNextLeaf() {
			while (stack.top() && stack.top()->getKey() == 0) {
				operator++();
			}
			return stack.top();
    	}

	private:
    	void buildStack(Node* elem) {
    		while(elem) {
    			stack.push(elem);
    			elem = elem->getLeft();
    		}
    	}

	protected:
    	std::stack<Node*> stack;
	private:
    	sequence* seq;
	};

	class ConstIterator : public Iterator {
	public:
		ConstIterator(Node* elem): Iterator(elem, 0) {}

		friend bool operator!=(const ConstIterator& crI1, const ConstIterator& crI2) {
    		return crI1.stack!=crI2.stack;
    	}
		const T& operator*() {
			return *Iterator::getNextLeaf()->getKey();
		}
	};

	class ReverseIterator {
	public:
		ReverseIterator(Node* elem, sequence* sequence) : stack(), seq(sequence) {
    		buildStack(elem);
    	}
    	friend bool operator!=(const ReverseIterator& crI1, const ReverseIterator& crI2) {
    		return crI1.stack!=crI2.stack;
    	}
    	ReverseIterator operator++() {
    		Node* tmp = stack.top();
			stack.pop();
			buildStack(tmp->getLeft());
			return *this;
    	}
    	Dummy operator*() {
    		return Dummy(seq, stack.top());
    	}

	protected:
    	Node* getNextLeaf() {
			while (stack.top() && stack.top()->getKey() == 0) {
				operator++();
			}
			return stack.top();
    	}

	private:
    	void buildStack(Node* elem) {
    		while(elem) {
    			stack.push(elem);
    			elem = elem->getRight();
    		}
    	}

	protected:
    	std::stack<Node*> stack;
	private:
    	sequence* seq;
	};

	class ConstReverseIterator : public ReverseIterator {
	public:
		ConstReverseIterator(Node* elem): ReverseIterator(elem, 0) {}

		friend bool operator!=(const ConstReverseIterator& crI1, const ConstReverseIterator& crI2) {
    		return crI1.stack!=crI2.stack;
    	}
		const T& operator*() {
			return *Iterator::getNextLeaf()->getKey();
		}
	};

	Iterator begin() {
		return Iterator(m_Root, this);
	}
	Iterator end() {
		return Iterator(0, this);
	}
	ConstIterator cbegin() const {
		return ConstIterator(m_Root);
	}
	ConstIterator cend() const {
		return ConstIterator(0);
	}
	ReverseIterator rbegin() {
		return ReverseIterator(m_Root, this);
	}
	ReverseIterator rend() {
		return ReverseIterator(0, this);
	}
	ConstReverseIterator crbegin() const {
		return ConstReverseIterator(m_Root);
	}
	ConstReverseIterator crend() const {
		return ConstReverseIterator(0);
	}

	// constructors
public:
	sequence() : m_Root(0) {}
	sequence(T arg) {
		m_Root = new Leaf(new T(arg));
		increaseCounter();
	}
	sequence(const sequence& crArg) : m_Root(crArg.m_Root) {
		increaseCounter();
	}
	~sequence() {
		removeNodes();
	}

private:
	sequence(Node* arg) : m_Root(arg) {
		increaseCounter();
	}

	// methods
public:
	// TODO remove after tests
	void printCounter() {
		for(typename sequence<T>::InnerIterator iter = ibegin(); iter != iend(); ++iter) {
			std::cout << (*iter)->getCnt();
			std::cout << ", ";
		}
		std::cout << std::endl;
	}

private:
	// TODO iterativ
	void copyNodes(sequence<T>* seq) {
//		std::cout << "copyNodes active!" << std::endl;
		seq->m_Root = copyNode(this->m_Root);
		seq->increaseCounter();
	}

	Node* copyNode(Node* node) {
		if (!node) {
			return 0;
		}
		if (node->getKey() == 0) {
			return new Inner(copyNode(node->getLeft()), copyNode(node->getRight()));
		} else {
			return new Leaf(new T(*node->getKey()));
		}
	}

	void increaseCounter() {
		for(typename sequence<T>::InnerIterator iter = ibegin(); iter != iend(); ++iter) {
			++(*iter)->getCnt();
		}
	}
	void removeNodes() {
		for(typename sequence<T>::InnerIterator iter = ibegin(); iter != iend();) {
			// decrease counter
			if (--(*iter)->getCnt() == 0) {
				Node* node2Delete = *iter;
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
		sequence<T> res(new Inner(crArg1.m_Root, crArg2.m_Root));
		return res;
	}
	friend sequence<T> operator+(const sequence<T>& crArg1, const T& crArg2) {
		sequence<T> res(new Inner(crArg1.m_Root, new T(crArg2)));
		return res;
	}
	friend sequence<T> operator+(const T& crArg1, const sequence<T>& crArg2) {
		sequence<T> res(new Inner(new T(crArg1), crArg2.m_Root));
		return res;
	}
	friend std::ostream& operator<<(std::ostream& os, const sequence<T>& crArg) {
		for(typename sequence<T>::ConstIterator iter = crArg.cbegin(); iter != crArg.cend(); ++iter) {
			os << *iter << " ";
		}
		// TODO remove?
//		os << std::endl;
		return os;
	}

	sequence<T>& operator=(const sequence& crArg) {
		if(crArg.m_Root != m_Root) {
			removeNodes();
			m_Root = crArg.m_Root;
			increaseCounter();
		}
		return *this;
	}

private:
	Node* m_Root;
};
