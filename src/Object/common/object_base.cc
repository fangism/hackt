/**
	\file "Object/common/object_base.cc"
	Method definitions for base classes for semantic objects.  
	This file used to be "Object/common/object_base.cc"
		in a previous life.  
 	$Id: object_base.cc,v 1.2 2005/07/23 06:52:23 fang Exp $
 */

#ifndef	__OBJECT_COMMON_OBJECT_BASE_CC__
#define	__OBJECT_COMMON_OBJECT_BASE_CC__

#include <iostream>
#include "Object/common/object_base.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class object method definitions

void
object::bogus(void) const { }

//=============================================================================
// class object_handle method definitions

/**
	Constructs an object handle, wrapping around a non-owned
	pointer to the canonical object.  
	\param o pointer to object to be wrapped, 
		which may not be another handle.  
 */
object_handle::object_handle(const never_ptr<const object> o) :
		object(), obj(*o) {
	INVARIANT(!o.is_a<const object_handle>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
object_handle::what(ostream& o) const {
	return obj.what(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
object_handle::dump(ostream& o) const {
	return obj.dump(o << "(handle) ");
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_COMMON_OBJECT_BASE_CC__

