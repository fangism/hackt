/**
	\file "deque_iterator_test.cc"
	Testing if we can save away iterators of a deque.
	$Id: deque_iterator_test.cc,v 1.3 2004/11/30 01:26:40 fang Exp $
 */

#include <iostream>
#include <string>
#include <deque>
#include "memory/pointer_classes.h"
// #include "count_ptr.h"

using namespace std;
using namespace util::memory;
// using namespace COUNT_PTR_NAMESPACE;

typedef	deque<count_ptr<string> >		string_deck;

int main(int argc, char* argv[]) {
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

