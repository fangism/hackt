/**
	\file "sublist_test.cc"
	Test for sublist structure.  
	$Id: sublist_test.cc,v 1.3 2004/12/05 05:08:29 fang Exp $
 */

#include <iostream>
#include "sublist.tcc"
#include "STL/list.tcc"

#include "using_ostream.h"
USING_LIST
using namespace	util;

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

