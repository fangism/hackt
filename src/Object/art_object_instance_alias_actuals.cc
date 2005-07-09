/**
	\file "Object/art_object_instance_alias_actuals.cc"
	Method definitions of class instance_alias_info_actuals.
	$Id: art_object_instance_alias_actuals.cc,v 1.1.2.3 2005/07/09 23:13:16 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "Object/art_object_instance_alias_actuals.h"
#include "Object/art_object_instance_alias_empty.h"
#include <iostream>
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class instance_alias_info_empty method definitions

#if DEBUG_ALIAS_EMPTY
bool
instance_alias_info_empty::attach_actuals(const alias_actuals_type& a) const {
	return true;
}
#endif

const instance_alias_info_empty::alias_actuals_type
instance_alias_info_empty::null;

//=============================================================================
// class instance_alias_info_actuals method definitions

#if DEBUG_ALIAS_ACTUALS
bool
instance_alias_info_actuals::attach_actuals(const alias_actuals_type& a) const {
	if (actuals) {
		// already have!
		return false;
	} else {
		NEVER_NULL(a);
		actuals = a;
		return true;
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_alias_info_actuals::dump_actuals(ostream& o) const {
	return (actuals ? actuals->dump(o << '<') << '>' : o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Comparing relaxed actuals for the sake of connection checking.  
 */
good_bool
instance_alias_info_actuals::compare_and_update_actuals(
		alias_actuals_type& l, const alias_actuals_type& r) {
	if (r) {
	if (l) {
		// then compare them
		if (!l->must_be_equivalent(*r)) {
			cerr << "ERROR: attempted to connect instances with "
				"conflicting relaxed parameters!" << endl;
			// TODO: report where, more info!
			l->dump(cerr << "\tgot: ") << endl;
			r->dump(cerr << "\tand: ") << endl;
			// for now stop on 1st error
			return good_bool(false);
		}
		// else good to connect because l is relaxed
	} else {
		// then set them for the rest of this connection loop.
		l = r;
	}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_alias_info_actuals::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_VERBOSE;
	if (actuals)
		actuals->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_alias_info_actuals::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_VERBOSE;
	m.write_pointer(o, actuals);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_alias_info_actuals::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_VERBOSE;
	m.read_pointer(i, actuals);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

