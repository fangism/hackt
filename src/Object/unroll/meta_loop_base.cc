/**
	\file "Object/unroll/meta_loop_base.cc"
	$Id: meta_loop_base.cc,v 1.4.50.1 2006/09/02 03:58:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_LOOP_BASE_CC__
#define	__HAC_OBJECT_UNROLL_META_LOOP_BASE_CC__

#define	ENABLE_STACKTRACE		0

#include "Object/unroll/meta_loop_base.h"
#include "Object/expr/meta_range_expr.h"
#if USE_INSTANCE_PLACEHOLDERS
#include "Object/inst/value_placeholder.h"
#include "Object/traits/pint_traits.h"
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/unroll/param_instantiation_statement.h"
#include "Object/expr/param_expr_list.h"
#include "Object/expr/meta_range_list.h"
#include "Object/def/footprint.h"
#include "Object/unroll/unroll_context.h"
#endif
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
#if USE_INSTANCE_PLACEHOLDERS
/**
	Populates a footprint with an actual instance of the induction
	variable and returns a pointer to the scalar variable 
	for the caller to manipulate in a loop. 
 */
count_ptr<pint_scalar>
meta_loop_base::initialize_footprint(footprint& f) const {
	const pint_instantiation_statement
		pis(pint_traits::built_in_type_ptr, 
			index_collection_item_ptr_type());
	// fake a context, no additional information necessary to instantiate
	const unroll_context temp(&f);
	const good_bool g(pis.unroll(temp));
	INVARIANT(g.good);
	const count_ptr<pint_scalar>
		ret(f[ind_var->get_name()].is_a<pint_scalar>());
	NEVER_NULL(ret);
	// other back-linking collection to placeholder? for diagnostics?
	return ret;
}
#endif

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

#endif	// __HAC_OBJECT_UNROLL_META_LOOP_BASE_CC__

