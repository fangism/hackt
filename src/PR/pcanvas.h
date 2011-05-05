/**
	\file "PR/pcanvas.h"
	Medium on which placement is solved with physics.
	$Id: pcanvas.h,v 1.3 2011/05/05 06:50:46 fang Exp $
 */

#ifndef	__HAC_PR_PCANVAS_H__
#define	__HAC_PR_PCANVAS_H__

#include "PR/channel.h"
#include "PR/tile_instance.h"

/**
	Define to 1 to enable multi-terminal net physics.
	Don't really need this yet.
#define	PR_MULTINETS				0
 */

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
	/**
		Current set of coordinates: position, velocity...
		implement with separate state array for efficient
		state swapping (vector)
		Let previous value be maintained outside.
	 */
	vector<object_state>		current;
private:
	// energy values, computed
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
	shake(const real_type&);

	void
	compute_spring_forces(void);

	delta_type
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

	ostream&
	emit_dot(ostream&, const placer_options&) const;

	ostream&
	emit_fig(ostream&, const placer_options&) const;

};	// end class pcanvas

//=============================================================================

}	// end namespace PR

#endif	// __HAC_PR_PCANVAS_H__
