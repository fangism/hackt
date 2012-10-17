/**
	\file "Object/type/type_reference_base.hh"
	Base classes for type objects.  
	This file came from "Object/art_object_type_ref_base.h".
	$Id: type_reference_base.hh,v 1.4 2006/01/22 18:20:46 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_TYPE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_TYPE_TYPE_REFERENCE_BASE_H__

#include "util/persistent.hh"		// for persistent object interface

namespace HAC {
namespace entity {
using util::persistent;

//=============================================================================
/**
	Ultimate base class of all type-references, don't know if this
	is actually useful...
	Consider: spawning off a canonical type-reference for fully resolved
		types (finalized) and use those in instance-collections.  
 */
class type_reference_base : public persistent {
protected:
public:
	type_reference_base() : persistent() { }
virtual	~type_reference_base() { }

};	// end class type_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_TYPE_REFERENCE_BASE_H__

