// "art_object_definition.cc"

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

#include "art_object_definition.h"
#include "art_object_expr.h"
#include "art_built_ins.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class definition_base method definitions

/**
	Definition basic constructor.  
	\param p the parent scope, a namespace.  
 */
inline
definition_base::definition_base(const string& n,
		never_const_ptr<name_space> p) :
//		template_formals_set* tf : 
		scopespace(n, p),
//		template_formals(tf),
		template_formals_map(), 
		template_formals_list(), 
		defined(false) {
	// synchronize template formals with used_id_map
}

inline
definition_base::~definition_base() {
}

/**
	Only dumps the basic template information of the definition.  
 */
ostream&
definition_base::dump(ostream& o) const {
	what(o) << " " << key;
	if (!template_formals_list.empty()) {
		o << "<" << endl;
		template_formals_list_type::const_iterator i =
			template_formals_list.begin();
		for ( ; i!=template_formals_list.end(); i++) {
			(*i)->dump(o) << endl;
		}
		o << ">" << endl;
	}
	return o;
}

/**
	Used for checking when a type should have null template arguments.  
	\return true if this definition is not templated, 
		or the template formals signature is empty.  
 */
bool
definition_base::check_null_template_argument(void) const {
	if (template_formals_list.empty())
		return true;
	else {
		// make sure each formal has a default parameter value
		template_formals_list_type::const_iterator i =
			template_formals_list.begin();
		for ( ; i!=template_formals_list.end(); i++) {
			never_const_ptr<param_instantiation> p(*i);
			assert(p);
		// if any formal is missing a default value, then this 
		// definition cannot have null template arguments
			if (!p->default_value())
				return false;
			// else continue;	// keep checking
		}
		// if we've reached end of list, we're good!
		return true;
	}
}

/**
	Need template_formal_set to be a queryable-hashlist...
 */
never_const_ptr<param_instantiation>
definition_base::lookup_template_formal(const string& id) const {
	if (!template_formals_list.empty()) {
		never_const_ptr<param_instantiation> ret(
			(static_cast<const template_formals_map_type&>
			(template_formals_map))[id]);
		return ret;
	} else {
		return never_const_ptr<param_instantiation>(NULL);
	}
}

string
definition_base::get_name(void) const {
	return key;
}

string
definition_base::get_qualified_name(void) const {
	if (parent)
		return parent->get_qualified_name() +scope +key;
	else return key;
}

/**
	Sub-classes only need to re-implement if behavior is different.  
	e.g. an assertion fail for built-in types.  
 */
never_const_ptr<definition_base>
definition_base::set_context_definition(context& c) const {
	return c.set_current_definition_reference(*this);
}

/**
	Only definition aliases will return a different pointer, 
	the one for the original definition.  
	Remember to use this in type-checking.  
 */
never_const_ptr<definition_base>
definition_base::resolve_canonical(void) const {
	return never_const_ptr<definition_base>(this);
}

/**
	DO ME NOW!
	Adds an instantiation to the current definition's scope, and 
	also registers it in the list of template formals for 
	template argument checking.  
	What if template formal is an array, or collective?
	TO DO: convert to pointer-classes...
	\param f needs to be a param_instantiation... what about array?
		need to be non-const? storing to hash_map_of_ptr...
		must be modifiable for used_id_map
 */
never_const_ptr<instantiation_base>
definition_base::add_template_formal(excl_ptr<instantiation_base> f) {
	never_const_ptr<param_instantiation> pf(
		f.is_a<param_instantiation>());
	assert(pf);
#if 0
	if (!template_formals) {
		template_formals = new template_formals_set();
		assert(template_formals);
	}
#endif
	// check and make sure identifier wasn't repeated in formal list!
	never_const_ptr<object> probe(lookup_object_here(pf->get_name()));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return never_const_ptr<instantiation_base>(NULL);
	}

#if 0
	const never_const_ptr<param_instantiation>* ret =
		template_formals->append(pf->hash_string(),
			never_const_ptr<param_instantiation>(pf));
	assert(!ret);
#else
	template_formals_list.push_back(pf);
	template_formals_map[pf->hash_string()] = pf;
#endif
	// since we already checked used_id_map, there cannot be a repeat
	// in the template_formals_list!
	// template_formals_list and _map are strict subsets of used_id_map

	// COMPILE: pf is const, but used_id_map members are not
	// wrap around with object_handle?
	used_id_map[pf->hash_string()] = f;

	// sanity check
	assert(lookup_template_formal(pf->hash_string()));
	// later return a never_ptr<>
	return pf;
}

/**
	Virtually pure, not purely virtual...
	Only temporary.
	Override in appropriate subclasses.  
 */
never_const_ptr<instantiation_base>
definition_base::add_port_formal(excl_ptr<instantiation_base> f) {
	assert(0);
	return never_const_ptr<instantiation_base>(NULL);
}

//=============================================================================
// class datatype_definition method definitions

// make sure that this constructor is never invoked outside this file
inline
datatype_definition::datatype_definition(
		never_const_ptr<name_space> o,
		const string& n) :
//		template_formals_set* tf :
//		definition_base(n, o, tf)
		definition_base(n, o) {
}

inline
datatype_definition::~datatype_definition() {
}

never_const_ptr<fundamental_type_reference>
datatype_definition::set_context_fundamental_type(context& c) const {
	data_type_reference* dtr = new data_type_reference(
		never_const_ptr<datatype_definition>(this),
		c.get_current_template_arguments());
	assert(dtr);
	// CACHE the type_reference...
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

//=============================================================================
// class channel_definition method definitions

// make sure that this constructor is never invoked outside this file
inline
channel_definition::channel_definition(
		never_const_ptr<name_space> o, 
		const string& n) :
//		template_formals_set* tf :
//		definition_base(n, o, tf)
		definition_base(n, o) {
}

channel_definition::~channel_definition() {
}

never_const_ptr<fundamental_type_reference>
channel_definition::set_context_fundamental_type(context& c) const {
	channel_type_reference* dtr = new channel_type_reference(
		never_const_ptr<channel_definition>(this),
		c.get_current_template_arguments());
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

//=============================================================================
// class user_def_chan method definitions

user_def_chan::user_def_chan(never_const_ptr<name_space> o, 
		const string& name) :
		channel_definition(o, name) {
	// FINISH ME
}

user_def_chan::~user_def_chan() {
}

ostream&
user_def_chan::what(ostream& o) const {
	return o << "user-def-chan";
}

//=============================================================================
// class type_alias method definitions

/**
	Type alias constructor follows the argument pointer until, 
	it encounters a canonical type, one that is not an alias.  
	Later: allow template typdefs!
	\param o the namespace to which this belongs.  
	\param n the name of the aliased type.  
	\param t pointer to the actual type being aliased.  
 */
type_alias::type_alias(
		never_const_ptr<name_space> o, 
		const string& n, 
		never_const_ptr<definition_base> t) :
//		template_formals_set* tf :
//		definition_base(n, o, tf),
		definition_base(n, o),
		canonical(t->resolve_canonical()) {
	assert(canonical);
	// just in case t is not a canonical type, i.e. another alias...
}

/**
	Destructor, never deletes the canonical type pointer.  
 */
type_alias::~type_alias() { }

/**
	Fancy name for "just return the canonical pointer."
	\return the canonical pointer.  
 */
inline
never_const_ptr<definition_base>
type_alias::resolve_canonical(void) const {
	return canonical;
}

ostream&
type_alias::what(ostream& o) const {
	return o << "aliased-type: " << key;
}

//=============================================================================
// class built_in_datatype_def method definitions

/**
	Built-in data type marks itself as already defined.  
 */
built_in_datatype_def::built_in_datatype_def(
		never_const_ptr<name_space> o, 
		const string& n) :
		datatype_definition(o, n) {
	mark_defined();
}

/**
	Same constructor, but with one template formal parameter.  
	This constructor is dedicated to int<pint width>.  
 */
built_in_datatype_def::built_in_datatype_def(
		never_const_ptr<name_space> o, 
		const string& n, 
		excl_ptr<param_instantiation> p) :
		datatype_definition(o, n) {
	add_template_formal(p.as_a<instantiation_base>());
	mark_defined();
}

built_in_datatype_def::~built_in_datatype_def() { }

ostream&
built_in_datatype_def::what(ostream& o) const {
	return o << key;
}

/**
	Built-in data types should never be opened for modification.  
	Assert fails.  
 */
never_const_ptr<definition_base>
built_in_datatype_def::set_context_definition(context& c) const {
	assert(0);
	return c.set_current_definition_reference(*this);
}

never_const_ptr<fundamental_type_reference>
built_in_datatype_def::set_context_fundamental_type(context& c) const {
	data_type_reference* dtr = new data_type_reference(
		never_const_ptr<built_in_datatype_def>(this),
		c.get_current_template_arguments());
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

/**
	Checks data type equivalence.
	TO DO:
	Currently does NOT check template signature, which need to be 
	implemented eventually, but punting for now.  
	\param t the datatype_definition to be checked.
	\return true if types are equivalent.  
 */
bool
built_in_datatype_def::type_equivalent(const datatype_definition& t) const {
	never_const_ptr<built_in_datatype_def> b = 
		t.resolve_canonical().is_a<built_in_datatype_def>();
	if (b) {
		// later: check template signature! (for int<>)
		return key == b->key;
	} else {
		return false;
	}
}

//=============================================================================
// class built_in_param_def method definitions

/**
	Built-in param marks itself as already defined.  
 */
built_in_param_def::built_in_param_def(
		never_const_ptr<name_space> p,
		const string& n, 
		const param_type_reference& t) :
		definition_base(n, p), type_ref(&t) {
	mark_defined();
}

built_in_param_def::~built_in_param_def() {
}

ostream&
built_in_param_def::what(ostream& o) const {
	return o << key;
}

/**
	Really this should never be called, as built-in definitions
	cannot be opened for modification.  
	Assert fails.
 */
never_const_ptr<definition_base>
built_in_param_def::set_context_definition(context& c) const {
	assert(0);
	return c.set_current_definition_reference(*this);
}

/**
	Consider built-in type references, is this even used?
	Now uses hard-coded param_type_references.  
	Kludge object comparison...
	Consider passing default type_reference into constructor
		as a forward pointer.  
 */
never_const_ptr<fundamental_type_reference>
built_in_param_def::set_context_fundamental_type(context& c) const {
	return c.set_current_fundamental_type(*type_ref);
}

//=============================================================================
// class enum_member method definitions

enum_member::enum_member(const string& n) : object(), id(n) { }

enum_member::~enum_member() { }

ostream&
enum_member::what(ostream& o) const {
	return o << "enum-member";
}

ostream&
enum_member::dump(ostream& o) const {
	return o << id;
}

//=============================================================================
// class enum_datatype_def method definitions

enum_datatype_def::enum_datatype_def(never_const_ptr<name_space> o, 
		const string& n) : 
		datatype_definition(o, n) {
}

enum_datatype_def::~enum_datatype_def() {
}

ostream&
enum_datatype_def::what(ostream& o) const {
	return o << key;
}

never_const_ptr<fundamental_type_reference>
enum_datatype_def::set_context_fundamental_type(context& c) const {
	data_type_reference* dtr = new data_type_reference(
		never_const_ptr<enum_datatype_def>(this));
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

/**
	Type equivalence of enumerated types:
	must point to same definition, namely this!
 */
bool
enum_datatype_def::type_equivalent(const datatype_definition& t) const {
	never_const_ptr<enum_datatype_def> b = 
		t.resolve_canonical().is_a<enum_datatype_def>();
	return b == this;
}

/**
	Not the same as type-equivalence, which requires precise 
	pointer equality.  
	This is used for comparing prototypes and signatures of 
	declarations and definitions.  
	Report errors to stderr or stdout?
	Template this?  Nah...
	\param d the definition (signature) to compare, 
		the name MUST match, else comparison is pointless!
 */
bool
enum_datatype_def::require_signature_match(
		never_const_ptr<definition_base> d) const {
	assert(d);
	assert(key == d->get_name());
	never_const_ptr<enum_datatype_def>
		ed(d.is_a<enum_datatype_def>());
	if (ed) {
		// only names need to match...
		// no other signature information!  easy.
		return true;
	} else {
		// class type doesn't even match!  report error.
		d->what(cerr << key << " is already declared as a ")
			<< " but is being redeclared as a ";
		what(cerr) << "  ERROR!  ";
		return false;
	}
}

/**
	\return true if successfully added, false if there was conflict.  
 */
bool
enum_datatype_def::add_member(const token_identifier& em) {
	never_const_ptr<object> probe(lookup_object_here(em));
	if (probe) {
		never_const_ptr<enum_member> probe_em(
			probe.is_a<enum_member>());
		assert(probe_em);	// can't contain enything else
		return false;
	} else {
		used_id_map[em] = excl_ptr<enum_member>(
			new enum_member(em));
		return true;
	}
}

//=============================================================================
// class user_def_datatype method definitions

/// constructor for user defined type
user_def_datatype::user_def_datatype(
		never_const_ptr<name_space> o,
		const string& name) :
		datatype_definition(o, name)
//		template_params(), members()
		{
}

ostream&
user_def_datatype::what(ostream& o) const {
	return o << "used-defined-datatype: " << key;
}

/**
	Equivalance operator for user-defined types.  
	TO DO: actually write comparison, for now, just always returns false.  
	\param t the datatype_definition to be checked;
	\return true if the type names match, the (optional) template
		formals match, and the data formals match.  
 */
bool
user_def_datatype::type_equivalent(const datatype_definition& t) const {
	never_const_ptr<user_def_datatype> u = 
		t.resolve_canonical().is_a<user_def_datatype>();
	if (u) {
		// compare template_params
		// compare data members
		// for now...
		return false;
	} else {
		return false;
	}
}

//=============================================================================
// class process_definition method definitions

/**
	Constructor for a process definition symbol table entry.  
 */
process_definition::process_definition(
		never_const_ptr<name_space> o, 
		const string& s) :
//		template_formals_set* tf : 
//		definition_base(s, o, tf),
		definition_base(s, o),
//		port_formals()
		port_formals_list(), 
		port_formals_map()
		{
	// fill me in...
}

process_definition::~process_definition() {
	// fill me in...
}

ostream&
process_definition::what(ostream& o) const {
	return o << "process-definition";
}

/**
	Spill contents of the used_id_map.
	\param o the output stream.
	\return the same output stream.
 */
ostream&
process_definition::dump(ostream& o) const {
	definition_base::dump(o);	// dump template signature first

	// now dump ports
	{
		o << "(" << endl;
		port_formals_list_type::const_iterator i =
			port_formals_list.begin();
		for ( ; i!=port_formals_list.end(); i++) {
			(*i)->dump(o) << endl;
		}
		o << ")" << endl;
	}

	// now dump rest of contents
	o << "{" << endl;
	used_id_map_type::const_iterator i;
//	list<never_const_ptr<...> > bin;		// later sort
	o << "In definition \"" << key << "\", we have: {" << endl;
	for (i=used_id_map.begin(); i!=used_id_map.end(); i++) {
		o << "  " << i->first << " = ";
//		i->second->what(o) << endl;		// 1 level for now
		i->second->dump(o) << endl;
	}
	return o << "}" << endl;
}

never_const_ptr<fundamental_type_reference>
process_definition::set_context_fundamental_type(context& c) const {
	process_type_reference* dtr = new process_type_reference(
		never_const_ptr<process_definition>(this),
		c.get_current_template_arguments());
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

/**
	Adds a port formal instance to this process definition.  
 */
never_const_ptr<instantiation_base>
process_definition::add_port_formal(excl_ptr<instantiation_base> f) {
	assert(f);
	assert(!f.is_a<param_instantiation>());
	never_const_ptr<instantiation_base> pf(f);
	// check and make sure identifier wasn't repeated in formal list!
	never_const_ptr<object> probe(lookup_object_here(f->get_name()));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return never_const_ptr<instantiation_base>(NULL);
	}

	{
#if 0
	const never_const_ptr<instantiation_base>* ret =
		port_formals.append(f->hash_string(),
			never_const_ptr<instantiation_base>(f));
	assert(!ret);
#else
	// since we already checked used_id_map, there cannot be a repeat
	// in the port_formals_list!
	port_formals_list.push_back(pf);
	port_formals_map[f->get_name()] = pf;
#endif
	}

	used_id_map[f->hash_string()] = f;
	assert(!f);		// ownership transferred
	return pf;
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

