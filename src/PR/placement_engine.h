/**
	\file "PR/placement_engine.h"
	Physics simulator.
	$Id: placement_engine.h,v 1.1.2.1 2011/04/11 18:38:39 fang Exp $
 */

#ifndef	__HAC_PR_PLACEMENT_ENGINE_H__
#define	__HAC_PR_PLACEMENT_ENGINE_H__

#include "PR/pcanvas.h"
#include "util/named_ifstream_manager.h"	// needed by interpreter

namespace PR {
using util::ifstream_manager;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For solving placement.
	Physics, force-driven, with annealing.
 */
class placement_engine {
	/**
		High temperature: greater magnitude of random velocity vector
		added to each iteration.
	 */
	real_type			temperature;
	/**
		Dampening factor (positive).
	 */
	real_type			friction;
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
protected:
	/**
		Only needed to satisfy SIM::state_base interface.
		Interpreter state for the input stream.
		This is not checkpointed.  
	 */
	ifstream_manager				ifstreams;

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

	ifstream_manager&
	get_stream_manager(void) { return ifstreams; }

	void
	auto_proximity_radius(void);

	void
	iterate(void);

	int_type
	solve(void);

};	// end class placement_engine

//=============================================================================

}	// end namespace PR

#endif	// __HAC_PR_PLACEMENT_ENGINE_H__
