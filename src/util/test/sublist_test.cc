// "sublist_test.cc"
// stand-alone

#include <iostream>
#include "sublist.h"

using namespace std;
using namespace	SUBLIST_NAMESPACE;

template<class T>
ostream& operator << (ostream& o, const list<T>& l) {
	typename list<T>::const_iterator i = l.begin();
	if (!l.empty()) {
		o << *i;
		for (i++ ; i!=l.end(); i++)
			o << ", " << *i;
	}
	return o;
}

template<class T>
ostream& operator << (ostream& o, const sublist<T>& l) {
	typename sublist<T>::const_iterator i = l.begin();
	if (!l.empty()) {
		o << *i;
		for (i++ ; i!=l.end(); i++)
			o << ", " << *i;
	}
	return o;
}

int main(int argc, char* argv[]) {
	list<char> alphabet;
	{
	char c = 'a';
	for ( ; c <= 'z'; c++)
		alphabet.push_back(c);
	}
	cout << alphabet.size() << ": " << alphabet << endl;

	sublist<char> abcd(alphabet);
	cout << abcd.size() << ": " << abcd << endl;

	sublist<char> abcd2(abcd);
	cout << abcd2.size() << ": " << abcd2 << endl;

	{
	int i=0;
	for ( ; i<27; i++) {
		abcd.behead();
		cout << abcd.size() << ": " << abcd << endl;
		abcd2.betail();
		cout << abcd2.size() << ": " << abcd2 << endl;
	}
	}

	return 0;
}

