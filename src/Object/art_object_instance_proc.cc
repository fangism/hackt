/**
	\file "art_object_instance_proc.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	TODO: replace duplicate managed code with templates.
	$Id: art_object_instance_proc.cc,v 1.1 2004/12/11 21:26:51 fang Exp $
 */

#include <iostream>
#include <algorithm>

#include "art_object_instance_proc.h"
#include "art_object_inst_stmt_base.h"
#include "art_object_inst_ref.h"
#include "art_object_type_ref.h"
#include "art_object_expr_const.h"
#include "art_object_type_hash.h"
#include "multikey_qmap.tcc"
#include "persistent_object_manager.tcc"
#include "indent.h"

#include "ptrs_functional.h"
#include "compose.h"
#include "binders.h"

namespace ART {
namespace entity {
using std::string;
using namespace MULTIKEY_NAMESPACE;
using namespace ADS;

//=============================================================================
// class process_instance_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(process_instance_collection,
	PROCESS_INSTANCE_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

process_instance_collection::process_instance_collection(const scopespace& o, 
		const string& n, const size_t d) : parent_type(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance_collection::~process_instance_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The type of the first instantiation statement.  
 */
count_ptr<const fundamental_type_reference>
process_instance_collection::get_type_ref(void) const {
	INVARIANT(!index_collection.empty());
	return (*index_collection.begin())->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create an process reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
count_ptr<instance_reference_base>
process_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	return count_ptr<process_instance_reference>(
		new process_instance_reference(
			never_ptr<const process_instance_collection>(this), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
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
if (!m.register_transient_object(this, 
		PROCESS_INSTANCE_COLLECTION_TYPE_KEY, dimensions())) {
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance_collection*
process_instance_collection::make_proc_array(
		const scopespace& o, const string& n, const size_t d) {
	switch(d) {
		case 0: return new proc_array<0>(o, n);
		case 1: return new proc_array<1>(o, n);
		case 2: return new proc_array<2>(o, n);
		case 3: return new proc_array<3>(o, n);
		case 4: return new proc_array<4>(o, n);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
process_instance_collection::construct_empty(const int i) {
	switch(i) {
		case 0: return new proc_array<0>();
		case 1: return new proc_array<1>();
		case 2: return new proc_array<2>();
		case 3: return new proc_array<3>();
		case 4: return new proc_array<4>();
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance_collection::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	parent_type::write_object_base(m, o);
	// until new members added...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance_collection::load_object_base(
		persistent_object_manager& m, istream& i) {
	parent_type::load_object_base(m, i);
	// until new members added...
}

//=============================================================================
// class proc_instance_alias method definitions

ostream&
operator << (ostream& o, const proc_instance_alias& b) {
	INVARIANT(b.valid());
	return o << "(proc-alias)";
}

//=============================================================================
// class proc_array method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE
proc_array<D>::proc_array() : process_instance_collection(), collection() {
	depth = D;
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE
proc_array<D>::proc_array(const scopespace& o, const string& n) :
		process_instance_collection(o, n, D), collection() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE
proc_array<D>::~proc_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE
bool
proc_array<D>::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE
ostream&
proc_array<D>::what(ostream& o) const {
	return o << "proc-array<" << D << ">";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE
ostream&
proc_array<D>::dump_unrolled_instances(ostream& o) const {
	for_each(collection.begin(), collection.end(), key_dumper(o));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE 
ostream&
proc_array<D>::key_dumper::operator () (
		const typename collection_type::value_type& p) {
	return os << auto_indent << p.first << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates process instances at the specified indices.
	\param i fully-specified range of indices to instantiate.
 */
PROC_ARRAY_TEMPLATE_SIGNATURE
void
proc_array<D>::instantiate_indices(const index_collection_item_ptr_type& i) {
	NEVER_NULL(i);
	// indices is a range_expr_list (base class)
	// resolve into constants now using const_range_list
	// if unable, (b/c uninitialized) then report error
	const_range_list ranges;        // initially empty
	if (!i->resolve_ranges(ranges)) {
		// ranges is passed and returned by reference
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
	multikey_generator<D, int> key_gen;
	ranges.make_multikey_generator(key_gen);
	key_gen.initialize();
	do {
		// will create if necessary
		proc_instance_alias& pi(collection[key_gen]);
		if (pi.valid()) {
			// more detailed message, please!
			cerr << "ERROR: Index " << key_gen <<
				" already instantiated!" << endl;
			exit(1);
		}
		pi.instantiate();
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
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
PROC_ARRAY_TEMPLATE_SIGNATURE
const_index_list
proc_array<D>::resolve_indices(const const_index_list& l) const {
	const size_t l_size = l.size();
	if (dimensions() == l_size) {
		// already fully specified
		return l;
	}
	// convert indices to pair of list of multikeys
	if (!l_size) {
		return const_index_list(l, collection.is_compact());
	}
	// else construct slice
	list<int> lower_list, upper_list;
	transform(l.begin(), l.end(), back_inserter(lower_list),
		unary_compose(
			mem_fun_ref(&const_index::lower_bound),
			dereference<count_ptr, const const_index>()
		)
	);
	transform(l.begin(), l.end(), back_inserter(upper_list),
		unary_compose(
			mem_fun_ref(&const_index::upper_bound),
			dereference<count_ptr, const const_index>()
		)
	);
	return const_index_list(l,
		collection.is_compact_slice(lower_list, upper_list));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid proc_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
PROC_ARRAY_TEMPLATE_SIGNATURE
typename proc_array<D>::instance_ptr_type
proc_array<D>::lookup_instance(const unroll_index_type& i) const {
	INVARIANT(depth == i.dimensions());
	// will create and return an "uninstantiated" instance if not found
	const proc_instance_alias&
		b(collection[i]);
//		b(AS_A(const collection_type&, collection)[i]);
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return instance_ptr_type(const_cast<proc_instance_alias*>(&b));
	} else {
		// remove the blank we added?
		// not necessary, but could keep the collection "clean"
		cerr << "ERROR: reference to uninstantiated process " <<
			// process type name, please?
			get_qualified_name() << " at index: " << i << endl;
		return instance_ptr_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l list in which to accumulate instance references.
	\param r the ranges, must be valid, and fully resolved.
	\return false on error, e.g. if value doesn't exist or
		is uninitialized; true on success.
 */
PROC_ARRAY_TEMPLATE_SIGNATURE
bool
proc_array<D>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	INVARIANT(!r.empty());
	multikey_generator<D, int> key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const proc_instance_alias& pi(collection[key_gen]);
		if (pi.valid()) {
			l.push_back(instance_ptr_type(
				const_cast<proc_instance_alias*>(&pi)));
		} else {
			cerr << "FATAL: reference to uninstantiated process index "
				<< key_gen << endl;
			// process type name, please?
			l.push_back(instance_ptr_type(NULL));
		}
		ret &= pi.valid();
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE
void
proc_array<D>::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	INVARIANT(f.good());
	WRITE_POINTER_INDEX(f, m);
	parent_type::write_object_base(m, f);
	collection.write(f);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE
void
proc_array<D>::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	INVARIANT(f.good());
	STRIP_POINTER_INDEX(f, m);
	parent_type::load_object_base(m, f);
	collection.read(f);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class proc_array method definitions (specialized)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_array<0>::proc_array() : process_instance_collection(), the_instance() {
	depth = 0;
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_array<0>::proc_array(const scopespace& o, const string& n) :
		process_instance_collection(o, n, 0), the_instance() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_array<0>::~proc_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
proc_array<0>::is_partially_unrolled(void) const {
	return the_instance.valid();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
proc_array<0>::what(ostream& o) const {
	return o << "proc-scalar";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
proc_array<0>::dump_unrolled_instances(ostream& o) const {
	return o << auto_indent << the_instance << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates the_instance of process datatype.
	Ideally, the error should never trigger because
	re-instantiation / redeclaration of a scalar instance
	is easily detected (and actually detected) during the compile phase.
	\param i indices must be NULL because this is not an array.
 */
void
proc_array<0>::instantiate_indices(const index_collection_item_ptr_type& i) {
	INVARIANT(!i);
	if (the_instance.valid()) {
		// should never happen, but just in case...
		cerr << "ERROR: Scalar process already instantiated!" << endl;
		exit(1);
	}
	the_instance.instantiate();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This specialization isn't ever supposed to be called.
	\param l is list of indices, which may be under-specified,
		or even empty.
	\return empty index list, always.
 */
const_index_list
proc_array<0>::resolve_indices(const const_index_list& l) const {
	cerr << "WARNING: proc_array<0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid proc_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
proc_array<0>::instance_ptr_type
proc_array<0>::lookup_instance(const unroll_index_type& i) const {
	if (!the_instance.valid()) {
		cerr << "ERROR: Reference to uninstantiated process!" << endl;
		return instance_ptr_type(NULL);
	} else	return instance_ptr_type(
		const_cast<proc_instance_alias*>(&the_instance));
	// ok to return non-const reference to the type, 
	// perhaps it should be declared mutable?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
	\return false to signal error.  
 */
bool
proc_array<0>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	cerr << "WARNING: proc_array<0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_array<0>::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	INVARIANT(f.good());
	WRITE_POINTER_INDEX(f, m);
	parent_type::write_object_base(m, f);
	write_value(f, the_instance);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_array<0>::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	INVARIANT(f.good());
	STRIP_POINTER_INDEX(f, m);
	parent_type::load_object_base(m, f);
	read_value(f, the_instance);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

