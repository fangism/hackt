/**
	\file "integer_traits_test.cc"
	Testing compile-time integer traits facilities.
	$Id: integer_traits_test.cc,v 1.1.2.1 2005/01/19 01:21:42 fang Exp $
 */

#include <string>
#include <iostream>
#include "numeric/integer_traits.h"
#include "what.tcc"

using std::string;
using namespace util::numeric;
#include "using_ostream.h"
using util::what;
using util::ifthenelse_value;

namespace util {
template <bool B>
struct what<power_of_2_tag<B> > {
	typedef	string	name_type;
	static const name_type	name;
};

template <bool>
struct bool_string { };

template <>
struct bool_string<false> { static const char* str; };
const char* bool_string<false>::str = "false";

template <>
struct bool_string<true> { static const char* str; };
const char* bool_string<true>::str = "true";

template <bool B>
const typename what<power_of_2_tag<B> >::name_type
what<power_of_2_tag<B> >::name =
	string("power_of_2_tag<") +bool_string<B>::str +">";

}	// end namespace util

// must have external linkage
extern const char prime_str[];
extern const char composite_str[];
extern const char blank[];
extern const char is_not[];

const char blank[] = "";
const char is_not[] = " not";
const char prime_str[] = "prime";
const char composite_str[] = "composite";

template <size_t N>
static
void
test_value(void) {
	cout << "Testing traits of integer " << N << ':' << endl;

	// instantiation causes internal compiler error!
	cout << "... is";
	cout << ifthenelse_value<is_square<N>::value, const char*, blank, is_not>::value;
	cout << " a perfect square." << endl;

	cout << "nearest square-root is " << sqrt_floor<N>::value << endl;
	cout << "... is";
	cout << ifthenelse_value<is_power_of_2<N>::value, const char*, blank, is_not>::value;
	cout << " a power of 2." << endl;

	cout << "power_of_2 category is " << 
		what<typename power_of_2_traits<N>::category>::name << endl;

	if (is_power_of_2<N>::value) {
		static const size_t product = multiply_by_constant<N>(10);
		cout << "multiplication becomes shift: (10 * " << N <<
			") = (10 << " << significance<N>::value << ") = " <<
			product << endl;
	}

	cout << N << "! (factorial) = " << factorial<N>::value << endl;
	cout << "Fibonacci[" << N << "] = " << nth_fibonacci<N>::value << endl;

#if 0
	cout << N << " is " << ifthenelse_value<is_prime<N>::value, const char*, 		prime_str, composite_str>::value << endl;
#endif
	cout << endl;
}

int
main(int argc, char* argv[]) {

	test_value<0>();
	test_value<1>();
	test_value<2>();
	test_value<3>();
	test_value<4>();
#if 1
	test_value<5>();
	test_value<6>();
	test_value<7>();
	test_value<8>();
	test_value<9>();
	test_value<10>();
#endif
	return 0;
}

