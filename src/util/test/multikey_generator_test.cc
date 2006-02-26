/**
	\file "multikey_generator_test.cc"
	Test for multidimensional key generator, which steps 
	through rings of values of an N-dimensional cube slice.
	$Id: multikey_generator_test.cc,v 1.6 2006/02/26 02:28:05 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>
#include "util/multikey.tcc"

using namespace std;
using util::multikey;
using util::multikey_generator;

int
main (int, char*[]) {
	typedef	multikey<3,int>			key_type;
	typedef	multikey_generator<3,int>	generator_type;
	key_type min, max;
	min[0] = 1;
	min[1] = 4;
	min[2] = 7;
	max[0] = 3;
	max[1] = 6;
	max[2] = 9;
	generator_type iter(min, max);
	iter.initialize();
	const key_type end(iter);
	do {
		cout << iter[0] << ", " << iter[1] << ", " << iter[2] << endl;
		iter++;
	} while (iter != end);
	return 0;
}

