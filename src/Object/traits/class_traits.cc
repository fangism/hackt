/**
	\file "Object/traits/class_traits.cc"
	Traits and policy classes for instances.  
	This file used to be "Object/art_object_classification_details.cc".
	$Id: class_traits.cc,v 1.13 2011/04/02 01:46:07 fang Exp $
 */

#include "Object/traits/instance_traits.hh"
#include "Object/traits/data_traits.hh"
#include "Object/traits/param_traits.hh"
#include "Object/traits/value_traits.hh"
#include "Object/traits/node_traits.hh"
#include "Object/unroll/unroll_context.hh"
#include "Object/unroll/empty_instantiation_statement_type_ref_base.hh"

namespace HAC {
namespace entity {
//=============================================================================
const char
class_traits<int_tag>::tag_name[] = "int";

const char
class_traits<bool_tag>::tag_name[] = "bool";

const char
class_traits<enum_tag>::tag_name[] = "enum";

const char
class_traits<real_tag>::tag_name[] = "real";

const char
class_traits<string_tag>::tag_name[] = "string";

const char
class_traits<datastruct_tag>::tag_name[] = "struct";

const char
class_traits<process_tag>::tag_name[] = "process";

const char
class_traits<channel_tag>::tag_name[] = "channel";

const char
class_traits<pint_tag>::tag_name[] = "pint";

const char
class_traits<pint_tag>::value_type_name[] = "integer";

const char
class_traits<pbool_tag>::tag_name[] = "pbool";

const char
class_traits<pbool_tag>::value_type_name[] = "boolean";

const char
class_traits<preal_tag>::tag_name[] = "preal";

const char
class_traits<preal_tag>::value_type_name[] = "real-value";

const char
class_traits<pstring_tag>::tag_name[] = "pstring";

const char
class_traits<pstring_tag>::value_type_name[] = "string-value";

const char
class_traits<node_tag>::tag_name[] = "node";


//=============================================================================
#if 0
/**
	What's this doing here?
 */
unroll_context
null_parameter_type::make_unroll_context(void) const {
	return unroll_context(NULL, NULL);
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

