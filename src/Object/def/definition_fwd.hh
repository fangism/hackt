/**
	\file "Object/def/definition_fwd.hh"
	Forward declarations for all definition-related classes.  
	$Id: definition_fwd.hh,v 1.2 2006/04/12 08:53:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_DEFINITION_FWD_H__
#define	__HAC_OBJECT_DEF_DEFINITION_FWD_H__

namespace HAC {
namespace entity {

class definition_base;
class built_in_datatype_def;
class instance_collection_base;
class physical_instance_collection;
class user_def_chan;
class user_def_datatype;
class channel_definition_base;
class datatype_definition_base;
class enum_datatype_def;
class process_definition_base;
class process_definition;
class built_in_param_def;
class built_in_channel_def;
class typedef_base;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_DEFINITION_FWD_H__

