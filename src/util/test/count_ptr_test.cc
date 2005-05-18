/**
	\file "count_ptr_test.cc"
	Test program for checking reference-counted pointers.  
	$Id: count_ptr_test.cc,v 1.5.54.2 2005/05/18 03:58:07 fang Exp $
 */

#include <iostream>
#include <string>

#include "var.h"
#include "memory/count_ptr.tcc"

using std::string;
#include "using_ostream.h"
using namespace util::memory;


static void test0(void);
static void test1(void);
static void test2(void);

int main(int argc, char* argv[]) {
	test0();
	test1();
	test2();
	return 0;
}

void test0(void)  {
	count_ptr<string> s0(new string("hello"));
	cout << *s0 << endl;
	cout << "goodbye" << endl;
}

void test1(void)  {
	count_ptr<string> s0(new string("hello again"));
	cout << "#refs = " << s0.refs() << endl;

	count_ptr<string> s1(s0);
	cout << "#refs = " << s0.refs() << endl;
	cout << "#refs = " << s1.refs() << endl;
	cout << *s0 << endl;
	cout << *s1 << endl;

	*s1 = "umm, what now?";
	cout << *s0 << endl;
	cout << *s1 << endl;
	cout << "#refs = " << s0.refs() << ", " << s1.refs() << endl;

	count_ptr<string> s2(new string("who\'s this?"));
	cout << *s2 << endl;
	s0 = s2;
	cout << *s0 << ", " << *s1 << ", " << *s2 << endl;
	cout << "#refs = " << s0.refs() << ", " << s1.refs() 
		<< ", " << s2.refs() << endl;

	count_ptr<const string> s3(s2);
	cout << *s0 << ", " << *s1 << ", " << *s2 << ", " << *s3 << endl;
	cout << "#refs = " << s0.refs() << ", " << s1.refs() << endl;

	const count_ptr<const string> s4(new string("this is new!"));
	cout << *s4 << endl;
	count_ptr<const string> s5(s4);
	cout << "#refs = " << s4.refs() << ", " << s5.refs() << endl;
	cout << *s4 << endl;
	cout << *s5 << endl;

	cout << "goodbye again" << endl;
}

void test2(void)  {
	count_ptr<string> s0(new string("It\'s me again!"));
	cout << *s0 << endl;
	assert(s0.refs() == 1);
	s0 = s0;		// could be a problem if not careful!
	cout << "I\'m pointing to myself." << endl;
	assert(s0.refs() == 1);
	{
		count_ptr<string> s1(s0);
		assert(s1.refs() == 2);
		s1 = s1;
		assert(s1.refs() == 2);
		s0 = s1;
		assert(s1.refs() == 2);
		s1 = s0;
		assert(s1.refs() == 2);
	}
	assert(s0.refs() == 1);
	cout << "I\'m done." << endl;
}


