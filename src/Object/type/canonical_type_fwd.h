/**
	\file "Object/type/canonical_type_fwd.h"
	Forward declarations of canonical_type type references.  
	$Id: canonical_type_fwd.h,v 1.5 2006/01/22 18:20:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_CANONICAL_TYPE_FWD_H__
#define	__HAC_OBJECT_TYPE_CANONICAL_TYPE_FWD_H__

namespace HAC {
namespace entity {
//=============================================================================

class datatype_definition_base;
class user_def_datatype;
class channel_definition_base;
class user_def_chan;
class process_definition;

class canonical_type_base;

template <class>
class canonical_type;

// HACK ALERT:
// since built-in channels don't exactly conform to the normal
// meta-type, we need an exception to accommodate them.  To accomplish this, 
// we forward declare a specialization
template <>
class canonical_type<channel_definition_base>;

typedef	canonical_type<datatype_definition_base>
						canonical_generic_datatype;
typedef	canonical_type<user_def_datatype>	canonical_user_def_data_type;
typedef	canonical_type<channel_definition_base>	canonical_generic_chan_type;
typedef	canonical_type<user_def_chan>		canonical_user_def_chan_type;
// built-in channel type?
typedef	canonical_type<process_definition>	canonical_process_type;

//=============================================================================
// HACK alert until we take the time to redo built-in types correctly
template <class>
struct canonical_definition_load_policy;

// forward declaration of specialization to prevent mis-typing
template <>
struct canonical_definition_load_policy<datatype_definition_base>;

//-----------------------------------------------------------------------------
// define by specializations only
template <class>
struct unroll_port_instances_policy;

//-----------------------------------------------------------------------------
template <class>
struct canonical_type_footprint_frame_policy;

template <>
struct canonical_type_footprint_frame_policy<process_definition>;

//-----------------------------------------------------------------------------
template <class>
struct check_footprint_policy;

template <>
struct check_footprint_policy<process_definition>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_CANONICAL_TYPE_FWD_H__

