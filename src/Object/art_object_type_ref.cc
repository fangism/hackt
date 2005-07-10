/**
	\file "Object/art_object_type_ref.cc"
	Type-reference class method definitions.  
 	$Id: art_object_type_ref.cc,v 1.38.2.13 2005/07/10 19:37:25 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_TYPE_REF_CC__
#define	__OBJECT_ART_OBJECT_TYPE_REF_CC__

#define	ENABLE_STACKTRACE		0

#include <iostream>

#include "AST/art_parser_token_string.h"	// so token_identifier : string
#include "Object/art_object_definition_chan.h"
#include "Object/art_object_definition_data.h"
#include "Object/art_object_definition_proc.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_instance_bool.h"
#include "Object/art_object_instance_int.h"
#include "Object/art_object_instance_enum.h"
#include "Object/art_object_instance_struct.h"
#include "Object/art_object_instance_param.h"
#include "Object/art_object_value_collection.h"
#include "Object/art_object_inst_stmt.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/meta_range_list.h"
#include "Object/art_object_type_hash.h"
#include "util/persistent_object_manager.tcc"
#include "Object/art_built_ins.h"
#include "Object/art_object_int_traits.h"
#include "Object/art_object_bool_traits.h"
#include "Object/art_object_enum_traits.h"
#include "Object/art_object_struct_traits.h"
#include "Object/art_object_chan_traits.h"
#include "Object/art_object_proc_traits.h"
#include "Object/art_object_unroll_context.h"
#include "Object/art_object_inst_stmt_param.h"
#include "Object/art_object_inst_stmt_data.h"
#include "Object/art_object_inst_stmt_chan.h"
#include "Object/art_object_inst_stmt_proc.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/inst/null_collection_type_manager.h"
#include "Object/inst/parameterless_collection_type_manager.h"
#include "Object/inst/int_collection_type_manager.h"

#include "util/sstream.h"
#include "util/indent.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

//=============================================================================
// specializations

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::data_type_reference, DATA_TYPE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::builtin_channel_type_reference,
		BLTIN_CHANNEL_TYPE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_type_reference,
		USER_CHANNEL_TYPE_REFERENCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_type_reference, PROCESS_TYPE_REFERENCE_TYPE_KEY, 0)
}	// end namespace util

namespace ART {
namespace entity {
using std::ostringstream;
#include "util/using_ostream.h"
USING_STACKTRACE
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
	STACKTRACE("~fundamental_type_reference()");
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
/**
	TODO: channel type references may have no base definition!
 */
bool
fundamental_type_reference::may_be_collectibly_type_equivalent(
		const fundamental_type_reference& t) const {
	const count_ptr<const fundamental_type_reference>
		lt(make_canonical_type_reference());
	const count_ptr<const fundamental_type_reference>
		rt(t.make_canonical_type_reference());
	if (!lt || !rt) {
		cerr << "Internal compiler error: "
			"In fundamental_type_reference::may_be_collectibly_type_equivalent(), "
			"got null left-type or right-type after canonicalization.  "
			"FAAAAANNNNG!" << endl;
		THROW_EXIT;
	}
	// NOTE: this will not work on built-in channels!
	// NOTE: will require more re-work with nested template types.
	const never_ptr<const definition_base>
		ld(lt->get_base_def());
	const never_ptr<const definition_base>
		rd(rt->get_base_def());
	INVARIANT(ld && rd);
	INVARIANT(!ld.is_a<const typedef_base>());
	INVARIANT(!rd.is_a<const typedef_base>());
	if (ld != rd)
		return false;
	else	return lt->template_args.may_be_relaxed_equivalent(
			rt->template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is called by instance_collection's
	collection_type_manager<>::commit_type() when checking
	for type consistency between members of the same collection.  
 */
bool
fundamental_type_reference::must_be_collectibly_type_equivalent(
		const fundamental_type_reference& t) const {
	const count_ptr<const fundamental_type_reference>
		lt(make_canonical_type_reference());
	const count_ptr<const fundamental_type_reference>
		rt(t.make_canonical_type_reference());
	if (!lt || !rt) {
		cerr << "Internal compiler error: "
			"In fundamental_type_reference::must_be_collectibly_type_equivalent(), "
			"got null left-type or right-type after canonicalization.  "
			"FAAAAANNNNG!" << endl;
		THROW_EXIT;
	}
	// NOTE: this will not work on built-in channels!
	// NOTE: will require more re-work with nested template types.
	const never_ptr<const definition_base>
		ld(lt->get_base_def());
	const never_ptr<const definition_base>
		rd(rt->get_base_def());
	INVARIANT(ld && rd);
	INVARIANT(!ld.is_a<const typedef_base>());
	INVARIANT(!rd.is_a<const typedef_base>());
	if (ld != rd) {
		return false;
	} else {
#if 1
		const bool ret(lt->template_args.must_be_strict_equivalent(
			rt->template_args));
		if (!ret) {
			cerr << "ERROR: types mismatch!" << endl;
			lt->dump(cerr << "\tgot: ") << endl;
			rt->dump(cerr << "\tand: ") << endl;
		}
		return ret;
#else
		// 2005-07-08 refinement: (on hold... cancelled)
		const template_actuals& largs(lt->template_args);
		const template_actuals& rargs(rt->template_args);
		if (!largs.must_be_relaxed_equivalent(rargs))
			return false;
		const template_actuals::const_arg_list_ptr_type
			l_relaxed(largs.get_relaxed_args());
		const template_actuals::const_arg_list_ptr_type
			r_relaxed(rargs.get_relaxed_args());
		if (l_relaxed && r_relaxed)
			return l_relaxed->must_be_equivalent(*r_relaxed);
		else {
			if (l_relaxed || r_relaxed) {
				lt->dump(cerr << "got: ") << endl;
				rt->dump(cerr << "and: ") << endl;
			}
			INVARIANT(!l_relaxed && !r_relaxed);
			// because otherwise, the two types have different 
			// strictness, which is nonsense when it comes to
			// checking types for the same collection.  
			// Mismatches in strictness should be caught earlier
			// in the front-end, in scopespace::add_instance().
			return true;
		}
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
fundamental_type_reference::may_be_connectibly_type_equivalent(
		const fundamental_type_reference& t) const {
	const count_ptr<const fundamental_type_reference>
		lt(make_canonical_type_reference());
	const count_ptr<const fundamental_type_reference>
		rt(t.make_canonical_type_reference());
	if (!lt || !rt) {
		cerr << "Internal compiler error: "
			"In fundamental_type_reference::may_be_connectibly_type_equivalent(), "
			"got null left-type or right-type after canonicalization.  "
			"FAAAAANNNNG!" << endl;
		THROW_EXIT;
	}
	// NOTE: this will not work on built-in channels!
	// NOTE: will require more re-work with nested template types.
	const never_ptr<const definition_base>
		ld(lt->get_base_def());
	const never_ptr<const definition_base>
		rd(rt->get_base_def());
	INVARIANT(ld && rd);
	INVARIANT(!ld.is_a<const typedef_base>());
	INVARIANT(!rd.is_a<const typedef_base>());
	if (ld != rd)
		return false;
	else	return lt->template_args.may_be_strict_equivalent(
			rt->template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
fundamental_type_reference::must_be_connectibly_type_equivalent(
		const fundamental_type_reference& t) const {
	const count_ptr<const fundamental_type_reference>
		lt(make_canonical_type_reference());
	const count_ptr<const fundamental_type_reference>
		rt(t.make_canonical_type_reference());
	if (!lt || !rt) {
		cerr << "Internal compiler error: "
			"In fundamental_type_reference::must_be_connectibly_type_equivalent(), "
			"got null left-type or right-type after canonicalization.  "
			"FAAAAANNNNG!" << endl;
		THROW_EXIT;
	}
	// NOTE: this will not work on built-in channels!
	// NOTE: will require more re-work with nested template types.
	const never_ptr<const definition_base>
		ld(lt->get_base_def());
	const never_ptr<const definition_base>
		rd(rt->get_base_def());
	INVARIANT(ld && rd);
	INVARIANT(!ld.is_a<const typedef_base>());
	INVARIANT(!rd.is_a<const typedef_base>());
	if (ld != rd)
		return false;
	else	return lt->template_args.must_be_strict_equivalent(
			rt->template_args);
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
	STACKTRACE("fund_type_ref::write_object_base()");
	template_args.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
fundamental_type_reference::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE("fund_type_ref::load_object_base()");
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
data_type_reference::~data_type_reference() {
	STACKTRACE("~data_type_reference()");
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
/**
	Makes a copy of this type reference, but with strictly resolved
	constant parameter arguments.  
	Will eventually require a context-like object.  
	\todo resolve data-type aliases.  
	\return a copy of itself, but with type parameters resolved, 
		if applicable.  Returns NULL if there is error in resolution.  
 */
count_ptr<const data_type_reference>
data_type_reference::unroll_resolve(unroll_context& c) const {
	STACKTRACE("data_type_reference::unroll_resolve()");
	typedef	count_ptr<const this_type>	return_type;
	// can this code be factored out to type_ref_base?
	if (template_args) {
		// if template actuals depends on other template parameters, 
		// then we need to pass actuals into its own context!
		const template_actuals_transformer
			uc(c, template_args, 
				base_type_def->get_template_formals_manager());
		const template_actuals
			actuals(template_args.unroll_resolve(c));
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
		if (alias == &bool_def) {
			return return_type(
				bool_instance_collection::make_array(*s, id, d));
		} else if (alias == &int_def) {
			return return_type(
				int_instance_collection::make_array(*s, id, d));
		} else {
			DIE;	// WTF!?
			return return_type(NULL);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenience function for creating data int type-references.
	\returns newly allocated integer type reference.
 */
data_type_reference*
data_type_reference::make_quick_int_type_ref(const pint_value_type w) {
	// is an excl_ptr, incidentally
	const fundamental_type_reference::template_args_ptr_type
		width_params(new const_param_expr_list(
			count_ptr<const pint_const>(new pint_const(w))));
	const template_actuals tpl(width_params, 
		template_actuals::arg_list_ptr_type());
	return new data_type_reference(
		never_ptr<const built_in_datatype_def>(&int_def), tpl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a type reference to a non-typedef data type.  
 */
count_ptr<const fundamental_type_reference>
data_type_reference::make_canonical_type_reference(void) const {
	// INVARIANT(template_args.is_constant());		// NOT true
	return base_type_def->make_canonical_type_reference(template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	STACKTRACE("data_type_ref::collect_transients()");
	base_type_def->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_type_reference::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	STACKTRACE("data_type_ref::write_object()");
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
	STACKTRACE("data_type_ref::load_object()");
	m.read_pointer(f, base_type_def);
	parent_type::load_object_base(m, f);

	// MINOR HACK: shallow recursion and intercept built-in types
	// TODO: ALERT!!! case where base_type_def is a typedef alias?
	m.load_object_once(
		const_cast<datatype_definition_base*>(&*base_type_def));
	if (base_type_def->get_key() == "bool") {
		m.please_delete(&*base_type_def);	// HACKERY
		base_type_def =
			never_ptr<const datatype_definition_base>(&bool_def);
		// must flag visit specially
	} else if (base_type_def->get_key() == "int") {
		m.please_delete(&*base_type_def);	// HACKERY
		base_type_def =
			never_ptr<const datatype_definition_base>(&int_def);
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
channel_type_reference_base::dump_direction(ostream& o) const {
	if (direction == '!' || direction == '?')
		o << direction;
	// else no direction
	return o;
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
	dump_direction(o);
	o << '(';
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
	if (direction == '!' || direction == '?')
		o << direction;
	o << '(' << endl;
	{
	INDENT_SECTION(o); 
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
	return never_ptr<const built_in_channel_def>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only applicable if datatype_list_type is a vector.  
 */
void
builtin_channel_type_reference::reserve_datatypes(const size_t s) {
	datatype_list.reserve(s);
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
never_ptr<const builtin_channel_type_reference>
builtin_channel_type_reference::resolve_builtin_channel_type(void) const {
	typedef	never_ptr<const builtin_channel_type_reference>	return_type;
	return return_type(this);
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
	TODO: built-in channel types don't have their own template
	signatures (template_formals_manager) or home-base definitions, 
	however the data-types carried in the ports may be dependent
	on template parameters.  
	e.g. (implicitly) template <pint W> chan(int<W>) ...
	So really the canonical types of the data members should be
	resolved using the channel-definition's template signature.  
	How can that be passed down?
	Use a different signature context?
 */
count_ptr<const fundamental_type_reference>
builtin_channel_type_reference::make_canonical_type_reference(void) const {
	typedef count_ptr<const fundamental_type_reference>	return_type;
	cerr << "Fang, finish builtin_channel_type_reference::"
		"make_canonical_type_reference()!" << endl;
	return return_type(NULL);
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
	return dump_direction(fundamental_type_reference::dump(o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const definition_base>
channel_type_reference::get_base_def(void) const {
	return base_chan_def;
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
		cerr << "Fang, finish channel_type_reference::resolve_builtin_channel_type(): "
			"case where base_chan_def is not user_def_chan."
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
count_ptr<const fundamental_type_reference>
channel_type_reference::make_canonical_type_reference(void) const {
	typedef	count_ptr<const fundamental_type_reference>	return_type;
	cerr << "Fang, finish channel_type_reference::"
		"make_canonical_type_reference()!" << endl;
	return return_type(NULL);
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
	Makes a copy of this type reference, but with strictly resolved
	constant parameter arguments.  
	NOTE: this procudure will be the model for data and channel types'
		unroll_resolve, so perfect this first!
	TODO: resolve data-type aliases (may already be done now)
	\return a copy of itself, but with type parameters resolved, 
		if applicable.  Returns NULL if there is error in resolution.  
 */
count_ptr<const process_type_reference>
process_type_reference::unroll_resolve(unroll_context& c) const {
	STACKTRACE("process_type_reference::unroll_resolve()");
	typedef	count_ptr<const this_type>	return_type;
	// can this code be factored out to type_ref_base?
	if (template_args) {
		// if template actuals depends on other template parameters, 
		// then we need to pass actuals into its own context!
		const template_actuals_transformer
			uc(c, template_args, 
				base_proc_def->get_template_formals_manager());
		const template_actuals
			actuals(template_args.unroll_resolve(c));
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
/**
	NOTE: we allow canonicalization using non-const expressions
		in the type actuals.  
 */
count_ptr<const fundamental_type_reference>
process_type_reference::make_canonical_type_reference(void) const {
	// INVARIANT(template_args.is_constant());	// not true
	return base_proc_def->make_canonical_type_reference(template_args);
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
	INVARIANT(t == this);
	INVARIANT(!a);
	if (this->must_be_connectibly_type_equivalent(*pbool_type_ptr)) {
		return return_type(new pbool_instantiation_statement(
			pbool_type_ptr, d));
	} else if (this->must_be_connectibly_type_equivalent(*pint_type_ptr)) {
		return return_type(new pint_instantiation_statement(
			pint_type_ptr, d));
	} else {
		pbool_type_ptr->dump(cerr) << " at " << &*pbool_type_ptr << endl;
		pint_type_ptr->dump(cerr) << " at " << &*pint_type_ptr << endl;
		dump(cerr) << " at " << this << endl;
		DIE;		// WTF?
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
	if (this->must_be_connectibly_type_equivalent(*pbool_type_ptr))
		return excl_ptr<instance_collection_base>(
			pbool_instance_collection::make_array(*s, id, d));
	else if (this->must_be_connectibly_type_equivalent(*pint_type_ptr))
		return excl_ptr<instance_collection_base>(
			pint_instance_collection::make_array(*s, id, d));
	else {
		pbool_type_ptr->dump(cerr) << " at " << &*pbool_type_ptr << endl;
		pint_type_ptr->dump(cerr) << " at " << &*pint_type_ptr << endl;
		dump(cerr) << " at " << this << endl;
		DIE;		// WTF?
		return excl_ptr<instance_collection_base>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre template actuals have beeen resolved to constants.  
 */
count_ptr<const fundamental_type_reference>
param_type_reference::make_canonical_type_reference(void) const {
	static const pbool_traits::type_ref_ptr_type&
		pbool_type_ptr(pbool_traits::built_in_type_ptr);
	static const pint_traits::type_ref_ptr_type&
		pint_type_ptr(pint_traits::built_in_type_ptr);
	typedef	count_ptr<const fundamental_type_reference>	return_type;
	INVARIANT(!template_args);
	// don't return copy-construction, use built-in type refs
	return count_ptr<const fundamental_type_reference>(
		new this_type(*this));	// copy-constructor
	if (this->must_be_connectibly_type_equivalent(*pbool_type_ptr))
		return pbool_type_ptr;
	else if (this->must_be_connectibly_type_equivalent(*pint_type_ptr))
		return pint_type_ptr;
	else	DIE;
	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_METHODS_DUMMY_IMPLEMENTATION(param_type_reference)

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_TYPE_REF_CC__

