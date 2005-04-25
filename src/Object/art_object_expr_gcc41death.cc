/**
	\file "art_object_expr_gcc41death.cc"
	Class method definitions for semantic expression.  
 	$Id: art_object_expr_gcc41death.cc,v 1.1.2.1 2005/04/25 22:10:34 fang Exp $
 */

#ifndef	__ART_OBJECT_EXPR_GCC4DEAETH_CC__
#define	__ART_OBJECT_EXPR_GCC4DEAETH_CC__

#include "art_object_expr_gcc41death.h"
#define	DYNAMIC_INDEX_LIST_TYPE_KEY	"blah"

#include "persistent_object_manager_gcc41death.tcc"

//=============================================================================
namespace util {

#if 0
// needed for ICE
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::dynamic_index_list, DYNAMIC_INDEX_LIST_TYPE_KEY, 0)
#endif

}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
//=============================================================================
using util::read_value;

//=============================================================================
// class dynamic_index_list method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
dynamic_index_list::load_object(const persistent_object_manager& m, 
		istream& f) {
	count_ptr<index_expr> ip;
	m.read_pointer(f, ip);	// ICE!!!!!
}

//=============================================================================
}	// end namepace entity
}	// end namepace ART


#undef	DEBUG_LIST_VECTOR_POOL

#endif	// __ART_OBJECT_EXPR_GCC4DEAETH_CC__

