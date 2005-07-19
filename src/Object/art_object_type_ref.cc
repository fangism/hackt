/**
	\file "Object/art_object_type_ref.cc"
	Type-reference class method definitions.  
 	$Id: art_object_type_ref.cc,v 1.38.2.20 2005/07/19 05:22:07 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_TYPE_REF_CC__
#define	__OBJECT_ART_OBJECT_TYPE_REF_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

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
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/art_object_unroll_context.h"
#include "Object/art_object_inst_stmt_param.h"
#include "Object/art_object_inst_stmt_data.h"
#include "Object/art_object_inst_stmt_chan.h"
#include "Object/art_object_inst_stmt_proc.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/inst/null_collection_type_manager.h"
#include "Object/inst/parameterless_collection_type_manager.h"
#include "Object/inst/int_collection_type_manager.h"
#include "Object/inst/subinstance_manager.h"

#include "util/sstream.h"
#include "util/indent.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/reserve.h"

// conditional defines, after including "stacktrace.h"
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define STACKTRACE_DTOR(x)
#endif

#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif

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
using std::transform;
using std::back_inserter;
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
#if 1
	// HACK: to catch built-in channel types as a temporary workaround.
	const count_ptr<const builtin_channel_type_reference>
		lbcr(lt.is_a<const builtin_channel_type_reference>());
	const count_ptr<const builtin_channel_type_reference>
		rbcr(rt.is_a<const builtin_channel_type_reference>());
	if (lbcr && rbcr) {
		return lbcr->may_be_collectibly_channel_type_equivalent(*rbcr);
	} else if (lbcr ^ rbcr) {
		// type_mismatch_error(cerr, *lt, *rt);
		return false;
	}
#endif
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
#if 1
	// HACK: to catch built-in channel types as a temporary workaround.
	const count_ptr<const builtin_channel_type_reference>
		lbcr(lt.is_a<const builtin_channel_type_reference>());
	const count_ptr<const builtin_channel_type_reference>
		rbcr(rt.is_a<const builtin_channel_type_reference>());
	if (lbcr && rbcr) {
		return lbcr->must_be_collectibly_channel_type_equivalent(*rbcr);
	} else if (lbcr ^ rbcr) {
		type_mismatch_error(cerr, *lt, *rt);
		return false;
	}
#endif
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
		// type_mismatch_error(cerr, *lt, *rt);
		return false;
	} else {
		const bool ret(lt->template_args.must_be_strict_equivalent(
			rt->template_args));
		if (!ret) {
			type_mismatch_error(cerr, *lt, *rt);
		}
		return ret;
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
#if 1
	// HACK: to catch built-in channel types as a temporary workaround.
	const count_ptr<const builtin_channel_type_reference>
		lbcr(lt.is_a<const builtin_channel_type_reference>());
	const count_ptr<const builtin_channel_type_reference>
		rbcr(rt.is_a<const builtin_channel_type_reference>());
	if (lbcr && rbcr) {
		return lbcr->may_be_connectibly_channel_type_equivalent(*rbcr);
	} else if (lbcr ^ rbcr) {
		// type_mismatch_error(cerr, *lt, *rt);
		return false;
	}
#endif
	const never_ptr<const definition_base>
		ld(lt->get_base_def());
	const never_ptr<const definition_base>
		rd(rt->get_base_def());
	INVARIANT(ld && rd);
	INVARIANT(!ld.is_a<const typedef_base>());
	INVARIANT(!rd.is_a<const typedef_base>());
	if (ld != rd) {
		// type_mismatch_error(cerr, *lt, *rt);
		return false;
	} else {
		const bool ret(lt->template_args.may_be_strict_equivalent(
			rt->template_args));
#if 0
		if (!ret) {
			type_mismatch_error(cerr, *lt, *rt);
		}
#endif
		return ret;
	}
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
#if 1
	// HACK: to catch built-in channel types as a temporary workaround.
	const count_ptr<const builtin_channel_type_reference>
		lbcr(lt.is_a<const builtin_channel_type_reference>());
	const count_ptr<const builtin_channel_type_reference>
		rbcr(rt.is_a<const builtin_channel_type_reference>());
	if (lbcr && rbcr) {
		return lbcr->must_be_connectibly_channel_type_equivalent(*rbcr);
	} else if (lbcr ^ rbcr) {
		type_mismatch_error(cerr, *lt, *rt);
		return false;
	}
#endif
	const never_ptr<const definition_base>
		ld(lt->get_base_def());
	const never_ptr<const definition_base>
		rd(rt->get_base_def());
	INVARIANT(ld && rd);
	INVARIANT(!ld.is_a<const typedef_base>());
	INVARIANT(!rd.is_a<const typedef_base>());
	if (ld != rd) {
		// type_mismatch_error(cerr, *lt, *rt);
		return false;
	} else {
		const bool ret(lt->template_args.must_be_strict_equivalent(
			rt->template_args));
		if (!ret) {
			type_mismatch_error(cerr, *lt, *rt);
		}
		return ret;
	}
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
	Creates a translation context between formals and actuals.  
 */
unroll_context
data_type_reference::make_unroll_context(void) const {
	return unroll_context(template_args,
		base_type_def->get_template_formals_manager());
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
/**
	Transient work around for special cases only.  
	TODO (2005-07-12): implement for real.
 */
void
data_type_reference::unroll_port_instances(const unroll_context& c,
		subinstance_manager& sub) const {
#if 0
	INVARIANT(is_canonical());
	INVARIANT(is_resolved());
#else
	if (base_type_def == &bool_traits::built_in_definition) {
		// do nothing!
	} else if (base_type_def == &bool_traits::built_in_definition) {
		// do nothing... for now
		// don't really anticipate expanding bits fields.  
	} else if (base_type_def.is_a<const enum_datatype_def>()) {
		// do nothing
	} else {
		cerr << "FANG, finish data_type_reference::unroll_port_instances()!" << endl;
	}
#endif
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
	return new data_type_reference(never_ptr<const built_in_datatype_def>(
		&int_traits::built_in_definition), tpl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
data_type_reference::make_canonical_data_type_reference(void) const {
	// INVARIANT(template_args.is_constant());		// NOT true
	return base_type_def->make_canonical_type_reference(template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a type reference to a non-typedef data type.  
 */
count_ptr<const fundamental_type_reference>
data_type_reference::make_canonical_type_reference(void) const {
//	return base_type_def->make_canonical_type_reference(template_args);
	return make_canonical_data_type_reference();
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

	// MINOR HACK: shallow recursion and intercept built-in types
	// TODO: ALERT!!! case where base_type_def is a typedef alias?
	m.load_object_once(
		const_cast<datatype_definition_base*>(&*base_type_def));
	if (base_type_def->get_key() == "bool") {
		m.please_delete(&*base_type_def);	// HACKERY
		base_type_def = never_ptr<const datatype_definition_base>(
			&bool_traits::built_in_definition);
		// must flag visit specially
	} else if (base_type_def->get_key() == "int") {
		m.please_delete(&*base_type_def);	// HACKERY
		base_type_def = never_ptr<const datatype_definition_base>(
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
/**
	Evaluates whether or not two built-in channel types may be
	collectibly equivalent.  
 */
bool
builtin_channel_type_reference::may_be_collectibly_channel_type_equivalent(
		const this_type& t) const {
	const size_t ls(datatype_list.size());
	const size_t rs(t.datatype_list.size());
	if (ls != rs) {
		type_mismatch_error(cerr, *this, t);
		return false;
	}
	// use functional inner_product?
	// find mismatch?
	typedef datatype_list_type::const_iterator	const_iterator;
	const_iterator i(datatype_list.begin());
	const_iterator j(t.datatype_list.begin());
	const const_iterator e(datatype_list.end());
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
	Evaluates whether or not two built-in channel types are
	collectibly equivalent.  
 */
bool
builtin_channel_type_reference::must_be_collectibly_channel_type_equivalent(
		const this_type& t) const {
	const size_t ls(datatype_list.size());
	const size_t rs(t.datatype_list.size());
	if (ls != rs) {
		type_mismatch_error(cerr, *this, t);
		return false;
	}
	// use functional inner_product?
	// find mismatch?
	typedef datatype_list_type::const_iterator	const_iterator;
	const_iterator i(datatype_list.begin());
	const_iterator j(t.datatype_list.begin());
	const const_iterator e(datatype_list.end());
	for ( ; i!=e; i++, j++) {
		if (!(*i)->must_be_collectibly_type_equivalent(**j)) {
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
	const size_t ls(datatype_list.size());
	const size_t rs(t.datatype_list.size());
	if (ls != rs) {
		type_mismatch_error(cerr, *this, t);
		return false;
	}
	// use functional inner_product?
	// find mismatch?
	typedef datatype_list_type::const_iterator	const_iterator;
	const_iterator i(datatype_list.begin());
	const_iterator j(t.datatype_list.begin());
	const const_iterator e(datatype_list.end());
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
/**
	Evaluates whether or not two built-in channel types are
	connectibly equivalent.  
 */
bool
builtin_channel_type_reference::must_be_connectibly_channel_type_equivalent(
		const this_type& t) const {
	const size_t ls(datatype_list.size());
	const size_t rs(t.datatype_list.size());
	if (ls != rs) {
		type_mismatch_error(cerr, *this, t);
		return false;
	}
	// use functional inner_product?
	// find mismatch?
	typedef datatype_list_type::const_iterator	const_iterator;
	const_iterator i(datatype_list.begin());
	const_iterator j(t.datatype_list.begin());
	const const_iterator e(datatype_list.end());
	for ( ; i!=e; i++, j++) {
		if (!(*i)->must_be_connectibly_type_equivalent(**j)) {
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
	typedef	count_ptr<const this_type>	return_type;
	// can this code be factored out to type_ref_base?
#if 1
	INVARIANT(!template_args);
	const count_ptr<this_type> ret(new this_type);
	NEVER_NULL(ret);
	util::reserve(ret->datatype_list, datatype_list.size());
	transform(datatype_list.begin(), datatype_list.end(),
		back_inserter(ret->datatype_list), 
		datatype_resolver(c)	// helper functor
			// TODO: may need context passed in!
	);
	return ret;
#else
	// NOTE: built-in channel types (currently) cannot have template args
	// will have to resolve types of various data members
	cerr << "FANG, finish builtin_channel_type_reference::unroll_resolve()!"
		<< endl;
	return return_type(NULL);
#endif
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
#if 0
	cerr << "Fang, finish builtin_channel_type_reference::"
		"make_canonical_type_reference()!" << endl;
	return return_type(NULL);
#else
	INVARIANT(!template_args);
	const count_ptr<this_type> ret(new this_type);
	NEVER_NULL(ret);
	util::reserve(ret->datatype_list, datatype_list.size());
	transform(datatype_list.begin(), datatype_list.end(),
		back_inserter(ret->datatype_list), 
		datatype_canonicalizer()	// helper functor
	);
	return ret;
#endif
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
	cerr << "FANG, finish builtin_channel_type_reference::unroll_port_instances()!" << endl;
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
				ret(new this_type(base_chan_def, actuals));
			NEVER_NULL(ret);
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
	return base_chan_def->make_canonical_type_reference(template_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::unroll_port_instances(
		const unroll_context& c, 
		subinstance_manager& sub) const {
	cerr << "FANG, finish channel_type_reference::unroll_port_instances()!"
			<< endl;
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
bool
process_type_reference::is_canonical(void) const {
	return !base_proc_def.is_a<const process_definition_alias>();
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
	STACKTRACE("process_type_reference::unroll_resolve()");
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
#if 1
		const unroll_context
			cc(resolved_template_args, 
				proc_def->get_template_formals_manager());
		// should the contexts be chained?
		// or can the actuals always be resolved one scope at a time
#else
		// hold on, this is not equivalent
		const unroll_context cc(make_unroll_context());
#endif
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
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_TYPE_REF_CC__

