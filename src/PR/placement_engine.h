/**
	\file "PR/placement_engine.h"
	Physics simulator.
	$Id: placement_engine.h,v 1.1.2.3 2011/04/15 00:52:02 fang Exp $
 */

#ifndef	__HAC_PR_PLACEMENT_ENGINE_H__
#define	__HAC_PR_PLACEMENT_ENGINE_H__

#include <string>
#include "PR/pcanvas.h"
#include "util/named_ifstream_manager.h"	// needed by interpreter

namespace PR {
using std::istream;
using std::string;
using util::ifstream_manager;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For solving placement.
	Physics, force-driven, with annealing.
 */
class placement_engine {
public:
	vector<tile_type>		object_types;
	vector<channel_type>		channel_types;

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
		Object collisions are hard constraints, 
		otherwise, allow overlap and use corrective forces.  
		Default: false (soft) is easier to compute.
	 */
//	bool				hard_collisions;
	/**
		The objects and coordinates to solve in.
	 */
	pcanvas				space;
	/**
		Bounds for positions.  
	 */
	position_type			lower_bound;
	position_type			upper_bound;
protected:
	/**
		Only needed to satisfy SIM::state_base interface.
		Interpreter state for the input stream.
		This is not checkpointed.  
	 */
	ifstream_manager		ifstreams;

public:
	// these parameters may be publicly tweaked at any time

	/// time step
	time_type			time_step;
	/// position-change tolerance for determining convergence
	real_type			pos_tol;
	/// speed-change tolerance for determining convergence
	real_type			vel_tol;
	/// acceleration-change tolerance for determining convergence
	real_type			accel_tol;
public:

	explicit
	placement_engine(const size_t d);

	~placement_engine();

// begin stuff for command_registry
	ifstream_manager&
	get_stream_manager(void) { return ifstreams; }

	void
	add_source_path(const string& s) {
		ifstreams.add_path(s);
	}
// end stuff for command_registry

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

};	// end class placement_engine

//=============================================================================

}	// end namespace PR

#endif	// __HAC_PR_PLACEMENT_ENGINE_H__
