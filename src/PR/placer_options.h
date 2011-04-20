/**
	\file "PR/placer_options.h"
	Physics simulator.
	$Id: placer_options.h,v 1.1.2.1 2011/04/20 01:09:41 fang Exp $
 */

#ifndef	__HAC_PR_PLACER_OPTIONS_H__
#define	__HAC_PR_PLACER_OPTIONS_H__

#include <string>
#include "PR/numerics.h"

namespace PR {
using std::istream;
using std::ostream;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct placer_options {
	/**
		High temperature: greater magnitude of random velocity vector
		added to each iteration.
		Vary the temperature in different phases to simulate annealing.
	 */
	real_type			temperature;
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
	bool				watch_deltas;

	placer_options();
#if 0
	~placer_options();
#endif

	static
	ostream&
	list_parameters(ostream&);

	ostream&
	dump_parameters(ostream&) const;

	bool
	parse_parameter(const string&);

	bool
	parse_parameter(const option_value&);

	void
	save_checkpoint(ostream&) const;

	void
	load_checkpoint(istream&);

};	// end struct placer_options

//=============================================================================

}	// end namespace PR

#endif	// __HAC_PR_PLACER_OPTIONS_H__
