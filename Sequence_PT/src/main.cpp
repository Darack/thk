#include <iostream>
#include "sequence.h"

using namespace std;


template<class T>
void test(sequence<T> x,const T& a) {
	//trotz Call-by-value keine Kopie des aktuellen Parameters erzeugen,
	//sondern nur Verweise mit Referenzzählungen
	cout << x << endl;
	typename sequence<T>::Iterator i = x.begin();
	++i;
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
	++i;
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
	*i2 = 'X';
	cout << s1 << endl;
	cout << s2 << endl;

	cout << s2 << endl;
	test(s2,'?');
	// muss die gleiche Ausgabe wie 2 Zeilen
	// zuvor erzeugen
	cout << s2 << endl;

	cout << "new stuff here... " << endl;
	sequence<char> t5 = s2 + s1;
	for (sequence<char>::Iterator iter = t5.begin(); iter!=t5.end(); ++iter) {
		*iter = 'o';
	}
	cout << t5 << endl;
}
