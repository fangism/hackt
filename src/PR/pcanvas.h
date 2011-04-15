/**
	\file "PR/pcanvas.h"
	Medium on which placement is solved with physics.
	$Id: pcanvas.h,v 1.1.2.3 2011/04/15 00:52:02 fang Exp $
 */

#ifndef	__HAC_PR_PCANVAS_H__
#define	__HAC_PR_PCANVAS_H__

#include "PR/channel.h"
#include "PR/tile_instance.h"

namespace PR {
/**
	For placement solving.  
	Coordinates are real-valued.
 */
struct pcanvas {
	/**
		Yes, we can do physics in higher dimensions!
	 */
	const size_t			dimensions;

	vector<tile_instance>		objects;
	vector<channel_instance>	springs;
	vector<net_instance>		nets;

	// fixed ports

public:

	explicit
	pcanvas(const size_t d);

	~pcanvas();

	real_type
	auto_proximity_radius(void) const;

	real_type
	kinetic_energy(void) const;

	real_type
	potential_energy(void) const;


	ostream&
	dump(ostream&) const;

};	// end class pcanvas

//=============================================================================

}	// end namespace PR

#endif	// __HAC_PR_PCANVAS_H__
