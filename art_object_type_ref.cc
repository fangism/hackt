// "art_object_type_ref.cc"

#include <iostream>

#include "multidimensional_sparse_set.h"

#include "art_parser_debug.h"		// need this?
#include "art_parser_base.h"
#include "art_symbol_table.h"

// CAUTION on ordering of the following two include files!
// including "art_object.h" first will cause compiler to complain
// about redefinition of struct hash<> template upon specialization of
// hash<string>.  

#include "hash_specializations.h"		// substitute for the following

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
		excl_ptr<template_param_list> pl)
		: type_reference_base(), 
		template_params(pl) {
}

fundamental_type_reference::fundamental_type_reference(void) :
		type_reference_base(), template_params() {
		// NULL
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
		template_param_list::const_iterator i =
			template_params->begin();
		never_const_ptr<param_expr> e(*i);
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

never_const_ptr<fundamental_type_reference>
fundamental_type_reference::set_context_type_reference(context& c) const {
	return c.set_current_fundamental_type(*this);
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
		never_const_ptr<fundamental_type_reference> t) const {
	assert(t);
	never_const_ptr<definition_base> left(get_base_def());
	never_const_ptr<definition_base> right(t->get_base_def());
	if (left != right) {
		return false;
	}
	// TO DO: compare template arguments
	return true;
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
		never_const_ptr<datatype_definition> td) :
		fundamental_type_reference(), 
		base_type_def(td) {
	assert(base_type_def);
}

data_type_reference::data_type_reference(
		never_const_ptr<datatype_definition> td, 
		excl_ptr<template_param_list> pl) :
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
excl_ptr<instantiation_base>
data_type_reference::make_instantiation(never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new datatype_instantiation(*s, *this, id, d));
}

//=============================================================================
// class channel_type_reference method definitions

/**
	Concrete channel type reference.  
	\param cd reference to a channel definition.
	\param pl (optional) parameter list for templates.  
 */
channel_type_reference::channel_type_reference(
		never_const_ptr<channel_definition> cd, 
		excl_ptr<template_param_list> pl) :
		fundamental_type_reference(pl), 
		base_chan_def(cd) {
	assert(base_chan_def);
}

channel_type_reference::channel_type_reference(
		never_const_ptr<channel_definition> cd) :
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
excl_ptr<instantiation_base>
channel_type_reference::make_instantiation(never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new channel_instantiation(*s, *this, id, d));
}

//=============================================================================
// class process_type_reference method definitions

process_type_reference::process_type_reference(
		never_const_ptr<process_definition> pd) :
		fundamental_type_reference(), 
		base_proc_def(pd) {
	assert(base_proc_def);
}

process_type_reference::process_type_reference(
		never_const_ptr<process_definition> pd, 
		excl_ptr<template_param_list> pl) :
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
excl_ptr<instantiation_base>
process_type_reference::make_instantiation(
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new process_instantiation(*s, *this, id, d));
}

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
excl_ptr<instantiation_base>
param_type_reference::make_instantiation(never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	// hard coded... yucky, but efficient.  
	if (this == &pbool_type)
		return excl_ptr<instantiation_base>(
			new pbool_instantiation(*s, id, d));
	else if (this == &pint_type)
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

//=============================================================================
}	// end namespace entity
}	// end namespace ART

