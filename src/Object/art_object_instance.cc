// "art_object_instance.cc"

#include <iostream>

#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_expr.h"
#include "art_built_ins.h"
#include "art_object_IO.tcc"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
#if 0
namespace parser {
	extern const char scope[];		// "::"		// "::"
}
using namespace parser;
#endif

namespace entity {

//=============================================================================
// class instantiation_base method definitions

const never_const_ptr<instantiation_base>
instantiation_base::null(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
instantiation_base::instantiation_base() : object(), 
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_base::~instantiation_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overridden by param_instantiation.  
 */
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instantiation_base::pair_dump(ostream& o) const {
	o << "  " << get_name() << " = ";
	return dump(o) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instantiation_base::get_qualified_name(void) const {
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
instantiation_base::get_base_def(void) const {
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
instantiation_base::current_collection_state(void) const {
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
instantiation_base::collection_state_end(void) const {
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TO DO: this can only be done with non-formals.  Check this.  
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Queries whether or not this is a template formal, by 
	checking its membership in the owner.  
 */
bool
instantiation_base::is_template_formal(void) const {
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
instantiation_base::is_port_formal(void) const {
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
instantiation_base::template_formal_equivalent(
		never_const_ptr<instantiation_base> b) const {
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
instantiation_base::port_formal_equivalent(
		never_const_ptr<instantiation_base> b) const {
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
instantiation_base::formal_size_equivalent(
		never_const_ptr<instantiation_base> b) const {
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
			ic(i->is_a<const_range_list>());
		count_const_ptr<const_range_list>
			jc(j->is_a<const_range_list>());
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
	So far, only used by param_instantiation derivatives, 
		in the context of checking template formals.  
	May be useful else where for connections.  
	\return true if dimensions *may* match.  
 */
bool
instantiation_base::check_expression_dimensions(const param_expr& pe) const {
	assert(IS_A(const param_instantiation*, this));
	// else is not an expression class!

	if (depth != pe.dimensions()) {
		// number of dimensions doesn't even match!
		// useful error message?
		return false;
	}
	// dimensions match
	if (depth != 0) {
		assert(index_collection.size() == 1);
		// make sure sizes in each dimension
		index_collection_type::const_iterator i =
			index_collection.begin();
		count_const_ptr<const_range_list>
			crl(i->is_a<const_range_list>());
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
		assert(index_collection.size() == 0);
		return (pe.dimensions() == 0);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Utility function for walking index collection list
	and collecting pointers.  
 */
void
instantiation_base::collect_index_collection_pointers(
		persistent_object_manager& m) const {
	index_collection_type::const_iterator
		c_iter = index_collection.begin();
	const index_collection_type::const_iterator
		c_end = index_collection.end();
	for ( ; c_iter!=c_end; c_iter++) {
		const index_collection_item_ptr_type& p = *c_iter;
		assert(p);
		p->collect_transient_info(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write out serial list of pointers to index collection items, 
	with pointers translated into indicies.  
 */
void
instantiation_base::write_index_collection_pointers(
		persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	// need a specialization for count_ptrs
	m.write_pointer_list(f, index_collection);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Loads serial list of pointers into index collection items, 
	with indices translated into pointers.  
 */
void
instantiation_base::load_index_collection_pointers(
		persistent_object_manager& m) {
	istream& f = m.lookup_read_buffer(this);
	// need a specialization for count_ptrs
	m.read_pointer_list(f, index_collection);
}

//=============================================================================
// class datatype_instantiation method definitions

/**
	Private empty constructor.
 */
datatype_instantiation::datatype_instantiation() :
		instantiation_base(), type(NULL) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instantiation::datatype_instantiation(const scopespace& o, 
		count_const_ptr<data_type_reference> t, 
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d), type(t) {
	assert(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instantiation::~datatype_instantiation() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
datatype_instantiation::what(ostream& o) const {
	return o << "datatype-inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
datatype_instantiation::get_type_ref(void) const {
	return type;
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
datatype_instantiation::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions -- when attach_indices() invoked
	return count_ptr<datatype_instance_reference>(
		new datatype_instance_reference(
			never_const_ptr<datatype_instantiation>(this), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
#if 0
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a member reference to a datatype, 
	and pushes it onto the context's object_stack.  
	\param b is the parent owner of this instantiation referenced.  
 */
count_ptr<member_instance_reference_base>
datatype_instantiation::make_member_instance_reference(
		count_const_ptr<simple_instance_reference> b) const {
	assert(b);
	// maybe verify that b contains this, as sanity check
	return count_ptr<datatype_member_instance_reference>(
		new datatype_member_instance_reference(
			b, never_const_ptr<datatype_instantiation>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
#if 0
	c.push_object_stack(new_mir);
	return return_type(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
datatype_instantiation::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DATA_INSTANTIATION_TYPE)) {
	// don't bother visit the owner, assuming that's the caller
	type->collect_transient_info(m);
	// go through index_collection
	collect_index_collection_pointers(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
datatype_instantiation::construct_empty(void) {
	return new datatype_instantiation();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need special case handling for built-in types? int and bool?
 */
void
datatype_instantiation::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, owner);
	write_string(f, key);
	m.write_pointer(f, type);
	write_index_collection_pointers(m);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need special case handling for built-in types? int and bool?
 */
void
datatype_instantiation::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, owner);
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, type);
	load_index_collection_pointers(m);
	STRIP_OBJECT_FOOTER(f);
	if (index_collection.empty())
		depth = 0;
	else
		depth = (*index_collection.begin())->dimensions();
}
// else already visited
}

//=============================================================================
// class process_instantiation method definitions

/**
	Private empty constructor.
 */
process_instantiation::process_instantiation() :
		instantiation_base(), type(NULL) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instantiation::process_instantiation(const scopespace& o, 
		count_const_ptr<process_type_reference> pt,
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d), type(pt) {
	assert(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instantiation::~process_instantiation() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instantiation::what(ostream& o) const {
	return o << "process-inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
process_instantiation::get_type_ref(void) const {
	return type;
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
process_instantiation::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	return count_ptr<process_instance_reference>(
		new process_instance_reference(
			never_const_ptr<process_instantiation>(this), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
		// may attach in parser::instance_array::check_build()
#if 0
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a member reference to a process, 
	and pushes it onto the context's object_stack.  
	\param b is the parent owner of this instantiation referenced.  
 */
count_ptr<member_instance_reference_base>
process_instantiation::make_member_instance_reference(
		count_const_ptr<simple_instance_reference> b) const {
	assert(b);
	// maybe verify that b contains this, as sanity check
	return count_ptr<process_member_instance_reference>(
		new process_member_instance_reference(
			b, never_const_ptr<process_instantiation>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
#if 0
	c.push_object_stack(new_mir);
	return return_type(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PROCESS_INSTANTIATION_TYPE)) {
	// don't bother visit the owner, assuming that's the caller
	type->collect_transient_info(m);
	// go through index_collection
	collect_index_collection_pointers(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
process_instantiation::construct_empty(void) {
	return new process_instantiation();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, owner);
	write_string(f, key);
	m.write_pointer(f, type);
	write_index_collection_pointers(m);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, owner);
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, type);
	load_index_collection_pointers(m);
	STRIP_OBJECT_FOOTER(f);
	if (index_collection.empty())
		depth = 0;
	else
		depth = (*index_collection.begin())->dimensions();
}
// else already visited
}

//=============================================================================
// class param_instantiation method definitions

/**
	Private empty constructor.  
 */
param_instantiation::param_instantiation() :
		instantiation_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instantiation::param_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instantiation::~param_instantiation() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Override instantiation_base's dump() to suppress output of <>.
 */
ostream&
param_instantiation::what(ostream& o) const {
	return o << 
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_instantiation::dump(ostream& o) const {
	get_type_ref()->dump(o) << " " << key;
	index_collection_type::const_iterator i = index_collection.begin();
	const index_collection_type::const_iterator e = index_collection.end();
	for ( ; i!=e; i++) {
		assert(*i);
		(*i)->dump(o) << endl;
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
param_instantiation::may_be_initialized(void) const {
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
param_instantiation::must_be_initialized(void) const {
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
param_instantiation::is_loop_independent(void) const {
	
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	1) Parameters cannot be in public ports.  
	2) Thus they cannot even be referenced.  
	3) This is just a placeholder that should never be called.  
 */
count_ptr<member_instance_reference_base>
param_instantiation::make_member_instance_reference(
		count_const_ptr<simple_instance_reference> b) const {
	typedef	count_ptr<member_instance_reference_base>	return_type;
	assert(b);
	cerr << "Referencing parameter members is strictly forbidden!" << endl;
	assert(0);
	return return_type(NULL);
}

//=============================================================================
// class pbool_instantiation method definitions

/**
	Private empty constructor.  
 */
pbool_instantiation::pbool_instantiation() :
		param_instantiation(), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation::pbool_instantiation(const scopespace& o, 
		const string& n) :
		param_instantiation(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation::pbool_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d) :
		param_instantiation(o, n, d), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation::pbool_instantiation(const scopespace& o, 
		const string& n, 
		count_const_ptr<pbool_expr> i) :
		param_instantiation(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(i) {
	assert(type_check_actual_param_expr(*i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation::pbool_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d, 
		count_const_ptr<pbool_expr> i) :
		param_instantiation(o, n, d), ival(i) {
	assert(type_check_actual_param_expr(*i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_instantiation::what(ostream& o) const {
	return o << "pbool-inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
pbool_instantiation::get_type_ref(void) const {
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
pbool_instantiation::initialize(count_const_ptr<pbool_expr> e) {
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
pbool_instantiation::assign_default_value(count_const_ptr<param_expr> p) {
	count_const_ptr<pbool_expr> b(p.is_a<pbool_expr>());
	if (b && type_check_actual_param_expr(*b)) {
		ival = b;
		return true;
	}
	else return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<param_expr>
pbool_instantiation::default_value(void) const {
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<pbool_expr>
pbool_instantiation::initial_value(void) const {
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
pbool_instantiation::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	return count_ptr<param_instance_reference>(
		new pbool_instance_reference(
			never_ptr<pbool_instantiation>(
			const_cast<pbool_instantiation*>(this)), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
#if 0
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether or not a pbool was passed to a formal 
	pbool parameter in a template.  
	Should also check dimensionality and size.  
 */
bool
pbool_instantiation::type_check_actual_param_expr(const param_expr& pe) const {
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
pbool_instantiation::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PBOOL_INSTANTIATION_TYPE)) {
	// don't bother visit the owner, assuming that's the caller
	// go through index_collection
	collect_index_collection_pointers(m);
	if (ival)
		ival->collect_transient_info(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
pbool_instantiation::construct_empty(void) {
	return new pbool_instantiation();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation::write_object(persistent_object_manager& m) const {
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
pbool_instantiation::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, owner);
	read_string(f, const_cast<string&>(key));
	load_index_collection_pointers(m);
	m.read_pointer(f, ival);
	STRIP_OBJECT_FOOTER(f);
	if (index_collection.empty())
		depth = 0;
	else
		depth = (*index_collection.begin())->dimensions();
}
// else already visited
}

//=============================================================================
// class pint_instantiation method definitions

/**
	Private empty constructor.
 */
pint_instantiation::pint_instantiation() :
		param_instantiation(), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation::pint_instantiation(const scopespace& o, 
		const string& n) :
		param_instantiation(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation::pint_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d) :
		param_instantiation(o, n, d), ival(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation::pint_instantiation(const scopespace& o, 
		const string& n, 
		count_const_ptr<pint_expr> i) :
		param_instantiation(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(i) {
	assert(type_check_actual_param_expr(*i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation::pint_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d, 
		count_const_ptr<pint_expr> i) :
		param_instantiation(o, n, d), ival(i) {
	assert(type_check_actual_param_expr(*i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_instantiation::what(ostream& o) const {
	return o << "pint-inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
pint_instantiation::get_type_ref(void) const {
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
pint_instantiation::initialize(count_const_ptr<pint_expr> e) {
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
pint_instantiation::assign_default_value(count_const_ptr<param_expr> p) {
	count_const_ptr<pint_expr> i(p.is_a<pint_expr>());
	if (i && type_check_actual_param_expr(*i)) {
		ival = i;
		return true;
	}
	else return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<param_expr>
pint_instantiation::default_value(void) const {
	return ival;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<pint_expr>
pint_instantiation::initial_value(void) const {
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
pint_instantiation::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	return count_ptr<param_instance_reference>(
		new pint_instance_reference(
			never_ptr<pint_instantiation>(
			const_cast<pint_instantiation*>(this)), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
#if 0
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether or not a pint was passed to a formal 
	pint parameter in a template.  
	Should also check dimensionality and size.  
 */
bool
pint_instantiation::type_check_actual_param_expr(const param_expr& pe) const {
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
pint_instantiation::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PINT_INSTANTIATION_TYPE)) {
	// don't bother visit the owner, assuming that's the caller
	// go through index_collection
	collect_index_collection_pointers(m);
	if (ival)
		ival->collect_transient_info(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
pint_instantiation::construct_empty(void) {
	return new pint_instantiation();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation::write_object(persistent_object_manager& m) const {
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
pint_instantiation::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, owner);
	read_string(f, const_cast<string&>(key));
	load_index_collection_pointers(m);
	m.read_pointer(f, ival);
	STRIP_OBJECT_FOOTER(f);
	if (index_collection.empty())
		depth = 0;
	else
		depth = (*index_collection.begin())->dimensions();
}
// else already visited
}

//=============================================================================
// class channel_instantiation method definitions

/**
	Private empty constructor.  
 */
channel_instantiation::channel_instantiation() :
		instantiation_base(), type(NULL) {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instantiation::channel_instantiation(const scopespace& o, 
		count_const_ptr<channel_type_reference> ct,
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d), type(ct) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instantiation::~channel_instantiation() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instantiation::what(ostream& o) const {
	return o << "channel-inst";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_const_ptr<fundamental_type_reference>
channel_instantiation::get_type_ref(void) const {
	return type;
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
channel_instantiation::make_instance_reference(void) const {
	cerr << "channel_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	return count_ptr<channel_instance_reference>(
		new channel_instance_reference(
			never_const_ptr<channel_instantiation>(this), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
#if 0
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a member reference to a channel, 
	and pushes it onto the context's object_stack.  
	\param b is the parent owner of this instantiation referenced.  
 */
count_ptr<member_instance_reference_base>
channel_instantiation::make_member_instance_reference(
		count_const_ptr<simple_instance_reference> b) const {
	assert(b);
	// maybe verify that b contains this, as sanity check
	return count_ptr<channel_member_instance_reference>(
		new channel_member_instance_reference(
			b, never_const_ptr<channel_instantiation>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
#if 0
	c.push_object_stack(new_mir);
	return return_type(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, CHANNEL_INSTANTIATION_TYPE)) {
	// don't bother visit the owner, assuming that's the caller
	type->collect_transient_info(m);
	// go through index_collection
	collect_index_collection_pointers(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
channel_instantiation::construct_empty(void) {
	return new channel_instantiation();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, owner);
	write_string(f, key);
	m.write_pointer(f, type);
	write_index_collection_pointers(m);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, owner);
	read_string(f, const_cast<string&>(key));
	m.read_pointer(f, type);
	load_index_collection_pointers(m);
	STRIP_OBJECT_FOOTER(f);
	if (index_collection.empty())
		depth = 0;
	else
		depth = (*index_collection.begin())->dimensions();
}
// else already visited
}

//=============================================================================
//=============================================================================
// class instantiation_statement method definitions

/**	Private empty constructor. */
instantiation_statement::instantiation_statement(void) :
		object(), inst_base(NULL), indices(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_statement::instantiation_statement(
		never_ptr<instantiation_base> b, 
		const index_collection_item_ptr_type& i) :
		object(), inst_base(b), indices(i) {
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_statement::~instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instantiation_statement::what(ostream& o) const {
	return o << "instantiation-statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instantiation_statement::dump(ostream& o) const {
	inst_base->get_type_ref()->dump(o) << " ";
	o << inst_base->get_name();
	if (indices)
		indices->dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, INSTANTIATION_STATEMENT_TYPE)) {
	inst_base->collect_transient_info(m);
	if (indices)
		indices->collect_transient_info(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
object*
instantiation_statement::construct_empty(void) {
	return new instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);
	m.write_pointer(f, indices);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement::load_object(persistent_object_manager& m) {
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
}	// end namespace entity
}	// end namespace ART

