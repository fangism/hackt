/**
	\file "PR/numerics.cc"
	$Id: numerics.cc,v 1.1.2.4 2011/04/19 22:31:17 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>
#include <list>
#include "PR/numerics.h"
#include "util/vector_ops.h"
#include "util/numeric/abs.h"
#include "util/tokenize.h"
#include "util/string.tcc"
#include "util/optparse.h"
#include "util/IO_utils.tcc"
#include "util/numeric/random.h"
#include "util/numeric/constants.h"
#include "util/numeric/trigonometry.h"
#include "util/STL/container_iterator.h"
#if PR_VARIABLE_DIMENSIONS
#include "util/STL/valarray_iterator.h"
#endif
#include "util/stacktrace.h"

namespace util {
using std::ostream;
using std::istream;
using PR::real_vector;

/**
	Fixed size array for now.
	In general, could use fixed size array overloads for IO.
 */
void
write_value(ostream& o, const real_vector& v) {
#if PR_VARIABLE_DIMENSIONS
#error	"Unhandled case."
#else
	write_value(o, v[0]);
	write_value(o, v[1]);
	write_value(o, v[2]);
//	write_value(o, v[3]);
#endif
}

void
read_value(istream& i, real_vector& v) {
	read_value(i, v[0]);
	read_value(i, v[1]);
	read_value(i, v[2]);
//	read_value(i, v[3]);
}

}	// end namespace util

namespace PR {
#include "util/using_ostream.h"
using std::transform;
using std::accumulate;
using util::numeric::abs;
using namespace util::vector_ops;
using util::tokenize_char;
using util::string_list;
using util::strings::string_to_num;
using util::numeric::rand48;
using util::numeric::sincos;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream& o, const real_vector& v) {
	o << '<';
	typedef	std::container_const_iterator<real_vector>	iterator_policy;
	iterator_policy::type i(std::begin(v)), e(std::end(v));
if (i!=e) {
	o << *i;
	for (++i; i!=e; ++i) {
		o << ',' << *i;
	}
}
	return o << '>';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Use a different separator because comma appears in vectors.
	In the PR modules, no options take more than one value anyway.
 */
option_value
optparse(const string& s) {
	return util::optparse(s, ';');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** 
	Parses a vector of the form: "<val,val,...>"
	Angle brackets required, and no spaces between commas.
	\return non-zero to signal an error
 */
int
parse_real_vector(const string& s, real_vector& v) {
	STACKTRACE_VERBOSE;
	static const char err_msg[] =
		"Error parsing real_vector, expecting <...>";

	if (s.length() <= 2) {
		cerr << err_msg << endl;
		return 1;
	}
	if (s[0] != '<' || *--s.end() != '>') {
		cerr << err_msg << endl;
		return 1;
	}
//	STACKTRACE_INDENT_PRINT("vector looks ok 1" << endl);
	string t(++s.begin(), --s.end());
	string_list l;
	tokenize_char(t, l, ',');
#if PR_VARIABLE_DIMENSIONS
#error	"FINISH ME: parse_real_vector"
#else
	if (l.size() != PR_DIMENSIONS) {
		cerr << "Error parsing real_vector, expecting "
			<< PR_DIMENSIONS << " dimensions." << endl;
		return 1;
	}
//	STACKTRACE_INDENT_PRINT("vector looks ok 2" << endl);
	string_list::const_iterator
		i(l.begin()), e(l.end());
	real_vector::iterator r(std::begin(v));
	for ( ; i!=e; ++i, ++r) {
		if (string_to_num(*i, *r)) {
			cerr << "Error parsing coordinate." << endl;
			return 1;
		}
	}
#endif
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a uniformly oriented random vector.
 */
real_vector
random_unit_vector(void) {
	STACKTRACE_VERBOSE;
	typedef	rand48<double>			random_generator;
	const random_generator g;
	// trust compiler to fold these constants...
	const real_type phi = g() * M_PI;	// angle from north pole
	const real_type theta = g() * M_PI*2.0;	// longitude
	STACKTRACE_INDENT_PRINT("phi= " << phi*(180.0/M_PI) <<
		", theta=" << theta*(180.0/M_PI) << endl);
	real_type sinphi, sth, cth;
	real_vector ret;
	// optimized trig. calls!
	sincos(phi, sinphi, ret[2]);
	sincos(theta, sth, cth);
	ret[0] = sinphi *cth;
	ret[1] = sinphi *sth;
	STACKTRACE_INDENT_PRINT("rand-vec: " << ret << endl);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
real_type
normsq(const position_type& p) {
	position_type sq(p);
#if PR_VARIABLE_DIMENSIONS
	sq *= p;
	return sq.sum();
#else
	transform(p.begin(), p.end(), p.begin(), sq.begin(), 
		std::multiplies<real_type>());
	return accumulate(sq.begin(), sq.end(), 0.0);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
real_type
norm(const position_type& p) {
	return sqrt(normsq(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
real_type
rectilinear_distance(const real_vector& r1, const real_vector& r2) {
	real_vector d(r2 -r1);
	return sum(apply(d, abs<real_type>));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace PR

