// "art_object_type_ref.cc"

#include <iostream>

#include "art_parser_base.h"	// so token_identifier : string
#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_expr.h"
#include "art_object_IO.tcc"
#include "art_built_ins.h"
#include "art_utils.tcc"

#include "sstream.h"

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
fundamental_type_reference::fundamental_type_reference(void) :
		type_reference_base(), template_params() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
fundamental_type_reference::~fundamental_type_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
fundamental_type_reference::dump(ostream& o) const {
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
	// use fully qualified?  for hashing, no.
	// possible collision case?
	return get_base_def()->get_name() +template_param_string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
fundamental_type_reference::template_param_string(void) const {
	string ret("<");
	if (template_params) {
#if 0
		param_expr_list::const_iterator i =
			template_params->begin();
		count_const_ptr<param_expr> e(*i);
		if (e)	ret += e->hash_string();
		for (i++; i!=template_params->end(); i++) {
			ret += ",";		// add commas?
			e = *i;
			if (e)	ret += e->hash_string();
			// can e ever be NULL? yes...
		}
#else
		ostringstream o;
		template_params->dump(o);
		ret += o.str();
#endif
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
excl_const_ptr<fundamental_type_reference>
fundamental_type_reference::resolve_canonical_type(void) const {
	typedef	excl_const_ptr<fundamental_type_reference>	return_type;
	never_const_ptr<definition_base>
		base_def(get_base_def());
	assert(base_def);
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
		count_const_ptr<fundamental_type_reference> t, 
		index_collection_item_ptr_type d) {
	return t->make_instantiation_statement_private(t, d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Please explain.  
	Can't just use t->make_instance_collection() because t is a counted
	pointer, and the instanatiations share the reference to the type.  
	Can't discard counting into never_ptr.  
 */
// is static
excl_ptr<instance_collection_base>
fundamental_type_reference::make_instance_collection(
		count_const_ptr<fundamental_type_reference> t,
		never_const_ptr<scopespace> s,
		const token_identifier& id,
#if 0
		index_collection_item_ptr_type d
#else
		const size_t d
#endif
		) {
	return t->make_instance_collection_private(t, s, id, d);
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
	never_const_ptr<definition_base> left(get_base_def());
	never_const_ptr<definition_base> right(t.get_base_def());

	bool have_typedef = false;

	// TO resolve typedefs and aliases
	// self-recursive call to expand parameters...
	// or PUNT unrolling actual parameters until later...
	never_const_ptr<typedef_base> ltdb(left.is_a<typedef_base>());
	never_const_ptr<typedef_base> rtdb(right.is_a<typedef_base>());
	while (ltdb) {
		have_typedef = true;
		left = ltdb->get_base_type_ref()->get_base_def();
		ltdb = left.is_a<typedef_base>();
	}
	while (rtdb) {
		have_typedef = true;
		right = rtdb->get_base_type_ref()->get_base_def();
		rtdb = right.is_a<typedef_base>();
	}
#if 0
	if (ltdb) {
		ltdb->resolve_complete_type(template_params);
	}
	if (rtdb) {
		rtdb->resolve_complete_type(t.template_params);
	}
	assert(!ltdb && !rtdb);		// down to canonical definitions
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
excl_const_ptr<fundamental_type_reference>
	make_canonical_type_reference(void) const {

}
#endif


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
		never_const_ptr<datatype_definition_base> td) :
		fundamental_type_reference(), 
		base_type_def(td) {
	assert(base_type_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_type_reference::data_type_reference(
		never_const_ptr<datatype_definition_base> td, 
		excl_const_ptr<param_expr_list> pl) :
		fundamental_type_reference(pl), 
		base_type_def(td) {
	assert(base_type_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_type_reference::~data_type_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
data_type_reference::what(ostream& o) const {
	return o << "data-type-reference";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_const_ptr<definition_base>
data_type_reference::get_base_def(void) const {
	return base_type_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed data instantiation statement object.
 */
excl_ptr<instantiation_statement>
data_type_reference::make_instantiation_statement_private(
		count_const_ptr<fundamental_type_reference> t, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_statement>(
		new data_instantiation_statement(
			t.is_a<data_type_reference>(), d));
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
data_type_reference::make_instance_collection_private(
		count_const_ptr<fundamental_type_reference> t, 
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
#if 0
		index_collection_item_ptr_type d
#else
		const size_t d
#endif
		) const {
	return excl_ptr<instance_collection_base>(
		new datatype_instance_collection(*s,
			t.is_a<data_type_reference>(), id, d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DATA_TYPE_REFERENCE_TYPE)) {
	base_type_def->collect_transient_info(m);
	if (template_params)
		template_params->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
data_type_reference::construct_empty(void) {
	return new data_type_reference();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_type_reference::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);		// sanity check
	m.write_pointer(f, base_type_def);
	m.write_pointer(f, template_params);
	WRITE_OBJECT_FOOTER(f);			// sanity check
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	May need special case handling for built-in definitions!
 */
void
data_type_reference::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);		// sanity check
	m.read_pointer(f, base_type_def);
	m.read_pointer(f, template_params);
	STRIP_OBJECT_FOOTER(f);			// sanity check

	// recursion and intercept built-in types
	const_cast<datatype_definition_base&>(*base_type_def).load_object(m);
	if (template_params)
		const_cast<param_expr_list&>(*template_params).load_object(m);
	if (base_type_def->get_key() == "bool")
		base_type_def =
			never_const_ptr<datatype_definition_base>(&bool_def);
	else if (base_type_def->get_key() == "int")
		base_type_def =
			never_const_ptr<datatype_definition_base>(&int_def);
	// else leave the base definition as is
	// reference count will take care of discarded memory :)
}
// else already visited
}

//=============================================================================
// class channel_type_reference method definitions

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
		never_const_ptr<channel_definition_base> cd, 
		excl_const_ptr<param_expr_list> pl) :
		fundamental_type_reference(pl), 
		base_chan_def(cd) {
	assert(base_chan_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_type_reference::channel_type_reference(
		never_const_ptr<channel_definition_base> cd) :
		fundamental_type_reference(), 	// NULL
		base_chan_def(cd) {
	assert(base_chan_def);
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
never_const_ptr<definition_base>
channel_type_reference::get_base_def(void) const {
	return base_chan_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed channel instantiation statement object.
 */
excl_ptr<instantiation_statement>
channel_type_reference::make_instantiation_statement_private(
		count_const_ptr<fundamental_type_reference> t, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_statement>(
		new channel_instantiation_statement(
			t.is_a<channel_type_reference>(), d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed channel instance object.  
 */
excl_ptr<instance_collection_base>
channel_type_reference::make_instance_collection_private(
		count_const_ptr<fundamental_type_reference> t, 
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
#if 0
		index_collection_item_ptr_type d
#else
		const size_t d
#endif
		) const {
	return excl_ptr<instance_collection_base>(
		new channel_instance_collection(*s, 
			t.is_a<channel_type_reference>(), id, d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, CHANNEL_TYPE_REFERENCE_TYPE)) {
	base_chan_def->collect_transient_info(m);
	if (template_params)
		template_params->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
channel_type_reference::construct_empty(void) {
	return new channel_type_reference();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);		// sanity check
	m.write_pointer(f, base_chan_def);
	m.write_pointer(f, template_params);
	WRITE_OBJECT_FOOTER(f);			// sanity check
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_type_reference::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);		// sanity check
	m.read_pointer(f, base_chan_def);
	m.read_pointer(f, template_params);
	STRIP_OBJECT_FOOTER(f);			// sanity check
}
// else already visited
}

//=============================================================================
// class process_type_reference method definitions

/**
	Private empty constructor only accessible to the construct_empty
	method called during object allocation and de-serialization.  
 */
process_type_reference::process_type_reference() :
		fundamental_type_reference(), 
		base_proc_def(never_const_ptr<process_definition_base>(NULL)) {
	// do not assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_type_reference::process_type_reference(
		never_const_ptr<process_definition_base> pd) :
		fundamental_type_reference(), 
		base_proc_def(pd) {
	assert(base_proc_def);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_type_reference::process_type_reference(
		never_const_ptr<process_definition_base> pd, 
		excl_const_ptr<param_expr_list> pl) :
		fundamental_type_reference(pl), 
		base_proc_def(pd) {
	assert(base_proc_def);
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
never_const_ptr<definition_base>
process_type_reference::get_base_def(void) const {
	return base_proc_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed process instantiation statement object.
 */
excl_ptr<instantiation_statement>
process_type_reference::make_instantiation_statement_private(
		count_const_ptr<fundamental_type_reference> t, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_statement>(
		new process_instantiation_statement(
			t.is_a<process_type_reference>(), d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed process instance object.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
excl_ptr<instance_collection_base>
process_type_reference::make_instance_collection_private(
		count_const_ptr<fundamental_type_reference> t, 
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
#if 0
		index_collection_item_ptr_type d
#else
		const size_t d
#endif
		) const {
	return excl_ptr<instance_collection_base>(
		new process_instance_collection(*s, 
			t.is_a<process_type_reference>(), id, d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_type_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PROCESS_TYPE_REFERENCE_TYPE)) {
	base_proc_def->collect_transient_info(m);
	if (template_params)
		template_params->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
process_type_reference::construct_empty(void) {
	return new process_type_reference();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_type_reference::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);		// sanity check
	m.write_pointer(f, base_proc_def);
	m.write_pointer(f, template_params);
	WRITE_OBJECT_FOOTER(f);			// sanity check
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_type_reference::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);		// sanity check
	m.read_pointer(f, base_proc_def);
	m.read_pointer(f, template_params);
	STRIP_OBJECT_FOOTER(f);			// sanity check
}
// else already visited
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_type_reference::~param_type_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_type_reference::what(ostream& o) const {
	return o << "param-type-reference";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_const_ptr<definition_base>
param_type_reference::get_base_def(void) const {
	return base_param_def;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns a newly constructed param instantiation statement object.
 */
excl_ptr<instantiation_statement>
param_type_reference::make_instantiation_statement_private(
		count_const_ptr<fundamental_type_reference> t, 
		index_collection_item_ptr_type d) const {
	typedef	excl_ptr<instantiation_statement>	return_type;
	assert(t == this);
	if (this->must_be_equivalent(*pbool_type_ptr))
		return return_type(new pbool_instantiation_statement(d));
	else if (this->must_be_equivalent(*pint_type_ptr))
		return return_type(new pint_instantiation_statement(d));
	else {
		pbool_type_ptr->dump(cerr) << " at " << &*pbool_type_ptr << endl;
		pint_type_ptr->dump(cerr) << " at " << &*pint_type_ptr << endl;
		dump(cerr) << " at " << this << endl;
		assert(0);		// WTF?
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
param_type_reference::make_instance_collection_private(
		count_const_ptr<fundamental_type_reference> t, 
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
#if 0
		index_collection_item_ptr_type d
#else
		const size_t d
#endif
		) const {
	// hard coded... yucky, but efficient.  
	assert(t == this);
	if (this->must_be_equivalent(*pbool_type_ptr))
		return excl_ptr<instance_collection_base>(
			new pbool_instance_collection(*s, id, d));
	else if (this->must_be_equivalent(*pint_type_ptr))
		return excl_ptr<instance_collection_base>(
			new pint_instance_collection(*s, id, d));
	else {
		pbool_type_ptr->dump(cerr) << " at " << &*pbool_type_ptr << endl;
		pint_type_ptr->dump(cerr) << " at " << &*pint_type_ptr << endl;
		dump(cerr) << " at " << this << endl;
		assert(0);		// WTF?
		return excl_ptr<instance_collection_base>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
OBSOLETE ???
/**
	Special case of make_instance_collection, designated for making
	template formals.  
	Only reason for failure: if default expression doesn't type-check.  

	TO DO: bi-link instantiation_statement to instance_collection.

	\param s must be a definition, formals cannot be outside defs.  
	\return valid constructed formal instantiation if successful, 
		else NULL.  
 */
#if 0
excl_ptr<instance_collection_base>
#else
excl_ptr<instantiation_statement>
#endif
param_type_reference::make_template_formal(
		count_const_ptr<param_type_reference> t,
		never_const_ptr<definition_base> s,
		const token_identifier& id,
		index_collection_item_ptr_type d,
		count_const_ptr<param_expr> def) {
	// not sure if this is correct, what if is typedef?
#if 0
	typedef excl_ptr<instance_collection_base>	return_type;
#else
	typedef excl_ptr<instantiation_statement>	return_type;
#endif
	never_const_ptr<scopespace>
		defscope(s.is_a<scopespace>());
	assert(defscope);
	if (t->must_be_equivalent(*pbool_type_ptr)) {
		excl_ptr<pbool_instance_collection>
			ret(new pbool_instance_collection(*defscope, id, d));
		if (def) {
			if (!ret->assign_default_value(def)) {
				// error: type-check fail
				// useful error message?
				return return_type(NULL);
			}
		} 
		return return_type(ret);
	} else if (t->must_be_equivalent(*pint_type_ptr)) {
		excl_ptr<pint_instance_collection>
			ret(new pint_instance_collection(*defscope, id, d));
		if (def) {
			if (!ret->assign_default_value(def)) {
				// error: type-check fail
				// useful error message?
				return return_type(NULL);
			}
		} 
		return return_type(ret);
	} else {
		assert(0);	// WTF?
		return return_type(NULL);
	}
}
OBSOLETE ???
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

