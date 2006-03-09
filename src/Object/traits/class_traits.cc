/**
	\file "Object/traits/class_traits.cc"
	Traits and policy classes for instances.  
	This file used to be "Object/art_object_classification_details.cc".
	$Id: class_traits.cc,v 1.6.8.1 2006/03/09 05:51:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_CLASS_TRAITS_CC__
#define	__HAC_OBJECT_TRAITS_CLASS_TRAITS_CC__

#include "Object/traits/chan_traits.h"
#include "Object/traits/data_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/traits/param_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/preal_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/unroll/unroll_context.h"
#include "Object/unroll/empty_instantiation_statement_type_ref_base.h"

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

//=============================================================================
/**
	What's this doing here?
 */
unroll_context
null_parameter_type::make_unroll_context(void) const {
	return unroll_context();
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_CLASS_TRAITS_CC__

