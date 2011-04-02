/**
	\file "Object/unroll/meta_loop_base.cc"
	$Id: meta_loop_base.cc,v 1.10 2011/04/02 01:46:15 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "Object/unroll/meta_loop_base.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/inst/value_placeholder.h"
#include "Object/traits/pint_traits.h"
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/unroll/param_instantiation_statement.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/meta_range_list.h"
#include "Object/def/footprint.h"
#include "Object/unroll/unroll_context.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/inst/value_scalar.h"

// #include "common/TODO.h"
#include "util/persistent_object_manager.tcc"
// #include "util/IO_utils.h"
#include "util/memory/count_ptr.tcc"
// #include "util/stacktrace.h"

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.h"

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
/**
	Populates a footprint with an actual instance of the induction
	variable and returns a pointer to the scalar variable 
	for the caller to manipulate in a loop. 
 */
never_ptr<pint_scalar>
meta_loop_base::initialize_footprint(footprint& f) const {
	// temporary instantiation statement
	const never_ptr<const pint_value_placeholder> ivr(&*ind_var);
	const pint_instantiation_statement
		pis(ivr, pint_traits::built_in_type_ptr, 
			index_collection_item_ptr_type());
	// fake a context, no additional information necessary to instantiate
	const unroll_context temp(&f, &f);
	const good_bool g(pis.unroll(temp));
	// doesn't assign init. value
	INVARIANT(g.good);
	const string& key(ind_var->get_name());
	const never_ptr<pint_scalar>
		ret(f[key].is_a<pint_scalar>());
	NEVER_NULL(ret);
	// other back-linking collection to placeholder? for diagnostics?
	// assign it some value to make it valid initially
	ret->get_instance() = 0;
	return ret;
}

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

