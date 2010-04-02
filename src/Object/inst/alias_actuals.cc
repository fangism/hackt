/**
	\file "Object/inst/alias_actuals.cc"
	Method definitions of class instance_alias_info_actuals.
	This file was originally "Object/art_object_instance_alias_actuals.cc"
		in a previous life.  
	$Id: alias_actuals.cc,v 1.9 2010/04/02 22:18:17 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/expr/expr_dump_context.h"
#include <iostream>
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"

namespace HAC {
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
instance_alias_info_actuals::attach_actuals(const alias_actuals_type& a) {
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
#if 0
/**
	\inline?
 */
void
instance_alias_info_actuals::copy_actuals(const this_type& t) {
	actuals = t.actuals;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_alias_info_actuals::dump_actuals(ostream& o) const {
	return (actuals ? actuals->dump(o << '<',
		expr_dump_context::default_value) << '>' : o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Symmetric version of the above, will copy one set of actuals
	over to the other.  
 */
good_bool
instance_alias_info_actuals::symmetric_compare_and_update_actuals(
		alias_actuals_type& l, alias_actuals_type& r) {
	STACKTRACE_VERBOSE;
	if (l && r) {
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
	} else if (l) {
		// then set them for the rest of this connection loop.
		r = l;
	} else if (r) {
		l = r;
	}
	// else both NULL, do nothing
	return good_bool(true);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Comparing relaxed actuals for the sake of connection checking.  
 */
good_bool
instance_alias_info_actuals::compare_actuals(
		const alias_actuals_type& l, const alias_actuals_type& r) {
	if (l && r && !l->must_be_equivalent(*r)) {
		cerr << "ERROR: attempted to connect instances with "
			"conflicting relaxed parameters!" << endl;
		// TODO: report where, more info!
		l->dump(cerr << "\tgot: ", expr_dump_context::default_value)
			<< endl;
		r->dump(cerr << "\tand: ", expr_dump_context::default_value)
			<< endl;
		// for now stop on 1st error
		return good_bool(false);
	} else {
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_alias_info_actuals::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	if (actuals)
		actuals->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_alias_info_actuals::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.write_pointer(o, actuals);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instance_alias_info_actuals::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.read_pointer(i, actuals);
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	// relaxed actuals are needed during footprint reconstruction
	if (actuals)
		m.load_object_once(actuals);
	// Q: does this loading need to be deeply recursive?
	// reminder: is a const_param_expr_list
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

