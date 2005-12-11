/**
	\file "Object/unroll/meta_conditional_base.cc"
	Base for guarded bodies in meta language.  
	$Id: meta_conditional_base.cc,v 1.2.10.1 2005/12/11 00:45:57 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_META_CONDITIONAL_BASE_CC__
#define	__OBJECT_UNROLL_META_CONDITIONAL_BASE_CC__

#define	ENABLE_STACKTRACE		0

#include "Object/unroll/meta_conditional_base.h"
#include "Object/expr/pbool_expr.h"

// #include "common/TODO.h"
#include "util/persistent_object_manager.tcc"
// #include "util/IO_utils.h"
#include "util/memory/count_ptr.tcc"
// #include "util/stacktrace.h"

//=============================================================================
namespace HAC {
namespace entity {

//=============================================================================
// class meta_conditional_base method definitions

meta_conditional_base::meta_conditional_base() : guard() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_conditional_base::meta_conditional_base(const guard_ptr_type& g) :
		guard(g) {
	NEVER_NULL(guard);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_conditional_base::~meta_conditional_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_conditional_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	NEVER_NULL(guard);
	guard->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_conditional_base::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, guard);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_conditional_base::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, guard);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_UNROLL_META_CONDITIONAL_BASE_CC__

