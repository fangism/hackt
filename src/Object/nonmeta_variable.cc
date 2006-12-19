/**
	\file "Object/nonmeta_variable.cc"
	$Id: nonmeta_variable.cc,v 1.1.2.1 2006/12/19 23:44:04 fang Exp $
 */

#include "Object/nonmeta_variable.h"

namespace HAC {
namespace entity {
//=============================================================================
// class nonmeta_variable_base method definitions

nonmeta_variable_base::nonmeta_variable_base() : event_subscribers() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_variable_base::~nonmeta_variable_base() { }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

