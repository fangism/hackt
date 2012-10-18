/**
	\file "Object/type/canonical_type_fwd.hh"
	Forward declarations of canonical_type type references.  
	$Id: canonical_type_fwd.hh,v 1.7 2007/01/21 05:59:50 fang Exp $
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

class canonical_fundamental_chan_type_base;
class canonical_fundamental_chan_type;

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

/**
	Comparison operator needed for set sorting of
	built-in channel types.  
 */
bool
operator < (const canonical_generic_datatype&, 
		const canonical_generic_datatype&);

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
template <>
struct canonical_type_footprint_frame_policy<user_def_datatype>;
// template <>
// struct canonical_type_footprint_frame_policy<channel_definition_base>;

//-----------------------------------------------------------------------------
template <class>
struct check_footprint_policy;

template <>
struct check_footprint_policy<process_definition>;
template <>
struct check_footprint_policy<user_def_datatype>;
// template <>
// struct check_footprint_policy<channel_definition_base>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_CANONICAL_TYPE_FWD_H__

