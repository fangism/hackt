/**
	\file "PR/numerics.h"
	Defines some global types.
	$Id: numerics.h,v 1.1.2.1 2011/04/11 18:38:36 fang Exp $
 */

#ifndef	__HAC_PR_NUMERICS_H__
#define	__HAC_PR_NUMERICS_H__

/**
	Define to 1 to allow variable number of dimensions of space.
	If 0, fix to 3D.
	For now: fix to 3D.
 */
#define	VARIABLE_DIMENSIONS			0

#if VARIABLE_DIMENSIONS
#include <valarray>
#else
#include "util/array.h"
#endif
#include "util/size_t.h"

namespace PR {

typedef	ssize_t		int_type;
typedef	float		real_type;
typedef	float		time_type;

#if VARIABLE_DIMENSIONS
typedef	valarray<int_type>		int_vector;
typedef	valarray<real_type>		real_vector;
#else
enum {	PR_DIMENSIONS = 3 };
using util::array;
typedef	array<int_type, PR_DIMENSIONS>	int_vector;
typedef	array<real_type, PR_DIMENSIONS>	real_vector;
#endif

typedef	real_vector		position_type;
typedef	real_vector		velocity_type;
typedef	real_vector		acceleration_type;
typedef	real_vector		force_type;

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

#endif	// __HAC_PR_NUMERICS_H__

