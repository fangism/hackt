/**
	\file "art_object_type_ref.cc"
	Type-reference class method definitions.  
 	$Id: art_object_type_ref.cc,v 1.23.6.1 2005/02/02 07:59:48 fang Exp $
 */

#ifndef	__ART_OBJECT_TYPE_REF_CC__
#define	__ART_OBJECT_TYPE_REF_CC__

#define	ENABLE_STACKTRACE		0

#include <iostream>

#include "art_parser_base.h"	// so token_identifier : string
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_instance_bool.h"
#include "art_object_instance_int.h"
#include "art_object_instance_enum.h"
#include "art_object_instance_struct.h"
#include "art_object_instance_param.h"
#include "art_object_inst_stmt.h"
#include "art_object_expr_base.h"
#include "art_object_type_hash.h"
#include "persistent_object_manager.tcc"
#include "art_built_ins.h"

#include "sstream.h"
#include "stacktrace.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {
using std::ostringstream;
USING_STACKTRACE

//=============================================================================
// class fundamental_type_reference method definitions

fundamental_type_reference::fundamental_type_reference(
		excl_ptr<const param_expr_list>& pl) :
		type_reference_base(), template_params(pl) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
fundamental_type_reference::fundamental_type_reference(void) :
		type_reference_base(), template_params() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
fundamental_type_reference::~fundamental_type_reference() {
	STACKTRACE("~fundamental_type_reference()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
fundamental_type_reference::dump(ostream& o) const {
//	STACKTRACE("fundamental_type_reference::dump()");
	return o << hash_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluates type reference as a flat string, for caching purposes.  
	We unconditionally add the <> to the key even if there is no template
	specifier to guarantee that hash_string for a type-reference 
	cannot collide with the hash string for the non-templated definition.  
	Should we use fully qualified names for hashing?
	\return string to be used for hashing.  
 */
string
fundamental_type_reference::hash_string(void) const {
//	STACKTRACE("fundamental_type_reference::hash_string()");
	// use fully qualified?  for hashing, no.
	// possible collision case?
	return get_base_def()->get_name() +template_param_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
fundamental_type_reference::template_param_string(void) const {
	string ret("<");
	if (template_params) {
		ostringstream o;
		template_params->dump(o);
		ret += o.str();
	}
	ret += ">";
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
fundamental_type_reference::get_qualified_name(void) const {
	return get_base_def()->get_qualified_name() +template_param_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<param_expr_list>
fundamental_type_reference::get_copy_template_params(void) const {
	if (template_params)
		return template_params->make_copy();
	else	return excl_ptr<param_expr_list>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
UNVEIL LATER
/**
	Resolve canonical type.  Flattens the type through typedefs.  
	Precondition: all template parameters must have been resolved to 
	constants.  Other formals acceptable, as non-constants?
	Passes its template_params top-down.  
	Implementation: converts list of template parameters into 
	a hash_map from param_instance_collection (identifier) to 
	actual parameter expression.  Effectively does substitution.  
	\return the base equivalent type, unraveling parameters
		through typedefs.  If base definition is not a typedef, 
		simply returns a deep copy of itself.  
 */
excl_ptr<const fundamental_type_reference>
fundamental_type_reference::resolve_canonical_type(void) const {
	typedef	excl_ptr<const fundamental_type_reference>	return_type;
	never_ptr<const definition_base>
		base_def(get_base_def());
	NEVER_NULL(base_def);
if (base_def.is_a<typedef_base>()) {
	// then we need to resolve it recursively
	// precondition: actuals' and formals' types already checked
	// first, construct the actuals map (yes, on the stack)
	template_actuals_map_type actuals_map;
	if (template_params) {
		base_def->fill_template_actuals_map(
			actuals_map, *template_params);
		// Make a deep copy of the typedef's template parameters, 
		// substituting appropriately in the parameter expressions.

		// How shall we substitute?  major dilemma... PUNT
	} else {
		// else leave it empty, no further context should be required 
		// for the referenced type.  
	}
	// FINISH ME
} else {
	// is not a typedef, just return a deep-copy of itself.
	if (template_params)
		return return_type(base_def->make_fundamental_type_reference(
			template_params->make_copy()));
	else
		return return_type(base_def->make_fundamental_type_reference());
}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// is static
excl_ptr<instantiation_statement>
fundamental_type_reference::make_instantiation_statement(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) {
	return t->make_instantiation_statement_private(t, d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns true if the types *may* be equivalent.  
	Easy for non-template types, but for template types, 
	sometimes parameters may be determined by other parameters
	and thus are not statically constant for comparison.  
	\return false if there is definite mismatch in type.  
	TO DO: resolve typedefs!
 */
bool
fundamental_type_reference::may_be_equivalent(
		const fundamental_type_reference& t) const {
	never_ptr<const definition_base> left(get_base_def());
	never_ptr<const definition_base> right(t.get_base_def());

	bool have_typedef = false;

	// TO resolve typedefs and aliases
	// self-recursive call to expand parameters...
	// or PUNT unrolling actual parameters until later...
	never_ptr<const typedef_base>
		ltdb(left.is_a<const typedef_base>());
	never_ptr<const typedef_base>
		rtdb(right.is_a<const typedef_base>());
	while (ltdb) {
		have_typedef = true;
		left = ltdb->get_base_type_ref()->get_base_def();
		ltdb = left.is_a<const typedef_base>();
	}
	while (rtdb) {
		have_typedef = true;
		right = rtdb->get_base_type_ref()->get_base_def();
		rtdb = right.is_a<const typedef_base>();
	}
#if 0
	if (ltdb) {
		ltdb->resolve_complete_type(template_params);
	}
	if (rtdb) {
		rtdb->resolve_complete_type(t.template_params);
	}
	INVARIANT(!ltdb && !rtdb);		// down to canonical definitions
#endif
	if (have_typedef) {
		if (left != right)
			return false;
		// overly conservative: doesn't even examine template params
		// unrolling of actual template param actuals will
		// eventually be checked.  
		// In other words, I'm punting this for now.  
		else	return true;
		// should compare flattened unrolled param lists
		// after substitutions (ideally)
	}
	// else continue normal type-check comparison

	if (left != right) {
#if 0
		left->dump(cerr << "left: ") << endl;
		right->dump(cerr << "right: ") << endl;
#endif
		return false;
	}
	// else base definitions are equal
	if (template_params) {
		if (!t.template_params) {
			// template params should be filled in with defaults
			// as necessary.  
			return false;
		} else {
			return template_params->may_be_equivalent(
				*t.template_params);
		}
	} else {
		return (!t.template_params);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must be equivalent.  
	Conservatively returns false.  
 */
bool
fundamental_type_reference::must_be_equivalent(
		const fundamental_type_reference& t) const {
	const never_ptr<const definition_base> left(get_base_def());
	const never_ptr<const definition_base> right(t.get_base_def());
	// may need to resolve alias? TO DO
	if (left != right) {
#if 0
		left->dump(cerr << "left: ") << endl;
		right->dump(cerr << "right: ") << endl;
#endif
		return false;
	}
	// else base definitions are equal
	// TO DO: compare template arguments
	// if values are default, do we fill in template args?  we should
	if (template_params) {
		cerr << "TO DO: finish static parameter comparison!" << endl;
		if (!t.template_params) {
			// template params should be filled in with defaults
			// as necessary.  
			return false;
		} else {
			return template_params->must_be_equivalent(
				*t.template_params);
		}
	} else {
		// both must be NULL
		return (!t.template_params);
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
declared now, 
UNVEIL LATER
/**
	Takes a type reference with a typedef and resolves it to 
	its canonical definition, with parameters substituted
	appropriately.  
	We need a method for substituting parameters.  
	What should this method do if this is already canonical?
	Preconditions: parameters must be initialized?
 */
excl_ptr<const fundamental_type_reference>
	make_canonical_type_reference(void) const {

}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
fundamental_type_reference::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (template_params)
		template_params->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
fundamental_type_reference::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE("fund_type_ref::write_object_base()");
	m.write_pointer(o, template_params);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
fundamental_type_reference::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE("fund_type_ref::load_object_base()");
	m.read_pointer(i, template_params);
	if (template_params)
		m.load_object(const_cast<param_expr_list*>(&*template_params));
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

DEFAULT_PERSISTENT_TYPE_REGISTRATION(data_type_reference, 
	DATA_TYPE_REFERENCE_TYPE_KEY)

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
		excl_ptr<const param_expr_list>& pl) :
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
	Makes a copy of this type reference, but with strictly resolved
	constant parameter arguments.  
	Will eventually require a context-like object.  
	\return a copy of itself, but with type parameters resolved, 
		if applicable.  Returns NULL if there is error in resolution.  
 */
count_ptr<const data_type_reference>
data_type_reference::unroll_resolve(unroll_context& c) const {
	STACKTRACE("data_type_reference::unroll_resolve()");
	typedef	count_ptr<const data_type_reference>	return_type;
	// eventually pass a context argument
	if (template_params) {
		excl_ptr<const param_expr_list>
			actuals = template_params->unroll_resolve(c)
				.as_a_xfer<const param_expr_list>();
		if (actuals) {
			return return_type(new data_type_reference(
				base_type_def, actuals));
		} else {
			cerr << "ERROR resolving template arguments." << endl;
			return return_type(NULL);
		}
	} else {
		return return_type(new data_type_reference(base_type_def));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed data instantiation statement object.
 */
excl_ptr<instantiation_statement>
data_type_reference::make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) const {
	return excl_ptr<instantiation_statement>(
		new data_instantiation_statement(
			t.is_a<const data_type_reference>(), d));
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
		return return_type(struct_instance_collection
			::make_struct_array(*s, id, d));
	} else if (alias.is_a<const enum_datatype_def>()) {
		return return_type(enum_instance_collection
			::make_enum_array(*s, id, d));
	} else {
		// what about typedefs/aliases of built-in types? Ahhhh....
		INVARIANT(alias.is_a<const built_in_datatype_def>());
		// just compare pointers
		if (alias == &bool_def) {
			return return_type(bool_instance_collection
				::make_bool_array(*s, id, d));
		} else if (alias == &int_def) {
			return return_type(int_instance_collection
				::make_int_array(*s, id, d));
		} else {
			DIE;	// WTF!?
			return return_type(NULL);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DATA_TYPE_REFERENCE_TYPE_KEY)) {
	STACKTRACE("data_type_ref::collect_transients()");
	base_type_def->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
data_type_reference::construct_empty(const int i) {
	return new data_type_reference();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_type_reference::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	STACKTRACE("data_type_ref::write_object()");
//	ostream& f = m.lookup_write_buffer(this);
//	WRITE_POINTER_INDEX(f, m);		// sanity check
	m.write_pointer(f, base_type_def);
	parent_type::write_object_base(m, f);
//	WRITE_OBJECT_FOOTER(f);			// sanity check
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	May need special case handling for built-in definitions!
 */
void
data_type_reference::load_object(const persistent_object_manager& m, 
		istream& f) {
// if (!m.flag_visit(this)) {
	STACKTRACE("data_type_ref::load_object()");
//	istream& f = m.lookup_read_buffer(this);
//	STRIP_POINTER_INDEX(f, m);		// sanity check
	m.read_pointer(f, base_type_def);
	parent_type::load_object_base(m, f);
//	STRIP_OBJECT_FOOTER(f);			// sanity check

	// MINOR HACK: recursion and intercept built-in types
	// TODO: ALERT!!! case where base_type_def is a typedef alias?
	m.load_object(const_cast<datatype_definition_base*>(&*base_type_def));
	if (base_type_def->get_key() == "bool")
		base_type_def =
			never_ptr<const datatype_definition_base>(&bool_def);
	else if (base_type_def->get_key() == "int")
		base_type_def =
			never_ptr<const datatype_definition_base>(&int_def);
	// else leave the base definition as is
	// reference count will take care of discarded memory :)
// }
// else already visited
}

//=============================================================================
// class channel_type_reference method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(channel_type_reference, 
	CHANNEL_TYPE_REFERENCE_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
channel_type_reference::channel_type_reference() :
		fundamental_type_reference(), 
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
		excl_ptr<const param_expr_list>& pl) :
		fundamental_type_reference(pl), 
		base_chan_def(cd) {
	NEVER_NULL(base_chan_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_type_reference::channel_type_reference(
		const never_ptr<const channel_definition_base> cd) :
		fundamental_type_reference(), 	// NULL
		base_chan_def(cd) {
	NEVER_NULL(base_chan_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_type_reference::~channel_type_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_type_reference::what(ostream& o) const {
	return o << "channel-type-reference";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const definition_base>
channel_type_reference::get_base_def(void) const {
	return base_chan_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed channel instantiation statement object.
 */
excl_ptr<instantiation_statement>
channel_type_reference::make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) const {
	return excl_ptr<instantiation_statement>(
		new channel_instantiation_statement(
			t.is_a<const channel_type_reference>(), d));
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
		channel_instance_collection::make_chan_array(*s, id, d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, CHANNEL_TYPE_REFERENCE_TYPE_KEY)) {
	base_chan_def->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
channel_type_reference::construct_empty(const int i) {
	return new channel_type_reference();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::write_object(const persistent_object_manager& m, 
		ostream& f) const {
//	ostream& f = m.lookup_write_buffer(this);
//	WRITE_POINTER_INDEX(f, m);		// sanity check
	m.write_pointer(f, base_chan_def);
	parent_type::write_object_base(m, f);
//	WRITE_OBJECT_FOOTER(f);			// sanity check
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::load_object(const persistent_object_manager& m, 
		istream& f) {
// if (!m.flag_visit(this)) {
//	istream& f = m.lookup_read_buffer(this);
//	STRIP_POINTER_INDEX(f, m);		// sanity check
	m.read_pointer(f, base_chan_def);
	parent_type::load_object_base(m, f);
//	STRIP_OBJECT_FOOTER(f);			// sanity check
// }
// else already visited
}

//=============================================================================
// class process_type_reference method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(process_type_reference, 
	PROCESS_TYPE_REFERENCE_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		excl_ptr<const param_expr_list>& pl) :
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
	Returns a newly constructed process instantiation statement object.
 */
excl_ptr<instantiation_statement>
process_type_reference::make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) const {
	return excl_ptr<instantiation_statement>(
		new process_instantiation_statement(
			t.is_a<const process_type_reference>(), d));
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
		process_instance_collection::make_proc_array(*s, id, d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PROCESS_TYPE_REFERENCE_TYPE_KEY)) {
	base_proc_def->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
process_type_reference::construct_empty(const int i) {
	return new process_type_reference();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_type_reference::write_object(const persistent_object_manager& m, 
		ostream& f) const {
//	ostream& f = m.lookup_write_buffer(this);
//	WRITE_POINTER_INDEX(f, m);		// sanity check
	m.write_pointer(f, base_proc_def);
	parent_type::write_object_base(m, f);
//	WRITE_OBJECT_FOOTER(f);			// sanity check
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_type_reference::load_object(const persistent_object_manager& m, 
		istream& f) {
// if (!m.flag_visit(this)) {
//	istream& f = m.lookup_read_buffer(this);
//	STRIP_POINTER_INDEX(f, m);		// sanity check
	m.read_pointer(f, base_proc_def);
	parent_type::load_object_base(m, f);
//	STRIP_OBJECT_FOOTER(f);			// sanity check
// }
// else already visited
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
	Returns a newly constructed param instantiation statement object.
 */
excl_ptr<instantiation_statement>
param_type_reference::make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) const {
	typedef	excl_ptr<instantiation_statement>	return_type;
	INVARIANT(t == this);
	if (this->must_be_equivalent(*pbool_type_ptr))
		return return_type(new pbool_instantiation_statement(d));
	else if (this->must_be_equivalent(*pint_type_ptr))
		return return_type(new pint_instantiation_statement(d));
	else {
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
	if (this->must_be_equivalent(*pbool_type_ptr))
		return excl_ptr<instance_collection_base>(
			pbool_instance_collection::make_pbool_array(*s, id, d));
	else if (this->must_be_equivalent(*pint_type_ptr))
		return excl_ptr<instance_collection_base>(
			pint_instance_collection::make_pint_array(*s, id, d));
	else {
		pbool_type_ptr->dump(cerr) << " at " << &*pbool_type_ptr << endl;
		pint_type_ptr->dump(cerr) << " at " << &*pint_type_ptr << endl;
		dump(cerr) << " at " << this << endl;
		DIE;		// WTF?
		return excl_ptr<instance_collection_base>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PERSISTENT_METHODS_DUMMY_IMPLEMENTATION(param_type_reference)

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_TYPE_REF_CC__

