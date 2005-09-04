/**
	\file "integer_traits_test.cc"
	Testing compile-time integer traits facilities.
	$Id: integer_traits_test.cc,v 1.4 2005/09/04 21:15:11 fang Exp $
 */

#include <string>
#include <iostream>
#include "util/numeric/integer_traits.h"
#include "util/what.tcc"

using std::string;
using namespace util::numeric;
#include "util/using_ostream.h"
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
test_prime(void) {
	// OBSERVE: no functioncall!
	cout << N << " is " << ifthenelse_value<is_prime<N>::value,
		const char*, prime_str, composite_str>::value << endl;
}

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

	cout << "next greater odd number is " << next_odd<N>::value << endl;
	cout << "next odd after sqrt_floor is " <<
		next_odd<sqrt_floor<N>::value>::value << endl;
	cout << "first prime before that is " <<
		prev_prime<next_odd<sqrt_floor<N>::value>::value>::value
		<< endl;
	cout << "and the prime before that is " <<
		prev_prime<prev_prime<
			next_odd<sqrt_floor<N>::value>::value
		>::value>::value << endl;
	cout << "last prime is " << prev_prime<next_odd<N>::value>::value
		<< endl;
	test_prime<N>();
#if 0
	cout << "next prime is " << next_prime<next_odd<N>::value>::value
		<< endl;
#endif
	cout << endl;
}

int
main(int, char*[]) {

	test_value<0>();
	test_value<1>();
	test_value<2>();
	test_value<3>();
	test_value<4>();
	test_value<5>();
	test_value<6>();
	test_value<7>();
	test_value<8>();
	test_value<9>();
	test_value<10>();
	test_value<11>();
	test_value<12>();
	test_value<13>();
#if 1
	// factorial overflows beyond N = 13
	test_prime<14>();
	test_prime<15>();
	test_prime<16>();
	test_prime<17>();
	test_prime<18>();
	test_prime<19>();

	test_prime<20>();
	test_prime<21>();
	test_prime<22>();
	test_prime<23>();
	test_prime<24>();
	test_prime<25>();
	test_prime<26>();
	test_prime<27>();
	test_prime<28>();
	test_prime<29>();

	test_prime<30>();
	test_prime<31>();
	test_prime<32>();
	test_prime<33>();
	test_prime<34>();
	test_prime<35>();
	test_prime<36>();
	test_prime<37>();
	test_prime<38>();
	test_prime<39>();

	test_prime<40>();
	test_prime<41>();
	test_prime<42>();
	test_prime<43>();
	test_prime<44>();
	test_prime<45>();
	test_prime<46>();
	test_prime<47>();
	test_prime<48>();
	test_prime<49>();

	test_prime<50>();
	test_prime<51>();
	test_prime<52>();
	test_prime<53>();
	test_prime<54>();
	test_prime<55>();
	test_prime<56>();
	test_prime<57>();
	test_prime<58>();
	test_prime<59>();

	test_prime<60>();
	test_prime<61>();
	test_prime<62>();
	test_prime<63>();
	test_prime<64>();
	test_prime<65>();
	test_prime<66>();
	test_prime<67>();
	test_prime<68>();
	test_prime<69>();

	test_prime<70>();
	test_prime<71>();
	test_prime<72>();
	test_prime<73>();
	test_prime<74>();
	test_prime<75>();
	test_prime<76>();
	test_prime<77>();
	test_prime<78>();
	test_prime<79>();

	test_prime<80>();
	test_prime<81>();
	test_prime<82>();
	test_prime<83>();
	test_prime<84>();
	test_prime<85>();
	test_prime<86>();
	test_prime<87>();
	test_prime<88>();
	test_prime<89>();

	test_prime<90>();
	test_prime<91>();
	test_prime<92>();
	test_prime<93>();
	test_prime<94>();
	test_prime<95>();
	test_prime<96>();
	test_prime<97>();
	test_prime<98>();
	test_prime<99>();
#endif

	return 0;
}

