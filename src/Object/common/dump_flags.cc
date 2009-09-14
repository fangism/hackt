/**
	\file "Object/common/dump_flags.cc"
	Global static initializer for default.  
	$Id: dump_flags.cc,v 1.6 2009/09/14 21:16:47 fang Exp $
 */

#include "Object/common/dump_flags.h"

namespace HAC {
namespace entity {
//=============================================================================
// class dump_flags method and static member definitions

const dump_flags
dump_flags::default_value = dump_flags();

const dump_flags
dump_flags::no_definition_owner = dump_flags(false, true, true);

const dump_flags
dump_flags::no_owners = dump_flags(false, false, false);

/**
	This is appropriate for cflat use.  
 */
const dump_flags
dump_flags::no_leading_scope = dump_flags(false, false, false);

/**
	Verbose dumping flags for diagnostics.  
 */
const dump_flags
dump_flags::verbose = dump_flags(true, true, true);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dump_flags::dump_flags() :
		show_definition_owner(true),
		show_namespace_owner(true),
		show_leading_scope(true),
		process_member_separator("."),
		struct_member_separator(".")
{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dump_flags::dump_flags(const bool _show_def_owner, 
		const bool _show_ns_owner, 
		const bool _show_leading_scope) :
		show_definition_owner(_show_def_owner),
		show_namespace_owner(_show_ns_owner),
		show_leading_scope(_show_leading_scope),
		process_member_separator("."),
		struct_member_separator(".")
{ }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

