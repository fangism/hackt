/**
	\file "PR/pcanvas.h"
	Medium on which placement is solved with physics.
	$Id: pcanvas.h,v 1.1.2.4 2011/04/16 01:51:53 fang Exp $
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
#if PR_MULTINETS
	vector<net_instance>		nets;
#endif

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
