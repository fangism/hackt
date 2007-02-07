/**
	\file "Object/expr/loop_nonmeta_expr.tcc"
	$Id: loop_nonmeta_expr.tcc,v 1.1.2.1 2007/02/07 22:44:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_LOOP_NONMETA_EXPR_TCC__
#define	__HAC_OBJECT_EXPR_LOOP_NONMETA_EXPR_TCC__

#include <iostream>
#include "Object/expr/loop_nonmeta_expr.h"
#include "Object/expr/const_range.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/expr_visitor.h"
#include "Object/def/footprint.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_placeholder.h"
#include "Object/inst/value_scalar.h"
#include "Object/inst/pint_instance.h"
#include "Object/traits/pint_traits.h"
#include "Object/unroll/unroll_context.h"
#include "Object/type/canonical_generic_datatype.h"
#include "Object/common/dump_flags.h"
#include "common/ICE.h"
#include "util/persistent_object_manager.h"
#include "util/what.h"
#include "util/qmap.h"
#include "util/IO_utils.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::persistent_traits;
using util::write_value;
using util::read_value;

//=============================================================================
// class loop_nonmeta_expr method definitions

/**
	Private empty constructor, used in deserialization reconstruction.  
 */
template <class E>
loop_nonmeta_expr<E>::loop_nonmeta_expr() :
		meta_loop_base(), parent_type(), 
		ex(), op() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
loop_nonmeta_expr<E>::loop_nonmeta_expr(
		const ind_var_ptr_type& i, const range_ptr_type& r, 
		const operand_ptr_type& e, const op_type* o) :
		meta_loop_base(i, r), parent_type(), 
		ex(e), op(o) {
	NEVER_NULL(op);
	NEVER_NULL(ex);
	INVARIANT(ex->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
loop_nonmeta_expr<E>::~loop_nonmeta_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
ostream&
loop_nonmeta_expr<E>::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
ostream&
loop_nonmeta_expr<E>::dump(ostream& o, const expr_dump_context& c) const {
	NEVER_NULL(this->ind_var);
	NEVER_NULL(this->range);
	o << '(' << op_key_type(binary_expr_type::reverse_op_map[this->op])
		<< ':' << this->ind_var->get_name() << ':';
	this->range->dump(o, c) << ": ";
	return this->ex->dump(o, c) << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
count_ptr<const data_type_reference>
loop_nonmeta_expr<E>::get_unresolved_data_type_ref(void) const {
	return ex->get_unresolved_data_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Can/should this use a stricter must-equivalence check?
 */
template <class E>
canonical_generic_datatype
loop_nonmeta_expr<E>::get_resolved_data_type_ref(
		const unroll_context& c) const {
	return ex->get_resolved_data_type_ref(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
void
loop_nonmeta_expr<E>::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This converts a nonmeta loop expression into its corresponding
	expanded tree.  
 */
template <class E>
count_ptr<const typename loop_nonmeta_expr<E>::expr_base_type>
loop_nonmeta_expr<E>::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const expr_base_type>& _p) const {
	typedef count_ptr<const expr_base_type>		return_type;
	static const expr_dump_context& dc(expr_dump_context::default_value);
	INVARIANT(_p == this);
	const_range cr;
	if (!this->range->unroll_resolve_range(c, cr).good) {
		cerr << "Error resolving range expression: ";
		range->dump(cerr, dc) << endl;
		return return_type(NULL);
	}
	const pint_value_type min = cr.lower();
	const pint_value_type max = cr.upper();
	if (min > max) {
		cerr << "Error: loop expression cannot operate on empty range."
			<< endl;
		return return_type(NULL);
	}

	footprint f;
	const never_ptr<pint_scalar> var(this->initialize_footprint(f));
	pint_value_type& p(var->get_instance().value);
	const unroll_context cc(&f, c);

	p = min;
	// aggregate expression tree here
	operand_ptr_type agg(ex->unroll_resolve_copy(cc, ex));
	if (!agg) {
		cerr << "Error resolving operand in loop expression for ";
		this->ind_var->dump_qualified_name(cerr, dump_flags::verbose)
			<< " = " << p << endl;
		return return_type(NULL);
	}
	for (++p; p <= max; ++p) {
		const operand_ptr_type rx(ex->unroll_resolve_copy(cc, ex));
		if (!rx) {
			cerr << "Error resolving operand in loop expression for ";
			this->ind_var->dump_qualified_name(cerr, dump_flags::verbose)
				<< " = " << p << endl;
			return return_type(NULL);
		}
		// left-associative construction
		agg = count_ptr<const binary_expr_type>(
			new binary_expr_type(agg, this->op, rx));
		NEVER_NULL(agg);
	}
	return agg;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never call this for now, count on it being expanded.
	\return resolved constant or NULL if resolution failed.  
 */
template <class E>
count_ptr<const typename loop_nonmeta_expr<E>::const_expr_type>
loop_nonmeta_expr<E>::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const expr_base_type>& p) const {
	ICE_NEVER_CALL(cerr);
	return count_ptr<const const_expr_type>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Also not supposed to be called because this should be expanded away.
 */
template <class E>
count_ptr<const const_param>
loop_nonmeta_expr<E>::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const expr_base_type>& p) const {
	return __nonmeta_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
void
loop_nonmeta_expr<E>::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	meta_loop_base::collect_transient_info_base(m);
	ex->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
void
loop_nonmeta_expr<E>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	meta_loop_base::write_object_base(m, f);
	write_value(f, op_key_type(binary_expr_type::reverse_op_map[op]));
	m.write_pointer(f, ex);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
void
loop_nonmeta_expr<E>::load_object(const persistent_object_manager& m, 
		istream& f) {
	meta_loop_base::load_object_base(m, f);
	{
	op_key_type o;
	read_value(f, o);
	op = binary_expr_type::op_map[o];
	}
	m.read_pointer(f, ex);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_LOOP_NONMETA_EXPR_TCC__

