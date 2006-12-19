/**
	\file "Object/nonmeta_context.cc"
	$Id: nonmeta_context.cc,v 1.1.2.1 2006/12/19 23:43:59 fang Exp $
 */

#include "Object/nonmeta_context.h"

namespace HAC {
namespace entity {
//=============================================================================
// class nonmeta_context method definitions

nonmeta_context::nonmeta_context(const state_manager& s, 
		nonmeta_state_manager& v, const size_t pid) :
		sm(s), values(v), process_index(pid) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_context::~nonmeta_context() { }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

