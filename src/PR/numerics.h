/**
	\file "PR/numerics.h"
	Defines some global types and operations on vectors.
	Some of this could eventually be migrated to util/vector_ops.h
	$Id: numerics.h,v 1.1.2.6 2011/04/26 02:21:13 fang Exp $
 */

#ifndef	__HAC_PR_NUMERICS_H__
#define	__HAC_PR_NUMERICS_H__

/**
	Define to 1 to allow variable number of dimensions of space.
	If 0, fix to 3D.
	For now: fix to 3D.
	TODO: investigate compiler __vector types.
	http://gcc.gnu.org/onlinedocs/gcc/Vector-Extensions.html
 */
#define	PR_VARIABLE_DIMENSIONS			0

#include <iosfwd>
#include <string>
#if PR_VARIABLE_DIMENSIONS
#include <valarray>
#else
#include "util/array.h"
#endif
#include "util/size_t.h"
#include "util/optparse_fwd.h"
#include "util/IO_utils_fwd.h"

namespace PR {
using std::string;
using util::option_value;
using std::ostream;

typedef	ssize_t		int_type;
typedef	float		real_type;
typedef	float		time_type;

#if PR_VARIABLE_DIMENSIONS
typedef	valarray<int_type>		int_vector;
typedef	valarray<real_type>		real_vector;
#else
#if defined(HAVE_COMPILER_VECTOR_EXTENSIONS)
enum {	PR_DIMENSIONS = 4 };	// smallest power of 2 >= 3
typedef	int_type v4si __attribute__ ((vector_size (PR_DIMENSIONS*sizeof(int_type))));
typedef	real_type v4f __attribute__ ((vector_size (PR_DIMENSIONS*sizeof(real_type))));
typedef	v4si			int_vector;
typedef	v4f			real_vector;
#else
enum {	PR_DIMENSIONS = 3 };
using util::array;
typedef	array<int_type, PR_DIMENSIONS>	int_vector;
typedef	array<real_type, PR_DIMENSIONS>	real_vector;
#endif
#endif

typedef	real_vector		position_type;
typedef	real_vector		velocity_type;
typedef	real_vector		acceleration_type;
typedef	real_vector		force_type;
typedef	real_vector		energy_type;

extern
ostream&
operator << (ostream&, const real_vector&);

/// overrides the default implementation in util
extern
option_value
optparse(const string&);

extern
int
parse_real_vector(const string&, real_vector&);

extern
real_vector
random_unit_vector(void);

#if 0
extern
real_type
sum(const real_vector&);

extern
real_type
min(const real_vector&);

extern
real_type
max(const real_vector&);

extern
void
zero(real_vector&);
#endif

extern
real_type
normsq(const real_vector&);

/**
	\return sqrt(normsq())
 */
extern
real_type
norm(const real_vector&);

/**
	\return sum of absolute values of elements
 */
extern
real_type
sum_abs(const real_vector&);

/**
	Sum of delta-x, delta-y, ... which reflects orthogonal wire length.
	rectilinear a.k.a. Manhattan distance
 */
extern
real_type
rectilinear_distance(const real_vector&, const real_vector&);

/**
	\returns the vector between two nearest points.
	http://homepage.univie.ac.at/franz.vesely/notes/hard_ellipse/hell/hell.html
	Numerical iterative solution, thus requires a tolerance.
 */
extern
real_vector
minimum_distance_between_ellipsoids(
	const real_vector&, const real_vector&,
	const real_vector&, const real_vector&);

}	// end namespace PR

// specializations for vector
namespace util {
extern
void
write_value(std::ostream&, const PR::real_vector&);

extern
void
read_value(std::istream&, PR::real_vector&);

}	// end namespace util

#endif	// __HAC_PR_NUMERICS_H__

