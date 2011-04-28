/**
	\file "PR/placer_options.h"
	Physics simulator.
	$Id: placer_options.h,v 1.1.2.7 2011/04/28 21:44:22 fang Exp $
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
#if 0
	/**
		Threshold of acceleration to move.
	 */
	real_type			static_friction;
	/**
		Constant negative force against direction of motion.
	 */
	real_type			kinetic_friction;
#endif
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
		This is linear with stretch distance.
	 */
	real_type			repulsion_coeff;
	/**
		Constant repulsion force, additive on top of linear term.
	 */
	real_type			repulsion_constant;
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

	real_type			x_gravity_coeff;
	real_type			y_gravity_coeff;
	real_type			z_gravity_coeff;
	real_type			x_gravity_constant;
	real_type			y_gravity_constant;
	real_type			z_gravity_constant;

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
	real_type			energy_tol;
	size_t				min_iterations;
	/**
		Precision for displaying numbers.
	 */
	int_type			precision;
	/**
		If true, print object coordinates after every iterate()
	 */
	bool				watch_objects;
	bool				watch_deltas;
	bool				watch_energy;
	bool				report_iterations;

	placer_options();
#if 0
	~placer_options();
#endif

	void
	clamp_position(real_vector&) const;

	bool
	watch_anything(void) const {
		return watch_objects || watch_deltas || watch_energy;
	}

	static
	ostream&
	list_parameters(ostream&);

	ostream&
	dump_parameters(ostream&) const;

	ostream&
	emit_dot_bb(ostream&) const;

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
