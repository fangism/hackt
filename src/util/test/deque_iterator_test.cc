/**
	\file "deque_iterator_test.cc"
	Testing if we can save away iterators of a deque.
	$Id: deque_iterator_test.cc,v 1.6 2006/02/26 02:28:04 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>
#include <string>
#include <deque>
#include "util/memory/pointer_classes.hh"

using std::deque;
using std::string;
#include "util/using_ostream.hh"
using namespace util::memory;

typedef	deque<count_ptr<string> >		string_deck;

int main(int, char*[]) {
	string_deck foo;
	foo.push_front(count_ptr<string>(new string("first")));
	string_deck::const_iterator one = foo.begin();
	cout << **one << endl;
	foo.push_front(count_ptr<string>(new string("second")));
	string_deck::const_iterator two = foo.begin();
	cout << **one << ", " << **two << endl;
	foo.push_front(count_ptr<string>(new string("third")));
	string_deck::const_iterator three = foo.begin();
	cout << **one << ", " << **two << ", " << **three << endl;
	foo.push_front(count_ptr<string>(new string("fourth")));
	string_deck::const_iterator four = foo.begin();
	cout << **one << ", " << **two << ", " << **three << ", " << **four << endl;

	{
		cout << "walking forward with each iterator: " << endl;
		string_deck::const_iterator bar;
		bar = one;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
		bar = two;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
		bar = three;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
		bar = four;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
	}
	{
		cout << "walking forward with each iterator: " << endl;
		foo.pop_back();
		string_deck::const_iterator bar;
		bar = one;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
		bar = two;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
		bar = three;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
		bar = four;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
	}
	{
		cout << "walking forward with each iterator: " << endl;
		foo.pop_back();
		string_deck::const_iterator bar;
		bar = one;
#if 0
		while (bar != foo.end()) {
			// bad results here!
			cout << **bar << endl;
			bar++;
		}
#endif
		bar = two;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
		bar = three;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
		bar = four;
		while (bar != foo.end()) {
			cout << **bar << endl;
			bar++;
		}
	}


	cout << "done." << endl;

	return 0;
}

