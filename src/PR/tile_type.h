/**
	\file "PR/tile_type.h"
	Object types.
	$Id: tile_type.h,v 1.2 2011/05/03 19:20:51 fang Exp $
 */

#ifndef	__HAC_PR_TILE_TYPE_H__
#define	__HAC_PR_TILE_TYPE_H__

#include <iosfwd>
// #include <vector>
#include <string>
#include "PR/numerics.h"
// #include "PR/channel.h"

/**
	Define to 1 to enable pin information on tiles.
	Number of pins not relevant until we are doing routing.
 */
#define	PR_TILE_PINS			0

/**
	Define to 1 to enable non-uniform mass.
	Otherwise treat all tile objects as unit mass (1.0).  
	Rationale for disabling: mass does not affect equilibrium, 
		only transient behavior.
 */
#define	PR_TILE_MASS			0

namespace PR {
// using std::vector;
using std::string;
using std::ostream;
using std::istream;

//=============================================================================
/**
	Physical properties of tile type.
	Objects are modeled as point masses.
	Dimensions are integer because they will eventually be grid-aligned.
 */
struct tile_properties {
	/// dimension in number of 'tracks'
	position_type			size;		// height, width, depth
#if PR_TILE_MASS
	/**
		Weight is for momentum/acceleration calculations.  
	 */
	real_type			mass;
#endif
	/**
		Spring constant for repelling near-collision neighbors.  
	 */
//	real_type			repulsion_constant;
	/**
		Bounce factor for collisions.
	 */
//	real_type			coeff_restitution;
//	bool				rotatable;
// pin_info?

	tile_properties();

	~tile_properties();

	real_type
	maximum_dimension(void) const;

	bool
	parse_property(const string&);

	bool
	parse_property(const option_value&);

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
	emit_dot(ostream&, const real_type&) const;

};	// end struct tile_properties

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PR_TILE_PINS
#if 1
struct pin_location {
	int_type			x;
	int_type			y;
	int_type			metal_layer;
};	// end struct pin
#else
typedef	position_type			pin_location;	// is real_type
#endif

struct tile_pin {
	/// port index
	int_type			index;
	pin_location			loc;
};
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Object type information.
 */
struct tile_type {
	tile_properties			properties;
#if PR_TILE_PINS
	vector<tile_pin>		pins;
#endif
	// local blockage, routing information
};	// end class tile_type
#else
typedef	tile_properties			tile_type;
#endif

//=============================================================================
}	// end namespace PR

#endif	// __HAC_PR_TILE_TYPE_H__
