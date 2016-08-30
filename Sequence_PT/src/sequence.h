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
		Leaf(T* key) : m_pKey(key) {}
		~Leaf() {
			delete m_pKey;
		}

	public:
		Node* getLeft() {
			return 0;
		}
		Node* getRight() {
			return 0;
		}
		const T* getKey() const {
			return m_pKey;
		}
		void setKey(const T& key) {
			*m_pKey = key;
		}

	private:
		T* m_pKey;
	};

	struct Inner : Node {
		Inner(Node* left, Node* right) : m_pLeft(left), m_pRight(right) {}
		Inner(Node* left, T* key) : m_pLeft(left), m_pRight(new Leaf(key)) {}
		Inner(T* key, Node* right) : m_pLeft(new Leaf(key)), m_pRight(right) {}
		Inner(T* key) : m_pLeft(new Leaf(key)), m_pRight(0) {}
		~Inner() {}

	public:
    	Node* getLeft() {
        	return m_pLeft;
        }
    	Node* getRight() {
        	return m_pRight;
        }
		const T* getKey() const {
			return 0;
		}
		void setKey(const T& key) {
			// do nothing
		}

	private:
    	Node* m_pLeft;
    	Node* m_pRight;
	};

	class SeqHelper {
		friend class sequence;
    public:
    	SeqHelper(sequence<T>* pSeq, Node* pEditNode, unsigned uiLeaf) : m_pSeq(pSeq), m_pEditNode(pEditNode), m_uiLeafNo(uiLeaf) {}
    	~SeqHelper() {}

    	/*
    	 *	Emulates the same behaviour on a given outputstream as operator* of ConstIterator;
    	 */
    	friend std::ostream& operator<<(std::ostream& os, const SeqHelper& crArg) {
    		os << *SeqHelper::m_pEditNode->getKey();
    		return os;
    	}

    	/*
    	 *	Assigns the given value to the editNode.
    	 *	Checks for self-assignment and the reference counter,
    	 *	if counter>1, the full sequence has to be copied.
    	 */
    	SeqHelper& operator=(const T& crArg) {
    		if (*m_pEditNode->getKey() != crArg) {
    			if (m_pEditNode->getCnt() > 1) {
    				sequence<T> tmp(*m_pSeq);
    				m_pSeq->removeNodes();
    				tmp.copyNodes(m_pSeq);

    				for (sequence<T>::Iterator iter = (*m_pSeq).begin(); iter!=(*m_pSeq).end(); ++iter) {
    					if ( *iter.getCurrentLeaf() == *m_pEditNode && m_uiLeafNo == iter.m_uiLeafNo) {
    						iter.getCurrentLeaf()->setKey(crArg);
    					}
    				}
    			} else {
    				m_pEditNode->setKey(crArg);
    			}
    		}
    		return *this;
    	}

    private:
    	sequence<T>* m_pSeq;
    	Node* m_pEditNode;
    	unsigned m_uiLeafNo;
	};

	class InnerIterator {
    	friend class sequence;
	private:
    	InnerIterator(Node* pElem) : m_Stack() {
    		buildStack(pElem);
    	}

    	InnerIterator operator++() {
    		Node* tmp = m_Stack.top();
			m_Stack.pop();
			buildStack(tmp->getRight());
			return *this;
    	}
    	Node* operator*() {
    		return m_Stack.top();
    	}
    	friend bool operator!=(const InnerIterator& crI1, const InnerIterator& crI2) {
    		return crI1.m_Stack!=crI2.m_Stack;
    	}

    	void buildStack(Node* pElem) {
    		while(pElem) {
    			m_Stack.push(pElem);
    			pElem = pElem->getLeft();
    		}
    	}
    	std::stack<Node*> m_Stack;
	};

	InnerIterator ibegin() {
		return InnerIterator(m_pRoot);
	}
	InnerIterator iend() {
		return InnerIterator(0);
	}

public:
	class Iterator {
		friend SeqHelper& SeqHelper::operator=(const T& crArg);
	public:
		Iterator(Node* pElem, sequence* pSeq) : m_Stack(), m_pSeq(pSeq), m_uiLeafNo(0) {
    		buildStack(pElem);
    	}
    	friend bool operator!=(const Iterator& crI1, const Iterator& crI2) {
    		return crI1.m_Stack!=crI2.m_Stack;
    	}
    	Iterator operator++() {
    		do {
				Node* tmp = m_Stack.top();
				m_Stack.pop();
				buildStack(tmp->getRight());
    		} while (!m_Stack.empty() && m_Stack.top()->getKey() == 0);

    		++m_uiLeafNo;
			return *this;
    	}
    	SeqHelper operator*() {
    		return SeqHelper(m_pSeq, m_Stack.top(), m_uiLeafNo);
    	}

	private:
    	void buildStack(Node* pElem) {
    		while(pElem) {
    			m_Stack.push(pElem);
    			pElem = pElem->getLeft();
    		}
    	}
    	Node* getCurrentLeaf() {
    		return m_Stack.top();
    	}

	protected:
    	std::stack<Node*> m_Stack;
	private:
    	sequence* m_pSeq;
    	unsigned m_uiLeafNo;
	};

	class ConstIterator : public Iterator {
	public:
		ConstIterator(Node* elem): Iterator(elem, 0) {}

		friend bool operator!=(const ConstIterator& crI1, const ConstIterator& crI2) {
    		return crI1.m_Stack!=crI2.m_Stack;
    	}
		const T& operator*() {
			return *Iterator::m_Stack.top()->getKey();
		}
	};

	class ReverseIterator {
	public:
		ReverseIterator(Node* pElem, sequence* pSeq) : m_Stack(), m_pSeq(pSeq), m_uiLeafNo(pSeq->size()-1) {
    		buildStack(pElem);
    	}
    	friend bool operator!=(const ReverseIterator& crI1, const ReverseIterator& crI2) {
    		return crI1.m_Stack!=crI2.m_Stack;
    	}
    	ReverseIterator operator++() {
    		do {
				Node* tmp = m_Stack.top();
				m_Stack.pop();
				buildStack(tmp->getLeft());
    		} while (!m_Stack.empty() && m_Stack.top()->getKey() == 0);

    		--m_uiLeafNo;
			return *this;
    	}
    	SeqHelper operator*() {
    		return SeqHelper(m_pSeq, m_Stack.top(), m_uiLeafNo);
    	}

	private:
    	void buildStack(Node* pElem) {
    		while(pElem) {
    			m_Stack.push(pElem);
    			pElem = pElem->getRight();
    		}
    	}

	protected:
    	std::stack<Node*> m_Stack;
	private:
    	sequence* m_pSeq;
    	unsigned m_uiLeafNo;
	};

	class ConstReverseIterator : public ReverseIterator {
	public:
		ConstReverseIterator(Node* elem): ReverseIterator(elem, 0) {}

		friend bool operator!=(const ConstReverseIterator& crI1, const ConstReverseIterator& crI2) {
    		return crI1.m_Stack!=crI2.m_Stack;
    	}
		const T& operator*() {
			return *ReverseIterator::m_Stack.top()->getKey();
		}
	};

	Iterator begin() {
		return Iterator(m_pRoot, this);
	}
	Iterator end() {
		return Iterator(0, this);
	}
	ConstIterator cbegin() const {
		return ConstIterator(m_pRoot);
	}
	ConstIterator cend() const {
		return ConstIterator(0);
	}
	ReverseIterator rbegin() {
		return ReverseIterator(m_pRoot, this);
	}
	ReverseIterator rend() {
		return ReverseIterator(0, this);
	}
	ConstReverseIterator crbegin() const {
		return ConstReverseIterator(m_pRoot);
	}
	ConstReverseIterator crend() const {
		return ConstReverseIterator(0);
	}

	// constructors
public:
	sequence() : m_pRoot(0) {}
	sequence(const T arg) {
		m_pRoot = new Leaf(new T(arg));
		increaseCounter();
	}
	sequence(const sequence& crArg) : m_pRoot(crArg.m_pRoot) {
		increaseCounter();
	}
	~sequence() {
		removeNodes();
	}

private:
	sequence(Node* pArg) : m_pRoot(pArg) {
		increaseCounter();
	}

	// methods
public:
	bool isEmpty() const {
		return m_pRoot == 0;
	}

	unsigned size() {
		unsigned res = 0;
		for (Iterator i = begin(); i!=end(); ++i) {
			++res;
		}
		return res;
	}

	/*
	 * Convenience method for testing. Also prints counters of inner nodes.
	 */
	void printCounter() {
		for(typename sequence<T>::InnerIterator iter = ibegin(); iter != iend(); ++iter) {
			std::cout << (*iter)->getCnt();
			std::cout << ", ";
		}
		std::cout << std::endl;
	}

private:
	void copyNodes(sequence<T>* pSeq) {
		pSeq->m_pRoot = copyNode(this->m_pRoot);
		pSeq->increaseCounter();
	}

	Node* copyNode(Node* pNode) {
		if (!pNode) {
			return 0;
		}
		if (pNode->getKey() == 0) {
			return new Inner(copyNode(pNode->getLeft()), copyNode(pNode->getRight()));
		} else {
			return new Leaf(new T(*pNode->getKey()));
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
				Node* pDeleteNode = *iter;
				++iter;
				delete pDeleteNode;
			} else {
				++iter;
			}
		}
	}

	// operators
public:
	friend sequence<T> operator+(const sequence<T>& crArg1, const sequence<T>& crArg2) {
		return sequence<T>(new Inner(crArg1.m_pRoot, crArg2.m_pRoot));
	}
	friend sequence<T> operator+(const sequence<T>& crArg1, const T& crArg2) {
		return crArg1.isEmpty() ? sequence<T>(crArg2) : sequence<T>(new Inner(crArg1.m_pRoot, new T(crArg2)));
	}
	friend sequence<T> operator+(const T& crArg1, const sequence<T>& crArg2) {
		return crArg2.isEmpty() ? sequence<T>(crArg1) : sequence<T>(new Inner(new T(crArg1), crArg2.m_pRoot));
	}
	friend std::ostream& operator<<(std::ostream& os, const sequence<T>& crArg) {
		for(typename sequence<T>::ConstIterator iter = crArg.cbegin(); iter != crArg.cend(); ++iter) {
			os << *iter << " ";
		}
		return os;
	}

	sequence<T>& operator=(const sequence& crArg) {
		if(crArg.m_pRoot != m_pRoot) {
			removeNodes();
			m_pRoot = crArg.m_pRoot;
			increaseCounter();
		}
		return *this;
	}

private:
	Node* m_pRoot;
};
