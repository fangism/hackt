/**
	\file "PR/placement_engine.h"
	Physics simulator.
	$Id: placement_engine.h,v 1.1.2.17 2011/04/28 21:44:21 fang Exp $
 */

#ifndef	__HAC_PR_PLACEMENT_ENGINE_H__
#define	__HAC_PR_PLACEMENT_ENGINE_H__

#include <string>
#include "PR/pcanvas.h"
#include "PR/placer_options.h"
#include "PR/gravity_well.h"
#include "sim/state_base.h"
#include "util/named_ifstream_manager.h"	// needed by interpreter

/**
	Define to 1 to enable aligning to hyperplanes.
	Rationale: snapping to grid.
	Goal: 1
	Status: in testing
 */
#define	ENABLE_GRAVITY_WELLS		1

namespace PR {
using std::istream;
using std::string;
using util::ifstream_manager;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For solving placement.
	Physics, force-driven, with annealing.
 */
class placement_engine : public HAC::SIM::state_base {
	typedef	placement_engine	this_type;
protected:
	vector<tile_type>		object_types;
	vector<channel_type>		channel_types;
public:
	placer_options			opt;
	/**
		The objects and coordinates to solve in.
	 */
	pcanvas				space;
protected:
#if ENABLE_GRAVITY_WELLS
	/**
		Gravity wells for aligning 
	 */
	gravity_well			x_wells;
	gravity_well			y_wells;
	gravity_well			z_wells;
#endif

	typedef	channel_instance		proximity_edge;
	/**
		Proximity cache is just a collection of
		repulsion springs that are generated on-the-fly.
	 */
	vector<proximity_edge>		proximity_cache;
	/**
		Current simulation time.
	 */
	time_type			elapsed_time;
	// run-time updates values
	real_type			proximity_potential_energy;
	real_type			gravity_potential_energy;
	/**
		Name of auto-saved checkpoint.
	 */
	string				autosave_name;
	/**
		Signals that forces have not been updated.
		This should be set every time positions are updated.
	 */
	bool				need_force_recalc;
public:

	explicit
	placement_engine(const size_t d);

	~placement_engine();

	void
	auto_proximity_radius(void);

	void
	add_object_type(const tile_type&);

	void
	add_channel_type(const channel_type&);

	void
	add_object(const tile_instance&);

	bool
	add_channel(const channel_instance&);

#if ENABLE_GRAVITY_WELLS
	void
	add_x_well(const real_type& r) { x_wells.add_single_well(r); }

	void
	add_y_well(const real_type& r) { y_wells.add_single_well(r); }

	void
	add_z_well(const real_type& r) { z_wells.add_single_well(r); }

	void
	add_x_wells(const real_type& r, const real_type& s, const real_type& t)
		{ x_wells.add_strided_wells(r, s, t); }

	void
	add_y_wells(const real_type& r, const real_type& s, const real_type& t)
		{ y_wells.add_strided_wells(r, s, t); }

	void
	add_z_wells(const real_type& r, const real_type& s, const real_type& t)
		{ z_wells.add_strided_wells(r, s, t); }

	void
	snap_to_gravity_wells(void);
#endif

	void
	scatter(void);

	size_t
	num_object_types(void) const {
		return object_types.size();
	}

	size_t
	num_channel_types(void) const {
		return channel_types.size();
	}

	const tile_type&
	get_object_type(const size_t i) const {
		return object_types[i];
	}

	const channel_type&
	get_channel_type(const size_t i) const {
		return channel_types[i];
	}

	size_t
	num_objects(void) const {
		return space.objects.size();
	}

	bool
	pin_object(const size_t);

	bool
	unpin_object(const size_t);

	bool
	place_object(const size_t, const real_vector&);

	// force recalc because changing parameters invalidates energy
	bool
	parse_parameter(const string& s) {
		need_force_recalc = true;
		return opt.parse_parameter(s);
	}

	bool
	parse_parameter(const option_value& o) {
		need_force_recalc = true;
		return opt.parse_parameter(o);
	}

	ostream&
	dump_parameters(ostream&) const;

	void
	autosave(const string& s) { autosave_name = s; }

// simulation
	void
	kill_momentum(void);

	template <real_type (this_type::*MF)(void)>
	void
	__repeat_until_converge(const char*, const char*);

	delta_type
	iterate(void);

	void
	iterate_no_delta(void) {
		iterate();
	}

	ostream&
	watch_iterate(ostream&) const;

	real_type
	__gradient_slide(void);

	void
	gradient_slide(void) {
		__gradient_slide();
	}

	void
	repeat_gradient_slide(void);

	void
	simple_converge(void);

	void
	adaptive_converge(void);

	real_type
	__descend_potential_energy(void);

	void
	descend_potential_energy(void) {
		__descend_potential_energy();
	}

	void
	repeat_descend_potential_energy(void);

// energy
	real_type
	kinetic_energy(void) const {
		return space.kinetic_energy();
	}

	real_type
	potential_energy(void) const {
		return space.potential_energy()
			+proximity_potential_energy
#if ENABLE_GRAVITY_WELLS
			+gravity_potential_energy
#endif
			;
	}

	real_type
	update_potential_energy(void) {
		return space.update_potential_energy()
			+update_proximity_potential_energy()
#if ENABLE_GRAVITY_WELLS
			+update_gravity_potential_energy()
#endif
			;
	}

	ostream&
	dump_object_types(ostream&) const;

	ostream&
	dump_channel_types(ostream&) const;

	ostream&
	dump_positions(ostream&) const;

	ostream&
	dump_objects(ostream&) const;

	ostream&
	dump_channels(ostream&) const;

	ostream&
	dump_wells(ostream&) const;

	ostream&
	dump_energy(ostream&);

	ostream&
	dump(ostream&) const;

	ostream&
	emit_dot(ostream&) const;

// checkpointing features
	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

	static
	ostream&
	dump_checkpoint(ostream&, istream&);

// tracing features
	bool
	open_trace(const string&);

	void
	close_trace(void);

private:
	void
	bootstrap_forces(void);

	void
	bootstrap_forces_silent(void);

	void
	initialize_default_types(void);

	void
	zero_forces(void);

	void
	calculate_forces(void);

	typedef	vector<object_state>::const_iterator	object_iterator;
	typedef	vector<object_iterator>			object_iterator_array;

	void
	create_object_iterator_array(object_iterator_array&) const;

	delta_type
	update_positions(void);

	void
	refresh_proximity_cache(void);

	// incrementally update
	void
	update_proximity_cache(void);

	void
	clear_proximity_cache(void);

	void
	compute_collision_forces(void);

	const real_type&
	update_proximity_potential_energy(void);

	const real_type&
	update_gravity_potential_energy(void);

	typedef	gravity_well::node_set_type		gravity_map_type;

	template <size_t>
	real_type
	__compute_gravity_energy(const gravity_map_type&, 
		const real_type&, const real_type&, object_iterator_array&);

	template <size_t>
	real_type
	__compute_gravity_forces(const gravity_map_type&, 
		const real_type&, const real_type&, object_iterator_array&);

	template <size_t>
	void
	__snap_to_gravity_wells(const gravity_map_type&, 
		object_iterator_array&);

	void
	compute_gravity_forces(void);

};	// end class placement_engine

//=============================================================================

}	// end namespace PR

#endif	// __HAC_PR_PLACEMENT_ENGINE_H__
