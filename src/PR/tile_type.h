/**
	\file "PR/tile_type.h"
	$Id: tile_type.h,v 1.1.2.1 2011/04/11 18:38:43 fang Exp $
 */

#ifndef	__HAC_PR_TILE_TYPE_H__
#define	__HAC_PR_TILE_TYPE_H__

#include <vector>
#include "PR/channel.h"
#include "util/vector_ops.h"

namespace PR {

//=============================================================================
/**
	Physical properties of tile type.
	Dimensions are integer because they will eventually be grid-aligned.
 */
struct tile_properties {
	/// dimension in number of 'tracks'
	position_type			size;		// height, width, depth
	/**
		Weight is for momentum/acceleration calculations.  
	 */
	real_type			default_mass;
	/**
		Bounce factor for collisions.
	 */
//	real_type			coeff_restitution;
//	bool				rotatable;

	real_type
	maximum_dimension(void) const {
		return util::vector_ops::max(size);
	}

};	// end struct tile_properties

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct tile_type {
	tile_properties			properties;
	vector<tile_pin>		pins;
	// local blockage, routing information
};	// end class tile_type

//=============================================================================
}	// end namespace PR

#endif	// __HAC_PR_TILE_TYPE_H__
