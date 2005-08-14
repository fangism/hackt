/**
	\file "Object/type/canonical_type.cc"
	Explicit template instantiation of canonical type classes.  
	Probably better to include the .tcc where needed, 
	as this is just temporary and convenient.  
	$Id: canonical_type.cc,v 1.1.2.2 2005/08/14 03:38:20 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "Object/type/canonical_type.tcc"
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/channel_type_reference.h"
#include "Object/type/process_type_reference.h"

namespace ART {
namespace entity {
//=============================================================================
// specialized definitions

void
canonical_definition_load_policy<datatype_definition_base>::operator ()
		(const persistent_object_manager& m,
		never_ptr<const definition_type>& d) const {
	data_type_reference::intercept_builtin_definition_hack(m, d);
}

//=============================================================================
template class canonical_type<datatype_definition_base>;
template class canonical_type<user_def_datatype>;
#if !SPECIALIZE_CANONICAL_CHAN_TYPE
template class canonical_type<channel_definition_base>;
#endif
template class canonical_type<user_def_chan>;
template class canonical_type<process_definition>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

