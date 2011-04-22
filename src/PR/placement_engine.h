/**
	\file "PR/placement_engine.h"
	Physics simulator.
	$Id: placement_engine.h,v 1.1.2.9 2011/04/22 01:28:22 fang Exp $
 */

#ifndef	__HAC_PR_PLACEMENT_ENGINE_H__
#define	__HAC_PR_PLACEMENT_ENGINE_H__

#include <string>
#include "PR/pcanvas.h"
#include "PR/placer_options.h"
#include "sim/state_base.h"
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
//	typedef	std::pair<size_t, size_t>	proximity_edge;
	typedef	channel_instance		proximity_edge;
	/**
		Proximity cache is just a collection of
		repulsion springs that are generated on-the-fly.
	 */
	vector<proximity_edge>		proximity_cache;
#endif
	time_type			elapsed_time;
protected:
	// run-time updates values
	real_type			proximity_potential_energy;

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
	parse_parameter(const string& s) {
		return opt.parse_parameter(s);
	}

	bool
	parse_parameter(const option_value& o) {
		return opt.parse_parameter(o);
	}

	ostream&
	dump_parameters(ostream&) const;

	void
	autosave(const string& s) { autosave_name = s; }

// simulation
	void
	kill_momentum(void) {
		space.kill_momentum();
	}

	void
	iterate(void);

	void
	gradient_search(void);

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
		return space.potential_energy() +proximity_potential_energy;
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
	zero_forces(void);

	void
	refresh_proximity_cache(void);

	// incrementally update
	void
	update_proximity_cache(void);

	void
	clear_proximity_cache(void);

	void
	compute_collision_forces(void);

};	// end class placement_engine

//=============================================================================

}	// end namespace PR

#endif	// __HAC_PR_PLACEMENT_ENGINE_H__
