/**
	\file "bitmanip_test.cc"
	$Id: bitmanip_test.cc,v 1.2 2007/08/29 15:50:25 fang Exp $
 */
#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>
#include <iomanip>
#include "util/bitmanip.h"
#include "util/size_t.h"
#include "util/macros.h"

using std::cout;
using std::endl;
using util::numeric::popcount;
using util::numeric::parity;
using util::numeric::clz;
using util::numeric::ctz;
using util::numeric::ffs;
using util::numeric::msb;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class R, class A>
static
void
test_func_step(R (*f)(A), const char* fn, 
		size_t min, const size_t max, const size_t step) {
	INVARIANT(min < max);
	do {
		cout << fn << "(0x";
		cout << std::hex << min;
		cout << ") = " << std::dec << size_t((*f)(A(min))) << endl;
		min += step;
	} while (min <= max);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class R, class A>
static
void
test_func_shift(R (*f)(A), const char* fn, 
		size_t min, const size_t count, const size_t step) {
	size_t i = 0;
	do {
		cout << fn << "(0x";
		cout << std::hex << min;
		cout << ") = " << std::dec << size_t((*f)(A(min))) << endl;
		min <<= 1;
		min += step;
		++i;
	} while (i < count);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class R, class A>
static
void
test_func_shift_repeat(R (*f)(A), const char* fn, 
		size_t min, const size_t count) {
	size_t i = 0;
	do {
		cout << fn << "(0x";
		cout << std::hex << A(min);	// mask out overflow
		cout << ") = " << std::dec << size_t((*f)(A(min))) << endl;
		min <<= 1;
		++i;
	} while (i < count);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
main(const int, char*[]) {
	// don't bother testing long / long longs, depends on LP32 vs 64
	test_func_step(&parity<unsigned char>,
		"parity<uchar>", 0, 0xFF, 1);
	test_func_step(&parity<unsigned short>,
		"parity<ushort>", 1, 0xF00D, 0xF7);
	test_func_step(&parity<unsigned int>,
		"parity<uint>", 1, 0xC00000DE, 0xFAAAAC);

	test_func_step(&popcount<unsigned char>,
		"popcount<uchar>", 0, 0xFF, 1);
	test_func_step(&popcount<unsigned short>,
		"popcount<ushort>", 1, 0xF00D, 0xF7);
	test_func_step(&popcount<unsigned int>,
		"popcount<uint>", 0x10, 0xC00000DE, 0xFAAAAC);

	test_func_step(&clz<unsigned char>,
		"clz<uchar>", 1, 0xFF, 1);
	test_func_step(&clz<unsigned short>,
		"clz<ushort>", 1, 0xF00D, 0xF7);
	test_func_shift(&clz<unsigned short>,
		"clz<ushort>", 1, 16, 0);
	test_func_shift(&clz<unsigned short>,
		"clz<ushort>", 1, 15, 1);
	test_func_shift(&clz<unsigned int>,
		"clz<uint>", 1, 32, 0);
	test_func_shift(&clz<unsigned int>,
		"clz<uint>", 1, 31, 1);

	test_func_step(&msb<unsigned char>,
		"msb<uchar>", 1, 0xFF, 1);
	test_func_step(&msb<unsigned short>,
		"msb<ushort>", 1, 0xF00D, 0xF7);
	test_func_shift(&msb<unsigned short>,
		"msb<ushort>", 1, 16, 0);
	test_func_shift(&msb<unsigned short>,
		"msb<ushort>", 1, 15, 1);
	test_func_shift(&msb<unsigned int>,
		"msb<uint>", 1, 32, 0);
	test_func_shift(&msb<unsigned int>,
		"msb<uint>", 1, 31, 1);

	test_func_step(&ctz<unsigned char>,
		"ctz<uchar>", 1, 0xFF, 1);
	test_func_step(&ctz<unsigned short>,
		"ctz<ushort>", 0x1, 0xF00C, 0x3F5);
	test_func_step(&ctz<unsigned short>,
		"ctz<ushort>", 0xF7, 0xF00C, 0xF7);
	test_func_shift_repeat(&ctz<unsigned short>,
		"ctz<ushort>", 1, 16);
	test_func_shift_repeat(&ctz<unsigned short>,
		"ctz<ushort>", 0xFFFF, 16);
	test_func_shift_repeat(&ctz<unsigned int>,
		"ctz<uint>", 1, 32);
	test_func_shift_repeat(&ctz<unsigned int>,
		"ctz<uint>", 0xFFFFFFFF, 32);

	test_func_step(&ffs<unsigned char>,
		"ffs<uchar>", 0, 0xFF, 1);
	test_func_step(&ffs<unsigned short>,
		"ffs<ushort>", 0, 0xF00C, 0xF7);
	test_func_shift_repeat(&ffs<unsigned short>,
		"ffs<ushort>", 0, 0);
	test_func_shift_repeat(&ffs<unsigned short>,
		"ffs<ushort>", 1, 16);
	test_func_shift_repeat(&ffs<unsigned short>,
		"ffs<ushort>", 0xFFFF, 16);
	test_func_shift_repeat(&ffs<unsigned int>,
		"ffs<uint>", 0, 0);
	test_func_shift_repeat(&ffs<unsigned int>,
		"ffs<uint>", 1, 32);
	test_func_shift_repeat(&ffs<unsigned int>,
		"ffs<uint>", 0xFFFFFFFF, 32);

	return 0;
}


