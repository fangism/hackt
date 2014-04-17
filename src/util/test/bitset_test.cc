/**
	\file "bitset_test.cc"
	$Id: $
 */
#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>
#include <iomanip>
#include "util/bitset.tcc"
#include "util/size_t.h"

using std::cout;
using std::endl;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t NB>
static
void
test_find_first(void) {
	typedef	util::bitset<NB>	bitset_type;
	cout << "util::bitset<" << NB << ">::find_first tests" << endl;
	bitset_type b;
	cout << b.find_first() << endl;
	b.set(0);
	cout << b.find_first() << endl;
	b.set(1);
	cout << b.find_first() << endl;
	b.reset(0);
	cout << b.find_first() << endl;
	b.set(35);
	cout << b.find_first() << endl;
	b.reset(1);
	cout << b.find_first() << endl;
	b.set(32);
	cout << b.find_first() << endl;
	b.set(49);
	cout << b.find_first() << endl;
	b.reset();
	b.set(64);
	cout << b.find_first() << endl;
	b.set(99);
	cout << b.find_first() << endl;
	b.reset(64);
	cout << b.find_first() << endl;
	b.reset(99);
	b.set(127);
	cout << b.find_first() << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Start searching from i.
 */
template <size_t NB>
static
void
__test_find_next_0(const size_t i) {
	typedef	util::bitset<NB>	bitset_type;
	bitset_type b;
	cout << "util::bitset<" << NB << ">::find_next test: set none"
		", search from " << i << ": " << b.find_next(i) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set 1 bits, start searching from i.
 */
template <size_t NB>
static
void
__test_find_next_1(const size_t s1, const size_t i) {
	typedef	util::bitset<NB>	bitset_type;
	bitset_type b;
	b.set(s1);
	cout << "util::bitset<" << NB << ">::find_next test: set " <<
		s1 << ", search from " << i << ": " << b.find_next(i) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set two bits, start searching from i.
 */
template <size_t NB>
static
void
__test_find_next_2(const size_t s1, const size_t s2, const size_t i) {
	typedef	util::bitset<NB>	bitset_type;
	bitset_type b;
	b.set(s1);
	b.set(s2);
	cout << "util::bitset<" << NB << ">::find_next test: set " <<
		s1 << " and " << s2 << ", search from " << i <<
		": " << b.find_next(i) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
static
void
__test_find_all(const T& t) {
	size_t i = t.find_first();
	cout << "find all: " << i;
	for ( ; i<t.size(); ) {
		i = t.find_next(i);
		cout << ", " << i;
	}
	cout << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
main(const int, char*[]) {
	test_find_first<128>();
	__test_find_next_0<128>(0);
	__test_find_next_0<128>(31);
	__test_find_next_0<128>(32);
	__test_find_next_0<128>(63);
	__test_find_next_0<128>(64);
	__test_find_next_0<128>(92);
	__test_find_next_1<128>(0, 0);
	__test_find_next_1<128>(0, 1);
	__test_find_next_1<128>(1, 1);
	__test_find_next_1<128>(1, 0);
	__test_find_next_1<128>(15, 1);
	__test_find_next_1<128>(16, 1);
	__test_find_next_1<128>(16, 15);
	__test_find_next_1<128>(15, 16);
	__test_find_next_1<128>(33, 21);
	__test_find_next_1<128>(33, 33);
	__test_find_next_1<128>(33, 97);
	__test_find_next_1<128>(121, 33);
	__test_find_next_1<128>(121, 97);
	__test_find_next_2<128>(0, 2, 0);
	__test_find_next_2<128>(1, 2, 0);
	__test_find_next_2<128>(3, 6, 1);
	__test_find_next_2<128>(3, 34, 1);
	__test_find_next_2<128>(3, 84, 1);
	__test_find_next_2<128>(23, 84, 1);
	__test_find_next_2<128>(41, 84, 2);
	__test_find_next_2<128>(71, 84, 2);
	__test_find_next_2<128>(31, 84, 31);
	__test_find_next_2<128>(31, 84, 34);
	__test_find_next_2<128>(33, 84, 34);
	__test_find_next_2<128>(34, 84, 34);
	__test_find_next_2<128>(35, 84, 34);
	__test_find_next_2<128>(35, 84, 97);
	__test_find_next_2<128>(35, 84, 77);
	__test_find_next_2<128>(35, 84, 37);
	__test_find_next_2<128>(35, 123, 37);
	__test_find_next_2<128>(39, 123, 37);
	__test_find_next_2<128>(39, 123, 41);
	typedef	util::bitset<128>	bitset_type;
{
	bitset_type b;
	__test_find_all(b);
}{
	bitset_type b;
	b.set(1);
	__test_find_all(b);
	b.set(111);
	__test_find_all(b);
	b.set(115);
	__test_find_all(b);
}{
	bitset_type b;
	b.set(77);
	__test_find_all(b);
	b.set(44);
	__test_find_all(b);
	b.set(45);
	__test_find_all(b);
}{
	bitset_type b;
	b.set(99);
	__test_find_all(b);
	b.set(111);
	__test_find_all(b);
	b.set(11);
	__test_find_all(b);
}{
	bitset_type b;
	b.set(31);
	__test_find_all(b);
	b.set(32);
	__test_find_all(b);
	b.set(16);
	__test_find_all(b);
	b.set(100);
	__test_find_all(b);
}{
	bitset_type b;
	b.set(63);
	__test_find_all(b);
	b.set(64);
	__test_find_all(b);
	b.set(4);
	__test_find_all(b);
	b.set(114);
	__test_find_all(b);
}
	return 0;
}


