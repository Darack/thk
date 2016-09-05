#include <iostream>
#include <stack>

template<class T>
class sequence {
private:
	/*
	 * 	Base class for all nodes.
	 * 	Every node has a reference counter.
	 */
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

		/*
		 * This operator can only compare leaf nodes, inner nodes don't need to be compared.
		 */
    	friend bool operator==(const Node& crN1, const Node& crN2) {
    		if (crN1.getKey() && crN2.getKey()) {
    			return *crN1.getKey() == *crN2.getKey();
    		}
    		return false;
    	}

	private:
		unsigned* m_puiCnt;
	};

	/*
	 * 	This class has access to the saved data.
	 *	It is a leaf node of the sequence tree, it has no left or right
	 *	descendant.
	 */
	struct Leaf : Node {
		Leaf(T* key) : m_pKey(key) {}
		/*
		 * 	If this leaf is deleted, also delete its data.
		 * 	The destructor should be called by the sequence after considering its
		 * 	reference counter.
		 */
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

	/*
	 * 	Inner nodes are nodes linking a sequence to its data in the right order.
	 * 	Inner nodes must not save data, new data is always saved in a leaf node.
	 * 	Descendants can be leaf nodes or inner nodes.
	 */
	struct Inner : Node {
		Inner(Node* left, Node* right) : m_pLeft(left), m_pRight(right) {}
		Inner(Node* left, T* key) : m_pLeft(left), m_pRight(new Leaf(key)) {}
		Inner(T* key, Node* right) : m_pLeft(new Leaf(key)), m_pRight(right) {}
		Inner(T* key) : m_pLeft(new Leaf(key)), m_pRight(0) {}
		/*
		 * 	Do NOT delete your descendants, the reference counter needs to be controlled by the
		 * 	sequence.
		 */
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
			// do nothing (see above)
		}

	private:
    	Node* m_pLeft;
    	Node* m_pRight;
	};

	/*
	 *	This class is returned by Iterator::operator* and ReverseIterator::operator*.
	 *	In case of a write on a node, this class is used by operator= to copy the sequence
	 *	and change the node.
	 *	pSeq: 		sequence that is copied and assigned new nodes.
	 *	pEditNode:	node that could be modified
	 *	uiLeaf: 	leaf counter of the node
	 */
	class SeqHelper {
    public:
    	SeqHelper(sequence<T>* pSeq, Node* pEditNode, unsigned uiLeaf) : m_pSeq(pSeq), m_pEditNode(pEditNode), m_uiLeafNo(uiLeaf) {}
    	~SeqHelper() {}

    	/*
    	 *	Emulates the same behaviour on a given outputstream as operator* of the const Iterators;
    	 */
    	friend std::ostream& operator<<(std::ostream& os, const SeqHelper& crArg) {
    		os << *SeqHelper::m_pEditNode->getKey();
    		return os;
    	}

    	/*
    	 *	Assigns the given value to the editNode.
    	 *	Checks for self-assignment and the reference counter,
    	 *	if counter greater than one, the full sequence has to be copied.
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
    						return *this;
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

	/*
	 * 	This class iterates over all nodes of the sequence, not ignoring inner nodes.
	 * 	Thus the reference counter of inner nodes can be accessed.
	 */
	class InnerIterator {
    	friend class sequence;
	private:
    	InnerIterator(Node* pElem) : m_Stack() {
    		buildStack(pElem);
    	}

    	/*
    	 *	If a right descendant exists, take its left-most descendant.
    	 *	If no right descendant exists, look on the stack.
    	 */
    	InnerIterator& operator++() {
    		Node* tmp = m_Stack.top();
			m_Stack.pop();
			buildStack(tmp->getRight());
			return *this;
    	}
    	/*
    	 * 	The current node is always at the top of the stack.
    	 */
    	Node* operator*() {
    		return m_Stack.top();
    	}
    	/*
    	 * 	Compare the stacks of two iterators to check if they are equal.
    	 */
    	friend bool operator!=(const InnerIterator& crI1, const InnerIterator& crI2) {
    		return crI1.m_Stack!=crI2.m_Stack;
    	}

    	/*
    	 *	If the element exists, go as far left as possible and push all.
    	 */
    	void buildStack(Node* pElem) {
    		while(pElem) {
    			m_Stack.push(pElem);
    			pElem = pElem->getLeft();
    		}
    	}
    	std::stack<Node*> m_Stack;
	};

	/*
	 * 	Begin at the top of the tree structure.
	 */
	InnerIterator ibegin() {
		return InnerIterator(m_pRoot);
	}
	/*
	 * 	Iterate as far as a node exists.
	 */
	InnerIterator iend() {
		return InnerIterator(0);
	}

public:
	/*
	 * 	Iterator with write access, works similar as InnerIterator.
	 * 	To return the above described helper class in operator*,
	 * 	this iterator needs to know what sequence it called and to count what leaf number
	 * 	the current node is.
	 * 	All inner nodes are skipped.
	 */
	class Iterator {
		friend SeqHelper& SeqHelper::operator=(const T& crArg);
	public:
		Iterator(Node* pElem, sequence* pSeq) : m_Stack(), m_pSeq(pSeq), m_uiLeafNo(0) {
    		buildStack(pElem);
    	}
    	friend bool operator!=(const Iterator& crI1, const Iterator& crI2) {
    		return crI1.m_Stack!=crI2.m_Stack;
    	}
    	Iterator& operator++() {
    		do {
				Node* tmp = m_Stack.top();
				m_Stack.pop();
				buildStack(tmp->getRight());
    		} while (!m_Stack.empty() && m_Stack.top()->getKey() == 0);

    		++m_uiLeafNo;
			return *this;
    	}
    	Iterator operator++(int) {
    		Iterator tmp(*this);
    		operator++();
			return tmp;
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

	/*
	 * 	Iterator without write access to the leafs.
	 * 	Returns the data type directly in operator*.
	 */
	class ConstIterator : public Iterator {
	public:
		ConstIterator(Node* elem): Iterator(elem, 0) {}

		const T& operator*() {
			return *Iterator::m_Stack.top()->getKey();
		}
	};

	/*
	 * Works similar to Iterator, only in reverse order in buildStack() and
	 * operator++().
	 */
	class ReverseIterator {
	public:
		ReverseIterator(Node* pElem, sequence* pSeq) : m_Stack(), m_pSeq(pSeq), m_uiLeafNo(pSeq->size()-1) {
    		buildStack(pElem);
    	}
    	friend bool operator!=(const ReverseIterator& crI1, const ReverseIterator& crI2) {
    		return crI1.m_Stack!=crI2.m_Stack;
    	}
    	ReverseIterator& operator++() {
    		do {
				Node* tmp = m_Stack.top();
				m_Stack.pop();
				buildStack(tmp->getLeft());
    		} while (!m_Stack.empty() && m_Stack.top()->getKey() == 0);

    		--m_uiLeafNo;
			return *this;
    	}
    	ReverseIterator operator++(int) {
    		ReverseIterator tmp(*this);
    		operator++();
			return tmp;
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

	/*
	 * 	Similar to ConstIterator with reverse order.
	 */
	class ConstReverseIterator : public ReverseIterator {
	public:
		ConstReverseIterator(Node* elem): ReverseIterator(elem, 0) {}

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
	/*
	 *	Create a new empty sequence.
	 */
	sequence() : m_pRoot(0) {}
	/*
	 *	Create a new sequence with a leaf node.
	 */
	sequence(const T arg) {
		m_pRoot = new Leaf(new T(arg));
		increaseCounter();
	}
	/*
	 * 	Copy constructor of sequence.
	 * 	Instead of copying all nodes, copy the pointer to its
	 * 	root element.
	 */
	sequence(const sequence& crArg) : m_pRoot(crArg.m_pRoot) {
		increaseCounter();
	}
	~sequence() {
		removeNodes();
	}

private:
	/*
	 *	Private constructor for internal use.
	 *	Creates a sequence with a given inner node and its descendants.
	 */
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
	/*
	 *	Copies the nodes of a given sequence recursively.
	 */
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

	/*
	 * 	Increase the reference counter by all inner and leaf nodes by one.
	 */
	void increaseCounter() {
		for(typename sequence<T>::InnerIterator iter = ibegin(); iter != iend(); ++iter) {
			++(*iter)->getCnt();
		}
	}
	/*
	 * 	Decrease the reference counter by all inner and leaf nodes by one.
	 * 	If a counter is then equal to zero, delete its node.
	 */
	void removeNodes() {
		for(typename sequence<T>::InnerIterator iter = ibegin(); iter != iend();) {
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
	/*
	 * 	Concat two sequences by creating a new inner nodes above them.
	 */
	friend sequence<T> operator+(const sequence<T>& crArg1, const sequence<T>& crArg2) {
		return sequence<T>(new Inner(crArg1.m_pRoot, crArg2.m_pRoot));
	}
	/*
	 * 	Only create a new inner node if the sequence is not empty.
	 */
	friend sequence<T> operator+(const sequence<T>& crArg1, const T& crArg2) {
		return crArg1.isEmpty() ? sequence<T>(crArg2) : sequence<T>(new Inner(crArg1.m_pRoot, new T(crArg2)));
	}
	friend sequence<T> operator+(const T& crArg1, const sequence<T>& crArg2) {
		return crArg2.isEmpty() ? sequence<T>(crArg1) : sequence<T>(new Inner(new T(crArg1), crArg2.m_pRoot));
	}
	/*
	 * 	Print a sequence on a given outputstream by printing the data of its leaf nodes.
	 */
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
