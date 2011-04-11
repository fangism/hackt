// "PR/tile_instance.h"

#ifndef	__HAC_PR_TILE_INSTANCE_H__
#define	__HAC_PR_TILE_INSTANCE_H__

#include <set>
#include "PR/numerics.h"
#include "util/vector_ops.h"

namespace PR {
using std::set;
struct tile_type;

//=============================================================================
/**
	An object to place.
	TODO: Q: Is it more efficient to have undirected edges here
		for channel/springs?  (lower-triangle sparse)
	TODO: support grid-alignment and gravity forces
 */
struct tile_instance {
	/**
		Current position of object.
	 */
	position_type			position;
	/**
		Non-zero if simulating with object momentum.
	 */
	velocity_type			velocity;
	/**
		mutable, to be updated/accumulated, recomputed with 
		every interation.
	 */
	acceleration_type		acceleration;

	/**
		Object type.
	 */
	const tile_type*		type;
	/**
		Sparse 2d matrix of objects that are sufficiently close to 
		one another.  This matrix is dynamic and is constantly updated 
		with multidimensional sliding windows (intersection thereof).
	 */
	set<int_type>			proximity_cache;

	real_type			mass;

#if 0
	/**
		If true, cell is vertically flipped.
	 */
	bool				vflip;
	/**
		If true, cell is horizontally flipped.
	 */
	bool				hflip;
#endif
	/**
		If true, prevent this from moving.
	 */
	bool				fixed;

public:
	/**
		\param d number of dimensions.
	 */
	explicit
	tile_instance(const size_t d);

	void
	zero_force(void) {
		util::vector_ops::fill(acceleration, 0.0);
	}

	bool
	is_fixed(void) const {
		return fixed;
	}

	void
	fix(void) {
		fixed = true;
		util::vector_ops::fill(velocity, 0.0);
	}

	void
	unfix(void) {
		fixed = false;
	}


};	// end struct tile_instance

//=============================================================================
}	// end namespace PR

#endif	// __HAC_PR_TILE_INSTANCE_H__
