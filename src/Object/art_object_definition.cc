/**
	\file "art_object_definition.cc"
	Method definitions for definition-related classes.  
 	$Id: art_object_definition.cc,v 1.15 2004/11/30 02:33:13 fang Exp $
 */

#include <iostream>

#include "art_parser_base.h"

#include "hash_specializations.h"		// substitute for the following

#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_expr.h"
#include "persistent_object_manager.tcc"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class definition_base method definitions

const never_ptr<const definition_base>
definition_base::null(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Definition basic constructor.  
 */
inline
definition_base::definition_base() :
		object(), persistent(), 
		template_formals_map(), 
		template_formals_list(), 
		defined(false) {
	// synchronize template formals with used_id_map
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
definition_base::~definition_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only dumps the basic template information of the definition.  
	Default behavior may be overridden by subclasses.  
 */
ostream&
definition_base::dump(ostream& o) const {
	const string key = get_key();
	what(o) << " " << key;
	dump_template_formals(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ostream&
definition_base::pair_dump(ostream& o) const {
	o << "  " << get_key() << " = ";
	return dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
definition_base::dump_template_formals(ostream& o) const {
	// sanity check
	assert(template_formals_list.size() == template_formals_map.size());
	if (!template_formals_list.empty()) {
		o << "<" << endl;
		template_formals_list_type::const_iterator
			i = template_formals_list.begin();
		const template_formals_list_type::const_iterator
			e = template_formals_list.end();
		for ( ; i!=e; i++) {
			// sanity check
			assert((*i)->is_template_formal());
			(*i)->dump(o) << endl;
		}
		o << ">" << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a map from template formals to actual values.  
	Precondition: list passed by reference must be initially empty.
	Considering making appropriate virtual call interface in 
		param_expr_list?
 */
void
definition_base::fill_template_actuals_map(
		template_actuals_map_type& am, 
		const param_expr_list& al) const {
	assert(am.empty());
	assert(template_formals_list.size() == al.size());
	// convert to virtual call interface to param_expr_list?
	const const_param_expr_list* cpl =
		IS_A(const const_param_expr_list*, &al);
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &al);
	template_formals_list_type::const_iterator f_iter =
		template_formals_list.begin();
if (cpl) {
	const_param_expr_list::const_iterator i = cpl->begin();
	for ( ; f_iter!=template_formals_list.end(); f_iter++, i++) {
		// const-reference saves unnecessary copying
		const template_formals_value_type& tf(*f_iter);
		// reminder: value type is pointer to param_instance_collection
		assert(tf);
		// reminder: actuals map is of count_ptr
		assert(*i);
		am[tf->get_name()] = *i;
	}
} else {
	assert(dpl);
	dynamic_param_expr_list::const_iterator i = dpl->begin();
	for ( ; f_iter!=template_formals_list.end(); f_iter++, i++) {
		// const-reference saves unnecessary copying
		const template_formals_value_type& tf(*f_iter);
		// reminder: value type is pointer to param_instance_collection
		assert(tf);
		// reminder: actuals map is of count_ptr
		assert(*i);
		am[tf->get_name()] = *i;
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only looks up the identifier in the set of template formals.  
 */
never_ptr<const param_instance_collection>
definition_base::lookup_template_formal(const string& id) const {
	return static_cast<const template_formals_map_type&>
		(template_formals_map)[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Searches template formals set *ONLY* for a matching object.  
	Subclasses should override this to search their respective scopes.  
 */
never_ptr<const object>
definition_base::lookup_object_here(const string& id) const {
	return lookup_template_formal(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Used for checking when a type should have null template arguments.  
	Really just a special case of general template argument checking.  
	\return true if this definition is not templated, 
		or the template formals signature is empty, 
		or default parameters are available for all formals.  
 */
bool
definition_base::check_null_template_argument(void) const {
	if (template_formals_list.empty())
		return true;
	// else make sure each formal has a default parameter value
	template_formals_list_type::const_iterator i =
		template_formals_list.begin();
	for ( ; i!=template_formals_list.end(); i++) {
		never_ptr<const param_instance_collection> p(*i);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A default lookup that always returns NULL.  
	Overridden in process_definition.  
 */
never_ptr<const instance_collection_base>
definition_base::lookup_port_formal(const string& id) const {
	return never_ptr<const instance_collection_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compares the sequence of template formals for a generic definition.  
	\return true if they are equivalent.  
 */
bool
definition_base::equivalent_template_formals(
		never_ptr<const definition_base> d) const {
	assert(d);
	const template_formals_list_type& dtemp = d->template_formals_list;
	if (template_formals_list.size() != dtemp.size()) {
		// useful error message here
		cerr << "ERROR: number of template formal parameters "
			"doesn\'t match!" << endl;
		return false;
	}
	template_formals_list_type::const_iterator i =
		template_formals_list.begin();
	template_formals_list_type::const_iterator j = dtemp.begin();
	for ( ; i!=template_formals_list.end() && j!=dtemp.end(); i++, j++) {
		never_ptr<const param_instance_collection> itf(*i);
		never_ptr<const param_instance_collection> jtf(*j);
		assert(itf);		// template formals not optional
		assert(jtf);		// template formals not optional
		// only type and size need to be equal, not name
		if (!itf->template_formal_equivalent(jtf)) {
			// useful error message goes here
			cerr << "ERROR: template formals do not match!" << endl;
			return false;
		}
		// else continue checking
	}
	// sanity check, we made sure sizes match.
	assert(i==template_formals_list.end() && j==dtemp.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
definition_base::get_name(void) const {
	return get_key();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
definition_base::get_qualified_name(void) const {
	const string key = get_key();
	never_ptr<const scopespace> parent(get_parent());
	if (parent)
		return parent->get_qualified_name() +scope +key;
	else return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Certifies the template arguments against this definition's
	template signature.  
	This also replaces NULL arguments in the list with defaults
	where appropriate.  
	\param ta modifiable list of template argument expressions.
		If this list is empty, however, it will try to 
		construct the default arguments entirely.  
	\return true if arguments successfully type-checked 
		and default arguments supplied in missing places.  
 */
bool
definition_base::certify_template_arguments(
		never_ptr<dynamic_param_expr_list> ta) const {
if (ta) {
	// first, number of arguments must match
	const size_t a_size = ta->size();
	const size_t f_size = template_formals_list.size();
	const template_formals_list_type::const_iterator f_end =
		template_formals_list.end();
	template_formals_list_type::const_iterator f_iter =
		template_formals_list.begin();
	if (a_size != f_size) {
		if (a_size)
			return false;
		// else a_size == 0, passed actuals list is empty, 
		// try to fill in all default arguments
		for ( ; f_iter!=f_end; f_iter++) {
			never_ptr<const param_instance_collection> pinst(*f_iter);
			assert(pinst);
			count_ptr<const param_expr>
				default_expr(pinst->default_value());
			if (!default_expr) {
				// no default value to supply
				return false;
			} else {
				ta->push_back(default_expr);
			}
		}
		// if it fails, then list will be incomplete.  
		// if this point is reached, then fill-in was successfull
		return true;
	}
	dynamic_param_expr_list::iterator p_iter = ta->begin();
	for ( ; f_iter!=f_end; p_iter++, f_iter++) {
		// need method to check param_instance_collection against param_expr
		// eventually also work for complex aggregate types!
		// "I promise this pointer is only local."  
		count_ptr<const param_expr> pex(*p_iter);
		never_ptr<const param_instance_collection> pinst(*f_iter);
		assert(pinst);
		if (pex) {
			// type-check assignment, conservative w.r.t. arrays
			if (!pinst->type_check_actual_param_expr(*pex)) {
				// error message?
				return false;
			}
			// else continue checking successive arguments
		} else {
			// no parameter expression given, 
			// check for default -- if exists, use it, 
			// else is error
			count_ptr<const param_expr>
				default_expr(pinst->default_value());
			if (!default_expr) {
				// error message?
				return false;
			} else {
				// else, actually assign it a copy in the list
				*p_iter = default_expr;
			}
		}
	}
	// end of checking reached, everything passed
	return true;
} else {
	// no arguments supplied, make sure template specification is
	// null, or every formal has default values.  
	return check_null_template_argument();
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default: return false (if unimplemented)
	Temporarily prints an error message.  
 */
bool
definition_base::certify_port_actuals(const object_list& ol) const {
	cerr << "Default definition_base::certify_port_actuals() = false."
		<< endl;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prerequisiste for calling this: must satisfy
		check_null_template_arguments.  
	\returns a list of default parameter expressions.  
 */
excl_ptr<dynamic_param_expr_list>
definition_base::make_default_template_arguments(void) const {
	assert(check_null_template_argument());
	if (template_formals_list.empty())
		return excl_ptr<dynamic_param_expr_list>(NULL);
	// defaulting to dynamic_param_expr_list
	dynamic_param_expr_list* ret = new dynamic_param_expr_list;
	template_formals_list_type::const_iterator i = 
		template_formals_list.begin();
	for ( ; i!=template_formals_list.end(); i++) {
		count_ptr<const param_expr> d((*i)->default_value());
		assert(d);	// everything must have default
		ret->push_back(d);
	}
	return excl_ptr<dynamic_param_expr_list>(ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	DO ME NOW!
	Adds an instantiation to the current definition's scope, and 
	also registers it in the list of template formals for 
	template argument checking.  
	What if template formal is an array, or collective?
	TO DO: convert to pointer-classes...
	\param f needs to be a param_instance_collection... what about array?
		need to be non-const? storing to hash_map_of_ptr...
		must be modifiable for used_id_map
 */
never_ptr<const instance_collection_base>
definition_base::add_template_formal(
		never_ptr<instantiation_statement> i, 
		const token_identifier& id) {
	// const string id(pf->get_name());	// won't have name yet!
	// check and make sure identifier wasn't repeated in formal list!
	{
	never_ptr<const object>
	probe(lookup_object_here(id));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return never_ptr<const instance_collection_base>(NULL);
	} 
	}

	// since we already checked used_id_map, there cannot be a repeat
	// in the template_formals_list!
	// template_formals_list and _map are strict subsets of used_id_map

	// COMPILE: pf is const, but used_id_map members are not
	// wrap around with object_handle?

	// this construction is ugly, TO DO: define clean interface
	scopespace* ss = IS_A(scopespace*, this);
	assert(ss);
	// this creates and adds to the definition
	// and bi-links statement to collection
	never_ptr<const param_instance_collection>
		pf(ss->add_instance(i, id).is_a<const param_instance_collection>());
	assert(pf);
	assert(pf->get_name() == id);	// consistency check

	template_formals_list.push_back(pf);
	template_formals_map[id] = pf;

	// sanity check
	assert(lookup_template_formal(id));
	// later return a never_ptr<>
	return pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Virtually pure, not purely virtual...
	Only temporary.
	Override in appropriate subclasses.  
 */
never_ptr<const instance_collection_base>
definition_base::add_port_formal(
		never_ptr<instantiation_statement> f, 
		const token_identifier& i) {
	assert(0);
	return never_ptr<const instance_collection_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: calling this is unnecessary if one guarantees that
	the template formals are a strict subset of the used_id_map.  
	However, it can't hurt to revisit pointers.
 */
void
definition_base::collect_template_formal_pointers(
		persistent_object_manager& m) const {
	template_formals_list_type::const_iterator
		iter = template_formals_list.begin();
	const template_formals_list_type::const_iterator
		end = template_formals_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Template formals will need to be in list order.
	Just write out the list, the hash_qmap is redundant.  
 */
void
definition_base::write_object_template_formals(
		const persistent_object_manager& m) const {
	assert(template_formals_list.size() == template_formals_map.size());
	ostream& f = m.lookup_write_buffer(this);
	m.write_pointer_list(f, template_formals_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Template formals are loaded in list order.
	Remember that the redundant hash_map also needs to be reconstructed.  
	Another method will add the entries to the corresponding
	used_id_map where appropriate.  
 */
void
definition_base::load_object_template_formals(
		persistent_object_manager& m) {
	istream& f = m.lookup_read_buffer(this);
	m.read_pointer_list(f, template_formals_list);
	// then copy list into hash_map to synchronize
	template_formals_list_type::const_iterator
		iter = template_formals_list.begin();
	const template_formals_list_type::const_iterator
		end = template_formals_list.end();
	for ( ; iter!=end; iter++) {
		template_formals_value_type inst_ptr = *iter;
		assert(inst_ptr);
		// we need to load the instantiation to use its key!
		const_cast<param_instance_collection*>(&*inst_ptr)->load_object(m);
		template_formals_map[inst_ptr->get_name()] = inst_ptr;
	}
	assert(template_formals_list.size() == template_formals_map.size());
}

//=============================================================================
// class typedef_base method definitions

typedef_base::typedef_base() : 
		definition_base(), scopespace(), sequential_scope() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef_base::~typedef_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
typedef_base::get_qualified_name(void) const {
	return definition_base::get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Description dump works for all alias types.  
 */
ostream&
typedef_base::dump(ostream& o) const {
	what(o) << ": " << get_key();
	dump_template_formals(o) << " = ";
	get_base_type_ref()->dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
UNVEIL LATER
/**
	Should this recursively resolve typedefs?
	\param pa the actual parameters passed.  
		Should we require that it be "initialized" i.e. constant or 
		dependent on other template formals (in the case of typedefs
		within template definitions)?
		If dependent on template formals, this type cannot be 
		fully resolved.  
		Only accept const_param_expr_list?  Yeah.  
	\return fully expanded type ONLY IF all parameters are bound
		to static scalar constants, otherwise NULL.  
		Non-scalars and formal values are only resolved at unroll-time.
		Returning NULL is not error, is just conservative w.r.t.
		type-checking.  
 */
excl_ptr<const fundamental_type_reference>
typedef_base::resolve_complete_type(never_ptr<const param_expr_list> pa) const {
	typedef	excl_ptr<const fundamental_type_reference>	return_type;
	never_ptr<const fundamental_type_ref>
		btr(get_base_type_ref());
	never_ptr<const definition_base>
		bd(btr->get_base_def());
	// what if base definition is another typedef?
	excl_ptr<param_expr_list>
		pl(btr->get_copy_template_params());
if (pa) {
	// precondition: all arguments are already supplied, 
	// perform recursive expression substitutions as necessary.
} else {
	// assuming it was type-checked before
	// this is a valid null-template type.  
	// need to return a private exclusive copy of the type-reference
	// no substitution necessary, should already be a complete type
	if (pl) {
		if (pl->is_static_constant()) {
			excl_ptr<dynamic_param_expr_list>
				dpl(pl.is_a_xfer<dynamic_param_expr_list>());
			assert(dpl);	// temporary
			count_ptr<const fundamental_type_reference>
				cftr(bd->make_fundamental_type_reference(dpl));
			assert(cftr.refs() == 1);
			return return_type(cftr.exclusive_release());
		} else {
			// not static constant scalar, conservatively, 
			return return_type(NULL);
		}
	} else {
		count_ptr<const fundamental_type_reference>
			cftr(bd->make_fundamental_type_reference());
		assert(cftr.refs() == 1);
		return return_type(cftr.exclusive_release());
	}
}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper for making a type reference with default template args.  
 */
count_ptr<const fundamental_type_reference>
definition_base::make_fundamental_type_reference(void) const {
	return make_fundamental_type_reference(
		make_default_template_arguments());
}

//=============================================================================
// class datatype_definition_base method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
datatype_definition_base::datatype_definition_base() :
		definition_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
datatype_definition_base::~datatype_definition_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<definition_base>
datatype_definition_base::make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const {
	return excl_ptr<definition_base>(
		new datatype_definition_alias(id, s));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
datatype_definition_base::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list> ta) const {
	if (certify_template_arguments(ta)) {
		return count_ptr<const fundamental_type_reference>(
			new data_type_reference(
				never_ptr<const datatype_definition_base>(this), 
				excl_ptr<const param_expr_list>(ta)));
	} else {
		cerr << "ERROR: failed to make data_type_reference "
			"because template argument types do not match." << endl;
		return count_ptr<const fundamental_type_reference>(NULL);
	}
}

//=============================================================================
// class channel_definition_base method definitions

// make sure that this constructor is never invoked outside this file
inline
channel_definition_base::channel_definition_base() :
		definition_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_definition_base::~channel_definition_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<definition_base>
channel_definition_base::make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const {
	return excl_ptr<definition_base>(
		new channel_definition_alias(id, s));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
channel_definition_base::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list> ta) const {
	if (certify_template_arguments(ta)) {
		return count_ptr<const fundamental_type_reference>(
			new channel_type_reference(
				never_ptr<const channel_definition_base>(this), 
				excl_ptr<const param_expr_list>(ta)));
	} else {
		cerr << "ERROR: failed to make channel_type_reference "
			"because template argument types do not match." << endl;
		return count_ptr<const fundamental_type_reference>(NULL);
	}
}

//=============================================================================
// class user_def_chan method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(user_def_chan, 
	USER_DEF_CHAN_DEFINITION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
user_def_chan::user_def_chan(never_ptr<const name_space> o, 
		const string& name) :
		definition_base(), 
		channel_definition_base(), 
		scopespace(),
		sequential_scope(), 
		key(name), 
		parent(o) {
	// FINISH ME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
user_def_chan::~user_def_chan() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
user_def_chan::what(ostream& o) const {
	return o << "user-def-chan";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
user_def_chan::dump(ostream& o) const {
	return o << "fang, get off your lazy ass and "
		"write user_def_chan::dump()!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
user_def_chan::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
user_def_chan::get_qualified_name(void) const {
	if (parent)
		return parent->get_qualified_name() + scope + key;
	else return string(scope) + key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
user_def_chan::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
user_def_chan::lookup_object_here(const string& id) const {
	return scopespace::lookup_object_here(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void
user_def_chan::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, USER_DEF_CHAN_DEFINITION_TYPE_KEY)) {

	// recursively visit members...
	sequential_scope::collect_object_pointer_list(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	Just allocate with bogus arguments.  
 */
persistent*
user_def_chan::construct_empty(const int i) {
	return new user_def_chan(never_ptr<const name_space>(NULL), "");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
user_def_chan::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	write_string(f, key);
	m.write_pointer(f, parent);
	write_object_template_formals(m);
//	write_object_port_formals(m);
	write_object_used_id_map(m);
	// connections and assignments
	sequential_scope::write_object_pointer_list(m);
	// body
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads in fields from input stream.  
 */
void
user_def_chan::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	load_object_template_formals(m);
//	load_object_port_formals(m);
	load_object_used_id_map(m);
	// connections and assignments
	sequential_scope::load_object_pointer_list(m);
	// body
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_chan::load_used_id_map_object(excl_ptr<persistent>& o) {
	if (o.is_a<instance_collection_base>()) {
		add_instance(o.is_a_xfer<instance_collection_base>());
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to user-def channel definition." << endl;
	}
}

//=============================================================================
// class channel_definition_alias method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(channel_definition_alias, 
	CHANNEL_TYPEDEF_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_definition_alias::channel_definition_alias(
		const string& n, never_ptr<const scopespace> p) :
		definition_base(), 
		channel_definition_base(), 
		typedef_base(), 
		key(n), parent(p), base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_definition_alias::~channel_definition_alias() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_definition_alias::what(ostream& o) const {
	return o << "channel-definition-alias";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
channel_definition_alias::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
channel_definition_alias::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const fundamental_type_reference>
channel_definition_alias::get_base_type_ref(void) const {
	return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_definition_alias::assign_typedef(
		excl_ptr<const fundamental_type_reference> f) {
	assert(f);
	base = f.is_a_xfer<const channel_type_reference>();
	assert(base);
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
channel_definition_alias::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, CHANNEL_TYPEDEF_TYPE_KEY)) {
	base->collect_transient_info(m);
//	collect_used_id_map_pointers(m);	// covers formals?
	collect_template_formal_pointers(m);
	sequential_scope::collect_object_pointer_list(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty allocator.  
 */
persistent*
channel_definition_alias::construct_empty(const int i) {
	return new channel_definition_alias("",
		never_ptr<const scopespace>(NULL));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
channel_definition_alias::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	write_string(f, key);
	m.write_pointer(f, parent);
	m.write_pointer(f, base);
	write_object_template_formals(m);
	sequential_scope::write_object_pointer_list(m);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_definition_alias::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	m.read_pointer(f, base);
	load_object_template_formals(m);
	sequential_scope::load_object_pointer_list(m);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really, typedefs shouldn't have any non-formal members...
 */
void
channel_definition_alias::load_used_id_map_object(excl_ptr<persistent>& o) {
	cerr << "WARNING: didn't expect to call "
		"channel_definition_alias::load_used_id_map_object()." << endl;
	if (o.is_a<instance_collection_base>()) {
		add_instance(o.is_a_xfer<instance_collection_base>());
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to channel typedef." << endl;
	}
}

//=============================================================================
// class built_in_datatype_def method definitions

/**
	Built-in data type marks itself as already defined.  
 */
built_in_datatype_def::built_in_datatype_def(
		never_ptr<const name_space> o, 
		const string& n) :
		definition_base(), 
		datatype_definition_base(),
		scopespace(), 
		key(n), 
		parent(o) {
	mark_defined();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same constructor, but with one template formal parameter.  
	This constructor is dedicated to int<pint width>.  
 */
built_in_datatype_def::built_in_datatype_def(
		never_ptr<const name_space> o, 
		const string& n, 
		excl_ptr<param_instance_collection> p) :
		definition_base(), 
		datatype_definition_base(), 
		scopespace(), 
		key(n), 
		parent(o) {
	assert(p);
//	const string param_str(p->get_name());
	add_template_formal(p.as_a_xfer<instance_collection_base>());
	mark_defined();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
built_in_datatype_def::~built_in_datatype_def() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
built_in_datatype_def::what(ostream& o) const {
	return o << "built-in-datatype-def";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
built_in_datatype_def::dump(ostream& o) const {
	return datatype_definition_base::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
built_in_datatype_def::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
built_in_datatype_def::get_qualified_name(void) const {
	return datatype_definition_base::get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
built_in_datatype_def::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
built_in_datatype_def::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list> ta) const {
	if (certify_template_arguments(ta)) {
		return count_ptr<const fundamental_type_reference>(
			new data_type_reference(
				never_ptr<const built_in_datatype_def>(this), 
				excl_ptr<const param_expr_list>(ta)));
	} else {
		cerr << "ERROR: failed to make built_in_data_type_reference "
			"because template argument types do not match." << endl;
		return count_ptr<const fundamental_type_reference>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since built-in types do not correspond to scopespaces, 
	we have to override definition_base::add_template_formal.  
	Used in construction of built-in types in art_built_ins.cc.
	KLUDGE: redesign interface classes, please!
 */
never_ptr<const instance_collection_base>
built_in_datatype_def::add_template_formal(
		excl_ptr<instance_collection_base> f
		) {
	never_ptr<const param_instance_collection> pf(
		f.is_a<const param_instance_collection>());
	assert(pf);
	// check and make sure identifier wasn't repeated in formal list!
	never_ptr<const object> probe(
		datatype_definition_base::lookup_object_here(pf->get_name()));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return never_ptr<const instance_collection_base>(NULL);
	}

	template_formals_list.push_back(pf);
	template_formals_map[pf->hash_string()] = pf;
	// since we already checked used_id_map, there cannot be a repeat
	// in the template_formals_list!
	// template_formals_list and _map are strict subsets of used_id_map

	// no used_id_map to update, b/c this is not a scopespace!

	// sanity check
	assert(lookup_template_formal(pf->hash_string()));
	// later return a never_ptr<>
	return pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Leeching off of datatype definition.  
	Will be handled specially (replaced) by data_type_reference.  
 */
void
built_in_datatype_def::collect_transient_info(
		persistent_object_manager& m) const {
	m.register_transient_object(this, USER_DEF_DATA_DEFINITION_TYPE_KEY);
	// don't bother with parent pointer to built-in namespace
	collect_template_formal_pointers(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	HACKERY ALERT!
	Since we're mimicking a user-defined datatype, this should
	match the format for user_def_datatype.  
	This object will not live long.  
	We assume that no user-defined datatype can have the same
	name as the built-in types because they are reserved keywords.  
 */
void
built_in_datatype_def::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	write_string(f, key);
	// use bogus parent pointer
	m.write_pointer(f, never_ptr<const name_space>(NULL));
	// bogus template and port formals
	write_object_template_formals(m);	// is empty
//	write_object_port_formals(m);
	write_object_used_id_map(m);
	// connections and assignments

	// need to IMITATE sequential_scope::write_object_pointer_list
	list<never_ptr<const instantiation_statement> > bogus;
	m.write_pointer_list(f, bogus);		// empty

	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
built_in_datatype_def::load_object(persistent_object_manager& m) {
	cerr << "ERROR: built_in_datatype_def::load_object() "
		"should never be called!" << endl;
	assert(0);
	exit(1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
built_in_datatype_def::load_used_id_map_object(excl_ptr<persistent>& o) {
	cerr << "ERROR: built_in_datatype_def::load_used_id_map_object() "
		"should never be called!" << endl;
	assert(0);
	exit(1);
}

//=============================================================================
// class built_in_param_def method definitions

/**
	Built-in param marks itself as already defined.  
 */
built_in_param_def::built_in_param_def(
		never_ptr<const name_space> p,
		const string& n) :
		definition_base(),
		key(n), 
		parent(p) {
	mark_defined();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
built_in_param_def::~built_in_param_def() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
built_in_param_def::what(ostream& o) const {
	return o << "built-in-param-def";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
built_in_param_def::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
built_in_param_def::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Cannot alias built-in parameter types!
	Why? Because I said so.  
 */
excl_ptr<definition_base>
built_in_param_def::make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const {
	assert(0);
	return excl_ptr<definition_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	PROBLEM: built_in types cannot be owned with excl_ptr!!!
	There's one shared static built-in reference for each type.  
	One solution: do away with built-in type_reference?
	Or have a caller check for built-ins and replace?
	Constructed built-in type references won't be used
	in param_instance_collections, should check but then ignore.  
	Managed cache may solve this...
	\param ta template arguments are never used.  
 */
count_ptr<const fundamental_type_reference>
built_in_param_def::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list> ta) const {
	assert(!ta);
	return count_ptr<const fundamental_type_reference>(
		new param_type_reference(
			never_ptr<const built_in_param_def>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PERSISTENT_METHODS_DUMMY_IMPLEMENTATION(built_in_param_def)

//=============================================================================
// class enum_member method definitions

enum_member::enum_member(const string& n) : object(), id(n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum_member::~enum_member() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_member::what(ostream& o) const {
	return o << "enum-member";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_member::dump(ostream& o) const {
	return o << id;
}

//=============================================================================
// class enum_datatype_def method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(enum_datatype_def, 
	ENUM_DEFINITION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum_datatype_def::enum_datatype_def(never_ptr<const name_space> o, 
		const string& n) : 
		definition_base(), 
		datatype_definition_base(), 
		scopespace(), 
		key(n), 
		parent(o) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum_datatype_def::~enum_datatype_def() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_datatype_def::what(ostream& o) const {
	return o << "enum-datatype-def";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_datatype_def::dump(ostream& o) const {
	return what(o) << ": " << key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
enum_datatype_def::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
enum_datatype_def::get_qualified_name(void) const {
	if (parent)
		return parent->get_qualified_name() + scope + key;
	else	return string(scope) + key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
enum_datatype_def::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		never_ptr<const definition_base> d) const {
	assert(d);
	assert(key == d->get_name());
	never_ptr<const enum_datatype_def>
		ed(d.is_a<const enum_datatype_def>());
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if successfully added, false if there was conflict.  
 */
bool
enum_datatype_def::add_member(const token_identifier& em) {
	never_ptr<const object> probe(scopespace::lookup_object_here(em));
	if (probe) {
		never_ptr<const enum_member> probe_em(
			probe.is_a<const enum_member>());
		assert(probe_em);	// can't contain enything else
		return false;
	} else {
#if 0
		// gcc-3.4.0 rejects, thinking that excl_ptr is const
		used_id_map[em] = excl_ptr<enum_member>(
			new enum_member(em));
#else
		excl_ptr<enum_member> member_ptr(new enum_member(em));
		used_id_map[em] = member_ptr;
		assert(!member_ptr);
#endif
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
enum_datatype_def::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, ENUM_DEFINITION_TYPE_KEY)) {

// later: template formals

}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty allocator.  
 */
persistent*
enum_datatype_def::construct_empty(const int i) {
	return new enum_datatype_def(never_ptr<const name_space>(NULL), "");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
enum_datatype_def::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);

	// Index number: not necessary, but can't hurt
	write_value(f, m.lookup_ptr_index(this));

	write_string(f, key);

	m.write_pointer(f, parent);

	// template formals (list)

	// port formals (list)

	// body
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_datatype_def::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);

	// Strip away index number.
	{
	long index;
	read_value(f, index);
	}
	read_string(f, const_cast<string&>(key));

	m.read_pointer(f, parent);

	// template formals (list)

	// port formals (list)

	// body
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enums have no members other than strings.  
	This should never be called.  
 */
void
enum_datatype_def::load_used_id_map_object(excl_ptr<persistent>& o) {
	cerr << "ERROR: not supposed to call "
		"enum_datatype_def::load_used_id_map_object()!" << endl;
	assert(0);
}

//=============================================================================
// class user_def_datatype method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(user_def_datatype, 
	USER_DEF_DATA_DEFINITION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// constructor for user defined type
user_def_datatype::user_def_datatype(
		never_ptr<const name_space> o,
		const string& name) :
		definition_base(), 
		datatype_definition_base(), 
		scopespace(),
		sequential_scope(), 
		key(name), 
		parent(o) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
user_def_datatype::~user_def_datatype() {
}

ostream&
user_def_datatype::what(ostream& o) const {
	return o << "user-defined-datatype";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
user_def_datatype::dump(ostream& o) const {
	return what(o) << ": " << key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
user_def_datatype::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
user_def_datatype::get_qualified_name(void) const {
	if (parent)
		return parent->get_qualified_name() + scope + key;
	else return string(scope) + key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
user_def_datatype::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
user_def_datatype::lookup_object_here(const string& id) const {
	return scopespace::lookup_object_here(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
user_def_datatype::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, USER_DEF_DATA_DEFINITION_TYPE_KEY)) {

// later: template formals
	sequential_scope::collect_object_pointer_list(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty allocator.  
 */
persistent*
user_def_datatype::construct_empty(const int i) {
	return new user_def_datatype(never_ptr<const name_space>(NULL), "");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
	Note: this must be kept consistent with 
	built_in_datatype_def::write_object.  
 */
void
user_def_datatype::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	write_string(f, key);
	m.write_pointer(f, parent);
	write_object_template_formals(m);
//	write_object_port_formals(m);
	write_object_used_id_map(m);
	// connections and assignments
	sequential_scope::write_object_pointer_list(m);
	// body
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_datatype::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	load_object_template_formals(m);
//	load_object_port_formals(m);
	load_object_used_id_map(m);
	// connections and assignments
	sequential_scope::load_object_pointer_list(m);
	// body
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
user_def_datatype::load_used_id_map_object(excl_ptr<persistent>& o) {
	if (o.is_a<instance_collection_base>()) {
		add_instance(o.is_a_xfer<instance_collection_base>());
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to user-def data definition." << endl;
	}
}

//=============================================================================
// class datatype_definition_alias method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(datatype_definition_alias, 
	DATA_TYPEDEF_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_definition_alias::datatype_definition_alias(
		const string& n, never_ptr<const scopespace> p) :
		definition_base(), 
		datatype_definition_base(), 
		typedef_base(), 
		key(n), 
		parent(p) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_definition_alias::~datatype_definition_alias() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
datatype_definition_alias::what(ostream& o) const {
	return o << "datatype-definition-alias";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
datatype_definition_alias::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
datatype_definition_alias::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const fundamental_type_reference>
datatype_definition_alias::get_base_type_ref(void) const {
	return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
datatype_definition_alias::assign_typedef(
		excl_ptr<const fundamental_type_reference> f) {
	assert(f);
	base = f.is_a_xfer<const data_type_reference>();
	assert(base);
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
datatype_definition_alias::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list> ta) const {
	if (certify_template_arguments(ta)) {
		return count_ptr<const fundamental_type_reference>(
			new data_type_reference(
				never_ptr<const datatype_definition_alias>(this), 
				excl_ptr<const param_expr_list>(ta)));
	} else {
		cerr << "ERROR: failed to make data_type_alias type reference "
			"because template argument types do not match." << endl;
		return count_ptr<const fundamental_type_reference>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
datatype_definition_alias::require_signature_match(
		never_ptr<const definition_base> d) const {
	cerr << "TO DO: finish datatype_definition_alias::require_signature_match()!" << endl;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
datatype_definition_alias::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DATA_TYPEDEF_TYPE_KEY)) {
	base->collect_transient_info(m);
//	collect_used_id_map_pointers(m);	// covers formals?
	collect_template_formal_pointers(m);
	sequential_scope::collect_object_pointer_list(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty allocator.  
 */
persistent*
datatype_definition_alias::construct_empty(const int i) {
	return new datatype_definition_alias("",
		never_ptr<const scopespace>(NULL));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
datatype_definition_alias::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	write_string(f, key);
	m.write_pointer(f, parent);
	m.write_pointer(f, base);
	write_object_template_formals(m);
	sequential_scope::write_object_pointer_list(m);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
datatype_definition_alias::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	m.read_pointer(f, base);
	load_object_template_formals(m);
	sequential_scope::load_object_pointer_list(m);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really, typedefs shouldn't have any non-formal members...
 */
void
datatype_definition_alias::load_used_id_map_object(excl_ptr<persistent>& o) {
	cerr << "WARNING: didn't expect to call "
		"datatype_definition_alias::load_used_id_map_object()." << endl;
	if (o.is_a<instance_collection_base>()) {
		add_instance(o.is_a_xfer<instance_collection_base>());
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to datatype typedef." << endl;
	}
}

//=============================================================================
// class process_definition_base method definitions

process_definition_base::process_definition_base() :
		definition_base() {
}

process_definition_base::~process_definition_base() { }

excl_ptr<definition_base>
process_definition_base::make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const {
	return excl_ptr<definition_base>(
		new process_definition_alias(id, s));
}

//=============================================================================
// class process_definition method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(process_definition, 
	PROCESS_DEFINITION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructor for a process definition symbol table entry.  
 */
process_definition::process_definition(
		never_ptr<const name_space> o, 
		const string& s) :
		definition_base(), 
		process_definition_base(),
		scopespace(),
		sequential_scope(), 
		key(s), 
		parent(o), 
		port_formals_list(), 
		port_formals_map() {
	// fill me in...
	// assert(o);		// no: because of partial reconstruction
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_definition::~process_definition() {
	// fill me in...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_definition::what(ostream& o) const {
	return o << "process-definition";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
//	list<never_ptr<const ...> > bin;		// later sort
	o << "In definition \"" << key << "\", we have: " << endl;
	for (i=used_id_map.begin(); i!=used_id_map.end(); i++) {
		o << "  " << i->first << " = ";
//		i->second->what(o) << endl;		// 1 level for now
		i->second->dump(o) << endl;
	}
	return o << "}" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
process_definition::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
process_definition::get_qualified_name(void) const {
	return parent->get_qualified_name() + scope + key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
process_definition::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
process_definition::lookup_object_here(const string& s) const {
	return scopespace::lookup_object_here(s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Override's definition_base's port formal lookup.  
	\return pointer to port's instantiation if found, else NULL.  
 */
never_ptr<const instance_collection_base>
process_definition::lookup_port_formal(const string& id) const {
	return static_cast<const port_formals_map_type&>(port_formals_map)[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Validates a list of objects (instance references) against
	the port formal specification.  
	\return true if type checks (is conservative).
 */
bool
process_definition::certify_port_actuals(const object_list& ol) const {
#if 0
	cerr << "process_definition::certify_port_actuals(): FINISH ME!"
		<< endl;
#endif
	const size_t num_formals = port_formals_list.size();
	const size_t num_actuals = ol.size();
	if (port_formals_list.size() != ol.size()) {
		cerr << "Number of port actuals (" << num_actuals <<
			") doesn\'t match the number of port formals (" <<
			num_formals << ").  ERROR!  " << endl;
		return false;
	}
	object_list::const_iterator
		a_iter = ol.begin();
	port_formals_list_type::const_iterator
		f_iter = port_formals_list.begin();
	const port_formals_list_type::const_iterator
		f_end = port_formals_list.end();
	size_t i = 1;
	for ( ; f_iter!=f_end; f_iter++, a_iter++, i++) {
		const count_ptr<const object> a_obj(*a_iter);
		if (a_obj) {
			const count_ptr<const instance_reference_base>
				a_iref(a_obj.is_a<const instance_reference_base>());
			const never_ptr<const instance_collection_base>
				f_inst(*f_iter);
			// FINISH ME
			const count_ptr<const instance_reference_base>
				f_iref(f_inst->make_instance_reference());
			if (!f_iref->may_be_type_equivalent(*a_iref)) {
				cerr << "ERROR: actual instance reference "
					<< i << " of port connection "
					"doesn\'t match the formal type/size."
					<< endl << "\tgot: ";
				a_iref->dump_type_size(cerr);
				f_iref->dump_type_size(
					cerr << ", expected: ") << endl;
				return false;
			}
			// else continue checking
		}
		// else is NULL, no connection to check, just continue
	}
	// if we've made it here, then no errors!
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
process_definition::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list> ta) const {
	typedef count_ptr<const fundamental_type_reference>	return_type;
	if (certify_template_arguments(ta)) {
		return return_type(new process_type_reference(
				never_ptr<const process_definition>(this),
				excl_ptr<const param_expr_list>(ta)));
	} else {
		cerr << "ERROR: failed to make process_type_reference "
			"because template argument types do not match." << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a port formal instance to this process definition.  
 */
never_ptr<const instance_collection_base>
process_definition::add_port_formal(
		never_ptr<instantiation_statement> f, 
		const token_identifier& id) {
	assert(f);
	assert(!f.is_a<param_instantiation_statement>());
	// check and make sure identifier wasn't repeated in formal list!
	{
	never_ptr<const object>
	probe(lookup_object_here(id));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return never_ptr<const instance_collection_base>(NULL);
	}
	}

	never_ptr<const instance_collection_base>
		pf(add_instance(f, id));
	assert(pf);
	assert(pf->get_name() == id);

	{
	// since we already checked used_id_map, there cannot be a repeat
	// in the port_formals_list!
	port_formals_list.push_back(pf);
	port_formals_map[id] = pf;
	assert(lookup_port_formal(id));
	}

	return pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks that template formals set and port formals set 
	are equivalent.  
	Equivalence should be commutative?
	Question: what do we do about template parameters with defaults?
	Need to update grammar: only allow defaults in the definition.  
	\param d the definition to check against.  
	\return true if equivalent, else false.  
 */
bool
process_definition::require_signature_match(
		never_ptr<const definition_base> d) const {
	assert(d);
	never_ptr<const process_definition>
		pd(d.is_a<const process_definition>());
	if (!pd) {
		cerr << "ERROR: definition " << d->get_name() <<
			" is not even a process!" << endl;
		return false;
	}
	// check for name match
	if (key != pd->get_name()) {
		cerr << "ERROR: names " << key << " and " << d->get_name() <<
			" don\'t even match!" << endl;
		return false;
	}
	// check for owner-namespace match
	if (parent != pd->parent) {
		cerr << "ERROR: definition owner namespaces don\'t match: "
			<< endl << "\tgot: " << parent->get_qualified_name()
			<< " and " << pd->parent->get_qualified_name() << endl;
		return false;
	}
	// check for template formal list match (in order)
	if (!equivalent_template_formals(pd)) {
		cerr << "ERROR: template formals do not match!  " << endl;
		return false;
	}
	// check for port formal list match (in order)
	if (!equivalent_port_formals(pd)) {
		return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Port formals are equivalent if their order of instantiations
	matches exactly, type, size, and even name.  
 */
bool
process_definition::equivalent_port_formals(
		never_ptr<const process_definition> p) const {
	assert(p);
	const port_formals_list_type& pports = p->port_formals_list;
	if (port_formals_list.size() != pports.size()) {
		cerr << "ERROR: number of port formal parameters "
			"doesn\'t match!" << endl;
		return false;
	}
	port_formals_list_type::const_iterator i = 
		port_formals_list.begin();
	port_formals_list_type::const_iterator j = pports.begin();
	for ( ; i!=port_formals_list.end() && j!=pports.end(); i++, j++) {
		never_ptr<const instance_collection_base> ipf(*i);
		never_ptr<const instance_collection_base> jpf(*j);
		assert(ipf);
		assert(jpf);
		if (!ipf->port_formal_equivalent(jpf)) {
			// descriptive error message, please
			cerr << "ERROR: port formals do not match!" << endl;
			return false;
		}
	}
	assert(i==port_formals_list.end() && j==pports.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
process_definition::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, PROCESS_DEFINITION_TYPE_KEY)) {
	// no need to visit template formals, port formals, separately, 
	// b/c they're all registered in the used_id_map.  
	collect_used_id_map_pointers(m);
	sequential_scope::collect_object_pointer_list(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary allocation.  
 */
persistent*
process_definition::construct_empty(const int i) {
	return new process_definition(never_ptr<const name_space>(NULL), "");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
process_definition::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	write_string(f, key);
	m.write_pointer(f, parent);
	write_object_template_formals(m);
	write_object_port_formals(m);
	write_object_used_id_map(m);
	// connections and assignments
	sequential_scope::write_object_pointer_list(m);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	load_object_template_formals(m);
	load_object_port_formals(m);
	load_object_used_id_map(m);
	// connections and assignments
	sequential_scope::load_object_pointer_list(m);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition::load_used_id_map_object(excl_ptr<persistent>& o) {
	if (o.is_a<instance_collection_base>()) {
		add_instance(o.is_a_xfer<instance_collection_base>());
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to process definition." << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Port formals will need to be in list order.
	Just write out the list, the hash_qmap is redundant.  
 */
void
process_definition::write_object_port_formals(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	m.write_pointer_list(f, port_formals_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Template formals are loaded in list order.
	Remember that the redundant hash_map also needs to be reconstructed.  
	Another method will add the entries to the corresponding
	used_id_map where appropriate.  
 */
void
process_definition::load_object_port_formals(
		persistent_object_manager& m) {
	istream& f = m.lookup_read_buffer(this);
	m.read_pointer_list(f, port_formals_list);
	// then copy list into hash_map to synchronize
	port_formals_list_type::const_iterator
		iter = port_formals_list.begin();
	const port_formals_list_type::const_iterator
		end = port_formals_list.end();
	for ( ; iter!=end; iter++) {
		port_formals_value_type inst_ptr = *iter;
		assert(inst_ptr);
		const_cast<instance_collection_base*>(&*inst_ptr)->load_object(m);
		port_formals_map[inst_ptr->get_name()] = inst_ptr;
	}
}

//=============================================================================
// class process_definition_alias method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(process_definition_alias, 
	PROCESS_TYPEDEF_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
process_definition_alias::process_definition_alias() :
		definition_base(), 
		process_definition_base(), 
		typedef_base(), 
		key(), 
		parent(NULL), base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_definition_alias::process_definition_alias(const string& n, 
		never_ptr<const scopespace> p) :
		definition_base(), 
		process_definition_base(), 
		typedef_base(), 
		key(n), 
		parent(p), base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_definition_alias::~process_definition_alias() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_definition_alias::what(ostream& o) const {
	return o << "process-definition-alias";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string&
process_definition_alias::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const scopespace>
process_definition_alias::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const fundamental_type_reference>
process_definition_alias::get_base_type_ref(void) const {
	return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns a complete type reference (possibly using formal parameters)
	to this typedef.  
	Can only be done once, for obvious reasons.  
 */
bool
process_definition_alias::assign_typedef(
		excl_ptr<const fundamental_type_reference> f) {
	assert(f);
	base = f.is_a_xfer<const process_type_reference>();
	assert(base);
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
process_definition_alias::make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list> ta) const {
	if (certify_template_arguments(ta)) {
		return count_ptr<const fundamental_type_reference>(
			new process_type_reference(
				never_ptr<const process_definition_alias>(this), 
				excl_ptr<const param_expr_list>(ta)));
	} else {
		cerr << "ERROR: failed to make process_definition_alias "
			"type reference because template argument types "
			"do not match." << endl;
		return count_ptr<const fundamental_type_reference>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects reachable pointers and register them
	with the persistent object manager.  
 */
void
process_definition_alias::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PROCESS_TYPEDEF_TYPE_KEY)) {
	base->collect_transient_info(m);
//	collect_used_id_map_pointers(m);	// covers formals?
	collect_template_formal_pointers(m);
	sequential_scope::collect_object_pointer_list(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Empty allocator.  
 */
persistent*
process_definition_alias::construct_empty(const int i) {
	return new process_definition_alias();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, manager will call this once.  
 */
void
process_definition_alias::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	write_string(f, key);
	m.write_pointer(f, parent);
	m.write_pointer(f, base);
	write_object_template_formals(m);
	sequential_scope::write_object_pointer_list(m);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_definition_alias::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, parent);
	m.read_pointer(f, base);
	load_object_template_formals(m);
	sequential_scope::load_object_pointer_list(m);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really, typedefs shouldn't have any non-formal members...
 */
void
process_definition_alias::load_used_id_map_object(excl_ptr<persistent>& o) {
	cerr << "WARNING: didn't expect to call "
		"process_definition_alias::load_used_id_map_object()." << endl;
	if (o.is_a<instance_collection_base>()) {
		add_instance(o.is_a_xfer<instance_collection_base>());
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to process typedef." << endl;
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

