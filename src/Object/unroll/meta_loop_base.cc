/**
	\file "Object/unroll/meta_loop_base.cc"
	$Id: meta_loop_base.cc,v 1.2.10.1 2005/12/11 00:45:58 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_META_LOOP_BASE_CC__
#define	__OBJECT_UNROLL_META_LOOP_BASE_CC__

#define	ENABLE_STACKTRACE		0

#include "Object/unroll/meta_loop_base.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/inst/pint_value_collection.h"

// #include "common/TODO.h"
#include "util/persistent_object_manager.tcc"
// #include "util/IO_utils.h"
#include "util/memory/count_ptr.tcc"
// #include "util/stacktrace.h"

//=============================================================================
namespace HAC {
namespace entity {

//=============================================================================
// class meta_loop_base method definitions

meta_loop_base::meta_loop_base() : ind_var(), range() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_loop_base::meta_loop_base(const ind_var_ptr_type& i, 
		const range_ptr_type& r) :
		ind_var(i), range(r) {
	NEVER_NULL(ind_var);
	NEVER_NULL(range);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_loop_base::~meta_loop_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_loop_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	NEVER_NULL(ind_var);
	NEVER_NULL(range);
	ind_var->collect_transient_info(m);
	range->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_loop_base::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, ind_var);
	m.write_pointer(o, range);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_loop_base::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, ind_var);
	m.read_pointer(i, range);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_UNROLL_META_LOOP_BASE_CC__

