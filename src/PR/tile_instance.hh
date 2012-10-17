// "PR/tile_instance.hh"

#ifndef	__HAC_PR_TILE_INSTANCE_H__
#define	__HAC_PR_TILE_INSTANCE_H__

#include "PR/tile_type.hh"
#include "PR/channel_type.hh"
#include "util/vector_ops.hh"

/**
	Define to 1 to have each tile_instance contain its own 
	proximity_cache to neighbors.  
	If defined to 0, proximity_cache is kept globally elsewhere.
#define	PR_LOCAL_PROXIMITY_CACHE			0
 */

/**
	Define to 1 to keep current coordinate state within tile_instance.
	Otherwise, keep in a separate array/struct.  
	Goal: 0 for efficient swapping of array of states.
#define	PR_STATE_IN_TILE			0
 */

namespace HAC {
namespace PR {
struct placer_options;
struct tile_instance;

//=============================================================================
/**
	Type used to return changes in position/velocity.
 */
typedef	std::pair<real_type, real_type>		delta_type;

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

	// delta_type
	void
	update(const time_type&, const real_type&);

#if 0
	void
	update(const time_type&, const real_type&, const object_state&);
#endif

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

	ostream&
	emit_dot(ostream&, const placer_options&) const;

	ostream&
	emit_fig(ostream&, const tile_instance&, const placer_options&) const;

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
		_kinetic_energy_2 = 0.0;
	}

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

	static
	real_type
	current_attraction_potential_energy(
		const tile_instance&, const tile_instance&,
		const channel_properties&,
		const object_state&, const object_state&);

	static
	real_type
	current_repulsion_potential_energy(
		const tile_instance&, const tile_instance&,
		const channel_properties&, const real_type&,
		const object_state&, const object_state&);

	static
	void
	apply_single_force(const tile_instance&, const force_type&,
		object_state&);

	static
	void
	apply_pairwise_force(const tile_instance&, const tile_instance&,
		const force_type&, object_state&, object_state&);

	template <size_t N>
	static
	real_type
	dimension_well_potential_energy(const tile_instance&, 
		const real_type& x, const real_type& g1, const real_type& g0,
		const object_state&);

	template <size_t N>
	static
	real_type
	attract_to_dimension_well(const tile_instance&, 
		const real_type& x, const real_type& g1, const real_type& g0,
		object_state&);

	static
	real_type
	apply_attraction_forces(const tile_instance&, const tile_instance&,
		const channel_properties&,
		object_state&, object_state&);

	static
	real_type
	apply_repulsion_forces(const tile_instance&, const tile_instance&,
		const channel_properties&, const real_type&,
		object_state&, object_state&);

	const real_type&
	update_kinetic_energy_2(const velocity_type&);

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
}	// end namespace HAC

#endif	// __HAC_PR_TILE_INSTANCE_H__
