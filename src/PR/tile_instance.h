// "PR/tile_instance.h"

#ifndef	__HAC_PR_TILE_INSTANCE_H__
#define	__HAC_PR_TILE_INSTANCE_H__

/**
	Define to 1 to have each tile_instance contain its own 
	proximity_cache to neighbors.  
	If defined to 0, proximity_cache is kept globally elsewhere.
 */
#define	PR_LOCAL_PROXIMITY_CACHE			0

#if PR_LOCAL_PROXIMITY_CACHE
#include <set>
#endif
#include "PR/tile_type.h"
#include "util/vector_ops.h"

namespace PR {

//=============================================================================
struct object_state {
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

	object_state();

	void
	zero_force(void) {
		util::vector_ops::fill(acceleration, 0.0);
	}

	void
	place(const real_vector& v) {
		position = v;
		previous_position = v;
	}

	void
	update(const time_type&, const real_type&);

	real_type
	rectilinear_delta_position(void) const {
		return rectilinear_distance(previous_position, position);
	}

	real_type
	rectilinear_delta_velocity(void) const {
		return rectilinear_distance(previous_velocity, velocity);
	}

	ostream&
	dump(ostream&) const;

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

};	// end struct object_state

//-----------------------------------------------------------------------------
/**
	An object to place.
	TODO: Q: Is it more efficient to have undirected edges here
		for channel/springs?  (lower-triangle sparse)
	TODO: support grid-alignment and gravity forces
 */
struct tile_instance : public object_state {
//	force_type			force;

	/**
		Object type, copied, not linked.
	 */
	tile_type			properties;
#if PR_LOCAL_PROXIMITY_CACHE
	/**
		Sparse graph of objects that are sufficiently close to 
		one another.  This matrix is dynamic and is constantly updated 
		with multidimensional sliding windows (intersection thereof).
	 */
	std::set<int_type>		proximity_cache;
#endif
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

	static bool			dump_properties;

public:
	tile_instance();

	explicit
	tile_instance(const tile_type&);

	~tile_instance();

	bool
	is_fixed(void) const {
		return fixed;
	}

	void
	fix(void) {
		fixed = true;
		util::vector_ops::fill(velocity, 0.0);
		previous_velocity = velocity;
	}

	void
	unfix(void) {
		fixed = false;
	}

#if 0
	real_vector
	delta_position(void) const {
		return position -previous_position;
	}

	real_vector
	delta_velocity(void) const {
		return velocity -previous_velocity;
	}
#endif

	ostream&
	dump(ostream&) const;

#if PR_LOCAL_PROXIMITY_CACHE
	void
	clear_proximity_cache(void) { proximity_cache.clear(); }
#endif

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
