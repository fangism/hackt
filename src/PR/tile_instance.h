// "PR/tile_instance.h"

#ifndef	__HAC_PR_TILE_INSTANCE_H__
#define	__HAC_PR_TILE_INSTANCE_H__

#include <set>
#include "PR/tile_type.h"
#include "util/vector_ops.h"

namespace PR {
using std::set;

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
	position_type			previous_position;
	/**
		Non-zero if simulating with object momentum.
	 */
	velocity_type			velocity;
	velocity_type			previous_velocity;
	/**
		mutable, to be updated/accumulated, recomputed with 
		every interation.
	 */
	acceleration_type		acceleration;
//	force_type			force;

	/**
		Object type, copied, not linked.
	 */
	tile_type			properties;
	/**
		Sparse graph of objects that are sufficiently close to 
		one another.  This matrix is dynamic and is constantly updated 
		with multidimensional sliding windows (intersection thereof).
	 */
	set<int_type>			proximity_cache;

#if 0
	// only matters for routing
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
	tile_instance();

	explicit
	tile_instance(const tile_type&);

	~tile_instance();

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

	void
	place(const real_vector& v) {
		position = v;
		previous_position = v;
	}

	void
	update(const time_type&);

	ostream&
	dump(ostream&) const;

	void
	clear_proximity_cache(void) { proximity_cache.clear(); }

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

	static
	ostream&
	dump_checkpoint(ostream&, istream&);

};	// end struct tile_instance

//=============================================================================
}	// end namespace PR

#endif	// __HAC_PR_TILE_INSTANCE_H__
