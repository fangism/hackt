/**
	\file "Object/type/type_reference.cc"
	Type-reference class method definitions.  
	This file originally came from "Object/art_object_type_ref.cc"
		in a previous life.  
 	$Id: type_reference.cc,v 1.16 2006/07/30 05:49:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_TYPE_REFERENCE_CC__
#define	__HAC_OBJECT_TYPE_TYPE_REFERENCE_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)

#include <iostream>

#include "AST/token_string.h"	// so token_identifier : string
#include "Object/def/param_definition.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/channel_definition_alias.h"
#include "Object/def/user_def_datatype.h"
#include "Object/def/built_in_datatype_def.h"
#include "Object/def/enum_datatype_def.h"
#include "Object/def/datatype_definition_alias.h"
#include "Object/def/process_definition.h"
#include "Object/def/process_definition_alias.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/channel_type_reference.h"
#include "Object/type/builtin_channel_type_reference.h"
#include "Object/type/process_type_reference.h"
#include "Object/type/param_type_reference.h"
#include "Object/type/canonical_type.h"			// or tcc?
#include "Object/inst/bool_instance_collection.h"
#include "Object/inst/int_instance_collection.h"
#include "Object/inst/enum_instance_collection.h"
#include "Object/inst/struct_instance_collection.h"
#include "Object/inst/pbool_value_collection.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/meta_range_list.h"
#include "Object/persistent_type_hash.h"
#include "util/persistent_object_manager.tcc"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/unroll/unroll_context.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/unroll/param_instantiation_statement.h"
#include "Object/unroll/datatype_instantiation_statement.h"
#include "Object/unroll/channel_instantiation_statement.h"
#include "Object/unroll/process_instantiation_statement.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/inst/null_collection_type_manager.h"
#include "Object/inst/parameterless_collection_type_manager.h"
#include "Object/inst/int_collection_type_manager.h"
#include "Object/inst/subinstance_manager.h"
#include "Object/type/canonical_generic_chan_type.h"
#include "common/ICE.h"
#include "common/TODO.h"

#include "util/sstream.h"
#include "util/indent.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/reserve.h"

//=============================================================================
// specializations

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::data_type_reference, DATA_TYPE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::builtin_channel_type_reference,
		BLTIN_CHANNEL_TYPE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::channel_type_reference,
		USER_CHANNEL_TYPE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::process_type_reference, PROCESS_TYPE_REFERENCE_TYPE_KEY, 0)
}	// end namespace util

namespace HAC {
namespace entity {
using std::ostringstream;
using std::transform;
using std::back_inserter;
#include "util/using_ostream.h"
using util::indent;
using util::auto_indent;
using util::persistent_traits;
using util::read_value;
using util::write_value;

//=============================================================================
// class fundamental_type_reference method definitions

fundamental_type_reference::fundamental_type_reference() :
		type_reference_base(), 
		template_args() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
fundamental_type_reference::fundamental_type_reference(
		const template_actuals& a) :
		type_reference_base(), template_args(a) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
fundamental_type_reference::~fundamental_type_reference() {
	STACKTRACE_DTOR("~fundamental_type_reference()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
fundamental_type_reference::dump(ostream& o) const {
	return template_args.dump(o << get_base_def()->get_name());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return non-modifiable copy of the template argument list.  
	So far only called by int_instance_collection.  
 */
const template_actuals&
fundamental_type_reference::get_template_params(void) const {
	return template_args;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Whether or not this type is complete, i.e. with relaxed
	actuals if the definitions template signature has any relaxed formals.  

	We make an exception for built-in channel types, which, 
	as of (2005-07-06) by construction, are only composed of
	strict data types.  See Implementation note in:
	AST::parser::data_type_ref_list::check_builtin_channel_type().
 */
bool
fundamental_type_reference::is_strict(void) const {
if (IS_A(const builtin_channel_type_reference*, this)) {
	return true;
}
	const bool expects =
		get_base_def()->get_template_formals_manager()
			.has_relaxed_formals();
	const bool has = template_args.get_relaxed_args();
	if (has) {
		INVARIANT(expects);
		return true;
	} else {
		return !expects;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if all template actuals are constants, 
		regardless of strictness.  
 */
bool
fundamental_type_reference::is_resolved(void) const {
	return template_args.is_resolved();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// is static
excl_ptr<instantiation_statement_base>
fundamental_type_reference::make_instantiation_statement(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d, 
		const const_template_args_ptr_type& a) {
	return t->make_instantiation_statement_private(t, d, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// is static
excl_ptr<instantiation_statement_base>
fundamental_type_reference::make_instantiation_statement(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) {
	const const_template_args_ptr_type null;
	return t->make_instantiation_statement_private(t, d, null);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
fundamental_type_reference::type_mismatch_error(ostream& o, 
		const this_type& l, const this_type& r) {
	o << "ERROR: types mismatch!" << endl;
	l.dump(o << "\tgot: ") << endl;
	r.dump(o << "\tand: ") << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should be pure virtual, but providing a temporary default-catch
	null implementation.  
 */
good_bool
fundamental_type_reference::unroll_register_complete_type(void) const {
	cerr << "WARNING: unimplemented fundamental_type_reference"
		"::unroll_register_complete_type() returns \'bad\'." << endl;
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
fundamental_type_reference::collect_transient_info_base(
		persistent_object_manager& m) const {
	template_args.collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
fundamental_type_reference::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("fund_type_ref::write_object_base()");
	template_args.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
fundamental_type_reference::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("fund_type_ref::load_object_base()");
	template_args.load_object_base(m, i);
}


//=============================================================================
#if 0
MAY BE OBSOLETE
// class collective_type_reference method definitions

collective_type_reference::collective_type_reference(
		const type_reference_base& b, 
		never_ptr<const array_index_list> d) :
		type_reference_base(), base(&b), dim(d) {
}

collective_type_reference::~collective_type_reference() {
	// we don't own the members
}

ostream&
collective_type_reference::what(ostream& o) const {
	return o << "collective-type-ref";
}

ostream&
collective_type_reference::dump(ostream& o) const {
	return what(o);			// temporary
}
#endif

//=============================================================================
// class data_type_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
data_type_reference::data_type_reference() :
		fundamental_type_reference(), 
		base_type_def(NULL) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_type_reference::data_type_reference(
		const definition_ptr_type td) :
		fundamental_type_reference(), 
		base_type_def(td) {
	NEVER_NULL(base_type_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_type_reference::data_type_reference(
		const definition_ptr_type td, 
		const template_actuals& pl) :
		fundamental_type_reference(pl), 
		base_type_def(td) {
	NEVER_NULL(base_type_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Downgrading from canonical to more general type reference.  
 */
data_type_reference::data_type_reference(
		const canonical_user_def_data_type& p) :
		fundamental_type_reference(p.get_template_params()), 
		base_type_def(p.get_base_def()) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_type_reference::~data_type_reference() {
	STACKTRACE_DTOR("~data_type_reference()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
data_type_reference::what(ostream& o) const {
	return o << "data-type-reference";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const definition_base>
data_type_reference::get_base_def(void) const {
	return base_type_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const datatype_definition_base>
data_type_reference::get_base_datatype_def(void) const {
	return base_type_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
data_type_reference::is_canonical(void) const {
	return !base_type_def.is_a<const datatype_definition_alias>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
data_type_reference::is_accepted_in_datatype(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
data_type_reference::is_accepted_in_channel(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The final blessing from the compiler that the template actuals
	meet the requirements specified by the base definition's 
	template formals.  
	Called from unroll_resolve().
	\return good if good.
 */
good_bool
data_type_reference::must_be_valid(void) const {
	return base_type_def->get_template_formals_manager()
		.must_validate_actuals(template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
/**
	Creates a translation context between formals and actuals.  
 */
unroll_context
data_type_reference::make_unroll_context(void) const {
	return unroll_context(template_args,
		base_type_def->get_template_formals_manager());
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Makes a copy of this type reference, but with strictly resolved
	constant parameter arguments.  
	Will eventually require a context-like object.  
	\return a copy of itself, but with type parameters resolved, 
		if applicable.  Returns NULL if there is error in resolution.  
 */
count_ptr<const data_type_reference>
data_type_reference::unroll_resolve(const unroll_context& c) const {
	STACKTRACE("data_type_reference::unroll_resolve()");
	typedef	count_ptr<const this_type>	return_type;
	// can this code be factored out to type_ref_base?
	if (template_args) {
		// if template actuals depends on other template parameters, 
		// then we need to pass actuals into its own context!
		unroll_context cc(make_unroll_context());
		cc.chain_context(c);
		const template_actuals
			actuals(template_args.unroll_resolve(cc));
		// check for errors??? at least try-catch
		if (actuals) {
			// the final type-check:
			// now they MUST size-type check
			const return_type
				ret(new this_type(base_type_def, actuals));
			NEVER_NULL(ret);
			return (ret->must_be_valid().good ?
				ret : return_type(NULL));
		} else {
			cerr << "ERROR resolving template arguments." << endl;
			return return_type(NULL);
		}
	} else {
		// need to check must_be_valid?
		const return_type ret(new this_type(base_type_def));
		INVARIANT(ret->must_be_valid().good);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Combines relaxed template arguments to make a complete strict type.  
	\param a non-NULL relaxed template actuals.
	\pre this->template_args doesn't already have relaxed actuals, 
		this is asserted in the template_actuals constructor.  
 */
count_ptr<const data_type_reference>
data_type_reference::merge_relaxed_actuals(
		const const_template_args_ptr_type& a) const {
	return count_ptr<const this_type>(new this_type(base_type_def, 
			template_actuals(template_args, a)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed data instantiation statement object.
 */
excl_ptr<instantiation_statement_base>
data_type_reference::make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d, 
		const const_template_args_ptr_type& a) const {
	return excl_ptr<instantiation_statement_base>(
		new data_instantiation_statement(
			t.is_a<const data_type_reference>(), d, a));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed data instance object.  
	TO DO: move all error checking into scopespace::add_instance
		for unification.  
		Handle cases for additions to sparse collections.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
excl_ptr<instance_collection_base>
data_type_reference::make_instance_collection(
		const never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const {
	typedef excl_ptr<instance_collection_base>	return_type;
/***
	datatype_instance_collection is now pure virtual, 
	we use a temporary shortcut to effectively sub-class...
	save us the trouble of expanding more classes until later...
***/
	NEVER_NULL(base_type_def);
	const never_ptr<const datatype_definition_base>
		alias(base_type_def->resolve_canonical_datatype_definition());
	// hideous switch-case... only temporary
	if (alias.is_a<const user_def_datatype>()) {
		return return_type(
			struct_instance_collection::make_array(*s, id, d));
	} else if (alias.is_a<const enum_datatype_def>()) {
		return return_type(
			enum_instance_collection::make_array(*s, id, d));
	} else {
		// what about typedefs/aliases of built-in types? Ahhhh....
		INVARIANT(alias.is_a<const built_in_datatype_def>());
		// just compare pointers
		if (alias == &bool_traits::built_in_definition) {
			return return_type(
				bool_instance_collection::make_array(*s, id, d));
		} else if (alias == &int_traits::built_in_definition) {
			return return_type(
				int_instance_collection::make_array(*s, id, d));
		} else {
			DIE;	// WTF!?
			return return_type(NULL);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct data_type_reference::canonical_compare_result_type {
	typedef	count_ptr<const this_type>	type_type;
	type_type				lt, rt;
	bool					equal;

	canonical_compare_result_type(const this_type&, const this_type&);
};	// end struct canonical_compare_result_type

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	What about integer widths?
	(rather not inline)
 */
data_type_reference::canonical_compare_result_type
	::canonical_compare_result_type(
		const this_type& l, const this_type& r) :
		lt(l.make_canonical_data_type_reference()),
		rt(r.make_canonical_data_type_reference()) {
	STACKTRACE_VERBOSE;
	if (!lt || !rt) {
	ICE(cerr, 
		cerr << "got null left-type or right-type "
			"after canonicalization.  FAAAAANNNNG!" << endl;
	)
	}
	const never_ptr<const datatype_definition_base> ld(lt->base_type_def);
	const never_ptr<const datatype_definition_base> rd(rt->base_type_def);
	INVARIANT(ld && rd);
	INVARIANT(!ld.is_a<const typedef_base>());
	INVARIANT(!rd.is_a<const typedef_base>());
#if ENABLE_STACKTRACE
	lt->dump(STACKTRACE_INDENT << "lt = ") << endl;
	rt->dump(STACKTRACE_INDENT << "rt = ") << endl;
	ld->dump(STACKTRACE_INDENT << "ld = ") << endl;
	rd->dump(STACKTRACE_INDENT << "rd = ") << endl;
#endif
	equal = (ld == rd);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
data_type_reference::may_be_collectibly_type_equivalent(
		const fundamental_type_reference& ft) const {
	const this_type& t(IS_A(const this_type&, ft));	// assert cast
	const canonical_compare_result_type eq(*this, t);
	if (!eq.equal)
		return false;
	else	return eq.lt->template_args.may_be_relaxed_equivalent(
			eq.rt->template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks connectivity.  
	This is called at parse-tree-check time and is conservative.  
	Precise type comparison occurs at unroll-time.  
 */
bool
data_type_reference::may_be_connectibly_type_equivalent(
		const fundamental_type_reference& ft) const {
	STACKTRACE_VERBOSE;
	const this_type& t(IS_A(const this_type&, ft));	// assert cast
	const canonical_compare_result_type eq(*this, t);
	if (!eq.equal)
		return false;
	else	return eq.lt->template_args.may_be_strict_equivalent(
			eq.rt->template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Used for checking statements of the form a:=b (CHP).  
	Need to make an exception for int<W> := int<0>.  
	\return true if types of expression may be assignable.  
 */
bool
data_type_reference::may_be_assignably_type_equivalent(
		const data_type_reference& ft) const {
	STACKTRACE_VERBOSE;
	const this_type& t(IS_A(const this_type&, ft));	// assert cast
	const canonical_compare_result_type eq(*this, t);
	if (!eq.equal)
		return false;
	if (base_type_def == &int_traits::built_in_definition) {
		// allow rvalue's width to be zero as a special case
		// to allow parameters ints as rvalues
		const count_ptr<const pint_const>
			width(eq.rt->template_args[0].is_a<const pint_const>());
		return (width && width->static_constant_value() == 0) ||
			eq.lt->template_args.may_be_strict_equivalent(
				eq.rt->template_args);
	}
	else	return eq.lt->template_args.may_be_strict_equivalent(
			eq.rt->template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Used for checking statements of the form 'a op b' (CHP).  
	Need to make an exception for int<W> := int<0>.  
	\return true if types of expression may be assignable.  
 */
bool
data_type_reference::may_be_binop_type_equivalent(
		const data_type_reference& ft) const {
	STACKTRACE_VERBOSE;
	const this_type& t(IS_A(const this_type&, ft));	// assert cast
	const canonical_compare_result_type eq(*this, t);
	if (!eq.equal)
		return false;
	if (base_type_def == &int_traits::built_in_definition) {
		// allow lhs or rhs's integer width to be 0
		const count_ptr<const pint_const>
			lw(eq.lt->template_args[0].is_a<const pint_const>()),
			rw(eq.rt->template_args[0].is_a<const pint_const>());
		return (lw && lw->static_constant_value() == 0) ||
			(rw && rw->static_constant_value() == 0) ||
			eq.lt->template_args.may_be_strict_equivalent(
				eq.rt->template_args);
	}
	else	return eq.lt->template_args.may_be_strict_equivalent(
			eq.rt->template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static helper function.
 */
void
data_type_reference::unroll_port_instances(
		const never_ptr<const definition_type> def, 
		const template_actuals& ta, 
		const unroll_context& c, subinstance_manager& sub) {
	if (def == &bool_traits::built_in_definition) {
		// do nothing!
	} else if (def == &int_traits::built_in_definition) {
		// do nothing... for now
		// don't really anticipate expanding bits fields.  
	} else if (def.is_a<const enum_datatype_def>()) {
		// do nothing
	} else {
	// copied from process_type_reference::unroll_port_instances
	// didn't scrutinze or check...
	STACKTRACE_VERBOSE;
	const never_ptr<const user_def_datatype>
		data_def(def.is_a<const user_def_datatype>());
	NEVER_NULL(data_def);
	const port_formals_manager& port_formals(data_def->get_port_formals());
	{
		STACKTRACE("local context");
		const template_actuals
			resolved_template_args(ta.unroll_resolve(c));
		const unroll_context
			cc(resolved_template_args, 
				data_def->get_template_formals_manager());
		// should the contexts be chained?
		// or can the actuals always be resolved one scope at a time?
		port_formals.unroll_ports(cc, sub);
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Transient work around for special cases only.  
	TODO (2005-07-12): implement for real.
 */
void
data_type_reference::unroll_port_instances(const unroll_context& c,
		subinstance_manager& sub) const {
	INVARIANT(is_resolved());
	INVARIANT(is_canonical());
	unroll_port_instances(base_type_def, template_args, c, sub);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenience function for creating data int type-references.
	\param w the integer width, here allowed to be 0.  
	\returns newly allocated integer type reference.
 */
data_type_reference*
data_type_reference::make_quick_int_type_ref(const pint_value_type w) {
	INVARIANT(w >= 0);
	// is an excl_ptr, incidentally
	const fundamental_type_reference::template_args_ptr_type
		width_params(new const_param_expr_list(
			count_ptr<const pint_const>(new pint_const(w))));
	const template_actuals tpl(width_params, 
		template_actuals::arg_list_ptr_type());
	return new data_type_reference(never_ptr<const built_in_datatype_def>(
		&int_traits::built_in_definition), tpl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_type<data_type_reference::definition_type>
data_type_reference::make_canonical_type(void) const {
	return base_type_def->make_canonical_type(template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
data_type_reference::make_canonical_data_type_reference(void) const {
	// INVARIANT(template_args.is_constant());		// NOT true
	return base_type_def->make_canonical_fundamental_type_reference(
		template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	STACKTRACE_PERSISTENT("data_type_ref::collect_transients()");
	base_type_def->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_type_reference::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	STACKTRACE_PERSISTENT("data_type_ref::write_object()");
	m.write_pointer(f, base_type_def);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	May need special case handling for built-in definitions!
	Call the HACK POLICE!!!
 */
void
data_type_reference::load_object(const persistent_object_manager& m, 
		istream& f) {
	STACKTRACE_PERSISTENT("data_type_ref::load_object()");
	m.read_pointer(f, base_type_def);
	parent_type::load_object_base(m, f);
	intercept_builtin_definition_hack(m, base_type_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Factored out this hack into a separate definition
	for re-use by canonical_generic_datatype.  
 */
void
data_type_reference::intercept_builtin_definition_hack(
		const persistent_object_manager& m, definition_ptr_type& d) {
	STACKTRACE_VERBOSE;
	// MAJOR HACK: shallow recursion and intercept built-in types
	// TODO: ALERT!!! case where base_type_def is a typedef alias?
	NEVER_NULL(d);
#if 0
	const never_ptr<const user_def_datatype>
		udd(d.is_a<const user_def_datatype>());
	if (udd) {
		m.load_object_once(const_cast<user_def_datatype*>(&*udd));
	} else {
		m.load_object_once(const_cast<datatype_definition_base*>(&*d));
	}
#else
	m.load_object_once(const_cast<datatype_definition_base*>(&*d));
#endif
	if (d->get_key() == "bool") {
		m.please_delete(&*d);			// HACKERY
		d = never_ptr<const datatype_definition_base>(
			&bool_traits::built_in_definition);
		// must flag visit specially
	} else if (d->get_key() == "int") {
		m.please_delete(&*d);	// HACKERY
		d = never_ptr<const datatype_definition_base>(
			&int_traits::built_in_definition);
		// must flag visit specially
	}
	// else leave the base definition as is
	// reference count will take care of discarded memory :)
}

//=============================================================================
// class channel_type_reference_base method definitions

// inline
channel_type_reference_base::channel_type_reference_base(
		const template_actuals& pl) :
		parent_type(pl), direction('\0') {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_type_reference_base::dump_direction(ostream& o, const char d) {
	if (d == '!' || d == '?')
		o << d;
	// else no direction
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_type_reference_base::is_accepted_in_datatype(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_type_reference_base::is_accepted_in_channel(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	parent_type::write_object_base(m, o);
	write_value(o, direction);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
	parent_type::load_object_base(m, i);
	read_value(i, direction);
}

//=============================================================================
// class builtin_channel_type_reference method definitions

/**
	Helper class functor for transforming data types into
	resolved types.  
 */
struct builtin_channel_type_reference::datatype_resolver {
	const unroll_context&		the_context;

	explicit
	datatype_resolver(const unroll_context& c) : the_context(c) { }

	datatype_ptr_type
	operator () (const datatype_ptr_type& dt) const {
		NEVER_NULL(dt);
		return dt->unroll_resolve(the_context);
	}
};	// end class datatype_resolver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper class functor for transforming data types into
	canonical types.  
 */
struct builtin_channel_type_reference::datatype_canonicalizer {
	datatype_ptr_type
	operator () (const datatype_ptr_type& dt) const {
		NEVER_NULL(dt);
		return dt->make_canonical_data_type_reference();
	}
};	// end class datatype_canonicalizer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper class functor for transforming data types into
	canonical types.  
 */
struct builtin_channel_type_reference::yet_another_datatype_canonicalizer {
	canonical_generic_datatype
	operator () (const datatype_ptr_type& dt) const {
		NEVER_NULL(dt);
		return dt->make_canonical_type();
	}
};	// end class yet_another_datatype_canonicalizer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
builtin_channel_type_reference::builtin_channel_type_reference() :
		parent_type(), datatype_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
builtin_channel_type_reference::~builtin_channel_type_reference() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
builtin_channel_type_reference::what(ostream& o) const {
	return o << "builtin-channel-type-reference";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Single-line compact dump of built-in channel type reference.  
	No newline at the end.  
 */
ostream&
builtin_channel_type_reference::dump(ostream& o) const {
//	STACKTRACE_VERBOSE;
	o << "chan";
	dump_direction(o, direction);
	o << '(';
	INVARIANT(datatype_list.size());
	datatype_list_type::const_iterator i(datatype_list.begin());
	const datatype_list_type::const_iterator e(datatype_list.end());
	(*i)->dump(o);
	for (i++; i!=e; i++) {
		(*i)->dump(o << ", ");
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Indented, one-type-per-line-formatted dump of built-in channel 
	type reference, called from at least user_def_chan::dump.
 */
ostream&
builtin_channel_type_reference::dump_long(ostream& o) const {
//	STACKTRACE_VERBOSE;
	o << "chan";
	dump_direction(o, direction);
	o << '(' << endl;
	{
	INDENT_SECTION(o); 
	INVARIANT(datatype_list.size());
	datatype_list_type::const_iterator i(datatype_list.begin());
	const datatype_list_type::const_iterator e(datatype_list.end());
	for ( ; i!=e; i++) {
		(*i)->dump(o << auto_indent) << endl;
	}
	}
	return o << auto_indent << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Built-in channel types have no base definition... yet.
	Perhaps a singleton global static definition should exist 
	for this purpose.  
	For now this returns NULL.  
 */
never_ptr<const definition_base>
builtin_channel_type_reference::get_base_def(void) const {
	cerr << "Got: builtin_chanel_type_reference::get_base_def(), "
		"did you really mean this?" << endl;
//	return never_ptr<const built_in_channel_def>(NULL);
	return never_ptr<const definition_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Built-in channel types cannot be typedefs!
 */
bool
builtin_channel_type_reference::is_canonical(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only applicable if datatype_list_type is a vector.  
 */
void
builtin_channel_type_reference::reserve_datatypes(const size_t s) {
	util::reserve(datatype_list, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
builtin_channel_type_reference::add_datatype(
		const datatype_list_type::value_type& t) {
	NEVER_NULL(t);
	datatype_list.push_back(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
builtin_channel_type_reference::datatype_ptr_type
builtin_channel_type_reference::index_datatype(const size_t i) const {
	return datatype_list[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
builtin_channel_type_reference::may_be_collectibly_type_equivalent(
		const fundamental_type_reference& t) const {
	const this_type* const bct(IS_A(const this_type*, &t));
	return bct ? may_be_collectibly_channel_type_equivalent(*bct) : false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
builtin_channel_type_reference::may_be_connectibly_type_equivalent(
		const fundamental_type_reference& t) const {
	const this_type* const bct(IS_A(const this_type*, &t));
	return bct ? may_be_connectibly_channel_type_equivalent(*bct) : false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct builtin_channel_type_reference::datatype_list_comparison {
	typedef this_type				arg_type;
	typedef datatype_list_type::const_iterator	const_iterator;
	const_iterator					li, ri;
	const const_iterator				le;
	bool						match;

	datatype_list_comparison(const arg_type&, const arg_type&);
};	// end struct datatyle_list_comparison

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
builtin_channel_type_reference::datatype_list_comparison
	::datatype_list_comparison(const arg_type& l, const arg_type& r) :
		li(l.datatype_list.begin()),
		ri(r.datatype_list.begin()),
		le(l.datatype_list.end()) {
	const size_t ls(l.datatype_list.size());
	const size_t rs(r.datatype_list.size());
	if (ls != rs) {
		type_mismatch_error(cerr, l, r);
		match = false;
	} else {
		match = true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluates whether or not two built-in channel types may be
	collectibly equivalent.  
 */
bool
builtin_channel_type_reference::may_be_collectibly_channel_type_equivalent(
		const this_type& t) const {
	typedef datatype_list_type::const_iterator	const_iterator;
	datatype_list_comparison c(*this, t);
	if (!c.match)
		return false;
	const_iterator& i(c.li), j(c.ri);
	const const_iterator& e(c.le);
	for ( ; i!=e; i++, j++) {
		if (!(*i)->may_be_collectibly_type_equivalent(**j)) {
			// already have error message
			return false;
		}
	}
	// else everything matches
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluates whether or not two built-in channel types may be
	connectibly equivalent.  
 */
bool
builtin_channel_type_reference::may_be_connectibly_channel_type_equivalent(
		const this_type& t) const {
	typedef datatype_list_type::const_iterator	const_iterator;
	datatype_list_comparison c(*this, t);
	if (!c.match)
		return false;
	const_iterator& i(c.li), j(c.ri);
	const const_iterator& e(c.le);
	for ( ; i!=e; i++, j++) {
		if (!(*i)->may_be_connectibly_type_equivalent(**j)) {
			// already have error message
			return false;
		}
	}
	// else everything matches
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const builtin_channel_type_reference>
builtin_channel_type_reference::resolve_builtin_channel_type(void) const {
	typedef	never_ptr<const builtin_channel_type_reference>	return_type;
	return return_type(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a translation context between formals and actuals.  
 */
unroll_context
builtin_channel_type_reference::make_unroll_context(void) const {
	// doesn't have template formals...
	return unroll_context();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Makes a copy of this type reference, but with strictly resolved
	constant parameter arguments.  
	Will eventually require a context-like object.  
	\return a copy of itself, but with type parameters resolved, 
		if applicable.  Returns NULL if there is error in resolution.  
	TODO: what if template-dependent type appears in type-list, 
		say inside a definition body?
 */
count_ptr<const channel_type_reference_base>
builtin_channel_type_reference::unroll_resolve(const unroll_context& c) const {
	STACKTRACE("builtin_channel_type_reference::unroll_resolve()");
	typedef	count_ptr<this_type>	return_type;
	INVARIANT(!template_args);
	const return_type ret(new this_type);
	NEVER_NULL(ret);
	ret->direction = direction;
	util::reserve(ret->datatype_list, datatype_list.size());
	transform(datatype_list.begin(), datatype_list.end(),
		back_inserter(ret->datatype_list), 
		datatype_resolver(c)	// helper functor
	);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed channel instantiation statement object.
	Currently same as (user-defined) channel_type_reference.  
	Perhaps fold into parent class?
	Q: Is there a need to distinguish between channel types
		for making instantiation statements?
	(2005-05-28: decide later)
 */
excl_ptr<instantiation_statement_base>
builtin_channel_type_reference::make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d, 
		const const_template_args_ptr_type& a) const {
	// technically built-in channel types never have relaxed actuals...
	return excl_ptr<instantiation_statement_base>(
		new channel_instantiation_statement(
			t.is_a<const this_type>(), d, a));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed channel instance object.  
	Q: Is there a need to distinguish between channel types
		for making instantiation collections?
	(2005-05-28: decide later)
 */
excl_ptr<instance_collection_base>
builtin_channel_type_reference::make_instance_collection(
		const never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const {
	return excl_ptr<instance_collection_base>(
		channel_instance_collection::make_array(*s, id, d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This gets complicated...
	Temporary hack: see canonical_generic_chan_type.
 */
canonical_generic_chan_type
builtin_channel_type_reference::make_canonical_type(void) const {
	canonical_generic_chan_type ret;
	ret.direction = direction;
	util::reserve(ret.datatype_list, datatype_list.size());
	transform(datatype_list.begin(), datatype_list.end(),
		back_inserter(ret.datatype_list), 
		yet_another_datatype_canonicalizer()	// helper functor
	);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't really know what to do, since built-in channel types 
	are supposed to be abstract, right?
	Interpret data-types as unnamed ports?
 */
void
builtin_channel_type_reference::unroll_port_instances(
		const unroll_context& c, 
		subinstance_manager& sub) const {
	FINISH_ME(Fang);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
builtin_channel_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	parent_type::collect_transient_info_base(m);
	m.collect_pointer_list(datatype_list);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
builtin_channel_type_reference::write_object(
		const persistent_object_manager& m, ostream& o) const {
	parent_type::write_object_base(m, o);
	m.write_pointer_list(o, datatype_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
builtin_channel_type_reference::load_object(
		const persistent_object_manager& m, istream& i) {
	parent_type::load_object_base(m, i);
	m.read_pointer_list(i, datatype_list);
}

//=============================================================================
// class channel_type_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
channel_type_reference::channel_type_reference() :
		parent_type(), 
		base_chan_def(NULL) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Concrete channel type reference.  
	\param cd reference to a channel definition.
	\param pl (optional) parameter list for templates.  
 */
channel_type_reference::channel_type_reference(
		const never_ptr<const channel_definition_base> cd, 
		const template_actuals& pl) :
		parent_type(pl), 
		base_chan_def(cd) {
	NEVER_NULL(base_chan_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_type_reference::channel_type_reference(
		const never_ptr<const channel_definition_base> cd) :
		parent_type(), 	// NULL
		base_chan_def(cd) {
	NEVER_NULL(base_chan_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_type_reference::~channel_type_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_type_reference::what(ostream& o) const {
	return o << "user-channel-type-reference";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_type_reference::dump(ostream& o) const {
	return dump_direction(fundamental_type_reference::dump(o), direction);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const definition_base>
channel_type_reference::get_base_def(void) const {
	return base_chan_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_type_reference::is_canonical(void) const {
	return !base_chan_def.is_a<const channel_definition_alias>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
channel_type_reference::must_be_valid(void) const {
	return base_chan_def->get_template_formals_manager()
		.must_validate_actuals(template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return what does it mean to return NULL?
	TODO: handles cases for resolving typedefs.  
 */
never_ptr<const builtin_channel_type_reference>
channel_type_reference::resolve_builtin_channel_type(void) const {
	typedef	never_ptr<const builtin_channel_type_reference>	return_type;
	const never_ptr<const user_def_chan>
		udc(base_chan_def.is_a<const user_def_chan>());
	if (!udc) {
		FINISH_ME(Fang);
		cerr << "Case where base_chan_def is not user_def_chan."
			<< endl;
		return return_type(NULL);
	}
	// KLUDGE: count_ptr coerced to never_ptr!
	// the consumer of this pointer better not hold onto it!
	// see entity::CHP::channel_send::push_back().
	return return_type(&udc->get_builtin_channel_type());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a translation context between formals and actuals.  
 */
unroll_context
channel_type_reference::make_unroll_context(void) const {
	return unroll_context(template_args,
		base_chan_def->get_template_formals_manager());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Makes a copy of this type reference, but with strictly resolved
	constant parameter arguments.  
	Will eventually require a context-like object.  
	\return a copy of itself, but with type parameters resolved, 
		if applicable.  Returns NULL if there is error in resolution.  
 */
count_ptr<const channel_type_reference_base>
channel_type_reference::unroll_resolve(const unroll_context& c) const {
	STACKTRACE("channel_type_reference::unroll_resolve()");
	typedef	count_ptr<this_type>	return_type;
	// can this code be factored out to type_ref_base?
	if (template_args) {
		// if template actuals depends on other template parameters, 
		// then we need to pass actuals into its own context!
		unroll_context cc(make_unroll_context());
		cc.chain_context(c);
		const template_actuals
			actuals(template_args.unroll_resolve(cc));
		// check for errors??? at least try-catch
		if (actuals) {
			// the final type-check:
			// now they MUST size-type check
			const return_type
				ret(new this_type(base_chan_def, actuals));
			NEVER_NULL(ret);
			ret->direction = direction;
			return (ret->must_be_valid().good ?
				ret : return_type(NULL));
		} else {
			cerr << "ERROR resolving template arguments." << endl;
			return return_type(NULL);
		}
	} else {
		// need to check must_be_valid?
		const return_type ret(new this_type(base_chan_def));
		INVARIANT(ret->must_be_valid().good);
		ret->direction = direction;
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct channel_type_reference::canonical_compare_result_type {
	typedef	count_ptr<const this_type>	type_type;
	type_type				lt, rt;
	bool					equal;

	canonical_compare_result_type(const this_type&, const this_type&);
};	// end struct canonical_compare_result_type

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// rather not inline
channel_type_reference::canonical_compare_result_type
	::canonical_compare_result_type(
		const this_type& l, const this_type& r) :
		lt(l.make_canonical_channel_type_reference()),
		rt(r.make_canonical_channel_type_reference()) {
	if (!lt || !rt) {
	ICE(cerr, 
		cerr << "got null left-type or right-type "
			"after canonicalization.  FAAAAANNNNG!" << endl;
	)
	}
	const never_ptr<const channel_definition_base> ld(lt->base_chan_def);
	const never_ptr<const channel_definition_base> rd(rt->base_chan_def);
	INVARIANT(ld && rd);
	INVARIANT(!ld.is_a<const typedef_base>());
	INVARIANT(!rd.is_a<const typedef_base>());
	equal = (ld == rd);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_type_reference::may_be_collectibly_type_equivalent(
		const fundamental_type_reference& ft) const {
	const this_type& t(IS_A(const this_type&, ft));	// assert cast
	const canonical_compare_result_type eq(*this, t);
	if (!eq.equal)
		return false;
	else	return eq.lt->template_args.may_be_relaxed_equivalent(
			eq.rt->template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_type_reference::may_be_connectibly_type_equivalent(
		const fundamental_type_reference& ft) const {
	const this_type& t(IS_A(const this_type&, ft));	// assert cast
	const canonical_compare_result_type eq(*this, t);
	if (!eq.equal)
		return false;
	else	return eq.lt->template_args.may_be_strict_equivalent(
			eq.rt->template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Combines relaxed template arguments to make a complete strict type.  
	\param a non-NULL relaxed template actuals.
	\pre this->template_args doesn't already have relaxed actuals, 
		this is asserted in the template_actuals constructor.  
 */
count_ptr<const channel_type_reference>
channel_type_reference::merge_relaxed_actuals(
		const const_template_args_ptr_type& a) const {
	return count_ptr<const this_type>(new this_type(base_chan_def, 
			template_actuals(template_args, a)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed channel instantiation statement object.
 */
excl_ptr<instantiation_statement_base>
channel_type_reference::make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d, 
		const const_template_args_ptr_type& a) const {
	return excl_ptr<instantiation_statement_base>(
		new channel_instantiation_statement(
			t.is_a<const this_type>(), d, a));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed channel instance object.  
 */
excl_ptr<instance_collection_base>
channel_type_reference::make_instance_collection(
		const never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const {
	return excl_ptr<instance_collection_base>(
		channel_instance_collection::make_array(*s, id, d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: recursively canonicalize types for data-parameters and ports?
 */
canonical_generic_chan_type
channel_type_reference::make_canonical_type(void) const {
	canonical_generic_chan_type
		ret(base_chan_def->make_canonical_type(template_args));
	ret.set_direction(direction);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const channel_type_reference>
channel_type_reference::make_canonical_channel_type_reference(void) const {
	const count_ptr<const channel_type_reference_base>
		_ret(base_chan_def->make_canonical_fundamental_type_reference(
			template_args));
	const count_ptr<const this_type>
		ret(_ret.is_a<const this_type>());
	if (_ret && !ret) {
		FINISH_ME_EXIT(Fang);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::unroll_port_instances(
		const unroll_context& c, 
		subinstance_manager& sub) const {
	FINISH_ME(Fang);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	base_chan_def->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	m.write_pointer(f, base_chan_def);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, base_chan_def);
	parent_type::load_object_base(m, f);
}

//=============================================================================
// class process_type_reference method definitions

/**
	Private empty constructor only accessible to the construct_empty
	method called during object allocation and de-serialization.  
 */
process_type_reference::process_type_reference() :
		fundamental_type_reference(), 
		base_proc_def(never_ptr<const process_definition_base>(NULL)) {
	// do not assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_type_reference::process_type_reference(
		const never_ptr<const process_definition_base> pd) :
		fundamental_type_reference(), 
		base_proc_def(pd) {
	NEVER_NULL(base_proc_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_type_reference::process_type_reference(
		const never_ptr<const process_definition_base> pd, 
		const template_actuals& pl) :
		fundamental_type_reference(pl), 
		base_proc_def(pd) {
	NEVER_NULL(base_proc_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Downgrading from canonical to more general type reference.  
 */
process_type_reference::process_type_reference(
		const canonical_process_type& p) :
		fundamental_type_reference(p.get_template_params()), 
		base_proc_def(p.get_base_def()) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_type_reference::~process_type_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_type_reference::what(ostream& o) const {
	return o << "process-type-reference";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const definition_base>
process_type_reference::get_base_def(void) const {
	return base_proc_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
process_type_reference::is_canonical(void) const {
	return !base_proc_def.is_a<const process_definition_alias>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
process_type_reference::is_accepted_in_datatype(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
process_type_reference::is_accepted_in_channel(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The final blessing from the compiler that the template actuals
	meet the requirements specified by the base definition's 
	template formals.  
	Called from unroll_resolve().
	\return good if good.
 */
good_bool
process_type_reference::must_be_valid(void) const {
	return base_proc_def->get_template_formals_manager()
		.must_validate_actuals(template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a translation context between formals and actuals.  
 */
unroll_context
process_type_reference::make_unroll_context(void) const {
	return unroll_context(template_args,
		base_proc_def->get_template_formals_manager());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Makes a copy of this type reference, but with strictly resolved
	constant parameter arguments.  
	NOTE: this procudure will be the model for data and channel types'
		unroll_resolve, so perfect this first!
	TODO: resolve data-type aliases (may already be done now)
	\return a copy of itself, but with type parameters resolved, 
		if applicable.  Returns NULL if there is error in resolution.  
 */
count_ptr<const process_type_reference>
process_type_reference::unroll_resolve(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	typedef	count_ptr<const this_type>	return_type;
	// can this code be factored out to type_ref_base?
	if (template_args) {
		// chaining the contexts solves the problem of having
		// parameters in the same actuals list resolve through
		// formal parameters of different scopes.  
		unroll_context cc(make_unroll_context());
		cc.chain_context(c);
		const template_actuals
			actuals(template_args.unroll_resolve(cc));
		if (actuals) {
			// the final type-check:
			// now they MUST size-type check
			const return_type
				ret(new this_type(base_proc_def, actuals));
			NEVER_NULL(ret);
			return (ret->must_be_valid().good ?
				ret : return_type(NULL));
		} else {
			cerr << "ERROR resolving template arguments." << endl;
			return return_type(NULL);
		}
	} else {
		// need to check must_be_valid?
		const return_type ret(new this_type(base_proc_def));
		INVARIANT(ret->must_be_valid().good);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prototype implementation of unrolling a complete type with
	template actuals specified.  
	TODO: template actuals need to be done for real...
	TODO: this will modify the base definition!
 */
good_bool
process_type_reference::unroll_register_complete_type(void) const {
	cerr << "process_type_reference::unroll_register_complete_type() "
		"in progress..." << endl;
	// base_proc_def is abstract, may be a typedef
	// base_proc_def->instantiate_public_structure(actuals);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Combines relaxed template arguments to make a complete strict type.  
	\param a non-NULL relaxed template actuals.
	\pre this->template_args doesn't already have relaxed actuals, 
		this is asserted in the template_actuals constructor.  
 */
count_ptr<const process_type_reference>
process_type_reference::merge_relaxed_actuals(
		const const_template_args_ptr_type& a) const {
	return count_ptr<const this_type>(new this_type(base_proc_def, 
			template_actuals(template_args, a)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed process instantiation statement object.
 */
excl_ptr<instantiation_statement_base>
process_type_reference::make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d, 
		const const_template_args_ptr_type& a) const {
	return excl_ptr<instantiation_statement_base>(
		new process_instantiation_statement(
			t.is_a<const process_type_reference>(), d, a));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed process instance object.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
excl_ptr<instance_collection_base>
process_type_reference::make_instance_collection(
		const never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const {
	return excl_ptr<instance_collection_base>(
		process_instance_collection::make_array(*s, id, d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// canonical_type<process_definition>
canonical_process_type
process_type_reference::make_canonical_type(void) const {
	return base_proc_def->make_canonical_type(template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const process_type_reference>
process_type_reference::make_canonical_process_type_reference(void) const {
	return base_proc_def->make_canonical_fundamental_type_reference(
		template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct process_type_reference::canonical_compare_result_type {
	typedef	count_ptr<const this_type>	type_type;
	type_type				lt, rt;
	bool					equal;

	canonical_compare_result_type(const this_type&, const this_type&);
};	// end struct canonical_compare_result_type

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// rather not inline
process_type_reference::canonical_compare_result_type
	::canonical_compare_result_type(
		const this_type& l, const this_type& r) :
		lt(l.make_canonical_process_type_reference()),
		rt(r.make_canonical_process_type_reference()) {
	if (!lt || !rt) {
	ICE(cerr, 
		cerr << "got null left-type or right-type "
			"after canonicalization.  FAAAAANNNNG!" << endl;
	)
	}
	const never_ptr<const process_definition_base> ld(lt->base_proc_def);
	const never_ptr<const process_definition_base> rd(rt->base_proc_def);
	INVARIANT(ld && rd);
	INVARIANT(!ld.is_a<const typedef_base>());
	INVARIANT(!rd.is_a<const typedef_base>());
	equal = (ld == rd);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
process_type_reference::may_be_collectibly_type_equivalent(
		const fundamental_type_reference& ft) const {
	const this_type& t(IS_A(const this_type&, ft));	// assert cast
	const canonical_compare_result_type eq(*this, t);
	if (!eq.equal)
		return false;
	else	return eq.lt->template_args.may_be_relaxed_equivalent(
			eq.rt->template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
process_type_reference::may_be_connectibly_type_equivalent(
		const fundamental_type_reference& ft) const {
	const this_type& t(IS_A(const this_type&, ft));	// assert cast
	const canonical_compare_result_type eq(*this, t);
	if (!eq.equal)
		return false;
	else	return eq.lt->template_args.may_be_strict_equivalent(
			eq.rt->template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Takes this type and expands its port subinstances.
	\pre this type must be canonical and resolved to constants.  
	\post the subinstance manager will be populated with
		instance collections of the proper port types, 
		according to the canonical definition.  
 */
void
process_type_reference::unroll_port_instances(
		const unroll_context& c, subinstance_manager& sub) const {
	STACKTRACE_VERBOSE;
	INVARIANT(is_resolved());
	INVARIANT(is_canonical());
	const never_ptr<const process_definition>
		proc_def(base_proc_def.is_a<const process_definition>());
	NEVER_NULL(proc_def);
	const port_formals_manager& port_formals(proc_def->get_port_formals());
	{
		STACKTRACE("local context");
		const template_actuals
			resolved_template_args(template_args.unroll_resolve(c));
		const unroll_context
			cc(resolved_template_args, 
				proc_def->get_template_formals_manager());
		// should the contexts be chained?
		// or can the actuals always be resolved one scope at a time?
		port_formals.unroll_ports(cc, sub);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	base_proc_def->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_type_reference::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	m.write_pointer(f, base_proc_def);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_type_reference::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, base_proc_def);
	parent_type::load_object_base(m, f);
}

//=============================================================================
// class param_type_reference method definitions

/**
	Only used in construction of built-in types.  
 */
param_type_reference::param_type_reference(
		const never_ptr<const built_in_param_def> pd) : 
		fundamental_type_reference(), 	// NULL
		base_param_def(pd) {
	NEVER_NULL(base_param_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_type_reference::~param_type_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_type_reference::what(ostream& o) const {
	return o << "param-type-reference";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const definition_base>
param_type_reference::get_base_def(void) const {
	return base_param_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parameter types cannot be typedef'd.
 */
bool
param_type_reference::is_canonical(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_type_reference::is_accepted_in_datatype(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_type_reference::is_accepted_in_channel(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed param instantiation statement object.
 */
excl_ptr<instantiation_statement_base>
param_type_reference::make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d, 
		const const_template_args_ptr_type& a) const {
	typedef	excl_ptr<instantiation_statement_base>	return_type;
	static const pbool_traits::type_ref_ptr_type&
		pbool_type_ptr(pbool_traits::built_in_type_ptr);
	static const pint_traits::type_ref_ptr_type&
		pint_type_ptr(pint_traits::built_in_type_ptr);
	static const pint_traits::type_ref_ptr_type&
		preal_type_ptr(preal_traits::built_in_type_ptr);
	INVARIANT(t == this);
	INVARIANT(!a);
	if (must_be_type_equivalent(*pbool_type_ptr)) {
		return return_type(new pbool_instantiation_statement(
			pbool_type_ptr, d));
	} else if (must_be_type_equivalent(*pint_type_ptr)) {
		return return_type(new pint_instantiation_statement(
			pint_type_ptr, d));
	} else if (must_be_type_equivalent(*preal_type_ptr)) {
		return return_type(new preal_instantiation_statement(
			preal_type_ptr, d));
	} else {
	ICE(cerr, 
		pbool_type_ptr->dump(cerr) << " at " << &*pbool_type_ptr << endl;
		pint_type_ptr->dump(cerr) << " at " << &*pint_type_ptr << endl;
		preal_type_ptr->dump(cerr) << " at " << &*preal_type_ptr << endl;
		dump(cerr << "this: ") << " at " << this << endl;
	);
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed param instance object.  
	Sort of kludged... built-in type case... YUCK, poor style.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
excl_ptr<instance_collection_base>
param_type_reference::make_instance_collection(
		const never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const {
	// hard coded... yucky, but efficient.  
	static const pbool_traits::type_ref_ptr_type&
		pbool_type_ptr(pbool_traits::built_in_type_ptr);
	static const pint_traits::type_ref_ptr_type&
		pint_type_ptr(pint_traits::built_in_type_ptr);
	static const preal_traits::type_ref_ptr_type&
		preal_type_ptr(preal_traits::built_in_type_ptr);
	if (must_be_type_equivalent(*pbool_type_ptr))
		return excl_ptr<instance_collection_base>(
			pbool_instance_collection::make_array(*s, id, d));
	else if (must_be_type_equivalent(*pint_type_ptr))
		return excl_ptr<instance_collection_base>(
			pint_instance_collection::make_array(*s, id, d));
	else if (must_be_type_equivalent(*preal_type_ptr))
		return excl_ptr<instance_collection_base>(
			preal_instance_collection::make_array(*s, id, d));
	else {
	ICE(cerr, 
		pbool_type_ptr->dump(cerr) << " at " << &*pbool_type_ptr << endl;
		pint_type_ptr->dump(cerr) << " at " << &*pint_type_ptr << endl;
		preal_type_ptr->dump(cerr) << " at " << &*preal_type_ptr << endl;
		dump(cerr << "this: ") << " at " << this << endl;
	);
		return excl_ptr<instance_collection_base>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
null_parameter_type
param_type_reference::make_canonical_type(void) const {
	return null_parameter_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_type_reference::may_be_collectibly_type_equivalent(
		const fundamental_type_reference& ft) const {
	const this_type& t(IS_A(const this_type&, ft));	// assert cast
	return must_be_type_equivalent(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_type_reference::may_be_connectibly_type_equivalent(
		const fundamental_type_reference& ft) const {
	const this_type& t(IS_A(const this_type&, ft));	// assert cast
	return must_be_type_equivalent(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_type_reference::must_be_type_equivalent(const this_type& t) const {
	return base_param_def == t.base_param_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a translation context between formals and actuals.  
 */
unroll_context
param_type_reference::make_unroll_context(void) const {
	return unroll_context();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This really should never be called.  
 */
void
param_type_reference::unroll_port_instances(
		const unroll_context&, subinstance_manager&) const {
	// no-op
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_METHODS_DUMMY_IMPLEMENTATION(param_type_reference)

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_TYPE_REFERENCE_CC__

