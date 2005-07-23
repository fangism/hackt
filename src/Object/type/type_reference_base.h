/**
	\file "Object/type/type_reference_base.h"
	Base classes for type objects.  
	This file came from "Object/art_object_type_ref_base.h".
	$Id: type_reference_base.h,v 1.2 2005/07/23 06:52:55 fang Exp $
 */

#ifndef	__OBJECT_TYPE_TYPE_REFERENCE_BASE_H__
#define	__OBJECT_TYPE_TYPE_REFERENCE_BASE_H__

#include "util/persistent.h"		// for persistent object interface

namespace ART {
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
}	// end namespace ART

#endif	// __OBJECT_TYPE_TYPE_REFERENCE_BASE_H__

