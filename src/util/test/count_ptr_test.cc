/**
	\file "count_ptr_test.cc"
	Test program for checking reference-counted pointers.  
	$Id: count_ptr_test.cc,v 1.2 2004/11/02 07:52:19 fang Exp $
 */

#include <iostream>
#include <string>

#include "var.h"
#include "count_ptr.h"

using namespace std;
using namespace COUNT_PTR_NAMESPACE;

void test0(void);
void test1(void);

int main(int argc, char* argv[]) {
	test0();
	test1();
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

	count_const_ptr<string> s3(s2);
	cout << *s0 << ", " << *s1 << ", " << *s2 << ", " << *s3 << endl;
	cout << "#refs = " << s0.refs() << ", " << s1.refs() << endl;

	const count_const_ptr<string> s4(new string("this is new!"));
	cout << *s4 << endl;
	count_const_ptr<string> s5(s4);
	cout << "#refs = " << s4.refs() << ", " << s5.refs() << endl;
	cout << *s4 << endl;
	cout << *s5 << endl;

	cout << "goodbye again" << endl;
}

