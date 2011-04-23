/**
	\file "PR/pcanvas.h"
	Medium on which placement is solved with physics.
	$Id: pcanvas.h,v 1.1.2.8 2011/04/23 22:56:42 fang Exp $
 */

#ifndef	__HAC_PR_PCANVAS_H__
#define	__HAC_PR_PCANVAS_H__

#include "PR/channel.h"
#include "PR/tile_instance.h"

/**
	Define to 1 to enable multi-terminal net physics.
	Don't really need this yet.
 */
#define	PR_MULTINETS				0

namespace PR {
struct placer_options;

/**
	For placement solving.  
	Coordinates are real-valued.
 */
struct pcanvas {
#if PR_VARIABLE_DIMENSIONS
	/**
		Yes, we can do physics in higher dimensions!
	 */
	const size_t			dimensions;
#endif
	vector<tile_instance>		objects;
	vector<channel_instance>	springs;
#if PR_MULTINETS
	vector<net_instance>		nets;
#endif
#if 0
	// implement with separate state array for efficient
	// state swapping (vector)
	vector<object_state>		coordinates;
#endif
private:
	/**
		The maximum distance changed by any single object
		in the previous iteration.
	 */
	real_type			_max_delta_position;
	/**
		The maximum velocity changed by any single object
		in the previous iteration.
	 */
	real_type			_max_delta_velocity;
	// energy values
	real_type			object_kinetic_energy;
	real_type			spring_potential_energy;

public:

	explicit
	pcanvas(const size_t d);

	~pcanvas();

	real_type
	auto_proximity_radius(void) const;

	const real_type&
	kinetic_energy(void) const {
		return object_kinetic_energy;
	}

	const real_type&
	potential_energy(void) const {
		return spring_potential_energy;
	}

	const real_type&
	update_potential_energy(void);

	void
	kill_momentum(void);

	void
	compute_spring_forces(void);

	void
	update_objects(const placer_options&);

	ostream&
	dump(ostream&) const;

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

	static
	ostream&
	dump_checkpoint(ostream&, istream&);

};	// end class pcanvas

//=============================================================================

}	// end namespace PR

#endif	// __HAC_PR_PCANVAS_H__
