// "PR/tile_instance.h"

#ifndef	__HAC_PR_TILE_INSTANCE_H__
#define	__HAC_PR_TILE_INSTANCE_H__

/**
	Define to 1 to have each tile_instance contain its own 
	proximity_cache to neighbors.  
	If defined to 0, proximity_cache is kept globally elsewhere.
#define	PR_LOCAL_PROXIMITY_CACHE			0
 */

#include "PR/tile_type.h"
#include "PR/channel_type.h"
#include "util/vector_ops.h"

/**
	Define to 1 to keep current coordinate state within tile_instance.
	Otherwise, keep in a separate array/struct.  
	Goal: 0 for efficient swapping.
 */
#define	PR_STATE_IN_TILE			0

namespace PR {

//=============================================================================
struct object_state {
	/**
		Current position of object.
	 */
	position_type			position;
//	position_type			previous_position;
	/**
		Non-zero if simulating with object momentum.
	 */
	velocity_type			velocity;
//	velocity_type			previous_velocity;
	/**
		mutable, to be updated/accumulated, recomputed with 
		every interation.
	 */
	acceleration_type		acceleration;


public:
	object_state();

	void
	zero_force(void) {
		util::vector_ops::fill(acceleration, 0.0);
	}

	void
	kill_momentum(void) {
		util::vector_ops::fill(velocity, 0.0);
	}


	void
	place(const real_vector& v) {
		position = v;
	}

	void
	update(const time_type&, const real_type&);

	void
	update(const time_type&, const real_type&, const object_state&);

#if 0
	real_type
	rectilinear_delta_position(void) const {
		return rectilinear_distance(previous.position, current.position);
	}

	real_type
	rectilinear_delta_velocity(void) const {
		return rectilinear_distance(previous.velocity, current.velocity);
	}
#endif

	ostream&
	dump(ostream&) const;

	ostream&
	dump_position(ostream&) const;

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
struct tile_instance {
//	force_type			force;

	/**
		Object type, copied, not linked.
	 */
	tile_type			properties;
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
private:
	/**
		If true, prevent this from moving.
	 */
	bool				fixed;
#if PR_STATE_IN_TILE
public:
	object_state			current;
	object_state			previous;
#endif
private:
	/**
		Kinetic energy is computed by update.
		without factor of 1/2.
	 */
	real_type			_kinetic_energy_2;

public:
	static bool			dump_properties;

public:
	tile_instance();

	explicit
	tile_instance(const tile_type&);

	~tile_instance();

	void
	kill_momentum(void) {
#if PR_STATE_IN_TILE
		util::vector_ops::fill(current.velocity, 0.0);
		previous.velocity = current.velocity;
#endif
		_kinetic_energy_2 = 0.0;
	}

#if PR_STATE_IN_TILE
	void
	place(const real_vector& v) {
		current.position = v;
//		previous.position = v;
	}

	void
	zero_force(void) {
		current.zero_force();
//		previous.zero_force();
	}
#endif

	bool
	is_fixed(void) const {
		return fixed;
	}

	void
	fix(void) {
		fixed = true;
		kill_momentum();
	}

	void
	unfix(void) {
		fixed = false;
	}

#if PR_STATE_IN_TILE
	void
	update(const time_type& t, const real_type& k) {
		previous = current;
		current.update(t, k);
	}
#endif

	static
	real_type
	current_attraction_potential_energy(
		const tile_instance&, const tile_instance&,
		const channel_properties&
#if !PR_STATE_IN_TILE
		, const object_state&, const object_state&
#endif
		);

	static
	real_type
	current_repulsion_potential_energy(
		const tile_instance&, const tile_instance&,
		const channel_properties&
#if !PR_STATE_IN_TILE
		, const object_state&, const object_state&
#endif
		);

#if PR_STATE_IN_TILE
	static
	void
	apply_pairwise_force(tile_instance&, tile_instance&, const force_type&);

	static
	real_type
	apply_attraction_forces(tile_instance&, tile_instance&,
		const channel_properties&);

	static
	real_type
	apply_repulsion_forces(tile_instance&, tile_instance&,
		const channel_properties&);
#else
	static
	void
	apply_pairwise_force(const tile_instance&, const tile_instance&,
		const force_type&, object_state&, object_state&);

	static
	real_type
	apply_attraction_forces(const tile_instance&, const tile_instance&,
		const channel_properties&, 
		object_state&, object_state&);

	static
	real_type
	apply_repulsion_forces(const tile_instance&, const tile_instance&,
		const channel_properties&, 
		object_state&, object_state&);
#endif

#if PR_STATE_IN_TILE
	const real_type&
	update_kinetic_energy_2(void);
#else
	const real_type&
	update_kinetic_energy_2(const velocity_type&);
#endif

	const real_type&
	kinetic_energy_2(void) const { return _kinetic_energy_2; }

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

#if PR_STATE_IN_TILE
	ostream&
	dump_position(ostream& o) const {
		return current.dump_position(o);
	}
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
