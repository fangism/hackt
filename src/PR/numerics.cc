/**
	\file "PR/numerics.cc"
	$Id: numerics.cc,v 1.2 2011/05/03 19:20:45 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>
#include <list>
#include "PR/numerics.hh"
#include "util/vector_ops.hh"
#include "util/numeric/abs.hh"
#include "util/tokenize.hh"
#include "util/string.tcc"
#include "util/array.tcc"
#include "util/optparse.hh"
#include "util/IO_utils.tcc"
#include "util/numeric/random.hh"
#include "util/numeric/constants.h"
#include "util/numeric/trigonometry.hh"
#include "util/STL/container_iterator.hh"
#if PR_VARIABLE_DIMENSIONS
#include "util/STL/valarray_iterator.hh"
#endif
#include "util/stacktrace.hh"

namespace util {
using std::ostream;
using std::istream;
using HAC::PR::real_vector;

#if PR_VARIABLE_DIMENSIONS
#else
#if !defined(HAVE_COMPILER_VECTOR_EXTENSIONS)
using namespace HAC::PR;
template class	array<int_type, PR_DIMENSIONS>;
template class	array<real_type, PR_DIMENSIONS>;
#endif
#endif

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

namespace HAC {
namespace PR {
#include "util/using_ostream.hh"
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
/**
	\param n upper bound of the the norm of the random vector.
	Distance (radius) is *uniformly* distributed.
 */
real_vector
random_scaled_vector(const real_type& n) {
	typedef	rand48<double>			random_generator;
	const random_generator g;	// ranges from [0.0,1.0]
	return random_unit_vector() * (g() *n);
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
	return std::sqrt(normsq(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
real_type
sum_abs(const real_vector& r) {
	real_vector rc(r);
	return sum(apply(rc, abs<real_type>));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
real_type
rectilinear_distance(const real_vector& r1, const real_vector& r2) {
	return sum_abs(r2 -r1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace PR
}	// end namespace HAC

