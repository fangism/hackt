/**
	\file "art_object_instance.cc"
	Method definitions for instance collection classes.
 	$Id: art_object_instance.cc,v 1.26 2004/12/07 02:22:08 fang Exp $
 */

#include <iostream>
#include <algorithm>

#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_instance_param.h"
#include "art_object_inst_ref.h"
#include "art_object_inst_stmt.h"
#include "art_object_expr_const.h"
#include "art_built_ins.h"
#include "art_object_type_hash.h"

#include "STL/list.tcc"
#include "persistent_object_manager.tcc"
#include "compose.h"
#include "binders.h"
#include "ptrs_functional.h"
// #include "indent.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {
using namespace ADS;		// for composition functors

//=============================================================================
// class instance_collection_base method definitions

const never_ptr<const instance_collection_base>
instance_collection_base::null(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
instance_collection_base::instance_collection_base() :
		object(), persistent(), 
		owner(NULL), key(), index_collection(), depth(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiation base constructor.  
	The first time an instance is declared, its dimensions are
	set by the array-dimension list, if provided, else 0.
	The first set of indices given will be pushed onto the 
	instance collection stack.  
	\param o the owning scope.  
	\param n the name of the instance (collection).
	\param d the number of dimensions of this collection ([0,4]).  
		WAS: initial collection of indices, already resolved 
		as param_expr's.  
 */
// inline
instance_collection_base::instance_collection_base(const scopespace& o, 
		const string& n,
		const size_t d) : 
		object(), owner(never_ptr<const scopespace>(&o)),
		key(n),
		index_collection(), 
		depth(d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instance_collection_base::~instance_collection_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overridden by param_instance_collection.  
 */
ostream&
instance_collection_base::dump(ostream& o) const {
	get_type_ref()->dump(o) << " " << key;
	index_collection_type::const_iterator i = index_collection.begin();
	for ( ; i!=index_collection.end(); i++) {
		assert(*i);
		index_collection_item_ptr_type ind((*i)->get_indices());
		if (ind)
			ind->dump(o) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instance_collection_base::pair_dump(ostream& o) const {
	o << "  " << get_name() << " = ";
	return dump(o) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instance_collection_base::get_qualified_name(void) const {
	if (owner)
		return owner->get_qualified_name() +"::" +key;
		// "::" should be the same as ART::parser::scope
	else return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Return's the type's base definition.
 */
never_ptr<const definition_base>
instance_collection_base::get_base_def(void) const {
	return get_type_ref()->get_base_def();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Grabs the current top of the deque of the index collection, 
	so the encapsulating instance reference know what
	instances were visible at the time of reference.  
	QUESTION: what if it's empty because it is not collective?
		will begin() = end()? should be...
 */
instantiation_state
instance_collection_base::current_collection_state(void) const {
	return index_collection.begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	By "end", we mean the beginning of the collection state deque, 
	the first item added to the the collection stack.  
	Can't actually dereference the returned iterator, 
	it's only useful for ending looped iterations.  
 */
instantiation_state
instance_collection_base::collection_state_end(void) const {
	return index_collection.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Will need two flavors: may and must?
	
	\return true if the new range *definitely* overlaps with previous
		static constant ranges.  Comparisons with dynamic ranges
		will conservatively return false; they will be resolved
		at unroll-time.  Also returns true if there was an error.  
		By "true", we mean a valid precise range of overlap.  
 */
const_range_list
instance_collection_base::detect_static_overlap(
		index_collection_item_ptr_type r) const {
	assert(r);
	assert(r->dimensions() == depth);
#if 0
	// DEBUG
	cerr << "In instance_collection_base::detect_static_overlap with this = "
		<< this << endl;
	r->dump(cerr << "index_collection_item_ptr_type r = ") << endl;
#endif
	if (r.is_a<const const_range_list>()) {
	index_collection_type::const_iterator i = index_collection.begin();
	for ( ; i!=index_collection.end(); i++) {
		// return upon first overlap error
		// later accumulate all overlaps.  
//		const_range_list ovlp((*i)->static_overlap(*r));
		const_range_list ovlp((*i)->get_indices()->static_overlap(*r));
		if (!ovlp.empty()) {
			return ovlp;
		}
		// else keep checking...
	}
	// if this point reached, then return false
	} // else just return false, can't know statically without analysis
	return const_range_list();	// empty constructed list
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TO DO: this can only be done with non-formals.  Check this.  
	If this instance is a collection, add the new range of indices
	which may be sparse or dense.  

	TO DO: type-check here?
	see scopespace::add_instance's definition body

	This is only applicable if this instantiation was initialized
	as a collective.  
	Pre-condition: The dimensions better damn well match!  
	\param r the instantiation statement with index ranges to be added.  
	\return Overlapping range (true) if error condition. 
	\sa detect_static_overlap
 */
const_range_list
instance_collection_base::add_instantiation_statement(
		index_collection_type::value_type r) {
	assert(r);
	index_collection_item_ptr_type i(r->get_indices());
	assert(depth || index_collection.empty());	// catches 0-D
	// TYPE CHECK!!!
	const_range_list overlap;
	if (i) {
		assert(depth == i->dimensions());
		overlap = detect_static_overlap(i);
	} else {
		assert(!depth);
	}
	// can the following accept NULL?
	index_collection.push_back(r);
	return overlap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Queries whether or not this is a template formal, by 
	checking its membership in the owner.  
 */
bool
instance_collection_base::is_template_formal(void) const {
	never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	if (def)
		return def->lookup_template_formal(key);
	else return false;		// owner is not a definition
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Queries whether or not this is a port formal, by 
	checking its membership in the owner.  
 */
bool
instance_collection_base::is_port_formal(void) const {
	never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	if (def)
		return def->lookup_port_formal(key);
	else return false;		// owner is not a definition
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For two template formals to be equivalent, their
	type and size must match, names need not.  
	Currently allows comparison of parameter and non-parameter
	formal types.  
	Is conservative because parameters (in sizes) may be dynamic, 
	or collective.  
 */
bool
instance_collection_base::template_formal_equivalent(
		never_ptr<const instance_collection_base> b) const {
	assert(b);
	// first make sure base types are equivalent.  
	count_ptr<const fundamental_type_reference>
		this_type(get_type_ref());
	count_ptr<const fundamental_type_reference>
		b_type(b->get_type_ref());
	if (!this_type->may_be_equivalent(*b_type)) {
		// then their instantiation types differ
		return false;
	}
	// then compare sizes and dimensionality
	return formal_size_equivalent(b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Much like equivalence for template formals, except that
	names also need to match for port formals.  
	Rationale: need to be able to refer to the public ports
	of a prototype, which must correspond to those of the definition, 
	and vice versa.  
 */
bool
instance_collection_base::port_formal_equivalent(
		never_ptr<const instance_collection_base> b) const {
	assert(b);
	// first make sure base types are equivalent.  
	count_ptr<const fundamental_type_reference>
		this_type(get_type_ref());
	count_ptr<const fundamental_type_reference>
		b_type(b->get_type_ref());
	if (!this_type->may_be_equivalent(*b_type)) {
		// then their instantiation types differ
		return false;
	}
	// then compare sizes and dimensionality
	if (!formal_size_equivalent(b))
		return false;
	// last, but not least, name must match
	return key == b->get_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just compares dimensionality and sizes of an instantiation
	in a template formal context.  
	This applies to both template formals and port formals.  
	Is conservative, not precise, in the case where one of the
	parameter sizes (dimension) is dynamic.  
	\param b the other template formal instantiation to compare against.  
	\return true if dimensionality and sizes are equal.  
 */
bool
instance_collection_base::formal_size_equivalent(
		never_ptr<const instance_collection_base> b) const {
	assert(b);
	if (depth != b->depth) {
		// useful error message here: dimensions don't match
		return false;
	}
	// formal instances can only be declared once, i.e. 
	// can't add instances to their collection.
	// and they must be dense arrays.  
	const size_t this_coll = index_collection.size();
	const size_t b_coll = b->index_collection.size();
	assert(this_coll <= 1);
	assert(b_coll <= 1);
	if (this_coll != b_coll) {
		// one is scalar, the other is array
		return false;
	}
	if (this_coll == 1) {
		// compare their collections
		const index_collection_type::const_iterator i =
			index_collection.begin();
		const index_collection_type::const_iterator j =
			b->index_collection.begin();
		// difficult: what if some dimensions are not static?
		// depends on some other former parameter?
		// This is when it would help to walk the 
		// former template formals list when visited with the second.  
		count_ptr<const const_range_list>
			ic((*i)->get_indices().is_a<const const_range_list>());
		count_ptr<const const_range_list>
			jc((*j)->get_indices().is_a<const const_range_list>());
		if (ic && jc) {
			// compare dense ranges in each dimension
			// must be equal!
			return (*ic == *jc);
		} else {
			// one of them is dynamic, thus we must conservatively
			return true;
		}
	} else {
		// both are scalar, single instances
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks for dimension and size equality between expression and 
	instantiation.  
	So far, only used by param_instance_collection derivatives, 
		in the context of checking template formals.  
	May be useful else where for connections.  
	\return true if dimensions *may* match.  
 */
bool
instance_collection_base::check_expression_dimensions(const param_expr& pe) const {
	assert(IS_A(const param_instance_collection*, this));
	// else is not an expression class!

	if (depth != pe.dimensions()) {
		// number of dimensions doesn't even match!
		// useful error message?
		return false;
	}
	// dimensions match
	if (depth != 0) {
		assert(index_collection.size() == 1);	// huh? true?
		// make sure sizes in each dimension
		index_collection_type::const_iterator i =
			index_collection.begin();
		count_ptr<const const_range_list>
			crl((*i)->get_indices().is_a<const const_range_list>());
		if (crl) {
			if (pe.has_static_constant_dimensions()) {
				const_range_list
					d(pe.static_constant_dimensions());
				return (*crl == d);
			} else {
				// is dynamic, conservatively return true
				return true;
			}
		} else {
			// is dynamic, conservatively return true
			return true;
		}
	} else {
		// depth == 0 means instantiation is a single instance.  
		// size may be zero b/c first statement hasn't been added yet
		assert(index_collection.size() <= 1);
		return (pe.dimensions() == 0);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Utility function for walking index collection list
	and collecting pointers.  
 */
void
instance_collection_base::collect_index_collection_pointers(
		persistent_object_manager& m) const {
	index_collection_type::const_iterator
		c_iter = index_collection.begin();
	const index_collection_type::const_iterator
		c_end = index_collection.end();
#if 0
	for ( ; c_iter!=c_end; c_iter++) {
		const index_collection_item_ptr_type& p = *c_iter;
		assert(p);
		p->collect_transient_info(m);
	}
#else
	for_each(c_iter, c_end, 
	unary_compose_void(
		bind2nd_argval_void(mem_fun_ref(
			&instance_management_base::collect_transient_info), m), 
		dereference<never_ptr, const instance_management_base>()
	)
	);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write out serial list of pointers to index collection items, 
	with pointers translated into indicies.  
	Also saves the depth (dimensions).
 */
void
instance_collection_base::write_index_collection_pointers(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	// need a specialization for count_ptrs
	write_value(f, depth);
	m.write_pointer_list(f, index_collection);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Loads serial list of pointers into index collection items, 
	with indices translated into pointers.  
	Also restores depth (dimensions).
 */
void
instance_collection_base::load_index_collection_pointers(
		persistent_object_manager& m) {
	istream& f = m.lookup_read_buffer(this);
	// need a specialization for count_ptrs
	read_value(f, depth);
	m.read_pointer_list(f, index_collection);
}

//=============================================================================
// class datatype_instance_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(datatype_instance_collection, 
	DATA_INSTANCE_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
datatype_instance_collection::datatype_instance_collection() :
		instance_collection_base() {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_collection::datatype_instance_collection(const scopespace& o, 
		const string& n, 
		const size_t d) : 
		instance_collection_base(o, n, d)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_collection::~datatype_instance_collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
datatype_instance_collection::what(ostream& o) const {
	return o << "datatype-inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
datatype_instance_collection::get_type_ref(void) const {
	assert(!index_collection.empty());
	return (*index_collection.begin())->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a datatype reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
count_ptr<instance_reference_base>
datatype_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions -- when attach_indices() invoked
	return count_ptr<datatype_instance_reference>(
		new datatype_instance_reference(
			never_ptr<const datatype_instance_collection>(this), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a member reference to a datatype, 
	and pushes it onto the context's object_stack.  
	\param b is the parent owner of this instantiation referenced.  
 */
count_ptr<member_instance_reference_base>
datatype_instance_collection::make_member_instance_reference(
		count_ptr<const simple_instance_reference> b) const {
	assert(b);
	// maybe verify that b contains this, as sanity check
	return count_ptr<datatype_member_instance_reference>(
		new datatype_member_instance_reference(
			b, never_ptr<const datatype_instance_collection>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
datatype_instance_collection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DATA_INSTANCE_COLLECTION_TYPE_KEY)) {
	// don't bother visit the owner, assuming that's the caller
	// go through index_collection
	collect_index_collection_pointers(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
datatype_instance_collection::construct_empty(const int i) {
	return new datatype_instance_collection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need special case handling for built-in types? int and bool?
 */
void
datatype_instance_collection::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, owner);
	write_string(f, key);
	write_index_collection_pointers(m);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need special case handling for built-in types? int and bool?
 */
void
datatype_instance_collection::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, owner);
	read_string(f, const_cast<string&>(key));
	load_index_collection_pointers(m);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class process_instance_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(process_instance_collection, 
	PROCESS_INSTANCE_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
process_instance_collection::process_instance_collection() :
		instance_collection_base() {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance_collection::process_instance_collection(const scopespace& o, 
		const string& n, 
		const size_t d) : 
		instance_collection_base(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance_collection::~process_instance_collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instance_collection::what(ostream& o) const {
	return o << "process-inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
process_instance_collection::get_type_ref(void) const {
	assert(!index_collection.empty());
	return (*index_collection.begin())->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a process reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
count_ptr<instance_reference_base>
process_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	return count_ptr<process_instance_reference>(
		new process_instance_reference(
			never_ptr<const process_instance_collection>(this), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
		// may attach in parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a member reference to a process, 
	and pushes it onto the context's object_stack.  
	\param b is the parent owner of this instantiation referenced.  
 */
count_ptr<member_instance_reference_base>
process_instance_collection::make_member_instance_reference(
		count_ptr<const simple_instance_reference> b) const {
	assert(b);
	// maybe verify that b contains this, as sanity check
	return count_ptr<process_member_instance_reference>(
		new process_member_instance_reference(
			b, never_ptr<const process_instance_collection>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance_collection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PROCESS_INSTANCE_COLLECTION_TYPE_KEY)) {
	// don't bother visit the owner, assuming that's the caller
	// go through index_collection
	collect_index_collection_pointers(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
process_instance_collection::construct_empty(const int i) {
	return new process_instance_collection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance_collection::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, owner);
	write_string(f, key);
	write_index_collection_pointers(m);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance_collection::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, owner);
	read_string(f, const_cast<string&>(key));
	load_index_collection_pointers(m);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class channel_instance_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(channel_instance_collection, 
	CHANNEL_INSTANCE_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
channel_instance_collection::channel_instance_collection() :
		instance_collection_base() {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance_collection::channel_instance_collection(const scopespace& o, 
		const string& n, 
		const size_t d) : 
		instance_collection_base(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance_collection::~channel_instance_collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instance_collection::what(ostream& o) const {
	return o << "channel-inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
channel_instance_collection::get_type_ref(void) const {
	assert(!index_collection.empty());
	return (*index_collection.begin())->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a channel reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
count_ptr<instance_reference_base>
channel_instance_collection::make_instance_reference(void) const {
	cerr << "channel_instance_collection::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	return count_ptr<channel_instance_reference>(
		new channel_instance_reference(
			never_ptr<const channel_instance_collection>(this), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a member reference to a channel, 
	and pushes it onto the context's object_stack.  
	\param b is the parent owner of this instantiation referenced.  
 */
count_ptr<member_instance_reference_base>
channel_instance_collection::make_member_instance_reference(
		count_ptr<const simple_instance_reference> b) const {
	assert(b);
	// maybe verify that b contains this, as sanity check
	return count_ptr<channel_member_instance_reference>(
		new channel_member_instance_reference(
			b, never_ptr<const channel_instance_collection>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance_collection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY)) {
	// don't bother visit the owner, assuming that's the caller
	// go through index_collection
	collect_index_collection_pointers(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
channel_instance_collection::construct_empty(const int i) {
	return new channel_instance_collection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance_collection::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, owner);
	write_string(f, key);
	write_index_collection_pointers(m);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance_collection::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, owner);
	read_string(f, const_cast<string&>(key));
	load_index_collection_pointers(m);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

