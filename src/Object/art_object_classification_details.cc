/**
	\file "art_object_classification_details.cc"
	Traits and policy classes for instances.  
	$Id: art_object_classification_details.cc,v 1.2.14.1 2005/03/09 19:24:52 fang Exp $
 */

#ifndef	__ART_OBJECT_CLASSIFICATION_DETAILS_CC__
#define	__ART_OBJECT_CLASSIFICATION_DETAILS_CC__

#include "art_object_classification_details.h"

namespace ART {
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
class_traits<pbool_tag>::tag_name[] = "pbool";

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_CLASSIFICATION_DETAILS_CC__

