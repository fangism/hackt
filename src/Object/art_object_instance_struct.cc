/**
	\file "art_object_instance_struct.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	TODO: replace duplicate managed code with templates.
	$Id: art_object_instance_struct.cc,v 1.9.2.4.4.1.2.1 2005/02/20 06:36:31 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_STRUCT_CC__
#define	__ART_OBJECT_INSTANCE_STRUCT_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_struct.h"
#include "art_object_inst_ref_data.h"
#include "art_object_expr_const.h"
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_type_hash.h"

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"

#include "multikey_qmap.tcc"
#include "persistent_object_manager.tcc"
#include "indent.h"
#include "stacktrace.h"

#include "ptrs_functional.h"
#include "dereference.h"
#include "compose.h"
#include "binders.h"

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_instance_collection, 
		STRUCT_INSTANCE_COLLECTION_TYPE_KEY)
}	// end namespace util

namespace ART {
namespace entity {
#include "using_ostream.h"
using std::string;
using util::multikey_generator;
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
USING_STACKTRACE
using util::write_value;
using util::read_value;
using util::indent;
using util::auto_indent;

//=============================================================================
// class struct_instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct_instance_collection::struct_instance_collection(const scopespace& o, 
		const string& n, const size_t d) : parent_type(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct_instance_collection::~struct_instance_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
struct_instance_collection::type_dump(ostream& o) const {
	cerr << "FANG!  write struct_inst_coll::type_dump()!" << endl;
	return o << "struct " << "???^" << dimensions;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This sets the actual type for the instance collection.  
	\param t the struct type reference, containing resolved
		template parameters if applicable.  
	\return false if type is set without error, else true.  
 */
bool
struct_instance_collection::commit_type(const type_ref_ptr_type& t) {
	STACKTRACE("struct_instance_collection::commit_type()");
//	INVARIANT(!is_partially_unrolled());
	cerr << "FANG: finish struct_instance_collection::commit_type()!"
		<< endl;
	return true;	// temporary: always return error
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create an struct data reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
count_ptr<instance_reference_base>
struct_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	return count_ptr<datatype_instance_reference>(
		new datastruct_instance_reference(
			never_ptr<const struct_instance_collection>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct_instance_collection::member_inst_ref_ptr_type
struct_instance_collection::make_member_instance_reference(
		const inst_ref_ptr_type& b) const {
	NEVER_NULL(b);
	return member_inst_ref_ptr_type(
		new datastruct_member_instance_reference(
			b, never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
struct_instance_collection::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		STRUCT_INSTANCE_COLLECTION_TYPE_KEY, dimensions)) {
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct_instance_collection*
struct_instance_collection::make_struct_array(
		const scopespace& o, const string& n, const size_t d) {
	switch(d) {
		case 0: return new struct_array<0>(o, n);
		case 1: return new struct_array<1>(o, n);
		case 2: return new struct_array<2>(o, n);
		case 3: return new struct_array<3>(o, n);
		case 4: return new struct_array<4>(o, n);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
struct_instance_collection::construct_empty(const int i) {
	switch(i) {
		case 0: return new struct_array<0>();
		case 1: return new struct_array<1>();
		case 2: return new struct_array<2>();
		case 3: return new struct_array<3>();
		case 4: return new struct_array<4>();
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//=============================================================================
// class struct_instance_alias method definitions

ostream&
operator << (ostream& o, const struct_instance_alias& b) {
	INVARIANT(b.valid());
	return o << "(struct-alias)";
}

//=============================================================================
// class struct_array method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STRUCT_ARRAY_TEMPLATE_SIGNATURE
struct_array<D>::struct_array() : parent_type(D), collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STRUCT_ARRAY_TEMPLATE_SIGNATURE
struct_array<D>::struct_array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STRUCT_ARRAY_TEMPLATE_SIGNATURE
struct_array<D>::~struct_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STRUCT_ARRAY_TEMPLATE_SIGNATURE
bool
struct_array<D>::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STRUCT_ARRAY_TEMPLATE_SIGNATURE
ostream&
struct_array<D>::what(ostream& o) const {
	return o << "struct-array<" << D << ">";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STRUCT_ARRAY_TEMPLATE_SIGNATURE
ostream&
struct_array<D>::dump_unrolled_instances(ostream& o) const {
	for_each(collection.begin(), collection.end(), key_dumper(o));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STRUCT_ARRAY_TEMPLATE_SIGNATURE 
ostream&
struct_array<D>::key_dumper::operator () (
		const typename collection_type::value_type& p) {
	return os << auto_indent << p.first << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates struct instances at the specified indices.
	\param i fully-specified range of indices to instantiate.
 */
STRUCT_ARRAY_TEMPLATE_SIGNATURE
void
struct_array<D>::instantiate_indices(const index_collection_item_ptr_type& i) {
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
		struct_instance_alias& pi(collection[key_gen]);
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
STRUCT_ARRAY_TEMPLATE_SIGNATURE
const_index_list
struct_array<D>::resolve_indices(const const_index_list& l) const {
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
			dereference<count_ptr<const const_index> >()
		)
	);
	transform(l.begin(), l.end(), back_inserter(upper_list),
		unary_compose(
			mem_fun_ref(&const_index::upper_bound),
			dereference<count_ptr<const const_index> >()
		)
	);
	return const_index_list(l,
		collection.is_compact_slice(lower_list, upper_list));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid struct_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
STRUCT_ARRAY_TEMPLATE_SIGNATURE
typename struct_array<D>::instance_ptr_type
struct_array<D>::lookup_instance(const multikey_index_type& i) const {
	INVARIANT(D == i.dimensions());
	// will create and return an "uninstantiated" instance if not found
	const key_type index(i);
	const struct_instance_alias&
		b(collection[index]);
//		b(AS_A(const collection_type&, collection)[i]);
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return instance_ptr_type(const_cast<struct_instance_alias*>(&b));
	} else {
		// remove the blank we added?
		// not necessary, but could keep the collection "clean"
		cerr << "ERROR: reference to uninstantiated struct " <<
			// struct type name, please?
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
STRUCT_ARRAY_TEMPLATE_SIGNATURE
bool
struct_array<D>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	INVARIANT(!r.empty());
	multikey_generator<D, pint_value_type> key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const struct_instance_alias& pi(collection[key_gen]);
		if (pi.valid()) {
			l.push_back(instance_ptr_type(
				const_cast<struct_instance_alias*>(&pi)));
		} else {
			cerr << "FATAL: reference to uninstantiated struct index "
				<< key_gen << endl;
			// struct type name, please?
			l.push_back(instance_ptr_type(NULL));
		}
		ret &= pi.valid();
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STRUCT_ARRAY_TEMPLATE_SIGNATURE
void
struct_array<D>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	parent_type::write_object_base(m, f);
#if 0
	collection.write(f);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STRUCT_ARRAY_TEMPLATE_SIGNATURE
void
struct_array<D>::load_object(const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);
#if 0
	collection.read(f);
#endif
}

//=============================================================================
// class struct_array method definitions (specialized)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct_array<0>::struct_array() : parent_type(0), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct_array<0>::struct_array(const scopespace& o, const string& n) :
		parent_type(o, n, 0), the_instance() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct_array<0>::~struct_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
struct_array<0>::is_partially_unrolled(void) const {
	return the_instance.valid();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
struct_array<0>::what(ostream& o) const {
	return o << "struct-scalar";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
struct_array<0>::dump_unrolled_instances(ostream& o) const {
	return o << auto_indent << the_instance << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates the_instance of struct datatype.
	Ideally, the error should never trigger because
	re-instantiation / redeclaration of a scalar instance
	is easily detected (and actually detected) during the compile phase.
	\param i indices must be NULL because this is not an array.
 */
void
struct_array<0>::instantiate_indices(const index_collection_item_ptr_type& i) {
	INVARIANT(!i);
	if (the_instance.valid()) {
		// should never happen, but just in case...
		cerr << "ERROR: Scalar struct already instantiated!" << endl;
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
struct_array<0>::resolve_indices(const const_index_list& l) const {
	cerr << "WARNING: struct_array<0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid struct_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
struct_array<0>::instance_ptr_type
struct_array<0>::lookup_instance(const multikey_index_type& i) const {
	if (!the_instance.valid()) {
		cerr << "ERROR: Reference to uninstantiated struct!" << endl;
		return instance_ptr_type(NULL);
	} else	return instance_ptr_type(
		const_cast<struct_instance_alias*>(&the_instance));
	// ok to return non-const reference to the type, 
	// perhaps it should be declared mutable?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
	\return false to signal error.  
 */
bool
struct_array<0>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	cerr << "WARNING: struct_array<0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
struct_array<0>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	parent_type::write_object_base(m, f);
	write_value(f, the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
struct_array<0>::load_object(const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);
	read_value(f, the_instance);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_STRUCT_CC__

