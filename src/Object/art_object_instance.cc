/**
	\file "art_object_instance.cc"
	Method definitions for instance collection and 
	instantiation statement classes.  
 */

#include <iostream>
#include <algorithm>

#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_expr.h"
#include "art_built_ins.h"
#include "persistent_object_manager.tcc"

#include "multikey_qmap.h"
#include "compose.h"
#include "binders.h"
#include "ptrs_functional.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {
using namespace ADS;		// for composition functors

//=============================================================================
// class instance_collection_base method definitions

const never_const_ptr<instance_collection_base>
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
		object(), owner(never_const_ptr<scopespace>(&o)),
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
never_const_ptr<definition_base>
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
	if (r.is_a<const_range_list>()) {
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
	if (i)	{
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
	never_const_ptr<definition_base>
		def(owner.is_a<definition_base>());
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
	never_const_ptr<definition_base>
		def(owner.is_a<definition_base>());
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
		never_const_ptr<instance_collection_base> b) const {
	assert(b);
	// first make sure base types are equivalent.  
	count_const_ptr<fundamental_type_reference>
		this_type(get_type_ref());
	count_const_ptr<fundamental_type_reference>
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
		never_const_ptr<instance_collection_base> b) const {
	assert(b);
	// first make sure base types are equivalent.  
	count_const_ptr<fundamental_type_reference>
		this_type(get_type_ref());
	count_const_ptr<fundamental_type_reference>
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
		never_const_ptr<instance_collection_base> b) const {
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
		count_const_ptr<const_range_list>
			ic((*i)->get_indices().is_a<const_range_list>());
		count_const_ptr<const_range_list>
			jc((*j)->get_indices().is_a<const_range_list>());
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
		count_const_ptr<const_range_list>
			crl((*i)->get_indices().is_a<const_range_list>());
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
		const_dereference<never_const_ptr, instance_management_base>()
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
count_const_ptr<fundamental_type_reference>
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
			never_const_ptr<datatype_instance_collection>(this), 
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
		count_const_ptr<simple_instance_reference> b) const {
	assert(b);
	// maybe verify that b contains this, as sanity check
	return count_ptr<datatype_member_instance_reference>(
		new datatype_member_instance_reference(
			b, never_const_ptr<datatype_instance_collection>(this)));
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
count_const_ptr<fundamental_type_reference>
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
			never_const_ptr<process_instance_collection>(this), 
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
		count_const_ptr<simple_instance_reference> b) const {
	assert(b);
	// maybe verify that b contains this, as sanity check
	return count_ptr<process_member_instance_reference>(
		new process_member_instance_reference(
			b, never_const_ptr<process_instance_collection>(this)));
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
// class param_instance_collection method definitions

/**
	Private empty constructor.  
 */
param_instance_collection::param_instance_collection() :
		instance_collection_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instance_collection::param_instance_collection(const scopespace& o, 
		const string& n, 
		const size_t d) : 
		instance_collection_base(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instance_collection::~param_instance_collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_instance_collection::dump(ostream& o) const {
	get_type_ref()->dump(o) << " " << key;
	index_collection_type::const_iterator i = index_collection.begin();
	const index_collection_type::const_iterator e = index_collection.end();
	for ( ; i!=e; i++) {
		assert(*i);
		index_collection_item_ptr_type ind((*i)->get_indices());
		if (ind)
			ind->dump(o) << endl;
	}
	count_const_ptr<param_expr> init_def(default_value());
	if (init_def) {
		if (is_template_formal())
			init_def->dump(o << " (default = ") << ")";
		else	init_def->dump(o << " (init = ") << ")";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	To determine whether or not this is a formal parameter, 
	look itself up in the owning namespace.  
 */
bool
param_instance_collection::is_template_formal(void) const {
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For multidimensional instances, we don't keep track of initialization
	of individual elements at compile-time, just conservatively 
	return true, that the instance MAY be initialized.  
	Template formals are considered initialized because concrete
	types will always have supplied parameters.  
	The counterpart must_be_initialized will check at unroll time
	whether or not an instance is definitely initialized.  
	\return true if the instance may be initialized.  
	\sa must_be_initialized
 */
bool
param_instance_collection::may_be_initialized(void) const {
	if (dimensions() || is_template_formal())
		return true;
	else {
		// is not a template formal, thus we interpret
		// the "default_value" field as a one-time initialization
		// value.  
		count_const_ptr<param_expr> ret(default_value());
		if (ret)
			return ret->may_be_initialized();
		// if there's no initial value, then it is definitely
		// NOT already initialized.  
		else return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	At compile time, we don't keep track of arrays, thus
	one cannot conclude that a member of an array is definitely 
	initialized.  
	\sa may_be_initialized
 */
bool
param_instance_collection::must_be_initialized(void) const {
	if (dimensions())
		return false;
	else if (is_template_formal())
		return true;
	else {
		// is not a template formal, thus we interpret
		// the "default_value" field as a one-time initialization
		// value.  
		count_const_ptr<param_expr> ret(default_value());
		if (ret)
			return ret->must_be_initialized();
		// if there's no initial value, then it is definitely
		// NOT already initialized.  
		else return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_instance_collection::is_static_constant(void) const {
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
param_instance_collection::is_loop_independent(void) const {
	
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	1) Parameters cannot be in public ports.  
	2) Thus they cannot even be referenced.  
	3) This is just a placeholder that should never be called.  
 */
count_ptr<member_instance_reference_base>
param_instance_collection::make_member_instance_reference(
		count_const_ptr<simple_instance_reference> b) const {
	typedef	count_ptr<member_instance_reference_base>	return_type;
	assert(b);
	cerr << "Referencing parameter members is strictly forbidden!" << endl;
	assert(0);
	return return_type(NULL);
}

//=============================================================================
// struct pbool_instance method definitions
// not really methods...

bool
operator == (const pbool_instance& p, const pbool_instance& q) {
	assert(p.instantiated && q.instantiated);
	if (p.valid && q.valid) {
		return p.value == q.value;
	} else return (p.valid == q.valid); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream& o, const pbool_instance& p) {
	assert(p.instantiated);
	if (p.valid) {
		return o << "?";
	} else	return o << p.value;
}

//=============================================================================
// class pbool_instance_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pbool_instance_collection, 
	PBOOL_INSTANCE_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
pbool_instance_collection::pbool_instance_collection() :
		param_instance_collection(), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instance_collection::pbool_instance_collection(const scopespace& o, 
		const string& n) :
		param_instance_collection(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instance_collection::pbool_instance_collection(const scopespace& o, 
		const string& n, 
		const size_t d) :
		param_instance_collection(o, n, d), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
pbool_instance_collection::pbool_instance_collection(const scopespace& o, 
		const string& n, 
		count_const_ptr<pbool_expr> i) :
		param_instance_collection(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(i) {
	assert(type_check_actual_param_expr(*i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instance_collection::pbool_instance_collection(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d, 
		count_const_ptr<pbool_expr> i) :
		param_instance_collection(o, n, d), ival(i) {
	assert(type_check_actual_param_expr(*i));
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_instance_collection::what(ostream& o) const {
	return o << "pbool-inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
pbool_instance_collection::get_type_ref(void) const {
	return pbool_type_ptr;
		// defined in "art_built_ins.h"
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes a parameter instance with an expression.
	Parameter instance collections cannot be initialized statically
	at compile-time because we're too lazy to keep track of 
	individual elements until unroll-time.  
	The ival may only be initialized once, enforced by assertions.  
	Note: a parameter is considered "usable" or initialized if it is 
	has a valid initial value expression OR it is a template formal.  
	Later, deal with loop indices.
	MAKE SURE this is not a template_formal, (can't assign to those).
	Ignore initializations of non-scalar params.  
		(don't keep track of arrays statically)
	Handled in unroll time with param_expression_assignment objects.  
	\param e the rvalue expression.
	\return true.  
	\sa may_be_initialized
	\sa must_be_initialized
 */
bool
pbool_instance_collection::initialize(count_const_ptr<pbool_expr> e) {
	assert(e);
	assert(!ival);		// must not already be initialized or assigned
	if (dimensions() == 0) {
		if (type_check_actual_param_expr(*e)) {
			ival = e;
			return true;
		} else {
			return false;
		}
	} 
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_instance_collection::assign_default_value(count_const_ptr<param_expr> p) {
	count_const_ptr<pbool_expr> b(p.is_a<pbool_expr>());
	if (b && type_check_actual_param_expr(*b)) {
		ival = b;
		return true;
	}
	else return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<param_expr>
pbool_instance_collection::default_value(void) const {
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<pbool_expr>
pbool_instance_collection::initial_value(void) const {
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
count_ptr<instance_reference_base>
pbool_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	return count_ptr<param_instance_reference>(
		new pbool_instance_reference(
			never_ptr<pbool_instance_collection>(
			const_cast<pbool_instance_collection*>(this)), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether or not a pbool was passed to a formal 
	pbool parameter in a template.  
	Should also check dimensionality and size.  
 */
bool
pbool_instance_collection::type_check_actual_param_expr(const param_expr& pe) const {
	const pbool_expr* pb(IS_A(const pbool_expr*, &pe));
	if (!pb) {
		// useful error message?
		return false;
	}
	// only for formal parameters is this assertion valid.  
	assert(index_collection.size() <= 1);
	// check dimensions (is conservative with dynamic sizes)
	return check_expression_dimensions(*pb);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instance_collection::instantiate_indices(
		const index_collection_item_ptr_type& i) {
	if (!collection)
		collection = excl_ptr<collection_type>(
			collection_type::make_multikey_qmap(depth));
	assert(collection);
	collection_type& collection_ref = *collection;
	if (i) {
#if 0
		cerr << "multidimensional index instantiation!  "
			"let's not get too fancy here..." << endl;
#endif
		// indices is a range_expr_list (base class)
		// resolve into constants now using const_range_list
		// if unable, (b/c uninitialized) then report error
		const_range_list ranges;	// initially empty
		if (!i->resolve_ranges(ranges)) {
			// fail
			cerr << "ERROR: unable to resolve indices "
				"for instantiation: ";
			i->dump(cerr) << endl;
			exit(1);
		} 
		// else success
		// now iterate through, unrolling one at a time...
		// stop as soon as there is a conflict
		// later: factor this out into common helper class
		excl_ptr<multikey_generator_base<int> > key_gen = 
			excl_ptr<multikey_generator_base<int> >(
			multikey_generator_base<int>::make_multikey_generator(
				ranges.size()));
		assert(key_gen);
		multikey_base<int>::iterator li =
			key_gen->get_lower_corner().begin();
		multikey_base<int>::iterator ui =
			key_gen->get_upper_corner().begin();
		const_range_list::const_iterator ri = ranges.begin();
		const const_range_list::const_iterator re = ranges.end();
		for ( ; ri != re; ri++, li++, ui++) {
			*li = ri->first;
			*ui = ri->second;
		}
		key_gen->initialize();
#if 0
		excl_ptr<multikey_base<int> > key_end = 
			excl_ptr<multikey_base<int> >(
			multikey_base<int>::make_multikey(ranges.size()));
		assert(key_end);
		copy(key_gen->begin(), key_gen->end(), key_end->begin());
		never_const_ptr<multikey_base<int> >
			key_gen_base(key_gen.is_a<multikey_base<int> >());
		assert(key_gen_base);
		never_const_ptr<multikey_base<int> >
			key_end_base(key_end.is_a<multikey_base<int> >());
		assert(key_end_base);
#endif
		multikey_generator_base<int>& key_gen_ref = *key_gen;
		const multikey_base<int>& key_end_ref =
			key_gen_ref.get_lower_corner();
		do {
#if 0
			multikey_base<int>::const_iterator ci =
				key_gen->begin();
			for ( ; ci!=key_gen->end(); ci++) {
				cerr << '[' << *ci << ']';
			}
			cerr << endl;
#endif
//			pbool_instance& pi = collection_ref[*key_gen_base];
			pbool_instance& pi = collection_ref[key_gen_ref];
			if (pi.instantiated) {
				cerr << "ERROR: Index already instantiated!"
					<< endl;
				exit(1);
			}
			pi.instantiated = true;
			assert(!pi.valid);
//			(*key_gen)++;
			key_gen_ref++;
		// while (*key_gen_base != *key_end_base);
		} while (key_gen_ref != key_end_ref);
	} else {
		// 0-D, or scalar
		assert(!depth);
		const never_ptr<scalar_type>
			the(collection.is_a<scalar_type>());
		assert(the);
		pbool_instance& pi = *the;
		if (pi.instantiated) {
			cerr << "ERROR: Already instantiated!" << endl;
			exit(1);
		}
		pi.instantiated = true;
		assert(!pi.valid);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version assumes collection is a scalar.  
 */
bool
pbool_instance_collection::lookup_value(bool& v) const {
	assert(!depth);
	if (!collection) {
		// hasn't been instantiated yet!
		cerr << "ERROR: Reference to uninstantiated pbool!" << endl;
		return false;
	}
	const never_const_ptr<scalar_type> the(collection.is_a<scalar_type>());
	assert(the);
	const pbool_instance& pi = *the;
	if (pi.valid) {
		v = pi.value;
	} else {
		dump(cerr << "ERROR: use of uninitialized ") << endl;
	}
	return pi.valid;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assumes that index resolves down to a single integer.  
	Returns value of a single integer, if it can be resolved.  
	If integer is uninitialized, report as error.  
 */
bool
pbool_instance_collection::lookup_value(bool& v,
		 const multikey_base<int>& i) const {
	assert(depth == i.dimensions());
	if (!collection) {
		// hasn't been instantiated yet!
		cerr << "ERROR: reference to uninstantiated pbool!" << endl;
		return false;
	}
	const pbool_instance& pi = (*collection)[i];
	if (pi.valid) {
		v = pi.value;
	} else {
		cerr << "ERROR: reference to uninitialized pbool " <<
			get_qualified_name() << " at index: " << i << endl;
	}
	return pi.valid;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l list in which to accumulate values.
	\param r the ranges, must be valid.
	\return false on error, e.g. if value doesn't exist or 
		is uninitialized; true on success.
 */
bool
pbool_instance_collection::lookup_value_collection(
		list<bool>& l, const const_range_list& r) const {
	assert(collection);
	const collection_type& collection_ref = *collection;
	assert(!r.empty());
	const excl_const_ptr<multikey_base<int> > lower(r.lower_multikey());
	const excl_const_ptr<multikey_base<int> > upper(r.upper_multikey());
	assert(lower);
	assert(upper);
	const excl_ptr<multikey_generator_base<int> >
		key_gen(multikey_generator_base<int>::make_multikey_generator(
			lower->dimensions()));
	assert(key_gen);
	multikey_generator_base<int>& key_gen_ref = *key_gen;
	copy(lower->begin(), lower->end(), key_gen->get_lower_corner().begin());
	copy(upper->begin(), upper->end(), key_gen->get_upper_corner().begin());
	key_gen->initialize();
	bool ret = true;
	do {
		const pbool_instance& pi = collection_ref[key_gen_ref];
		// assert(pi.instantiated);	// else earlier check failed
		if (!pi.instantiated)
			cerr << "FATAL: reference to uninstantiated pbool index "
				<< key_gen_ref << endl;
		else if (!pi.valid)
			cerr << "ERROR: reference to uninitialized pbool index "
				<< key_gen_ref << endl;
		ret &= (pi.valid && pi.instantiated);
		l.push_back(pi.value);
		key_gen_ref++;
	} while (key_gen_ref != key_gen_ref.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands indices which may be under-specified into explicit
	indices for the implicit subslice, if it is densely packed.  
	Depends on the current state of the collection.  
	\param l is list of indices, which may be under-specified, 
		or even empty.
	\return fully-specified index list, or empty list if there is error.
 */
const_index_list
pbool_instance_collection::resolve_indices(const const_index_list& l) const {
	const size_t l_size = l.size();
	assert(collection);
	if (dimensions() == l_size) {
		// already fully specified
		return l;
	}
	// convert indices to pair of list of multikeys
	if (!l_size) {
		return const_index_list(l, collection->is_compact());
	}
	// else construct slice
	list<int> lower_list, upper_list;
	transform(l.begin(), l.end(), back_inserter(lower_list), 
		unary_compose(
			mem_fun_ref(&const_index::lower_bound), 
			const_dereference<count_ptr, const_index>()
		)
	);
	transform(l.begin(), l.end(), back_inserter(upper_list), 
		unary_compose(
			mem_fun_ref(&const_index::upper_bound), 
			const_dereference<count_ptr, const_index>()
		)
	);
	return const_index_list(l, 
		collection->is_compact_slice(lower_list, upper_list));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns a single value.
	Only call this if this is scalar, 0-D.
	Decision: should we allow multiple assignments of the same value?
	\return true on error.  
 */
bool
pbool_instance_collection::assign(const bool b) {
	assert(collection);
	const never_ptr<scalar_type> the(collection.is_a<scalar_type>());
	assert(the);
	pbool_instance& pi = *the;
	return !(pi = b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns a single value, using an index.
	Only call this if this is non-scalar (array).  
	\return true on error.
 */
bool
pbool_instance_collection::assign(const multikey_base<int>& k, const bool b) {
	assert(collection);
	pbool_instance& pi = (*collection)[k];
	return !(pi = b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instance_collection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PBOOL_INSTANCE_COLLECTION_TYPE_KEY)) {
	// don't bother visit the owner, assuming that's the caller
	// go through index_collection
	collect_index_collection_pointers(m);
	if (ival)
		ival->collect_transient_info(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pbool_instance_collection::construct_empty(const int i) {
	return new pbool_instance_collection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instance_collection::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, owner);
	write_string(f, key);
	write_index_collection_pointers(m);
	m.write_pointer(f, ival);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instance_collection::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, owner);
	read_string(f, const_cast<string&>(key));
	load_index_collection_pointers(m);
	m.read_pointer(f, ival);
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// struct pint_instance method definitions
// not really methods...

bool
operator == (const pint_instance& p, const pint_instance& q) {
	assert(p.instantiated && q.instantiated);
	if (p.valid && q.valid) {
		return p.value == q.value;
	} else return (p.valid == q.valid); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream& o, const pint_instance& p) {
	assert(p.instantiated);
	if (p.valid) {
		return o << "?";
	} else	return o << p.value;
}

//=============================================================================
// class pint_instance_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_instance_collection, 
	PINT_INSTANCE_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pint_instance_collection::pint_instance_collection() :
		param_instance_collection(), ival(NULL), collection(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instance_collection::pint_instance_collection(const scopespace& o, 
		const string& n) :
		param_instance_collection(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instance_collection::pint_instance_collection(const scopespace& o, 
		const string& n, 
		const size_t d) :
		param_instance_collection(o, n, d), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instance_collection::pint_instance_collection(const scopespace& o, 
		const string& n, 
		count_const_ptr<pint_expr> i) :
		param_instance_collection(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(i) {
	assert(type_check_actual_param_expr(*i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instance_collection::pint_instance_collection(const scopespace& o, 
		const string& n, 
		const size_t d, 
		count_const_ptr<pint_expr> i) :
		param_instance_collection(o, n, d), ival(i) {
	assert(type_check_actual_param_expr(*i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_instance_collection::what(ostream& o) const {
	return o << "pint-inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
pint_instance_collection::get_type_ref(void) const {
	return pint_type_ptr;
		// defined in "art_built_ins.h"
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes a parameter instance with an expression.
	The ival may only be initialized once, enforced by assertions.  
	Note: a parameter is considered "usable" if it is 
	initialized OR it is a template formal.  
	Only bother initializing scalar variables, 
		ignore for initialization of non-scalars.  
	A real assignment will be tracked in a param_expression_assignment
		object.  
	MAKE sure this is not a template_formal!!! that is invalid!
	\param e the rvalue expression.
	\return false if there was error.  
	\sa may_be_initialized
	\sa must_be_initialized
 */
bool
pint_instance_collection::initialize(count_const_ptr<pint_expr> e) {
	assert(e);
	assert(!ival);
	if (dimensions() == 0) {
		if (type_check_actual_param_expr(*e)) {
			ival = e;
			return true;
		} else {
			return false;
		}
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_instance_collection::assign_default_value(count_const_ptr<param_expr> p) {
	count_const_ptr<pint_expr> i(p.is_a<pint_expr>());
	if (i && type_check_actual_param_expr(*i)) {
		ival = i;
		return true;
	}
	else return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<param_expr>
pint_instance_collection::default_value(void) const {
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<pint_expr>
pint_instance_collection::initial_value(void) const {
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
count_ptr<instance_reference_base>
pint_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	return count_ptr<param_instance_reference>(
		new pint_instance_reference(
			never_ptr<pint_instance_collection>(
			const_cast<pint_instance_collection*>(this)), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether or not a pint was passed to a formal 
	pint parameter in a template.  
	Should also check dimensionality and size.  
 */
bool
pint_instance_collection::type_check_actual_param_expr(const param_expr& pe) const {
	const pint_expr* pi(IS_A(const pint_expr*, &pe));
	if (!pi) {
		// useful error message?
		return false;
	}
	// only for formal parameters is this assertion valid.  
	assert(index_collection.size() <= 1);
	// check dimensions (is conservative with dynamic sizes)
	return check_expression_dimensions(*pi);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instance_collection::instantiate_indices(
		const index_collection_item_ptr_type& i) {
	if (!collection)
		collection = excl_ptr<collection_type>(
			collection_type::make_multikey_qmap(depth));
	assert(collection);
	collection_type& collection_ref = *collection;
	if (i) {
#if 0
		cerr << "multidimensional index instantiation!  "
			"let's not get too fancy here..." << endl;
#endif
		// indices is a range_expr_list (base class)
		// resolve into constants now using const_range_list
		// if unable, (b/c uninitialized) then report error
		const_range_list ranges;	// initially empty
		if (!i->resolve_ranges(ranges)) {
			// fail
			cerr << "ERROR: unable to resolve indices "
				"for instantiation: ";
			i->dump(cerr) << endl;
			exit(1);
		} 
		// else success
		// now iterate through, unrolling one at a time...
		// stop as soon as there is a conflict
		// later: factor this out into common helper class
		excl_ptr<multikey_generator_base<int> > key_gen = 
			excl_ptr<multikey_generator_base<int> >(
			multikey_generator_base<int>::make_multikey_generator(
				ranges.size()));
		assert(key_gen);
		multikey_base<int>::iterator li =
			key_gen->get_lower_corner().begin();
		multikey_base<int>::iterator ui =
			key_gen->get_upper_corner().begin();
		const_range_list::const_iterator ri = ranges.begin();
		const const_range_list::const_iterator re = ranges.end();
		for ( ; ri != re; ri++, li++, ui++) {
			*li = ri->first;
			*ui = ri->second;
		}
		key_gen->initialize();
#if 0
		excl_ptr<multikey_base<int> > key_end = 
			excl_ptr<multikey_base<int> >(
			multikey_base<int>::make_multikey(ranges.size()));
		assert(key_end);
		copy(key_gen->begin(), key_gen->end(), key_end->begin());
		never_const_ptr<multikey_base<int> >
			key_gen_base(key_gen.is_a<multikey_base<int> >());
		assert(key_gen_base);
		never_const_ptr<multikey_base<int> >
			key_end_base(key_end.is_a<multikey_base<int> >());
		assert(key_end_base);
#endif
		multikey_generator_base<int>& key_gen_ref = *key_gen;
		const multikey_base<int>& key_end_ref =
			key_gen_ref.get_lower_corner();
		do {
#if 0
			multikey_base<int>::const_iterator ci =
				key_gen->begin();
			for ( ; ci!=key_gen->end(); ci++) {
				cerr << '[' << *ci << ']';
			}
			cerr << endl;
#endif
//			pint_instance& pi = collection_ref[*key_gen_base];
			pint_instance& pi = collection_ref[key_gen_ref];
			if (pi.instantiated) {
				cerr << "ERROR: Index already instantiated!"
					<< endl;
				exit(1);
			}
			pi.instantiated = true;
			assert(!pi.valid);
//			(*key_gen)++;
			key_gen_ref++;
		// while (*key_gen_base != *key_end_base);
		} while (key_gen_ref != key_end_ref);
	} else {
		// 0-D, or scalar
		assert(!depth);
		const never_ptr<scalar_type>
			the(collection.is_a<scalar_type>());
		assert(the);
		pint_instance& pi = *the;
		if (pi.instantiated) {
			cerr << "ERROR: Already instantiated!" << endl;
			exit(1);
		}
		pi.instantiated = true;
		assert(!pi.valid);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version assumes collection is a scalar.  
 */
bool
pint_instance_collection::lookup_value(int& v) const {
	assert(!depth);
	if (!collection) {
		// hasn't been instantiated yet!
		cerr << "ERROR: Reference to uninstantiated pint!" << endl;
		return false;
	}
	const never_const_ptr<scalar_type> the(collection.is_a<scalar_type>());
	assert(the);
	const pint_instance& pi = *the;
	if (pi.valid) {
		v = pi.value;
	} else {
		dump(cerr << "ERROR: use of uninitialized ") << endl;
	}
	return pi.valid;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assumes that index resolves down to a single integer.  
	Returns value of a single integer, if it can be resolved.  
	If integer is uninitialized, report as error.  
 */
bool
pint_instance_collection::lookup_value(int& v,
		 const multikey_base<int>& i) const {
	assert(depth == i.dimensions());
	if (!collection) {
		// hasn't been instantiated yet!
		cerr << "ERROR: reference to uninstantiated pint!" << endl;
		return false;
	}
	const pint_instance& pi = (*collection)[i];
	if (pi.valid) {
		v = pi.value;
	} else {
		cerr << "ERROR: reference to uninitialized pint " <<
			get_qualified_name() << " at index: " << i << endl;
	}
	return pi.valid;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ABANDONED
/**
	Without arguments, this is an implicit reference to the entire
	collection.  
	If this collection is not dense, then this returns NULL.  
	Shouldn't call this function for scalar (0-D)?
	\return pointer to N-dimensional collection of integers, if all
		values are successfully resolved, else NULL.  
 */
count_const_ptr<pint_instance_collection::value_type>
pint_instance_collection::lookup_value_collection(void) const {
	typedef	count_const_ptr<pint_instance_collection::value_type>
			return_type;
	// first: are all pint_instances valid?  find the first invalid one...
	assert(collection);
	const collection_type::const_iterator cend = collection->end();
	collection_type::const_iterator invalid =
		find_if(collection->begin(), cend, 
			not(pint_instance::is_valid()));

	return return_type(NULL);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l list in which to accumulate values.
	\param r the ranges, must be valid.
	\return false on error, e.g. if value doesn't exist or 
		is uninitialized; true on success.
 */
bool
pint_instance_collection::lookup_value_collection(
		list<int>& l, const const_range_list& r) const {
	assert(collection);
	const collection_type& collection_ref = *collection;
	assert(!r.empty());
	const excl_const_ptr<multikey_base<int> > lower(r.lower_multikey());
	const excl_const_ptr<multikey_base<int> > upper(r.upper_multikey());
	assert(lower);
	assert(upper);
	const excl_ptr<multikey_generator_base<int> >
		key_gen(multikey_generator_base<int>::make_multikey_generator(
			lower->dimensions()));
	assert(key_gen);
	multikey_generator_base<int>& key_gen_ref = *key_gen;
	copy(lower->begin(), lower->end(), key_gen->get_lower_corner().begin());
	copy(upper->begin(), upper->end(), key_gen->get_upper_corner().begin());
	key_gen->initialize();
	bool ret = true;
	do {
		const pint_instance& pi = collection_ref[key_gen_ref];
		// assert(pi.instantiated);	// else earlier check failed
		if (!pi.instantiated)
			cerr << "FATAL: reference to uninstantiated pint index "
				<< key_gen_ref << endl;
		else if (!pi.valid)
			cerr << "ERROR: reference to uninitialized pint index "
				<< key_gen_ref << endl;
		ret &= (pi.valid && pi.instantiated);
		l.push_back(pi.value);
		key_gen_ref++;
	} while (key_gen_ref != key_gen_ref.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands indices which may be under-specified into explicit
	indices for the implicit subslice, if it is densely packed.  
	Depends on the current state of the collection.  
	\param l is list of indices, which may be under-specified, 
		or even empty.
	\return fully-specified index list, or empty list if there is error.
 */
const_index_list
pint_instance_collection::resolve_indices(const const_index_list& l) const {
	const size_t l_size = l.size();
	assert(collection);
	if (dimensions() == l_size) {
		// already fully specified
		return l;
	}
	// convert indices to pair of list of multikeys
	if (!l_size) {
		return const_index_list(l, collection->is_compact());
	}
	// else construct slice
	list<int> lower_list, upper_list;
	transform(l.begin(), l.end(), back_inserter(lower_list), 
		unary_compose(
			mem_fun_ref(&const_index::lower_bound), 
			const_dereference<count_ptr, const_index>()
		)
	);
	transform(l.begin(), l.end(), back_inserter(upper_list), 
		unary_compose(
			mem_fun_ref(&const_index::upper_bound), 
			const_dereference<count_ptr, const_index>()
		)
	);
	return const_index_list(l, 
		collection->is_compact_slice(lower_list, upper_list));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns a single value.
	Only call this if this is scalar, 0-D.
	Decision: should we allow multiple assignments of the same value?
	\return true on error.  
 */
bool
pint_instance_collection::assign(const int i) {
	assert(collection);
	const never_ptr<scalar_type> the(collection.is_a<scalar_type>());
	assert(the);
	pint_instance& pi = *the;
	return !(pi = i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns a single value, using an index.
	Only call this if this is non-scalar (array).  
	\return true on error.
 */
bool
pint_instance_collection::assign(const multikey_base<int>& k, const int i) {
	assert(collection);
	pint_instance& pi = (*collection)[k];
	return !(pi = i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instance_collection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PINT_INSTANCE_COLLECTION_TYPE_KEY)) {
	// don't bother visit the owner, assuming that's the caller
	// go through index_collection
	collect_index_collection_pointers(m);
	// Is ival really crucial in object?  will be unrolled anyhow
	if (ival)
		ival->collect_transient_info(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Later: will become dimension-specific.
 */
persistent*
pint_instance_collection::construct_empty(const int i) {
	return new pint_instance_collection();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instance_collection::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, owner);
	write_string(f, key);
	write_index_collection_pointers(m);
	m.write_pointer(f, ival);

#if 0
	// what's a good way of writing out multikey_qmaps?
	{
	static const size_t zero = 0;
	if (collection) {
		write_value(f, zero+1);
		write_map(f, *collection);
	} else {
		// since the first value of write_map is a size_t
		write_value(f, zero);
	}
	}
#endif
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instance_collection::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, owner);
	read_string(f, const_cast<string&>(key));
	load_index_collection_pointers(m);
	m.read_pointer(f, ival);

#if 0
	{
	size_t size;
	read_value(f, size);
	if (size) {
		read_map(f, *collection);
	}
	}
#endif
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
count_const_ptr<fundamental_type_reference>
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
			never_const_ptr<channel_instance_collection>(this), 
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
		count_const_ptr<simple_instance_reference> b) const {
	assert(b);
	// maybe verify that b contains this, as sanity check
	return count_ptr<channel_member_instance_reference>(
		new channel_member_instance_reference(
			b, never_const_ptr<channel_instance_collection>(this)));
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
//=============================================================================
// class instantiation_statement method definitions

/**	Private empty constructor. */
instantiation_statement::instantiation_statement(void) :
//		inst_base(NULL), type_base(NULL),
		indices(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_statement::instantiation_statement(
//		never_ptr<instance_collection_base> b, 
//		count_const_ptr<fundamental_type_reference> t, 
		const index_collection_item_ptr_type& i) :
//		inst_base(NULL), type_base(t), 
		indices(i) {
//	assert(inst_base);		// attach later...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_statement::~instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instantiation_statement::dump(ostream& o) const {
	count_const_ptr<fundamental_type_reference>
		type_base(get_type_ref());
	assert(type_base);
	type_base->dump(o) << " ";
	never_const_ptr<instance_collection_base>
		inst_base(get_inst_base());
	if(inst_base) {
		o << inst_base->get_name();
	} else {
		o << "<unknown>";
	}
	if (indices)
		indices->dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instantiation_statement::get_name(void) const {
	never_const_ptr<instance_collection_base>
		inst_base(get_inst_base());
	assert(inst_base);
	return inst_base->get_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference-counted pointer to instantiating indices, 
		which contains expressions, and may be null.  
 */
index_collection_item_ptr_type
instantiation_statement::get_indices(void) const {
	return indices;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
instantiation_statement::dimensions(void) const {
	if (indices)
		return indices->dimensions();
	else return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary, should be pure virtual in the end.
 */
void
instantiation_statement::unroll(void) const {
	cerr << "instantiation_statement::unroll(): Fang, finish me!" << endl;
}

//=============================================================================
// class param_instantiation_statement method definitions

/**
	Private empty constructor.
 */
param_instantiation_statement::param_instantiation_statement() :
		instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instantiation_statement::param_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		instantiation_statement(i) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instantiation_statement::~param_instantiation_statement() {
}

//=============================================================================
// class pbool_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pbool_instantiation_statement, 
	PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pbool_instantiation_statement::pbool_instantiation_statement() :
		object(), param_instantiation_statement(), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation_statement::pbool_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		object(), param_instantiation_statement(i), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation_statement::~pbool_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_instantiation_statement::what(ostream& o) const {
	return o << "pbool-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_instantiation_statement::dump(ostream& o) const {
	return instantiation_statement::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
pbool_instantiation_statement::attach_collection(
		never_ptr<instance_collection_base> i) {
	assert(!inst_base);
	inst_base = i.is_a<collection_type>();
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
pbool_instantiation_statement::get_inst_base(void) {
	assert(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_const_ptr<instance_collection_base>
pbool_instantiation_statement::get_inst_base(void) const {
	assert(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
pbool_instantiation_statement::get_type_ref(void) const {
	return pbool_type_ptr;		// built-in type pointer
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::unroll(void) const {
	assert(inst_base);
	inst_base->instantiate_indices(indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	assert(inst_base);
	inst_base->collect_transient_info(m);
	if (indices)
		indices->collect_transient_info(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pbool_instantiation_statement::construct_empty(const int i) {
	return new pbool_instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);
	m.write_pointer(f, indices);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst_base);
	m.read_pointer(f, indices);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class pint_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_instantiation_statement, 
	PINT_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pint_instantiation_statement::pint_instantiation_statement() :
		object(), param_instantiation_statement(), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation_statement::pint_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		object(), param_instantiation_statement(i), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation_statement::~pint_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_instantiation_statement::what(ostream& o) const {
	return o << "pint-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_instantiation_statement::dump(ostream& o) const {
	return instantiation_statement::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
pint_instantiation_statement::attach_collection(
		never_ptr<instance_collection_base> i) {
	assert(!inst_base);
	inst_base = i.is_a<collection_type>();
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
pint_instantiation_statement::get_inst_base(void) {
	assert(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_const_ptr<instance_collection_base>
pint_instantiation_statement::get_inst_base(void) const {
	assert(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
pint_instantiation_statement::get_type_ref(void) const {
	return pint_type_ptr;		// built-in type pointer
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::unroll(void) const {
	assert(inst_base);
	inst_base->instantiate_indices(indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PINT_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	assert(inst_base);
	inst_base->collect_transient_info(m);
	if (indices)
		indices->collect_transient_info(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pint_instantiation_statement::construct_empty(const int i) {
	return new pint_instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);
	m.write_pointer(f, indices);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst_base);
	m.read_pointer(f, indices);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class process_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(process_instantiation_statement, 
	PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
process_instantiation_statement::process_instantiation_statement() :
		object(), instantiation_statement(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instantiation_statement::process_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		object(), instantiation_statement(i),
		type(t), inst_base(NULL) {
	assert(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instantiation_statement::~process_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instantiation_statement::what(ostream& o) const {
	return o << "process-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instantiation_statement::dump(ostream& o) const {
	return instantiation_statement::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
process_instantiation_statement::attach_collection(
		never_ptr<instance_collection_base> i) {
	assert(!inst_base);
	inst_base = i.is_a<collection_type>();
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
process_instantiation_statement::get_inst_base(void) {
	assert(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_const_ptr<instance_collection_base>
process_instantiation_statement::get_inst_base(void) const {
	assert(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
process_instantiation_statement::get_type_ref(void) const {
	return type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	assert(inst_base);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	if (indices)
		indices->collect_transient_info(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
process_instantiation_statement::construct_empty(const int i) {
	return new process_instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);		assert(inst_base);
	m.write_pointer(f, type);		assert(type);
	m.write_pointer(f, indices);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst_base);		assert(inst_base);
	m.read_pointer(f, type);		assert(type);
	m.read_pointer(f, indices);
#if 0
	type->load_object(m);
	inst_base->load_object(m);
	if (indices)
		indices->load_object(m);
#endif
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class channel_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(channel_instantiation_statement, 
	CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
channel_instantiation_statement::channel_instantiation_statement() :
		object(), instantiation_statement(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instantiation_statement::channel_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		object(), instantiation_statement(i),
		type(t), inst_base(NULL) {
	assert(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instantiation_statement::~channel_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instantiation_statement::what(ostream& o) const {
	return o << "channel-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instantiation_statement::dump(ostream& o) const {
	return instantiation_statement::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
channel_instantiation_statement::attach_collection(
		never_ptr<instance_collection_base> i) {
	assert(!inst_base);
	inst_base = i.is_a<collection_type>();
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
channel_instantiation_statement::get_inst_base(void) {
	assert(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_const_ptr<instance_collection_base>
channel_instantiation_statement::get_inst_base(void) const {
	assert(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
channel_instantiation_statement::get_type_ref(void) const {
	return type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	assert(inst_base);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	if (indices)
		indices->collect_transient_info(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
channel_instantiation_statement::construct_empty(const int i) {
	return new channel_instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation_statement::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);		assert(inst_base);
	m.write_pointer(f, type);		assert(type);
	m.write_pointer(f, indices);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation_statement::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst_base);		assert(inst_base);
	m.read_pointer(f, type);		assert(type);
	m.read_pointer(f, indices);
#if 0
	type->load_object(m);
	inst_base->load_object(m);
	if (indices)
		indices->load_object(m);
#endif
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class data_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(data_instantiation_statement, 
	DATA_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
data_instantiation_statement::data_instantiation_statement() :
		object(), instantiation_statement(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_instantiation_statement::data_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		object(), instantiation_statement(i),
		type(t), inst_base(NULL) {
	assert(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_instantiation_statement::~data_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
data_instantiation_statement::what(ostream& o) const {
	return o << "data-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
data_instantiation_statement::dump(ostream& o) const {
	return instantiation_statement::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
data_instantiation_statement::attach_collection(
		never_ptr<instance_collection_base> i) {
	assert(!inst_base);
	inst_base = i.is_a<collection_type>();
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
data_instantiation_statement::get_inst_base(void) {
	assert(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_const_ptr<instance_collection_base>
data_instantiation_statement::get_inst_base(void) const {
	assert(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
data_instantiation_statement::get_type_ref(void) const {
	return type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DATA_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	assert(inst_base);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	if (indices)
		indices->collect_transient_info(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
data_instantiation_statement::construct_empty(const int i) {
	return new data_instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_instantiation_statement::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);		assert(inst_base);
	m.write_pointer(f, type);		assert(type);
	m.write_pointer(f, indices);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_instantiation_statement::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst_base);		assert(inst_base);
	m.read_pointer(f, type);		assert(type);
	m.read_pointer(f, indices);
#if 0
	type->load_object(m);
	inst_base->load_object(m);
	if (indices)
		indices->load_object(m);
#endif
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================

//=============================================================================
}	// end namespace entity
}	// end namespace ART

