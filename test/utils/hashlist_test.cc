// "hashlist_test.cc"

#include <iostream>
#include <string>
#include "hashlist_template_methods.h"

using namespace std;

const char* numstr[17] = {
	"zero", "one", "two", "three", "four", "five", "six", 
	"seven", "eight", "nine", "ten", "eleven", "twelve", 
	"thirteen", "fourteen", "fifteen", "sixteen"
};

int string_int_test(void);
int string_int_ptr_test(void);
int string_string_ptr_owned_test(void);
int string_float_ptr_owned_test(void);

int main(int argc, char* argv[]) {
	string_int_test();
	string_int_ptr_test();
	string_string_ptr_owned_test();
	string_float_ptr_owned_test();
	return 0;
}

int string_int_test(void) {
	hashlist<string,int>	hl;
	hl.append("one",1);
	hl.append("three",3);
	hl.append("twelve",12);
	hl.append("five",5);
	hl.append("two",2);
	hl.append("eight",8);
	hl.append("four",4);
	hl.append("seven",7);
	hl.append("eleven",11);
	hl.append("six",6);
	hl.append("ten",10);
	hl.append("nine",9);

	{
	hashlist<string,int>::const_iterator i;
	cout << "list order: ";
	for (i=hl.begin(); i!=hl.end(); i++)
		cout << *i << " ";
	}
	cout << endl;
	cout << "random access: ";
	{
	int i;
	for (i=0; i<17; i++) {
		int* j = hl[numstr[i]];
		if (j)	cout << *j << " ";
		else	cout << "(null) ";
	}
	}
	cout << endl;
	return 0;
}

int string_int_ptr_test(void) {
	int ints[13] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	hashlist<string,int*>	hl;
	hl.append("one",&ints[1]);
	hl.append("three",&ints[3]);
	hl.append("twelve",&ints[12]);
	hl.append("five",&ints[5]);
	hl.append("two",&ints[2]);
	hl.append("eight",&ints[8]);
	hl.append("four",&ints[4]);
	hl.append("seven",&ints[7]);
	hl.append("eleven",&ints[11]);
	hl.append("six",&ints[6]);
	hl.append("ten",&ints[10]);
	hl.append("nine",&ints[9]);

	{
	hashlist<string,int*>::const_iterator i;
	cout << "list order: ";
	for (i=hl.begin(); i!=hl.end(); i++)
		cout << **i << " ";
	}
	cout << endl;
	cout << "random access: ";
	{
	int i;
	for (i=0; i<17; i++) {
		int** j = hl[numstr[i]];
		if (j)	cout << **j << " ";
		else	cout << "(null) ";
	}
	}
	cout << endl;

	return 0;
}

int string_string_ptr_owned_test(void) {
	hashlist_of_ptr<string,string>	hl;
	hl.append(numstr[9], new string(numstr[9]));
	hl.append(numstr[6], new string(numstr[6]));
	hl.append(numstr[2], new string(numstr[2]));
	hl.append(numstr[10], new string(numstr[10]));
	hl.append(numstr[3], new string(numstr[3]));
	hl.append(numstr[11], new string(numstr[11]));
	hl.append(numstr[5], new string(numstr[5]));
	hl.append(numstr[7], new string(numstr[7]));
	hl.append(numstr[8], new string(numstr[8]));
	hl.append(numstr[4], new string(numstr[4]));
	hl.append(numstr[12], new string(numstr[12]));
	hl.append(numstr[1], new string(numstr[1]));

	{
	hashlist_of_ptr<string,string>::const_iterator i;
	cout << "list order: ";
	for (i=hl.begin(); i!=hl.end(); i++)
		cout << **i << " ";
	}
	cout << endl;
	cout << "random access: ";
	{
	int i;
	for (i=0; i<17; i++) {
		string** j = hl[numstr[i]];
		if (j)	cout << **j << " ";
		else	cout << "(null) ";
	}
	}
	cout << endl;

	return 0;
}

int string_float_ptr_owned_test(void) {
	hashlist_of_ptr<string,double>	hl;
	hl.append(numstr[9], new double(9));
	hl.append(numstr[6], new double(6));
	hl.append(numstr[2], new double(2));
	hl.append(numstr[10], new double(10));
	hl.append(numstr[3], new double(3));
	hl.append(numstr[11], new double(11));
	hl.append(numstr[5], new double(5));
	hl.append(numstr[7], new double(7));
	hl.append(numstr[8], new double(8));
	hl.append(numstr[4], new double(4));
	hl.append(numstr[12], new double(12));
	hl.append(numstr[1], new double(1));

	{
	hashlist_of_ptr<string,double>::const_iterator i;
	cout << "list order: ";
	for (i=hl.begin(); i!=hl.end(); i++)
		cout << **i << " ";
	}
	cout << endl;
	cout << "random access: ";
	{
	int i;
	for (i=0; i<17; i++) {
		double** j = hl[numstr[i]];
		if (j)	cout << **j << " ";
		else	cout << "(null) ";
	}
	}
	cout << endl;

	return 0;
}

