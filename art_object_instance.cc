// "art_object_instance.cc"

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
// #include "hashlist_template_methods.h"

#include "art_object_instance.h"
#include "art_object_expr.h"
#include "art_built_ins.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class instantiation_base method definitions

/**
	Instantiation base constructor.  
	The first time an instance is declared, its dimensions are
	set by the array-dimension list, if provided, else 0.
	The first set of indices given will be pushed onto the 
	instance collection stack.  
	\param o the owning scope.  
	\param n the name of the instance (collection).
	\param d initial collection of indices, already resolved 
		as param_expr's.  
 */
// inline
instantiation_base::instantiation_base(const scopespace& o, 
		const string& n,
		index_collection_item_ptr_type d) : 
		object(), owner(never_const_ptr<scopespace>(&o)),
		key(n),
		index_collection(), 
		depth(d ? d->dimensions() : 0) {
if (d) {
	index_collection.push_front(d);
} else {
	// push a NULL pointer?
}
}

instantiation_base::~instantiation_base() {
}

ostream&
instantiation_base::dump(ostream& o) const {
	get_type_ref()->dump(o) << " " << key;
	index_collection_type::const_iterator i = index_collection.begin();
	for ( ; i!=index_collection.end(); i++) {
		assert(*i);
		(*i)->dump(o) << endl;
	}
	return o;
}

string
instantiation_base::get_qualified_name(void) const {
	if (owner)
		return owner->get_qualified_name() +scope +key;
	else return key;
}

/**
	Grabs the current top of the deque of the index collection, 
	so the encapsulating instance reference know what
	instances were visible at the time of reference.  
	QUESTION: what if it's empty because it is not collective?
		will begin() = end()? should be...
 */
instantiation_state
instantiation_base::current_collection_state(void) const {
	return index_collection.begin();
}

/**
	By "end", we mean the beginning of the collection state deque, 
	the first item added to the the collection stack.  
	Can't actually dereference the returned iterator, 
	it's only useful for ending looped iterations.  
 */
instantiation_state
instantiation_base::collection_state_end(void) const {
	return index_collection.end();
}

/**
	Will need two flavors: may and must?
	
	\return true if the new range *definitely* overlaps with previous
		static constant ranges.  Comparisons with dynamic ranges
		will conservatively return false; they will be resolved
		at unroll-time.  Also returns true if there was an error.  
		By "true", we mean a valid precise range of overlap.  
 */
const_range_list
instantiation_base::detect_static_overlap(
		index_collection_item_ptr_type r) const {
	assert(r);
	assert(r->dimensions() == depth);
#if 0
	// DEBUG
	cerr << "In instantiation_base::detect_static_overlap with this = "
		<< this << endl;
	r->dump(cerr << "index_collection_item_ptr_type r = ") << endl;
#endif
	if (r.is_a<const_range_list>()) {
	index_collection_type::const_iterator i = index_collection.begin();
	for ( ; i!=index_collection.end(); i++) {
		// return upon first overlap error
		// later accumulate all overlaps.  
		const_range_list ovlp((*i)->static_overlap(*r));
		if (!ovlp.empty()) {
			return ovlp;
		}
		// else keep checking...
	}
	// if this point reached, then return false
	} // else just return false, can't know statically without analysis
	return const_range_list();	// empty constructed list
}

/**
	If this instance is a collection, add the new range of indices
	which may be sparse or dense.  
	This is only applicable if this instantiation was initialized
	as a collective.  
	Pre-condition: The dimensions better damn well match!  
	\param r the index ranges to be added.  
	\return Overlapping range (true) if error condition. 
	\sa detect_static_overlap
 */
const_range_list
instantiation_base::add_index_range(index_collection_item_ptr_type r) {
	assert(r);
	assert(depth);
	assert(depth == r->dimensions());
#if 0
	// DEBUG
	cerr << "In instantiation_base::add_index_range with this = "
		<< this << endl;
	r->dump(cerr << "index_collection_item_ptr_type r = ") << endl;
#endif
	const_range_list overlap(detect_static_overlap(r));
	index_collection.push_back(r);
	return overlap;
}

/**
	Merges index ranges from another instantiation base, 
	such as a redeclaration with more indices of the same collection.  
	Pre-condition: dimensions of i must match this!
	The only type of error caught from here are overlap errors.  
	\return true if there is definite overlap, signaling an error.  
 */
const_range_list
instantiation_base::merge_index_ranges(never_const_ptr<instantiation_base> i) {
	assert(i);
	assert(dimensions() == i->dimensions());
#if 0
	// DEBUG
	cerr << "In instantiation_base::merge_index_range with this = " <<
		this << endl;
	i->dump(cerr << "never_const_ptr<instantiation_base> i = ") << endl;
#endif
	// check type equality here, or push responsibility to caller?
	index_collection_type::const_reverse_iterator iter =
		i->index_collection.rbegin();
	for ( ; iter!=i->index_collection.rend(); iter++) {
		const_range_list ret(add_index_range(*iter));
		if (!ret.empty())
			return ret;
		// else keep checking...
	}
	return const_range_list();
}

//=============================================================================
// class datatype_instantiation method definitions

datatype_instantiation::datatype_instantiation(const scopespace& o, 
		const data_type_reference& t, 
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d), type(&t) {
	assert(type);
}

datatype_instantiation::~datatype_instantiation() {
}

ostream&
datatype_instantiation::what(ostream& o) const {
	return o << "datatype-inst";
}

never_const_ptr<fundamental_type_reference>
datatype_instantiation::get_type_ref(void) const {
	return type;
}

/**
	Create a datatype reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
never_const_ptr<instance_reference_base>
datatype_instantiation::make_instance_reference(context& c) const {
	cerr << "datatype_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	count_ptr<datatype_instance_reference> new_ir(
		new datatype_instance_reference(*this, 
			excl_ptr<index_list>(NULL)));
		// omitting index argument, set it later...
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
// class process_instantiation method definitions

process_instantiation::process_instantiation(const scopespace& o, 
		const process_type_reference& pt,
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d), type(&pt) {
}

process_instantiation::~process_instantiation() {
}

ostream&
process_instantiation::what(ostream& o) const {
	return o << "process-inst";
}

never_const_ptr<fundamental_type_reference>
process_instantiation::get_type_ref(void) const {
	return type;
}

/**
	Create a process reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
never_const_ptr<instance_reference_base>
process_instantiation::make_instance_reference(context& c) const {
	cerr << "process_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	count_ptr<process_instance_reference> new_ir(
		new process_instance_reference(*this, 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
// class param_instantiation method definitions

param_instantiation::param_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d) {
}

param_instantiation::~param_instantiation() {
}

/**
	Override instantiation_base's what() to suppress output of <>.
ostream&
param_instantiation::what(ostream& o) const {
	return o << 
}
**/

/**
	To determine whether or not this is a formal parameter, 
	look itself up in the owning namespace.  
 */
bool
param_instantiation::is_template_formal(void) const {
	// look itself up in owner namespace
	never_const_ptr<definition_base> def(owner.is_a<definition_base>());
	if (def) {
		return def->lookup_template_formal(key);
	} else {
		assert(owner.is_a<name_space>());
		// is owned by a namespace, i.e. actually instantiated
		return false;
	}
}

/**
	Checks whether or not instance has been assigned.
	Collectives -- conservatively return false.  
	In this context, "default_value" refers to the initialized
	value.  
	Should check whether or not this is a template formal first!

	Later distinguish between may_be_init_ and definitely_init_...
 */
bool
param_instantiation::is_initialized(void) const {
//	return default_value();		// used to be just this

	if (dimensions()) {
		// for may_be_initialized, return true, 
		// for definitely_initialized, return false.  
		return false;
	} else if (is_template_formal()) {
	// first check whether or not this is a template formal parameter
		return true;
	} else {
		// then is not a formal, default_value field is 
		// interpreted as an initial value.  
		count_const_ptr<param_expr> ret(default_value());
		if (ret)
			return ret->is_initialized();
		else return false;
	}
}

bool
param_instantiation::is_static_constant(void) const {
	if (dimensions()) {
		// conservatively return... depends on may or must...
		return false;
	} else if (is_template_formal()) {
		return false;
	} else {
		count_const_ptr<param_expr> ret(default_value());
		if (ret)
			return ret->is_static_constant();
		else return false;
	}
}

#if 0
/**
	Note: only one flavor needed (hopefully).  
	One should be able to statically determine whether or not
	something is loop-dependent.  
	Wait, does it even make sense for an "instantiation"?
	This should only be applicable to instance_references...
	put this on hold...
 */
bool
param_instantiation::is_loop_independent(void) const {
	
}
#endif

//=============================================================================
// class pbool_instantiation method definitions

pbool_instantiation::pbool_instantiation(const scopespace& o, 
		const string& n, 
		const pbool_expr* i) :
		param_instantiation(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(i) {
}

pbool_instantiation::pbool_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d, 
		const pbool_expr* i) :
		param_instantiation(o, n, d), ival(i) {
}

ostream&
pbool_instantiation::what(ostream& o) const {
	return o << "pbool-inst";
}

never_const_ptr<fundamental_type_reference>
pbool_instantiation::get_type_ref(void) const {
	return never_const_ptr<fundamental_type_reference>(&pbool_type);
		// defined in "art_built_ins.h"
}

/**
	Initializes a parameter instance with an expression.
	The ival may only be initialized once, enforced by assertions.  
	Note: a parameter is considered "usable" if it is 
	initialized OR it is a template formal.  
	\param e the rvalue expression.
	\return true if properly initialized.  
	\sa is_initialized
 */
bool
pbool_instantiation::initialize(count_const_ptr<pbool_expr> e) {
	assert(e);
	assert(!ival);		// must not already be initialized
	ival = e;
	return true;
}

count_const_ptr<param_expr>
pbool_instantiation::default_value(void) const {
	return ival;
}

count_const_ptr<pbool_expr>
pbool_instantiation::initial_value(void) const {
	return ival;
}


/**
	Create a param reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
	Different: param type reference are always referred to in the global
		scope because they cannot be templated!
		Therefore, cache them in the global (or built-in) namespace.  
	\return NULL.
 */
never_const_ptr<instance_reference_base>
pbool_instantiation::make_instance_reference(context& c) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	count_ptr<param_instance_reference> new_ir(
		new pbool_instance_reference(
			never_ptr<pbool_instantiation>(
			const_cast<pbool_instantiation*>(this)), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
}

#if 0
bool
pbool_instantiation::is_initialized(void) const {
	if (ival)
		return ival->is_initialized();
	else	return false;
#if 0
	// used to be just this...
	return ival;
#endif
}
#endif

//=============================================================================
// class pint_instantiation method definitions

pint_instantiation::pint_instantiation(const scopespace& o, 
		const string& n, 
		const pint_expr* i) :
		param_instantiation(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(i) {
}

pint_instantiation::pint_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d, 
		const pint_expr* i) :
		param_instantiation(o, n, d), ival(i) {
}

ostream&
pint_instantiation::what(ostream& o) const {
	return o << "pint-inst";
}

never_const_ptr<fundamental_type_reference>
pint_instantiation::get_type_ref(void) const {
	return never_const_ptr<fundamental_type_reference>(&pint_type);
		// defined in "art_built_ins.h"
}

/**
	Initializes a parameter instance with an expression.
	The ival may only be initialized once, enforced by assertions.  
	Note: a parameter is considered "usable" if it is 
	initialized OR it is a template formal.  
	\param e the rvalue expression.
	\sa is_initialized
 */
bool
pint_instantiation::initialize(count_const_ptr<pint_expr> e) {
	assert(e);
	assert(!ival);
	ival = e;
	return true;
}

count_const_ptr<param_expr>
pint_instantiation::default_value(void) const {
	return ival;
}

count_const_ptr<pint_expr>
pint_instantiation::initial_value(void) const {
	return ival;
}

/**
	Create a param reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
	Different: param type reference are always referred to in the global
		scope because they cannot be templated!
		Therefore, cache them in the global (or built-in) namespace.  
	\return NULL.
 */
never_const_ptr<instance_reference_base>
pint_instantiation::make_instance_reference(context& c) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	count_ptr<param_instance_reference> new_ir(
		new pint_instance_reference(
			never_ptr<pint_instantiation>(
			const_cast<pint_instantiation*>(this)), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
// class channel_instantiation method definitions

channel_instantiation::channel_instantiation(const scopespace& o, 
		const channel_type_reference& ct,
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d), type(&ct) {
}

channel_instantiation::~channel_instantiation() {
}

ostream&
channel_instantiation::what(ostream& o) const {
	return o << "channel-inst";
}

never_const_ptr<fundamental_type_reference>
channel_instantiation::get_type_ref(void) const {
	return type;
}

/**
	Create a channel reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
never_const_ptr<instance_reference_base>
channel_instantiation::make_instance_reference(context& c) const {
	cerr << "channel_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	count_ptr<channel_instance_reference> new_ir(
		new channel_instance_reference(*this, 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

