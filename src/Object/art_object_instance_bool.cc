/**
	\file "art_object_instance_bool.cc"
	Method definitions for boolean data type instance classes.
	$Id: art_object_instance_bool.cc,v 1.9.6.1 2005/02/02 07:59:45 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_BOOL_CC__
#define	__ART_OBJECT_INSTANCE_BOOL_CC__

#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_bool.h"
#include "art_object_inst_ref_data.h"
#include "art_object_expr_const.h"
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_type_hash.h"
#include "art_built_ins.h"

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"

#include "multikey_qmap.tcc"
#include "persistent_object_manager.tcc"
#include "indent.h"
#include "stacktrace.h"
#include "static_trace.h"
#include "memory/list_vector_pool.tcc"
#include "ptrs_functional.h"
#include "compose.h"
#include "binders.h"

STATIC_TRACE_BEGIN("instance-bool")

namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array<0>, "bool_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array<1>, "bool_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array<2>, "bool_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array<3>, "bool_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array<4>, "bool_array_4D")
namespace memory {
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::bool_scalar)
}	// end namespace memory
}	// end namespace util

namespace ART {
namespace entity {
using std::string;
using namespace MULTIKEY_NAMESPACE;
USING_UTIL_COMPOSE
using std::dereference;
using std::mem_fun_ref;
USING_STACKTRACE

//=============================================================================
// class bool_instance_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(bool_instance_collection,
	DBOOL_INSTANCE_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool_instance_collection::bool_instance_collection(const scopespace& o, 
		const string& n, const size_t d) : parent_type(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance_collection::~bool_instance_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	During unroll phase, this determines the type, except that
	this type is a built-in primary type, bool.  
	\return false normally, signaling no error.  
	\pre this is the first statement unrolled in this collection.  
 */
bool
bool_instance_collection::commit_type(const type_ref_ptr_type& t) {
	// INVARIANT(!is_partially_unrolled());
	INVARIANT(t->get_base_def() == &bool_def);
	// shouldn't have any parameters, NULL or empty list
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a bool data reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
count_ptr<instance_reference_base>
bool_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	return count_ptr<datatype_instance_reference>(
		new bool_instance_reference(
			never_ptr<const bool_instance_collection>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_collection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		DBOOL_INSTANCE_COLLECTION_TYPE_KEY, dimensions)) {
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance_collection*
bool_instance_collection::make_bool_array(
		const scopespace& o, const string& n, const size_t d) {
	switch(d) {
		case 0: return new bool_array<0>(o, n);
		case 1: return new bool_array<1>(o, n);
		case 2: return new bool_array<2>(o, n);
		case 3: return new bool_array<3>(o, n);
		case 4: return new bool_array<4>(o, n);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
bool_instance_collection::construct_empty(const int i) {
	switch(i) {
		case 0: return new bool_array<0>();
		case 1: return new bool_array<1>();
		case 2: return new bool_array<2>();
		case 3: return new bool_array<3>();
		case 4: return new bool_array<4>();
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//=============================================================================
// class bool_instance_alias method definitions

ostream&
operator << (ostream& o, const bool_instance_alias& b) {
	INVARIANT(b.valid());
	return o << "(bool-alias)";
}

//=============================================================================
// class bool_array method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE
bool_array<D>::bool_array() : parent_type(D), collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE
bool_array<D>::bool_array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE
bool_array<D>::~bool_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE
bool
bool_array<D>::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE
ostream&
bool_array<D>::what(ostream& o) const {
	return o << "bool-array<" << D << ">";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE
ostream&
bool_array<D>::dump_unrolled_instances(ostream& o) const {
	for_each(collection.begin(), collection.end(), key_dumper(o));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE 
ostream&
bool_array<D>::key_dumper::operator () (
		const typename collection_type::value_type& p) {
	return os << auto_indent << p.first << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates integer parameters at the specified indices.
	\param i fully-specified range of indices to instantiate.
 */
BOOL_ARRAY_TEMPLATE_SIGNATURE
void
bool_array<D>::instantiate_indices(const index_collection_item_ptr_type& i) {
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
		THROW_EXIT;
	}
	// else success
	// now iterate through, unrolling one at a time...
	// stop as soon as there is a conflict
	// later: factor this out into common helper class
	multikey_generator<D, pint_value_type> key_gen;
	ranges.make_multikey_generator(key_gen);
	key_gen.initialize();
	do {
		// will create if necessary
		bool_instance_alias& pi(collection[key_gen]);
		if (pi.valid()) {
			// more detailed message, please!
			cerr << "ERROR: Index " << key_gen <<
				" already instantiated!" << endl;
			THROW_EXIT;
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
BOOL_ARRAY_TEMPLATE_SIGNATURE
const_index_list
bool_array<D>::resolve_indices(const const_index_list& l) const {
	const size_t l_size = l.size();
	if (D == l_size) {
		// already fully specified
		return l;
	}
	// convert indices to pair of list of multikeys
	if (!l_size) {
		return const_index_list(l, collection.is_compact());
	}
	// else construct slice
	list<pint_value_type> lower_list, upper_list;
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
	\return valid bool_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
BOOL_ARRAY_TEMPLATE_SIGNATURE
typename bool_array<D>::instance_ptr_type
bool_array<D>::lookup_instance(const unroll_index_type& i) const {
	INVARIANT(D == i.dimensions());
	// will create and return an "uninstantiated" instance if not found
	const bool_instance_alias&
		b(collection[i]);
//		b(AS_A(const collection_type&, collection)[i]);
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return instance_ptr_type(const_cast<bool_instance_alias*>(&b));
	} else {
		// remove the blank we added?
		// not necessary, but could keep the collection "clean"
		cerr << "ERROR: reference to uninstantiated bool " <<
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
BOOL_ARRAY_TEMPLATE_SIGNATURE
bool
bool_array<D>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	INVARIANT(!r.empty());
	multikey_generator<D, pint_value_type> key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const bool_instance_alias& pi(collection[key_gen]);
		if (pi.valid()) {
			l.push_back(instance_ptr_type(
				const_cast<bool_instance_alias*>(&pi)));
		} else {
			cerr << "FATAL: reference to uninstantiated bool index "
				<< key_gen << endl;
			l.push_back(instance_ptr_type(NULL));
		}
		ret &= pi.valid();
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE
void
bool_array<D>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
//	ostream& f = m.lookup_write_buffer(this);
//	INVARIANT(f.good());
//	WRITE_POINTER_INDEX(f, m);
	parent_type::write_object_base(m, f);
	collection.write(f);
//	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE
void
bool_array<D>::load_object(const persistent_object_manager& m, istream& f) {
// if (!m.flag_visit(this)) {
//	istream& f = m.lookup_read_buffer(this);
//	INVARIANT(f.good());
//	STRIP_POINTER_INDEX(f, m);
	parent_type::load_object_base(m, f);
	collection.read(f);
//	STRIP_OBJECT_FOOTER(f);
// }
}

//=============================================================================
// class bool_array method definitions (specialized)

LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(bool_scalar, 256)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_array<0>::bool_array() : parent_type(0), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_array<0>::bool_array(const scopespace& o, const string& n) :
		parent_type(o, n, 0), the_instance() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_array<0>::~bool_array() {
	STACKTRACE("~bool_scalar()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
bool_array<0>::is_partially_unrolled(void) const {
	return the_instance.valid();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_array<0>::what(ostream& o) const {
	return o << "bool-scalar";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_array<0>::dump_unrolled_instances(ostream& o) const {
	return o << auto_indent << the_instance << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates the_instance of boolean datatype.
	Ideally, the error should never trigger because
	re-instantiation / redeclaration of a scalar instance
	is easily detected (and actually detected) during the compile phase.
	\param i indices must be NULL because this is not an array.
 */
void
bool_array<0>::instantiate_indices(const index_collection_item_ptr_type& i) {
	INVARIANT(!i);
	if (the_instance.valid()) {
		// should never happen, but just in case...
		cerr << "ERROR: Scalar bool already instantiated!" << endl;
		THROW_EXIT;
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
bool_array<0>::resolve_indices(const const_index_list& l) const {
	cerr << "WARNING: bool_array<0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid bool_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
bool_array<0>::instance_ptr_type
bool_array<0>::lookup_instance(const unroll_index_type& i) const {
	if (!the_instance.valid()) {
		cerr << "ERROR: Reference to uninstantiated bool!" << endl;
		return instance_ptr_type(NULL);
	} else	return instance_ptr_type(
		const_cast<bool_instance_alias*>(&the_instance));
	// ok to return non-const reference to the type, 
	// perhaps it should be declared mutable?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
	\return false to signal error.  
 */
bool
bool_array<0>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	cerr << "WARNING: bool_array<0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_array<0>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
//	ostream& f = m.lookup_write_buffer(this);
//	INVARIANT(f.good());
//	WRITE_POINTER_INDEX(f, m);
	parent_type::write_object_base(m, f);
	write_value(f, the_instance);
//	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_array<0>::load_object(const persistent_object_manager& m, istream& f) {
// if (!m.flag_visit(this)) {
//	istream& f = m.lookup_read_buffer(this);
//	INVARIANT(f.good());
//	STRIP_POINTER_INDEX(f, m);
	parent_type::load_object_base(m, f);
	read_value(f, the_instance);
//	STRIP_OBJECT_FOOTER(f);
// }
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

STATIC_TRACE_END("instance-bool")

#endif	// __ART_OBJECT_INSTANCE_BOOL_CC__

