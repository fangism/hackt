/**
	\file "Object/lang/PRS_literal_attribute_common.cc"
	$Id: PRS_literal_attribute_common.cc,v 1.2 2009/09/14 21:17:00 fang Exp $
 */

#include <iostream>
#include "Object/lang/PRS_literal_attribute_common.h"
#include "Object/lang/attribute_common.h"
#include "common/TODO.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
namespace PRS {
namespace literal_attributes {
#include "util/using_ostream.h"
using namespace entity::attributes;

//=============================================================================
/**
	Optional single integer (boolean) value.
 */
good_bool
LVT::__check_vals(const char* name, const values_type& v) {
        return check_optional_integer(name, v);
}

good_bool
SVT::__check_vals(const char* name, const values_type& v) {
        return check_optional_integer(name, v);
}

good_bool
HVT::__check_vals(const char* name, const values_type& v) {
        return check_optional_integer(name, v);
}

//=============================================================================
}	// end namespace literal_attributes
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

