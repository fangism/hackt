/**
	\file "Object/art_object_base.cc"
	Method definitions for base classes for semantic objects.  
 	$Id: art_object_base.cc,v 1.32.2.5 2005/05/13 21:16:40 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_BASE_CC__
#define	__OBJECT_ART_OBJECT_BASE_CC__

#include <iostream>
#include <numeric>

#include "util/STL/list.tcc"

#include "Object/art_object_base.h"
#include "Object/art_object_expr.h"		// for dynamic_param_expr_list
#include "Object/art_object_inst_ref_base.h"
#include "Object/art_object_assign.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_definition_base.h"

#include "util/boolean_types.h"

//=============================================================================

// whether or not unimplemented objects should be written/loaded to/from file. 
// #define	USE_UNDEFINED_OBJECTS		1

//=============================================================================
namespace ART {
namespace entity {
using std::accumulate;
#include "util/using_ostream.h"
using namespace util::memory;

// for function compositions
// using namespace ADS;

//=============================================================================
// general non-member function definitions

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
object_handle::object_handle(never_ptr<const object> o) :
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

#endif	// __OBJECT_ART_OBJECT_BASE_CC__

