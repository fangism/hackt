// "art_object_type_ref.cc"

#include <iostream>

#include "art_parser_base.h"	// so token_identifier : string
#include "art_symbol_table.h"
#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_expr.h"
#include "art_built_ins.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class fundamental_type_reference method definitions

fundamental_type_reference::fundamental_type_reference(
		excl_const_ptr<param_expr_list> pl)
		: type_reference_base(), 
		template_params(pl) {
}

fundamental_type_reference::fundamental_type_reference(void) :
		type_reference_base(), template_params() {
}

fundamental_type_reference::~fundamental_type_reference() {
}

ostream&
fundamental_type_reference::dump(ostream& o) const {
	return o << hash_string();
}

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
	// use fully qualified?  for hashing, no.
	// possible collision case?
	return get_base_def()->get_name() +template_param_string();
}

string
fundamental_type_reference::template_param_string(void) const {
	string ret("<");
	if (template_params) {
		param_expr_list::const_iterator i =
			template_params->begin();
		count_const_ptr<param_expr> e(*i);
		if (e)	ret += e->hash_string();
		for (i++ ; i!=template_params->end(); i++) {
			ret += ",";		// add commas?
			e = *i;
			if (e)	ret += e->hash_string();
			// can e ever be NULL? yes...
		}
	}
	ret += ">";
	return ret;
}

string
fundamental_type_reference::get_qualified_name(void) const {
	return get_base_def()->get_qualified_name() +template_param_string();
}

#if 0
never_const_ptr<fundamental_type_reference>
fundamental_type_reference::set_context_type_reference(context& c) const {
	return c.set_current_fundamental_type(*this);
}
#endif

/**
	Please explain.  
 */
// is static
excl_ptr<instantiation_base>
fundamental_type_reference::make_instantiation(
		count_const_ptr<fundamental_type_reference> t,
		never_const_ptr<scopespace> s,
		const token_identifier& id,
		index_collection_item_ptr_type d) {
	return t->make_instantiation_private(t, s, id, d);
}


/**
	Returns true if the types *may* be equivalent.  
	Easy for non-template types, but for template types, 
	sometimes parameters may be determined by other parameters
	and thus are not statically constant for comparison.  
	\return false if there is definite mismatch in type.  
	TO DO: static argument checking for template arguments.  
 */
bool
fundamental_type_reference::may_be_equivalent(
		const fundamental_type_reference& t) const {
	never_const_ptr<definition_base> left(get_base_def());
	never_const_ptr<definition_base> right(t.get_base_def());
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

/**
	Must be equivalent.  
	Conservatively returns false.  
 */
bool
fundamental_type_reference::must_be_equivalent(
		const fundamental_type_reference& t) const {
	never_const_ptr<definition_base> left(get_base_def());
	never_const_ptr<definition_base> right(t.get_base_def());
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

//=============================================================================
#if 0
MAY BE OBSOLETE
// class collective_type_reference method definitions

collective_type_reference::collective_type_reference(
		const type_reference_base& b, 
		never_const_ptr<array_index_list> d) :
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

data_type_reference::data_type_reference(
#if NEW_DEF_HIER
		never_const_ptr<datatype_definition_base> td
#else
		never_const_ptr<datatype_definition> td
#endif
		) :
		fundamental_type_reference(), 
		base_type_def(td) {
	assert(base_type_def);
}

data_type_reference::data_type_reference(
#if NEW_DEF_HIER
		never_const_ptr<datatype_definition_base> td, 
#else
		never_const_ptr<datatype_definition> td, 
#endif
		excl_const_ptr<param_expr_list> pl) :
		fundamental_type_reference(pl), 
		base_type_def(td) {
	assert(base_type_def);
}

data_type_reference::~data_type_reference() {
}

ostream&
data_type_reference::what(ostream& o) const {
	return o << "data-type-reference";
}

never_const_ptr<definition_base>
data_type_reference::get_base_def(void) const {
	return base_type_def;
}

/**
	Returns a newly constructed data instance object.  
	TO DO: move all error checking into scopespace::add_instance
		for unification.  
		Handle cases for additions to sparse collections.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
#if 0
excl_ptr<instantiation_base>
data_type_reference::make_instantiation(
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new datatype_instantiation(*s, *this, id, d));
}
#else
excl_ptr<instantiation_base>
data_type_reference::make_instantiation_private(
		count_const_ptr<fundamental_type_reference> t, 
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new datatype_instantiation(*s,
			t.is_a<data_type_reference>(), id, d));
}
#endif

//=============================================================================
// class channel_type_reference method definitions

/**
	Concrete channel type reference.  
	\param cd reference to a channel definition.
	\param pl (optional) parameter list for templates.  
 */
channel_type_reference::channel_type_reference(
#if NEW_DEF_HIER
		never_const_ptr<channel_definition_base> cd, 
#else
		never_const_ptr<channel_definition> cd, 
#endif
		excl_const_ptr<param_expr_list> pl) :
		fundamental_type_reference(pl), 
		base_chan_def(cd) {
	assert(base_chan_def);
}

channel_type_reference::channel_type_reference(
#if NEW_DEF_HIER
		never_const_ptr<channel_definition_base> cd
#else
		never_const_ptr<channel_definition> cd
#endif
		) :
		fundamental_type_reference(), 	// NULL
		base_chan_def(cd) {
	assert(base_chan_def);
}

channel_type_reference::~channel_type_reference() {
}

ostream&
channel_type_reference::what(ostream& o) const {
	return o << "channel-type-reference";
}

never_const_ptr<definition_base>
channel_type_reference::get_base_def(void) const {
	return base_chan_def;
}

/**
	Returns a newly constructed channel instance object.  
 */
#if 0
excl_ptr<instantiation_base>
channel_type_reference::make_instantiation(never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new channel_instantiation(*s, *this, id, d));
}
#else
excl_ptr<instantiation_base>
channel_type_reference::make_instantiation_private(
		count_const_ptr<fundamental_type_reference> t, 
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new channel_instantiation(*s, 
			t.is_a<channel_type_reference>(), id, d));
}
#endif

//=============================================================================
// class process_type_reference method definitions

process_type_reference::process_type_reference(
#if NEW_DEF_HIER
		never_const_ptr<process_definition_base> pd
#else
		never_const_ptr<process_definition> pd
#endif
		) :
		fundamental_type_reference(), 
		base_proc_def(pd) {
	assert(base_proc_def);
}

process_type_reference::process_type_reference(
#if NEW_DEF_HIER
		never_const_ptr<process_definition_base> pd, 
#else
		never_const_ptr<process_definition> pd, 
#endif
		excl_const_ptr<param_expr_list> pl) :
		fundamental_type_reference(pl), 
		base_proc_def(pd) {
	assert(base_proc_def);
}

process_type_reference::~process_type_reference() {
}

ostream&
process_type_reference::what(ostream& o) const {
	return o << "process-type-reference";
}

never_const_ptr<definition_base>
process_type_reference::get_base_def(void) const {
	return base_proc_def;
}

/**
	Returns a newly constructed process instance object.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
#if 0
excl_ptr<instantiation_base>
process_type_reference::make_instantiation(
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new process_instantiation(*s, *this, id, d));
}
#else
excl_ptr<instantiation_base>
process_type_reference::make_instantiation_private(
		count_const_ptr<fundamental_type_reference> t, 
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new process_instantiation(*s, 
			t.is_a<process_type_reference>(), id, d));
}
#endif

//=============================================================================
// class param_type_reference method definitions

/**
	Only used in construction of built-in types.  
 */
param_type_reference::param_type_reference(
		never_const_ptr<built_in_param_def> pd) : 
		fundamental_type_reference(), 	// NULL
		base_param_def(pd) {
	assert(base_param_def);
}

param_type_reference::~param_type_reference() {
}

ostream&
param_type_reference::what(ostream& o) const {
	return o << "param-type-reference";
}

never_const_ptr<definition_base>
param_type_reference::get_base_def(void) const {
	return base_param_def;
}

/**
	Returns a newly constructed param instance object.  
	Sort of kludged... built-in type case... YUCK, poor style.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
#if 0
excl_ptr<instantiation_base>
param_type_reference::make_instantiation(never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	// hard coded... yucky, but efficient.  
	if (this->must_be_equivalent(*pbool_type_ptr))
		return excl_ptr<instantiation_base>(
			new pbool_instantiation(*s, id, d));
	else if (this->must_be_equivalent(*pint_type_ptr))
		return excl_ptr<instantiation_base>(
			new pint_instantiation(*s, id, d));
	else {
		pbool_type.dump(cerr) << " at " << &pbool_type << endl;
		pint_type.dump(cerr) << " at " << &pint_type << endl;
		dump(cerr) << " at " << this << endl;
		assert(0);		// WTF?
		return excl_ptr<instantiation_base>(NULL);
	}
}
#else
excl_ptr<instantiation_base>
param_type_reference::make_instantiation_private(
		count_const_ptr<fundamental_type_reference> t, 
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	// hard coded... yucky, but efficient.  
	assert(t == this);
	if (this->must_be_equivalent(*pbool_type_ptr))
		return excl_ptr<instantiation_base>(
			new pbool_instantiation(*s, id, d));
	else if (this->must_be_equivalent(*pint_type_ptr))
		return excl_ptr<instantiation_base>(
			new pint_instantiation(*s, id, d));
	else {
		pbool_type_ptr->dump(cerr) << " at " << &*pbool_type_ptr << endl;
		pint_type_ptr->dump(cerr) << " at " << &*pint_type_ptr << endl;
		dump(cerr) << " at " << this << endl;
		assert(0);		// WTF?
		return excl_ptr<instantiation_base>(NULL);
	}
}
#endif

/**
	Special case of make_instantiation, designated for making
	template formals.  
	Only reason for failure: if default expression doesn't type-check.  
	\param s must be a definition, formals cannot be outside defs.  
	\return valid constructed formal instantiation if successful, 
		else NULL.  
 */
excl_ptr<instantiation_base>
param_type_reference::make_template_formal(
		count_const_ptr<param_type_reference> t,
		never_const_ptr<definition_base> s,
		const token_identifier& id,
		index_collection_item_ptr_type d,
		count_const_ptr<param_expr> def) {
	// not sure if this is correct, what if is typedef?
	never_const_ptr<scopespace>
		defscope(s.is_a<scopespace>());
	assert(defscope);
	if (t->must_be_equivalent(*pbool_type_ptr)) {
		excl_ptr<pbool_instantiation>
			ret(new pbool_instantiation(*defscope, id, d));
		if (def) {
			if (!ret->assign_default_value(def)) {
				// error: type-check fail
				// useful error message?
				return excl_ptr<instantiation_base>(NULL);
			}
		} 
		return excl_ptr<instantiation_base>(ret);
	} else if (t->must_be_equivalent(*pint_type_ptr)) {
		excl_ptr<pint_instantiation>
			ret(new pint_instantiation(*defscope, id, d));
		if (def) {
			if (!ret->assign_default_value(def)) {
				// error: type-check fail
				// useful error message?
				return excl_ptr<instantiation_base>(NULL);
			}
		} 
		return excl_ptr<instantiation_base>(ret);
	} else {
		assert(0);	// WTF?
		return excl_ptr<instantiation_base>(NULL);
	}
}


//=============================================================================
}	// end namespace entity
}	// end namespace ART

