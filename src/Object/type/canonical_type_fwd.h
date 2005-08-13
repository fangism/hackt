/**
	\file "Object/type/canonical_type_fwd.h"
	Forward declarations of canonical_type type references.  
	$Id: canonical_type_fwd.h,v 1.1.2.1.2.1 2005/08/13 17:32:02 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CANONICAL_TYPE_FWD_H__
#define	__OBJECT_TYPE_CANONICAL_TYPE_FWD_H__

namespace ART {
namespace entity {

#define	USE_CANONICAL_TYPE			1

class datatype_definition_base;
class user_def_datatype;
class channel_definition_base;
class user_def_chan;
class process_definition;

class canonical_type_base;

#if 0
template <class>
class canonical_type_parent;
#endif

template <class>
class canonical_type;

typedef	canonical_type<datatype_definition_base>
						canonical_generic_datatype;
typedef	canonical_type<user_def_datatype>	canonical_user_def_data_type;
typedef	canonical_type<channel_definition_base>	canonical_generic_chan_type;
typedef	canonical_type<user_def_chan>		canonical_user_def_chan_type;
// built-in channel type?
typedef	canonical_type<process_definition>	canonical_process_type;

}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TYPE_CANONICAL_TYPE_FWD_H__

