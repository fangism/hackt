/**
	\file "art_object_instance_bool.cc"
	Method definitions for boolean data type instance classes.
	$Id: art_object_instance_bool.cc,v 1.9.2.6.2.3.2.6 2005/02/25 21:08:31 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_BOOL_CC__
#define	__ART_OBJECT_INSTANCE_BOOL_CC__

#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_bool.h"
#include "art_object_inst_ref_data.h"
#include "art_object_member_inst_ref.h"
#include "art_object_expr_const.h"
#include "art_object_connect.h"
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_type_hash.h"
#include "art_built_ins.h"

#include "art_object_classification_details.h"
#if USE_INSTANCE_COLLECTION_TEMPLATE
#include "art_object_instance_collection.tcc"
#endif

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"

#if !USE_INSTANCE_COLLECTION_TEMPLATE
#include "multikey_set.tcc"
#include "ring_node.tcc"
#include "packed_array.tcc"

#include "persistent_object_manager.tcc"
#include "indent.h"
#include "stacktrace.h"
#include "static_trace.h"
#include "ptrs_functional.h"
#include "compose.h"
#include "binders.h"
#include "dereference.h"
#endif	// USE_INSTANCE_COLLECION_TEMPLATE

#include "memory/list_vector_pool.tcc"

#if !USE_INSTANCE_COLLECTION_TEMPLATE
// conditional defines, after including "stacktrace.h"
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif

#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE


STATIC_TRACE_BEGIN("instance-bool")

//=============================================================================
// module-local specializations

namespace util {
#if USE_INSTANCE_COLLECTION_TEMPLATE
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_scalar, "bool_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_1D, "bool_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_2D, "bool_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_3D, "bool_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_4D, "bool_array_4D")
#else
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array<0>, "bool_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array<1>, "bool_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array<2>, "bool_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array<3>, "bool_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array<4>, "bool_array_4D")
#endif

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_instance_collection, 
		DBOOL_INSTANCE_COLLECTION_TYPE_KEY)

namespace memory {
	// can we still lazy destroy with instance aliases?
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::bool_scalar)
}	// end namespace memory
}	// end namespace util

#if !USE_INSTANCE_COLLECTION_TEMPLATE
namespace std {
using ART::entity::bool_instance_alias;

template <size_t D>
struct _Select1st<bool_instance_alias<D> > :
	public _Select1st<typename bool_instance_alias<D>::parent_type> {
};	// end struct _Select1st

template <size_t D>
struct _Select2nd<bool_instance_alias<D> > :
	public _Select2nd<typename bool_instance_alias<D>::parent_type> {
};

}	// end namespace std
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
namespace ART {
namespace entity {
#if !USE_INSTANCE_COLLECTION_TEMPLATE
using std::string;
using std::_Select1st;
#include "using_ostream.h"
USING_UTIL_COMPOSE
using util::indent;
using util::auto_indent;
using util::dereference;
using std::for_each;
using std::mem_fun_ref;
USING_STACKTRACE
using util::multikey;
using util::value_writer;
using util::value_reader;
using util::read_value;
using util::write_value;
using util::persistent_traits;
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
// struct type_dumper specialization

#if USE_INSTANCE_COLLECTION_TEMPLATE
template <>
struct type_dumper<bool_tag> {
	typedef	class_traits<bool_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	ostream& os;
	type_dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		return os << "bool^" << c.get_dimensions();
	}
};	// end struct type_dumper<bool_tag>

//-----------------------------------------------------------------------------
template <>
struct collection_parameter_persistence<bool_tag> {
	typedef	class_traits<bool_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<bool_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	const persistent_object_manager& pom;

	collection_parameter_persistence(const persistent_object_manager& m) :
		pom(m) { }

	void
	operator () (ostream&, const instance_collection_generic_type&) const {
		// do nothing! bool has no parameters!
	}

	void
	operator () (istream&, instance_collection_generic_type&) const {
		// do nothing! bool has no parameters!
	}
};	// end struct collection_parameter_persistence
//-----------------------------------------------------------------------------

template <>
struct collection_type_committer<bool_tag> {
	typedef class_traits<bool_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<bool_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	// return true on error, false on success
	bool
	operator () (instance_collection_generic_type& c, 
		const type_ref_ptr_type& t) const {
		// INVARIANT(!is_partially_unrolled());
		INVARIANT(t->get_base_def() == &bool_def);
		// shouldn't have any parameters, NULL or empty list
		return false;
	}
};	// end struct collection_type_committer

#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
// class bool_instance_alias_info method definitions

#if !USE_INSTANCE_COLLECTION_TEMPLATE
// inline
bool_instance_alias_info::~bool_instance_alias_info() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Doesn't register itself because this is not directly
	dynamically allocated.  
 */
void
bool_instance_alias_info::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("bool_alias_info::collect_transients()");
	if (instance)
		instance->collect_transient_info(m);
	// eventually need to implement this...

	// shouldn't need to re-visit parent pointer, 
	// UNLESS it is visited from an alias cycle, 
	// in which case, the parent may not have been visited before...

	// this is allowed to be null ONLY if it belongs to a scalar
	// in which case it is not yet unrolled.  
	if (container)
		container->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_alias_info::dump_alias(ostream& o) const {
	DIE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Virtually pure virtual.  Never supposed to be called, 
	yet this definition must exist to allow construction
	of the types that immedately derived from this type.  
 */
void
bool_instance_alias_info::write_next_connection(
		const persistent_object_manager& m, ostream& o) const {
	DIE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_alias_info::load_next_connection(
		const persistent_object_manager& m, istream& i) {
	DIE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_alias_info::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT("bool_alias_info::write_object()");
//	NEVER_NULL(container);
	m.write_pointer(o, instance);
	m.write_pointer(o, container);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	How do we check that this is done once? use container pointer?
 */
void
bool_instance_alias_info::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT("bool_alias_info::load_object()");
	m.read_pointer(i, instance);
	m.read_pointer(i, container);
//	NEVER_NULL(container);		// may be null for scalar instance
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_alias_info::transient_info_collector::operator () (
		const bool_instance_alias_info& b) {
	b.collect_transient_info_base(manager);
}
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
// typedef bool_instance_alias_base function definitions

ostream&
operator << (ostream& o, const bool_instance_alias_base& b) {
	return o << "bool-alias @ " << &b;
}

//=============================================================================
#if !USE_INSTANCE_COLLECTION_TEMPLATE
// class bool_instance_alias method definitions

template <size_t D>
bool_instance_alias<D>::~bool_instance_alias() {
	STACKTRACE_DTOR("~bool_alias<D>()");
}

#if 0
template <size_t D>
ostream&
operator << (ostream& o, const bool_instance_alias<D>& b) {
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out the next instance alias in the connected set.  
 */
template <size_t D>
void
bool_instance_alias<D>::dump_alias(ostream& o) const {
	NEVER_NULL(container);
	o << container->get_qualified_name() <<
		multikey<D, pint_value_type>(key);
		// casting to multikey for the sake of printing [i] for D==1.
		// could use specialization to accomplish this...
		// bah, not important
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is called by element_writer to record the information
	necessary to reconstruct a connection.  
 */
template <size_t D>
void
bool_instance_alias<D>::write_next_connection(
		const persistent_object_manager& m, ostream& o) const {
	// which container did this alias come from?
	m.write_pointer(o, container);
	// what's its key in the container?
	value_writer<key_type> write_key(o);
	write_key(key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D>
void
bool_instance_alias<D>::load_next_connection(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, container);
	// reconstruction ordering problem:
	// container must have its instances alread loaded, though 
	// not necessarily constructed.
	// This is why instance re-population MUST be decoupled from
	// connection re-establishment *GRIN*.  
	// See? there's a reason for everything.  
	NEVER_NULL(container);
	// this is the safe way of ensuring that object is loaded once only.
	m.load_object_once(const_cast<bool_instance_collection*>(&*container));

	// the CONTAINER should read the key, because it is dimension-specific!
	// it should return a reference to the alias node, 
	// which can then be linked.  
	bool_instance_alias_base& n(container->load_reference(i));
	merge(n);	// re-link
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D>
void
bool_instance_alias<D>::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("bool_alias::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	bool_instance_alias_info::collect_transient_info_base(m);
	if (next != this)
		next->collect_transient_info_base(m);	// CYCLE?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 */
template <size_t D>
void
bool_instance_alias<D>::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT("bool_alias::write_object()");
#if 0
	value_writer<key_type> write_key(os);
	write_key(key);
#endif
	bool_instance_alias_info::write_object_base(m, o);
	// write the continuation alias pointer in a difference phase!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D>
void
bool_instance_alias<D>::load_object(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT("bool_alias::load_object()");
#if 0
	value_reader<key_type> kr(os);
	kr(e.key);
#endif
	bool_instance_alias_info::load_object_base(m, i);
	// load the continuation alias pointer in a difference phase!
}

//=============================================================================
// class bool_instance_alias<0> method definitions

bool_instance_alias<0>::~bool_instance_alias() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_alias<0>::dump_alias(ostream& o) const {
	NEVER_NULL(container);
	o << container->get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_alias<0>::write_next_connection(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, container);
	// no key to write!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this is the SAME as template version, this function need not be
	virtual!  
	Only issue: merge is a member function of bool_instance_alias_base, 
	which is a ring_node_derived<...>.
	May require another argument with a safe up-cast?
	Or just have this return the bool_instance_alias_base?
 */
void
bool_instance_alias<0>::load_next_connection(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, container);
	NEVER_NULL(container);
	// no key to read!
	// problem: container is a never_ptr<const ...>, yucky
	m.load_object_once(const_cast<bool_instance_collection*>(&*container));
	bool_instance_alias_base& n(container->load_reference(i));
	merge(n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_alias<0>::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("bool_alias<0>::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	bool_instance_alias_info::collect_transient_info_base(m);
	// next->collect_transient_info_base(m)?	CYCLE!
	// maybe not recursive cycle, just visit the next's container
	if (next != this)
		next->collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_alias<0>::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT("bool_alias<0>::write_object()");
	bool_instance_alias_info::write_object_base(m, o);
	// no key to write!
	// continuation pointer? write in separate phase or now?
	// see connection_writer, may factor out common code...
	NEVER_NULL(next);
	if (next == this) {
		write_value<char>(o, 0);
	} else {
		write_value<char>(o, 1);
		next->write_next_connection(m, o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_alias<0>::load_object(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT("bool_alias<0>::load_object()");
	bool_instance_alias_info::load_object_base(m, i);
	// no key to load!
	// continuation pointer? load in separate phase or now?
	char c;
	read_value(i, c);
	if (c) {
		load_next_connection(m, i);
	}
}
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
#if !USE_INSTANCE_COLLECTION_TEMPLATE
// class bool_instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance_collection::bool_instance_collection(const scopespace& o, 
		const string& n, const size_t d) : parent_type(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance_collection::~bool_instance_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_instance_collection::type_dump(ostream& o) const {
	return o << "bool^" << dimensions;
}

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
bool_instance_collection::member_inst_ref_ptr_type
bool_instance_collection::make_member_instance_reference(
		const inst_ref_ptr_type& b) const {
	NEVER_NULL(b);
	return member_inst_ref_ptr_type(
		new bool_member_instance_reference(
			b, never_ptr<const this_type>(this)));
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

#if !USE_INSTANCE_COLLECTION_TEMPLATE
template <size_t D>
ostream&
operator << (ostream& o, const bool_instance_alias<D>& b) {
	INVARIANT(b.valid());
	// show all aliases?
	return o << "(bool-alias-" << D << ")";
}
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

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
bool_array<D>::~bool_array() {
	STACKTRACE_DTOR("~bool_array<D>()");
}

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
bool_array<D>::key_dumper::operator () (const value_type& p) {
	os << auto_indent << _Select1st<value_type>()(p) << " = ";
	p.get_next()->dump_alias(os);
	return os << endl;
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
	bool err = false;
	do {
		const_iterator iter = collection.find(key_gen);
		if (iter == collection.end()) {
			// then we can insert a new one
			// create with back-ref!
			collection.insert(element_type(key_gen, 
				never_ptr<const this_type>(this)));
		} else {
			// found one that already exists!
			// more detailed message, please!
			cerr << "ERROR: Index " << key_gen << " of ";
			what(cerr) << ' ' << get_qualified_name() <<
				" already instantiated!" << endl;
			err = true;
		}
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	if (err)
		THROW_EXIT;
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
	\return valid bool_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
BOOL_ARRAY_TEMPLATE_SIGNATURE
typename bool_array<D>::instance_ptr_type
bool_array<D>::lookup_instance(const multikey_index_type& i) const {
	INVARIANT(D == i.dimensions());
	// will create and return an "uninstantiated" instance if not found
	const key_type index(i);
	const const_iterator it(collection.find(index));
	if (it == collection.end()) {
		cerr << "ERROR: reference to uninstantiated bool " <<
			get_qualified_name() << " at index: " << i << endl;
		return instance_ptr_type(NULL);
	}
	const element_type& b(*it);
	// can b be invalid anymore? not if this is an array...
	// arrays can only contain validly instantiated aliases.  
	// unlike scalars
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return instance_ptr_type(const_cast<element_type*>(&b));
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
		const const_iterator it(collection.find(key_gen));
		if (it == collection.end()) {
			cerr << "FATAL: reference to uninstantiated bool index "
				<< key_gen << endl;
			l.push_back(instance_ptr_type(NULL));
			ret = false;
		} else {
		const element_type& pi(*it);
		// again pi MUST be valid b/c arrays now only contain
		// valid instances. 
		if (pi.valid()) {
			l.push_back(instance_ptr_type(
				const_cast<element_type*>(&pi)));
		} else {
			cerr << "FATAL: reference to uninstantiated bool index "
				<< key_gen << endl;
			l.push_back(instance_ptr_type(NULL));
			ret = false;
		}
		}
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Packs resolved range of aliases into a collection.  
	\return true on error, else false.  
 */
BOOL_ARRAY_TEMPLATE_SIGNATURE
bool
bool_array<D>::unroll_aliases(const multikey_index_type& l,
		const multikey_index_type& u, alias_collection_type& a) const {
	typedef typename alias_collection_type::key_type
						collection_key_type;
	typedef typename alias_collection_type::iterator
						alias_collection_iterator;
	const key_type lower(l);	// this will assert dimension match!
	const key_type upper(u);	// this will assert dimension match!
	key_generator_type key_gen(lower, upper);
	key_gen.initialize();
	bool ret = false;
	alias_collection_iterator a_iter(a.begin());
	const const_iterator collection_end(collection.end());
	do {
		// really is a monotonic incremental search, 
		// don't need log(N) lookup each time, fix later...
		const const_iterator it(collection.find(key_gen));
		if (it == collection_end) {
			cerr << "FATAL: reference to uninstantiated int index "
				<< key_gen << endl;
			*a_iter = never_ptr<element_type>(NULL);
			ret = true;
		} else {
			const element_type& pi(*it);
			*a_iter = never_ptr<element_type>(
				const_cast<element_type*>(&pi));
		}
		a_iter++;
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	INVARIANT(a_iter == a.end());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads a key from binary stream then returns a reference to the 
	indexed instance alias.  
 */
BOOL_ARRAY_TEMPLATE_SIGNATURE
bool_instance_alias_base&
bool_array<D>::load_reference(istream& i) const {
	key_type k;
	value_reader<key_type> read_key(i);
	read_key(k);
	const iterator it(collection.find(k));
	INVARIANT(it != collection.end());
	// need const cast because set only returns const references/iterators
	return const_cast<element_type&>(*it);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Going to need some sort of element_reader counterpart.
	\param e is a reference to a bool_instance_alias<D>.
 */
BOOL_ARRAY_TEMPLATE_SIGNATURE
void
bool_array<D>::element_writer::operator () (const element_type& e) const {
	STACKTRACE_PERSISTENT("bool_array<D>::element_writer::operator()");
	value_writer<key_type> write_key(os);
	write_key(e.key);
	e.write_object_base(pom, os);
	// postpone connection writing until next phase
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This must perfectly complement element_writer::operator().
	construct the element locally first, then insert it into set.
 */
BOOL_ARRAY_TEMPLATE_SIGNATURE
void
bool_array<D>::element_loader::operator () (void) {
	key_type temp_key;
	value_reader<key_type> read_key(is);
	read_key(temp_key);
	element_type temp_elem(temp_key);
	temp_elem.load_object_base(pom, is);
	coll.insert(temp_elem);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Records a reference to the next alias in the connected ring, 
	by saving the necessary information to reconstruct the link.  
	This is necessary because the element_types are persistent, 
	by not dynamically reconstructed.  
 */
BOOL_ARRAY_TEMPLATE_SIGNATURE
void
bool_array<D>::connection_writer::operator() (const element_type& e) const {
	STACKTRACE_PERSISTENT("bool_array<D>::connection_writer::operator()");
	const bool_instance_alias_base* const next = e.get_next();
	NEVER_NULL(next);
	if (next != &e) {
		write_value<char>(os, 1);
		next->write_next_connection(pom, os);
	} else {
		write_value<char>(os, 0);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must complement connection_writer::operator().
	const_cast is an unfortunate consequence of set only
	returning const references and const iterators, where we intend
	the non-key part of the object to me mutable.  
 */
BOOL_ARRAY_TEMPLATE_SIGNATURE
void
bool_array<D>::connection_loader::operator() (const element_type& e) {
	STACKTRACE_PERSISTENT("bool_array<D>::connection_loader::operator()");
	char c;
	read_value(is, c);
	if (c) {
		element_type& elem(const_cast<element_type&>(e));
		// lookup the instance in the collection referenced
		// and connect them
		elem.load_next_connection(pom, is);
	}
	// else just leave it pointing to itself, 
	// which was how it was constructed
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE
void
bool_array<D>::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<parent_type>::type_key, D)) {
	STACKTRACE_PERSISTENT("bool_array<D>::collect_transients()");
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL_ARRAY_TEMPLATE_SIGNATURE
void
bool_array<D>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	STACKTRACE_PERSISTENT("bool_array<D>::write_object(HELLO)");
	parent_type::write_object_base(m, f);
	// need to know how many members to expect
	write_value(f, collection.size());
	for_each(collection.begin(), collection.end(), 
		element_writer(m, f)
	);
#if 1
	// punting connections...
	for_each(collection.begin(), collection.end(), 
		connection_writer(m, f)
	);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This requires extra caution, because this may recursively invoke
	load_object of other collections through connection and alias
	reconstruction.  Therefore, must go through 
	persistent_object_manager's visit_once check.  
 */
BOOL_ARRAY_TEMPLATE_SIGNATURE
void
bool_array<D>::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("bool_array<D>::load_object()");
	parent_type::load_object_base(m, f);
	// procedure:
	// 1) load all instantiated indices *without* their connections
	//	let them start out pointing to themselves.  
	// 2) each element contains information to reconstruct, 
	//	we need temporary local storage for it.
	size_t collection_size;
	read_value(f, collection_size);
	size_t i = 0;
	element_loader load_element(m, f, collection);
	for ( ; i < collection_size; i++) {
		load_element();
	}
#if 1
	// punting connections...
	for_each(collection.begin(), collection.end(), 
		connection_loader(m, f)
	);
#endif
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
	STACKTRACE_DTOR("~bool_scalar()");
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
	// this never gets called?
#if 0
	o << auto_indent << the_instance << endl;
#else
	the_instance.get_next()->dump_alias(o << " = ");
	return o;
#endif
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
	STACKTRACE("bool_scalar::instantiate()");
	INVARIANT(!i);
	if (the_instance.valid()) {
		// should never happen, but just in case...
		cerr << "ERROR: Scalar bool already instantiated!" << endl;
		THROW_EXIT;
	}
	the_instance.instantiate(never_ptr<const this_type>(this));
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
bool_array<0>::lookup_instance(const multikey_index_type& i) const {
	if (!the_instance.valid()) {
		cerr << "ERROR: Reference to uninstantiated bool!" << endl;
		return instance_ptr_type(NULL);
	} else	return instance_ptr_type(
		const_cast<instance_type*>(&the_instance));
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
/**
	\return true on error, false on success.
 */
bool
bool_array<0>::unroll_aliases(const multikey_index_type& l,
		const multikey_index_type& u, alias_collection_type& a) const {
	if (the_instance.valid()) {
		*(a.begin()) = never_ptr<instance_type>(
			const_cast<instance_type*>(&the_instance));
		return false;
	} else {
		cerr << "ERROR: Reference to uninstantiated int!" << endl;
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance_alias_base&
bool_array<0>::load_reference(istream& i) const {
	// no key to read!
	// const_cast: have to modify next pointers to re-establish connection, 
	// which is semantically allowed because we allow the alias pointers
	// to be mutable.  
	return const_cast<instance_type&>(the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_array<0>::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<parent_type>::type_key, 0)) {
	STACKTRACE_PERSISTENT("bool_scalar::collect_transients()");
	parent_type::collect_transient_info_base(m);
	the_instance.collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_array<0>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	STACKTRACE_PERSISTENT("bool_scalar::write_object()");
	parent_type::write_object_base(m, f);
	the_instance.write_object(m, f);
	// the_instance.write_object may write the continuation information
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_array<0>::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("bool_scalar::load_object()");
	parent_type::load_object_base(m, f);
	the_instance.load_object(m, f);
	// the_instance.load_object may load the continuation information
	// because this collection has been sufficiently loaded
}
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
#if USE_INSTANCE_COLLECTION_TEMPLATE
template class instance_collection<bool_tag>;
template class instance_array<bool_tag, 0>;
template class instance_array<bool_tag, 1>;
template class instance_array<bool_tag, 2>;
template class instance_array<bool_tag, 3>;
template class instance_array<bool_tag, 4>;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

STATIC_TRACE_END("instance-bool")

#endif	// __ART_OBJECT_INSTANCE_BOOL_CC__

