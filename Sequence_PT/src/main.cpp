#include <iostream>
#include "sequence.h"

using namespace std;


template<class T>
void test(sequence<T> x,const T& a) {
	//trotz Call-by-value keine Kopie des aktuellen Parameters erzeugen,
	//sondern nur Verweise mit Referenzzählungen
	cout << x << endl;
	typename sequence<T>::Iterator i = x.begin();
	i++;
	*i = a; // jetzt muss eine Kopie für x erzeugt werden
	cout << x << endl;
}

class A {
public:
	A() {
		cout << "A+" << endl;
	}

	A(const A& crArg) {
		cout << "A_copycons" << endl;
	}

	friend std::ostream& operator<<(std::ostream& os, const A& crArg) {
		os << "ich bin ein A" << endl;
		return os;
	}
};

int main() {
	//sequence<char> s1('a');
	//cout << s1;

	//A a;
	//sequence<A> s2(a);
	//cout << *(s2.getRoot()->m_puiCnt) << endl;
	//cout << a;
	//sequence<A> s3(s2);
	//cout << *(s2.getRoot()->m_puiCnt) << endl;

	sequence<char> s1('a');
	sequence<char> s2('b');
	cout << "s1: " << s1;
	cout << "s1 counter: ";
	s1.printCounter();
	cout << endl;

	cout << "s2: " << s2;
	cout << "s2 counter: ";
	s2.printCounter();
	cout << endl;

	sequence<char> t1 = s1+s2;
	cout << "t1=s1+s2: " << t1;
	cout << "t1 counter: ";
	t1.printCounter();
	cout << endl;

	sequence<char> s3 = 'e';
	sequence<char> t2 = s3+'f';
	cout << "t2=s3+'f': " << t2;
	cout << "t2 counter: ";
	t2.printCounter();
	cout << endl;

	cout << "-------------------------" << endl;

	{
		sequence<char> t3 = t1+t1;
		cout << "t3=t1+t1: " << t3;
		cout << "t3 counter: ";
		t3.printCounter();
		cout << endl;

//		sequence<char> t4 = t1+t2;
//		cout << "t4=t1+t2: " << t4;
//		cout << "t4 counter: ";
//		t4.printCounter();
//		cout << endl;

	}

	cout << "t1=s1+s2: " << t1;
	cout << "t1 counter: ";
	t1.printCounter();
	cout << endl;

	cout << "t2=s3+'f': " << t2;
	cout << "t2 counter: ";
	t2.printCounter();
	cout << endl;

	cout << "s1 counter: ";
	s1.printCounter();
	cout << "s2 counter: ";
	s2.printCounter();

	cout << endl;
	sequence<char> t5 = t2 + t1;
	cout << "output" << endl;
	cout << t5 << endl;
	cout << endl;
	for (sequence<char>::Iterator it = t5.begin(); it!=t5.end(); ++it) {
		*it = 'a';
	}
	cout << t5 << endl;

	// TODO regressionstest
	// TODO inhalt verändern, kopien anlegen bei ref > 1

	/*
	sequence<char> s1,s2('c');

	// hier dürfen noch keine Kopien der Teilsequenzen erzeugt werden,
	// lediglich Verweise mit Referenz-zählungen auf die Teilsequenzen
	// sollen verwaltet werden
	for(unsigned ui = 'a'; ui < + 'z'; ++ui) {
		s1 = s1 + (char)ui;
	}

	// intern keine Kopie erzeugen
	cout << (s2+s2) << endl;

	sequence<char>::Iterator i = s1.begin();
	i++;
	// keine Kopie von s1 erzeugen, da alle Elemente
	// ausschließlich von s1 benutzt werden (erkennbar
	// durch Referenzzählung)
	*i =  'x';
	cout << s1 << endl;

	// keine interne Kopie erzeugen
	s2 = s1;

	sequence<char>::ReverseIterator i2 = s2.rbegin();
	++i2;
	// jetzt muss eine eigenständige Kopie für s2 erzeugt
	// werden, da die Elemente von s2 und s1 verwendet
	// werden
	*i2 =  'X';

	cout << s2 << endl;
	test(s2,'?');
	// muss die gleiche Ausgabe wie 2 Zeilen
	// zuvor erzeugen
	cout << s2 << endl;
	*/
}
