#include <iostream>
#include "util/using_ostream.hh"

class A { };
class B { };
class C { };
class D { };
class AB : public A, public B { };
class CD : public C, public D { };
class ABCD : public AB, public CD { };

int
main(int, char*[]) {
	cout << "sizeof(A) = " << sizeof(A) << endl;
	cout << "sizeof(B) = " << sizeof(B) << endl;
	cout << "sizeof(C) = " << sizeof(C) << endl;
	cout << "sizeof(D) = " << sizeof(D) << endl;
	cout << "sizeof(AB) = " << sizeof(AB) << endl;
	cout << "sizeof(CD) = " << sizeof(CD) << endl;
	cout << "sizeof(ABCD) = " << sizeof(ABCD) << endl;
}

