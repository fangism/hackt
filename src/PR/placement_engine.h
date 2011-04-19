/**
	\file "PR/placement_engine.h"
	Physics simulator.
	$Id: placement_engine.h,v 1.1.2.6 2011/04/19 03:51:48 fang Exp $
 */

#ifndef	__HAC_PR_PLACEMENT_ENGINE_H__
#define	__HAC_PR_PLACEMENT_ENGINE_H__

#include <string>
#include "PR/pcanvas.h"
#include "sim/state_base.h"
#include "util/named_ifstream_manager.h"	// needed by interpreter

namespace PR {
using std::istream;
using std::string;
using util::ifstream_manager;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct placer_options {
	/**
		High temperature: greater magnitude of random velocity vector
		added to each iteration.
		Vary the temperature in different phases to simulate annealing.
	 */
	real_type			temperature;
#if 0
	/**
		Constant dampening factor (positive).
		A constant force that is always opposite the direction 
		of motion.
	 */
	real_type			friction;
#endif
	/**
		Viscous damping coefficient.
		A speed-dependent force in the opposite direction.
		Linear dampening factor.
	 */
	real_type			viscous_damping;
	/**
		Threshold distance to cache nearby-objects for collision
		and repulsion calculations.
		For this purpose, a cube zone is used.  
		A good value for this would be the maximum of any tile's 
		height or width.  
	 */
	real_type			proximity_radius;
	/**
		Repulsive spring coefficient, global.
	 */
	real_type			repulsion_coeff;
	/**
		Object collisions are hard constraints, 
		otherwise, allow overlap and use corrective forces.  
		Default: false (soft) is easier to compute.
	 */
//	bool				hard_collisions;
	/**
		Bounds for positions.  
	 */
	position_type			lower_corner;
	position_type			upper_corner;

	// these parameters may be publicly tweaked at any time

	/// time step
	time_type			time_step;
	/// position-change tolerance for determining convergence
	real_type			pos_tol;
	/// speed-change tolerance for determining convergence
	real_type			vel_tol;
#if 0
	/// acceleration-change tolerance for determining convergence
	real_type			accel_tol;
#endif
	/**
		Precision for displaying numbers.
	 */
	int_type			precision;
	/**
		If true, print object coordinates after every iterate()
	 */
	bool				watch_objects;

	placer_options();
#if 0
	~placer_options();
#endif

	void
	save_checkpoint(ostream&) const;

	void
	load_checkpoint(istream&);

};	// end struct placer_options

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For solving placement.
	Physics, force-driven, with annealing.
 */
class placement_engine : public HAC::SIM::state_base {
public:
	vector<tile_type>		object_types;
	vector<channel_type>		channel_types;
	placer_options			opt;
	/**
		The objects and coordinates to solve in.
	 */
	pcanvas				space;
#if !PR_LOCAL_PROXIMITY_CACHE
	typedef	std::pair<size_t, size_t>	proximity_edge;
	vector<proximity_edge>		proximity_cache;
#endif
	time_type			elapsed_time;
protected:
	string				autosave_name;

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

	void
	scatter(void);

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

	bool
	parse_parameter(const string&);

	bool
	parse_parameter(const option_value&);

	static
	ostream&
	list_parameters(ostream&);

	ostream&
	dump_parameters(ostream&) const;

	void
	autosave(const string& s) { autosave_name = s; }

	void
	iterate(void);

	int_type
	solve(void);

	ostream&
	dump_object_types(ostream&) const;

	ostream&
	dump_channel_types(ostream&) const;

	ostream&
	dump_objects(ostream&) const;

	ostream&
	dump_channels(ostream&) const;

	ostream&
	dump(ostream&) const;

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
	initialize_default_types(void);

	void
	clamp_position(real_vector&) const;

	void
	zero_forces(void);

	static
	void
	apply_pairwise_force(tile_instance&, tile_instance&,
		const force_type&);

	static
	void
	apply_attraction_forces(tile_instance&, tile_instance&,
#if PR_CHANNEL_TENSION
		const channel_properties&
#else
		channel_instance&
#endif
		);

	static
	void
	apply_repulsion_forces(tile_instance&, tile_instance&,
		const channel_properties&);

	void
	compute_spring_forces(void);

	void
	refresh_proximity_cache(void);

	// incrementally update
	void
	update_proximity_cache(void);

	void
	clear_proximity_cache(void);

	void
	compute_collision_forces(void);

	void
	update_velocity_and_position(void);

};	// end class placement_engine

//=============================================================================

}	// end namespace PR

#endif	// __HAC_PR_PLACEMENT_ENGINE_H__
