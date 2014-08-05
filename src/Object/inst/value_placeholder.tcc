/**
	\file "Object/inst/value_placeholder.tcc"
	Method definitions for parameter instance collection classes.
	This file was "Object/art_object_value_placeholder.tcc"
		in a previous life.  
 	$Id: value_placeholder.tcc,v 1.5 2006/11/07 06:35:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_VALUE_PLACEHOLDER_TCC__
#define	__HAC_OBJECT_INST_VALUE_PLACEHOLDER_TCC__

#ifndef	DEBUG_LIST_VECTOR_POOL
#define	DEBUG_LIST_VECTOR_POOL				0
#endif

#ifndef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0 && DEBUG_LIST_VECTOR_POOL
#endif

#ifndef ENABLE_STACKTRACE
#define ENABLE_STACKTRACE				0
#endif

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/common/extern_templates.hh"
// #define EXTERN_TEMPLATE_UTIL_PACKED_ARRAY

#include "Object/inst/value_placeholder.hh"
#include "Object/inst/value_collection.hh"	// for constructors
#include "Object/expr/const_collection.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/expr/param_expr.hh"
#include "Object/expr/const_index.hh"
#include "Object/expr/const_index_list.hh"
#include "Object/expr/const_range.hh"
#include "Object/expr/const_range_list.hh"
#include "Object/common/dump_flags.hh"
#include "Object/ref/meta_instance_reference_subtypes.hh"
#include "Object/ref/simple_nonmeta_instance_reference.hh"
#include "Object/unroll/instantiation_statement.hh"
#include "Object/def/definition_base.hh"
#include "Object/def/footprint.hh"
#include "Object/common/namespace.hh"
#include "Object/traits/classification_tags.hh"
#include "Object/type/param_type_reference.hh"
#include "Object/ref/meta_value_reference.hh"
#include "Object/ref/simple_meta_value_reference.hh"
#include "Object/ref/data_nonmeta_instance_reference.hh"
#include "Object/unroll/expression_assignment.hh"
#include "Object/inst/value_collection_pool_bundle.tcc"	// for allocate

#include "common/ICE.hh"

// #include "util/memory/list_vector_pool.tcc"
#include "util/memory/chunk_map_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/what.hh"
#include "util/multikey_qmap.tcc"		// include "qmap.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/compose.hh"
#include "util/binders.hh"
#include "util/dereference.hh"
#include "util/indent.hh"
#include "util/stacktrace.hh"

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
using util::indent;
using util::auto_indent;
using util::write_value;
using util::read_value;
using util::persistent_traits;
using util::memory::never_ptr;
using util::multikey_generator;

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE && ENABLE_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

//=============================================================================
// struct pint_instance method definitions
// not really methods...

#if 0
bool
operator == (const pint_instance& p, const pint_instance& q) {
	INVARIANT(p.instantiated && q.instantiated);
	if (p.valid && q.valid) {
		return p.value == q.value;
	} else return (p.valid == q.valid); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream& o, const pint_instance& p) {
	INVARIANT(p.instantiated);
	if (p.valid) {
		return o << p.value;
	} else	return o << "?";
}
#endif

//=============================================================================
// class value_placeholder method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
VALUE_PLACEHOLDER_CLASS::value_placeholder() :
		parent_type(), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
VALUE_PLACEHOLDER_CLASS::value_placeholder(const size_t d) :
		parent_type(d), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
VALUE_PLACEHOLDER_CLASS::value_placeholder(
		const typename parent_type::owner_ptr_raw_type& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Partial copy constructor.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
VALUE_PLACEHOLDER_CLASS::value_placeholder(const this_type& t, 
		const footprint& f) :
		parent_type(t, f), ival(NULL) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
VALUE_PLACEHOLDER_CLASS::~value_placeholder() {
//	STACKTRACE_DTOR("~value_placeholder()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
VALUE_PLACEHOLDER_CLASS::what(ostream& o) const {
#if 0
	return o << util::what<this_type>::name();
#else
	return o << traits_type::tag_name << " " << this->dimensions <<
		"-D (placeholder)";
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
VALUE_PLACEHOLDER_CLASS::what(ostream& o) const {
#if 0
	return o << "pint-inst<" << dimensions << ">";
#else
	return o << util::what<this_type>::name();
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Associates this instance placeholder with an
	initial instantiation statement pointer. 
	For port placeholders, this is the only instantiation statement.
	For others, it's just a hint for the initial type, 
		and may not even be used.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
VALUE_PLACEHOLDER_CLASS::attach_initial_instantiation_statement(
		const count_ptr<const instantiation_statement_base>& i) {
	if (!this->initial_instantiation_statement_ptr) {
		this->initial_instantiation_statement_ptr =
			i.template is_a<const initial_instantiation_statement_type>();
	}
	NEVER_NULL(this->initial_instantiation_statement_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
index_collection_item_ptr_type
VALUE_PLACEHOLDER_CLASS::get_initial_instantiation_indices(void) const {
	NEVER_NULL(this->initial_instantiation_statement_ptr);
	return this->initial_instantiation_statement_ptr->get_indices();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variant is called by template_formal_manager::dump_*.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
VALUE_PLACEHOLDER_CLASS::dump_formal(ostream& o) const {
	INVARIANT(this->is_template_formal());
	// this->dump_base(o);
	this->get_unresolved_type_ref()->dump(o) << ' ' << this->key;
	// placeholders don't have collections
#if 0
	this->dump_collection_only(o);
#endif
	expr_dump_context dc(expr_dump_context::default_value);
#if PROCESS_DEFINITION_IS_NAMESPACE
	dc.enclosing_scope = this->owner.template is_a<const scopespace>();
	NEVER_NULL(dc.enclosing_scope);
#else
	dc.enclosing_scope = this->owner;
#endif
	if (this->dimensions) {
		const index_collection_item_ptr_type
			i(this->get_initial_instantiation_indices());
		NEVER_NULL(i);
		i->dump(o, dc);
	}
	if (this->ival) {
		this->ival->dump(o << " (default = ", dc) << ")";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variant is used when there is an unroll_context available.  
	Intended for diagnostic use, printing resolved dimensions
	of template formal parameter value collection.  
	\param c unroll_context is needed in case of template dependence.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
VALUE_PLACEHOLDER_CLASS::dump_formal(ostream& o, const unroll_context& c) const {
	this->type_dump(o);
if (this->dimensions) {
	const index_collection_item_ptr_type
		i(this->get_initial_instantiation_indices());
	NEVER_NULL(i);
	const_range_list crl;
	if (!i->unroll_resolve_rvalues(crl, c).good) {
		ICE(cerr, 
			cerr << "Unable to deduce formal parameter collection "
				"size!  Little help, please." << endl;
		);
	}
	crl.dump(o << ' ');
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Was moved up from base class implementation.
	Obsolete? or still used?
	(used to be called virtually by template_formal_manager::dump)
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
VALUE_PLACEHOLDER_CLASS::dump(ostream& o, const dump_flags& df) const {
	parent_type::dump_base(o, df);
	const count_ptr<const param_expr>
		init_def(this->default_value());
	if (init_def) {
		expr_dump_context dc(expr_dump_context::default_value);
		dc.enclosing_scope = this->owner;
		if (this->is_template_formal())
			init_def->dump(o << " (default = ", dc) << ")";
		else    init_def->dump(o << " (init = ", dc) << ")";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
VALUE_PLACEHOLDER_CLASS::type_dump(ostream& o) const {
	return o << traits_type::tag_name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This code is ported from the old value_scalar::is_loop_variable.
	TODO: think of a better way to determine scope of variable.
	Perhaps loop scopes should be subscopes, because they spawn
	temporary local footprints. 
	The old method just looked up the variable key in the parent
	scope: if it was not found, then it was deduced a loop-variable.  
	Really lookup is only necessary for pints.  
	How hideous this is.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
bool
VALUE_PLACEHOLDER_CLASS::is_loop_variable(void) const {
	STACKTRACE_VERBOSE;
	NEVER_NULL(this->owner);
#if PROCESS_DEFINITION_IS_NAMESPACE
	return !this->dimensions && !this->owner->lookup_local(this->key);
#else
	return !this->dimensions && !this->owner->lookup_member(this->key);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<const param_type_reference>
VALUE_PLACEHOLDER_CLASS::get_param_type_ref(void) const {
	return traits_type::built_in_type_ptr;
		// declared in "traits/class_traits.h"
		// initialized in "traits/class_traits_types.cc"
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
VALUE_PLACEHOLDER_CLASS::get_unresolved_type_ref(void) const {
	return get_param_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes a parameter instance with an expression, 
	for the sake of COMPILE-TIME analysis only.
	The real assignment will be tracked in a param_expression_assignment
		object at UNROLL-TIME.  

	The ival may only be initialized once, enforced by assertions.  
	Note: a parameter is considered "usable" if it is 
	initialized OR it is a template formal.  
	Only bother initializing scalar variables, 
		ignore for initialization of non-scalars.  

	MAKE sure this is not a template_formal, 
	template-formal parameters are NEVER initialized, 
	only given default values.  

	\param e the rvalue expression.
	\return false if there was error.  
	\sa may_be_initialized
	\sa must_be_initialized
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
good_bool
VALUE_PLACEHOLDER_CLASS::initialize(const init_arg_type& e) {
	NEVER_NULL(e);
	INVARIANT(!ival);
	if (this->dimensions == 0) {
		if (may_type_check_actual_param_expr(*e).good) {
			ival = e;
			return good_bool(true);
		} else {
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigning default value(s) to parameters is only valid
	in the context of template-formal parameters.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
good_bool
VALUE_PLACEHOLDER_CLASS::assign_default_value(
		const count_ptr<const param_expr>& p) {
	const count_ptr<const expr_type> i(p.template is_a<const expr_type>());
	if (i && may_type_check_actual_param_expr(*i).good) {
		this->ival = i;
		return good_bool(true);
	}
	else return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Interprets ival as a default_value in the context of template
	formal parameters.
	\return pointer to default value expression.
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<const param_expr>
VALUE_PLACEHOLDER_CLASS::default_value(void) const {
//	STACKTRACE("VALUE_PLACEHOLDER_CLASS::default_value()");
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Interprets ival as an initial_value outside the context
	of template formals.  
	\return pointer to initial value expression.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<const typename VALUE_PLACEHOLDER_CLASS::expr_type>
VALUE_PLACEHOLDER_CLASS::initial_value(void) const {
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<meta_value_reference_base>
VALUE_PLACEHOLDER_CLASS::make_meta_value_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	return count_ptr<meta_value_reference_base>(
		new simple_meta_value_reference_type(
			never_ptr<this_type>(const_cast<this_type*>(this))));
		// omitting index argument
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	New: Create an rvalue reference to a meta parameter, possibly
		indexed with a nonmeta-value subscript.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<nonmeta_instance_reference_base>
VALUE_PLACEHOLDER_CLASS::make_nonmeta_instance_reference(void) const {
	typedef	count_ptr<nonmeta_instance_reference_base>	ptr_return_type;
	return ptr_return_type(new simple_nonmeta_instance_reference_type(
		never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether or not a param was passed to a formal 
	parameter in a template.  
	Should also check dimensionality and size.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
good_bool
VALUE_PLACEHOLDER_CLASS::may_type_check_actual_param_expr(
		const param_expr& pe) const {
	const never_ptr<const expr_type> pi(IS_A(const expr_type*, &pe));
	if (!pi) {
		// useful error message?
		return good_bool(false);
	}
	// this says that the only instantiation statement for this parameter
	// in the original declaration, which in this case was in the ports.  
	// only for formal parameters is this assertion valid.  
	// check dimensions (is conservative with dynamic sizes)
	return this->may_check_expression_dimensions(*pi);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether or not a param was passed to a formal 
	parameter in a template.  
	Should also check dimensionality and size.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
good_bool
VALUE_PLACEHOLDER_CLASS::must_type_check_actual_param_expr(
		const const_param& pe, const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	// only for formal parameters is this assertion valid.  
	// this says that the only instantiation statement for this parameter
	// in the original declaration, which in this case was in the ports.  
	// check dimensions (is conservative with dynamic sizes)
	return this->must_check_expression_dimensions(pe, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Despite the name, this doesn't copy...
	\return new instance collection for footprint.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
// typename VALUE_PLACEHOLDER_CLASS::instance_collection_generic_type*
instance_collection_base*
VALUE_PLACEHOLDER_CLASS::make_instance_collection_footprint_copy(
		footprint& f) const {
	return this->make_collection(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: distinguish between template formals and locals, 
		for sparse vs. dense collections.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
typename VALUE_PLACEHOLDER_CLASS::value_collection_generic_type*
VALUE_PLACEHOLDER_CLASS::make_collection(footprint& f) const {
	STACKTRACE_VERBOSE;
	value_collection_pool_bundle<Tag>&
		pool(f. template get_value_collection_pool_bundle<Tag>());
	// if (this->is_template_formal()) ...
	return pool.allocate_local_collection(
		f, never_ptr<const this_type>(this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param c the unroll_context in which to instantiate the parameter.
	\param e the value to assign to the parameter (optional).
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
good_bool
VALUE_PLACEHOLDER_CLASS::unroll_assign_formal_parameter(
		const unroll_context& c, 
		const count_ptr<const param_expr>& e) const {
	STACKTRACE_VERBOSE;
	// safe to instantiate the initial statement because
	// template formals are unconditional and packed
	NEVER_NULL(this->initial_instantiation_statement_ptr);
	const good_bool
		g(this->initial_instantiation_statement_ptr->unroll(c).good);
	INVARIANT(g.good);
	count_ptr<const expr_type> p(NULL);
	if (e) {
		p = e.template is_a<const expr_type>();
		if (!p) {
			cerr << "ERROR: parameter type mismatch." << endl;
			// TODO: more verbose error message
			return good_bool(false);
		}
	} else if (this->ival) {
		// check default expression
		p = this->ival;
	} else {
		cerr << "ERROR: template argument missing where "
			"no default is provided by the formal declaration."
			<< endl;
		return good_bool(false);
	}
	// create an auxiliary expression assignment statement
	const simple_meta_value_reference_type
		lv(never_ptr<const this_type>(this));
	return expression_assignment<Tag>::__unroll(c, lv, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No need to virtualize this method as long as 
	the dimension-specific subclasses have no pointers that 
	need to be visited.  
	initial_instantiation_statement_ptr can be NULL
	for the collections that are used in footprints.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
VALUE_PLACEHOLDER_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
	// don't bother with dimensions... will be reloaded.
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	// don't bother visit the owner, assuming that's the caller
	// go through index_collection
	parent_type::collect_transient_info_base(m);
	// Is ival really crucial in object?  will be unrolled anyhow
	if (ival)
		ival->collect_transient_info(m);
	// NEVER_NULL(this->initial_instantiation_statement_ptr);
	// not true for loop-induction variables!
	if (this->initial_instantiation_statement_ptr) {
		initial_instantiation_statement_ptr->collect_transient_info(m);
	}
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
VALUE_PLACEHOLDER_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE("value_placeholder<>::write_object_base()");
	parent_type::write_object_base(m, f);
	m.write_pointer(f, ival);
	m.write_pointer(f, this->initial_instantiation_statement_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
VALUE_PLACEHOLDER_CLASS::load_object(const persistent_object_manager& m, 
		istream& f) {
	STACKTRACE("value_placeholder<>::load_object_base()");
	parent_type::load_object_base(m, f);
	m.read_pointer(f, ival);
	m.read_pointer(f, this->initial_instantiation_statement_ptr);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_VALUE_PLACEHOLDER_TCC__

