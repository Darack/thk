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
			*m_Key = key;
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

	// TODO namen ändern?
	class Dummy {
		friend class sequence;
    public:
    	Dummy(sequence<T>* seq, Node* ch, unsigned leaf) : ptrSeq(seq), chNode(ch), ui_leafNo(leaf) {}
    	~Dummy() {}

    	friend std::ostream& operator<<(std::ostream& os, const Dummy& crArg) {
    		os << *Dummy::chNode->getKey();
    		return os;
    	}

    	// TODO fix iterator not working after write
    	Dummy& operator=(const T& crArg) {
    		// no self assignment
    		if (*chNode->getKey() != crArg) {
    			if (chNode->getCnt() > 1) {
    				sequence<T> tmp(*ptrSeq);
    				ptrSeq->removeNodes();
    				tmp.copyNodes(ptrSeq);

    				// change copied node
    				for (sequence<T>::Iterator iter = (*ptrSeq).begin(); iter!=(*ptrSeq).end(); ++iter) {
    					if ( *iter.getCurrentLeaf() == *chNode && ui_leafNo == iter.ui_leafNo) {
    						iter.getCurrentLeaf()->setKey(crArg);
    					}
    				}
    			} else {
    				// change node directly
    				chNode->setKey(crArg);
    			}
    		}
    		return *this;
    	}

    private:
    	sequence<T>* ptrSeq;
    	Node* chNode;
    	unsigned ui_leafNo;
	};

	class InnerIterator {
    	friend class sequence;
	private:
    	InnerIterator(Node* elem) : stack(), iLeaf(0) {
    		buildStack(elem);
    	}

    	InnerIterator operator++() {
    		Node* tmp = stack.top();
			stack.pop();
			buildStack(tmp->getRight());
			if (!stack.empty() && stack.top()->getKey() != 0) {
				++iLeaf;
			}
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
    	unsigned iLeaf;
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
		friend Dummy& Dummy::operator=(const T& crArg);
	public:
		Iterator(Node* elem, sequence* sequence) : stack(), seq(sequence), ui_leafNo(0) {
    		buildStack(elem);
    	}
    	friend bool operator!=(const Iterator& crI1, const Iterator& crI2) {
    		return crI1.stack!=crI2.stack;
    	}
    	Iterator operator++() {
    		// TODO check if stack.isEmpty()?
    		do {
				Node* tmp = stack.top();
				stack.pop();
				buildStack(tmp->getRight());
    		}

    		// TODO was passiert hier?
    		while (!stack.empty() && stack.top()->getKey() == 0);
    		++ui_leafNo;

			return *this;
    	}
    	Dummy operator*() {
    		return Dummy(seq, stack.top(), ui_leafNo);
    	}
    	Node* getNode() {
    		return stack.top();
    	}

	private:
    	void buildStack(Node* elem) {
    		while(elem) {
    			stack.push(elem);
    			elem = elem->getLeft();
    		}
    	}
    	Node* getCurrentLeaf() {
    		return stack.top();
    	}

	protected:
    	std::stack<Node*> stack;
	private:
    	sequence* seq;
    	unsigned ui_leafNo;
	};

	class ConstIterator : public Iterator {
	public:
		ConstIterator(Node* elem): Iterator(elem, 0) {}

		friend bool operator!=(const ConstIterator& crI1, const ConstIterator& crI2) {
    		return crI1.stack!=crI2.stack;
    	}
		const T& operator*() {
			return *Iterator::stack.top()->getKey();
		}
	};

	class ReverseIterator {
	public:
		ReverseIterator(Node* elem, sequence* sequence) : stack(), seq(sequence), ui_leafNo(sequence->size()-1) {
    		buildStack(elem);
    	}
    	friend bool operator!=(const ReverseIterator& crI1, const ReverseIterator& crI2) {
    		return crI1.stack!=crI2.stack;
    	}
    	ReverseIterator operator++() {
    		do {
				Node* tmp = stack.top();
				stack.pop();
				buildStack(tmp->getLeft());
    		}
    		while (!stack.empty() && stack.top()->getKey() == 0);
    		--ui_leafNo;
			return *this;
    	}
    	Dummy operator*() {
    		return Dummy(seq, stack.top(), ui_leafNo);
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
    	unsigned ui_leafNo;
	};

	class ConstReverseIterator : public ReverseIterator {
	public:
		ConstReverseIterator(Node* elem): ReverseIterator(elem, 0) {}

		friend bool operator!=(const ConstReverseIterator& crI1, const ConstReverseIterator& crI2) {
    		return crI1.stack!=crI2.stack;
    	}
		const T& operator*() {
			return *ReverseIterator::stack.top()->getKey();
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
	sequence(const T arg) {
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
	bool isEmpty() const {
		return m_Root == 0;
	}

	unsigned size() {
		unsigned res = 0;
		for (Iterator i = begin(); i!=end(); ++i) {
			++res;
		}
		return res;
	}

	// TODO remove after tests
	// TODO oder drinlassen für kelb?
	void printCounter() {
		for(typename sequence<T>::InnerIterator iter = ibegin(); iter != iend(); ++iter) {
			std::cout << (*iter)->getCnt();
			std::cout << ", ";
		}
		std::cout << std::endl;
	}

private:
	void copyNodes(sequence<T>* seq) {
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
			// decrease counter first
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
		return sequence<T>(new Inner(crArg1.m_Root, crArg2.m_Root));
	}
	friend sequence<T> operator+(const sequence<T>& crArg1, const T& crArg2) {
		return crArg1.isEmpty() ? sequence<T>(crArg2) : sequence<T>(new Inner(crArg1.m_Root, new T(crArg2)));
	}
	friend sequence<T> operator+(const T& crArg1, const sequence<T>& crArg2) {
		return crArg2.isEmpty() ? sequence<T>(crArg1) : sequence<T>(new Inner(new T(crArg1), crArg2.m_Root));
	}
	friend std::ostream& operator<<(std::ostream& os, const sequence<T>& crArg) {
		for(typename sequence<T>::ConstIterator iter = crArg.cbegin(); iter != crArg.cend(); ++iter) {
			os << *iter << " ";
		}
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
