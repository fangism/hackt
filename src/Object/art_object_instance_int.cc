/**
	\file "art_object_instance_int.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	TODO: replace duplicate managed code with templates.
	$Id: art_object_instance_int.cc,v 1.3 2004/12/12 04:53:05 fang Exp $
 */

#include <iostream>
#include <algorithm>

#include "art_object_instance_int.h"
#include "art_object_inst_ref_data.h"
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
// class int_instance_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(int_instance_collection,
	DINT_INSTANCE_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int_instance_collection::int_instance_collection(const scopespace& o, 
		const string& n) : parent_type(o, n) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_instance_collection::~int_instance_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a int data reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
count_ptr<instance_reference_base>
int_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	return count_ptr<datatype_instance_reference>(
		new int_instance_reference(
			never_ptr<const int_instance_collection>(this), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_collection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		DINT_INSTANCE_COLLECTION_TYPE_KEY, dimensions())) {
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_instance_collection*
int_instance_collection::make_int_array(
		const scopespace& o, const string& n, const size_t d) {
	switch(d) {
		case 0: return new int_array<0>(o, n);
		case 1: return new int_array<1>(o, n);
		case 2: return new int_array<2>(o, n);
		case 3: return new int_array<3>(o, n);
		case 4: return new int_array<4>(o, n);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
int_instance_collection::construct_empty(const int i) {
	switch(i) {
		case 0: return new int_array<0>();
		case 1: return new int_array<1>();
		case 2: return new int_array<2>();
		case 3: return new int_array<3>();
		case 4: return new int_array<4>();
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//=============================================================================
// class int_instance_alias method definitions

ostream&
operator << (ostream& o, const int_instance_alias& b) {
	INVARIANT(b.valid());
	return o << "(int-alias)";
}

//=============================================================================
// class int_array method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
int_array<D>::int_array() : int_instance_collection(), collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
int_array<D>::int_array(const scopespace& o, const string& n) :
		int_instance_collection(o, n), collection() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
int_array<D>::~int_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
bool
int_array<D>::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
size_t
int_array<D>::dimensions(void) const {
	return D;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
ostream&
int_array<D>::what(ostream& o) const {
	return o << "int-array<" << D << ">";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
ostream&
int_array<D>::dump_unrolled_instances(ostream& o) const {
	for_each(collection.begin(), collection.end(), key_dumper(o));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE 
ostream&
int_array<D>::key_dumper::operator () (
		const typename collection_type::value_type& p) {
	return os << auto_indent << p.first << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates integer parameters at the specified indices.
	\param i fully-specified range of indices to instantiate.
 */
INT_ARRAY_TEMPLATE_SIGNATURE
void
int_array<D>::instantiate_indices(const index_collection_item_ptr_type& i) {
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
		int_instance_alias& pi(collection[key_gen]);
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
INT_ARRAY_TEMPLATE_SIGNATURE
const_index_list
int_array<D>::resolve_indices(const const_index_list& l) const {
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
	\return valid int_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INT_ARRAY_TEMPLATE_SIGNATURE
typename int_array<D>::instance_ptr_type
int_array<D>::lookup_instance(const unroll_index_type& i) const {
	INVARIANT(D == i.dimensions());
	// will create and return an "uninstantiated" instance if not found
	const int_instance_alias&
		b(collection[i]);
//		b(AS_A(const collection_type&, collection)[i]);
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return instance_ptr_type(const_cast<int_instance_alias*>(&b));
	} else {
		// remove the blank we added?
		// not necessary, but could keep the collection "clean"
		cerr << "ERROR: reference to uninstantiated int " <<
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
INT_ARRAY_TEMPLATE_SIGNATURE
bool
int_array<D>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	INVARIANT(!r.empty());
	multikey_generator<D, int> key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const int_instance_alias& pi(collection[key_gen]);
		if (pi.valid()) {
			l.push_back(instance_ptr_type(
				const_cast<int_instance_alias*>(&pi)));
		} else {
			cerr << "FATAL: reference to uninstantiated int index "
				<< key_gen << endl;
			l.push_back(instance_ptr_type(NULL));
		}
		ret &= pi.valid();
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
void
int_array<D>::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	INVARIANT(f.good());
	WRITE_POINTER_INDEX(f, m);
	parent_type::write_object_base(m, f);
	collection.write(f);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
void
int_array<D>::load_object(persistent_object_manager& m) {
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
// class int_array method definitions (specialized)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_array<0>::int_array() : int_instance_collection(), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_array<0>::int_array(const scopespace& o, const string& n) :
		int_instance_collection(o, n), the_instance() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_array<0>::~int_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
int_array<0>::is_partially_unrolled(void) const {
	return the_instance.valid();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
int_array<0>::dimensions(void) const {
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_array<0>::what(ostream& o) const {
	return o << "int-scalar";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_array<0>::dump_unrolled_instances(ostream& o) const {
	return o << auto_indent << the_instance << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates the_instance of integer datatype.
	Ideally, the error should never trigger because
	re-instantiation / redeclaration of a scalar instance
	is easily detected (and actually detected) during the compile phase.
	\param i indices must be NULL because this is not an array.
 */
void
int_array<0>::instantiate_indices(const index_collection_item_ptr_type& i) {
	INVARIANT(!i);
	if (the_instance.valid()) {
		// should never happen, but just in case...
		cerr << "ERROR: Scalar int already instantiated!" << endl;
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
int_array<0>::resolve_indices(const const_index_list& l) const {
	cerr << "WARNING: int_array<0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid int_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
int_array<0>::instance_ptr_type
int_array<0>::lookup_instance(const unroll_index_type& i) const {
	if (!the_instance.valid()) {
		cerr << "ERROR: Reference to uninstantiated int!" << endl;
		return instance_ptr_type(NULL);
	} else	return instance_ptr_type(
		const_cast<int_instance_alias*>(&the_instance));
	// ok to return non-const reference to the type, 
	// perhaps it should be declared mutable?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
	\return false to signal error.  
 */
bool
int_array<0>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	cerr << "WARNING: int_array<0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_array<0>::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	INVARIANT(f.good());
	WRITE_POINTER_INDEX(f, m);
	parent_type::write_object_base(m, f);
	write_value(f, the_instance);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_array<0>::load_object(persistent_object_manager& m) {
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

