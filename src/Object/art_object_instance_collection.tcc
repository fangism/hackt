/**
	\file "art_object_instance_collection.tcc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	TODO: replace duplicate managed code with templates.
	$Id: art_object_instance_collection.tcc,v 1.1.4.9 2005/02/26 04:56:43 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_COLLECTION_TCC__
#define	__ART_OBJECT_INSTANCE_COLLECTION_TCC__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overridable debug switches

#ifndef	DEBUG_LIST_VECTOR_POOL
#define	DEBUG_LIST_VECTOR_POOL				0
#endif
#ifndef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#endif

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif
#ifndef	STACKTRACE_DESTRUCTORS
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#endif
#ifndef	STACKTRACE_PERSISTENTS
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_alias.h"
#include "art_object_instance_collection.h"

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"

#include "multikey_set.tcc"
#include "ring_node.tcc"
#include "packed_array.tcc"

// #include "memory/list_vector_pool.tcc"
#include "persistent_object_manager.tcc"
#include "indent.h"
#include "what.h"
#include "stacktrace.h"
#include "static_trace.h"
#include "ptrs_functional.h"
#include "compose.h"
#include "binders.h"
#include "dereference.h"

// conditional defines, after including "stacktrace.h"
#ifndef	STACKTRACE_DTOR
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif
#endif

#ifndef	STACKTRACE_PERSISTENT
#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif
#endif

//=============================================================================
// module-local specializations

namespace std {
using ART::entity::instance_alias;

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
struct _Select1st<INSTANCE_ALIAS_CLASS> :
	public _Select1st<typename INSTANCE_ALIAS_CLASS::parent_type> {
};      // end struct _Select1st

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
struct _Select2nd<INSTANCE_ALIAS_CLASS> :
	public _Select2nd<typename INSTANCE_ALIAS_CLASS::parent_type> {
};	// end struct _Select2nd
}	// end namespace std

//=============================================================================

namespace ART {
namespace entity {
using std::string;
using std::_Select1st;
#include "using_ostream.h"
using util::multikey_generator;
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
USING_STACKTRACE
using util::multikey;
using util::value_writer;
using util::value_reader;
using util::write_value;
using util::read_value;
using util::indent;
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// class instance_alias_info method definitions

INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
// inline
INSTANCE_ALIAS_INFO_CLASS::~instance_alias_info() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (this->instance)
		this->instance->collect_transient_info(m);
	// eventually need to implement this...

	// shouldn't need to re-visit parent pointer, 
	// UNLESS it is visited from an alias cycle, 
	// in which case, the parent may not have been visited before...

	// this is allowed to be null ONLY if it belongs to a scalar
	// in which case it is not yet unrolled.  
	if (this->container)
		this->container->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::dump_alias(ostream& o) const {
	DIE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Virtually pure virtual.  Never supposed to be called, 
	yet this definition must exist to allow construction
	of the types that immedately derived from this type.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::write_next_connection(
		const persistent_object_manager&, ostream&) const {
	DIE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really, pure virtual.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::load_next_connection(
		const persistent_object_manager&, istream&) {
	DIE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::write_object_base(
		const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, this->instance);
	m.write_pointer(o, this->container);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, this->instance);
	m.read_pointer(i, this->container);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::transient_info_collector::operator () (
		const INSTANCE_ALIAS_INFO_CLASS& i) {
	i.collect_transient_info_base(this->manager);
}

//=============================================================================
// typedef instance_alias_base function definitions

INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o,
	const typename INSTANCE_ALIAS_INFO_CLASS::instance_alias_base_type& i) {
	return o << class_traits<Tag>::tag_name << "-alias @ " << &i;
}

//=============================================================================
// class instance_alias method definitions

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
INSTANCE_ALIAS_CLASS::~instance_alias() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Prints out the next instance alias in the connected set.  
 */
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::dump_alias(ostream& o) const {
	NEVER_NULL(this->container);
	o << this->container->get_qualified_name() <<
		multikey<D, pint_value_type>(this->key);
		// casting to multikey for the sake of printing [i] for D==1.
		// could use specialization to accomplish this...
		// bah, not important
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::write_next_connection(
		const persistent_object_manager& m, ostream& o) const {
	NEVER_NULL(this->container);
	m.write_pointer(o, this->container);
#if 1
	value_writer<key_type> write_key(o);
	write_key(key);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::load_next_connection(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_alias<Tag,D>::load_next_connection()");
	m.read_pointer(i, container);
#if 1
	// reconstruction ordering problem:
	// container must have its instances alread loaded, though 
	// not necessarily constructed.
	// This is why instance re-population MUST be decoupled from
	// connection re-establishment *GRIN*.  
	// See? there's a reason for everything.  
	NEVER_NULL(this->container);
	// this is the safe way of ensuring that object is loaded once only.
	m.load_object_once(
		const_cast<instance_collection_generic_type*>(
			&*this->container));

	// the CONTAINER should read the key, because it is dimension-specific!
	// it should return a reference to the alias node, 
	// which can then be linked.  
	instance_alias_base_type& n(this->container->load_reference(i));
	merge(n);       // re-link
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,D>::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(m);
#if 1
	if (this->next != this)
		this->next->collect_transient_info_base(m);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 */
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,D>::write_object()");
#if 0
	// DO NOT write out key now, that is the job of the next phase!
	value_writer<key_type> write_key(os);
	write_key(key);
#endif
	INSTANCE_ALIAS_INFO_CLASS::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT("int_alias::load_object()");
#if 0
	// DO NOT load in key now, that is the job of the next phase!
	value_reader<key_type> kr(os);
	kr(e.key);
#endif
	INSTANCE_ALIAS_INFO_CLASS::load_object_base(m, i);
	// TODO: copy draft code from "art_object_instance_bool.cc"
}

//=============================================================================
// class KEYLESS_INSTANCE_ALIAS_CLASS method definitions

KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
KEYLESS_INSTANCE_ALIAS_CLASS::~instance_alias() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::dump_alias(ostream& o) const {
	NEVER_NULL(this->container);
	o << this->container->get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::write_next_connection(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, this->container);
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
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::load_next_connection(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_alias<Tag,0>::load_next_connection()");
	m.read_pointer(i, this->container);
#if 1
	NEVER_NULL(this->container);
	// no key to read!
	// problem: container is a never_ptr<const ...>, yucky
	m.load_object_once(
		const_cast<instance_collection_generic_type*>(
			&*this->container));
	instance_alias_base_type& n(this->container->load_reference(i));
	merge(n);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,0>::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(m);
	if (this->next != this)
		this->next->collect_transient_info_base(m);
	// TODO: copy draft code from "art_object_instance_bool.cc"
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,0>::write_object()");
	INSTANCE_ALIAS_INFO_CLASS::write_object_base(m, o);
	// no key to write!
	// continuation pointer?
#if 1
	NEVER_NULL(this->next);
	if (this->next == this) {
		write_value<char>(o, 0);
	} else {
		write_value<char>(o, 1);
		this->next->write_next_connection(m, o);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT("instance_alias<Tag,0>::load_object()");
	INSTANCE_ALIAS_INFO_CLASS::load_object_base(m, i);
	// no key to load!
#if 1
	char c;
	read_value(i, c);
	if (c) {
		this->load_next_connection(m, i);
	}
#endif
}

//=============================================================================
// class instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::instance_collection(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d), type_parameter() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::~instance_collection() {
	STACKTRACE("~instance_collection<>()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This needs to be specialized with a functor...
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
ostream&
INSTANCE_COLLECTION_CLASS::type_dump(ostream& o) const {
	type_dumper<Tag> dump_it(o);
	return dump_it(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	During unroll phase, this commits the type of the collection.  
	\param t the data integer type reference, containing width, 
		must already be resolved to a const_param_expr_list.  
	\return false on success, true on error.  
	\post the integer width is fixed for the rest of the program.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
bool
INSTANCE_COLLECTION_CLASS::commit_type(const type_ref_ptr_type& t) {
	// functor, specialized for each class
	return collection_type_committer<Tag>()(*this, t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create a int data reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<instance_reference_base>
INSTANCE_COLLECTION_CLASS::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<instance_reference_base>	return_type;
	return return_type(new instance_reference_type(
			never_ptr<const this_type>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
typename INSTANCE_COLLECTION_CLASS::member_inst_ref_ptr_type
INSTANCE_COLLECTION_CLASS::make_member_instance_reference(
		const inst_ref_ptr_type& b) const {
	NEVER_NULL(b);
	return member_inst_ref_ptr_type(
		new member_instance_reference_type(
			b, never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to return a legitmate reference to a parameter list!
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
never_ptr<const const_param_expr_list>
INSTANCE_COLLECTION_CLASS::get_actual_param_list(void) const {
	STACKTRACE("instance_collection::get_actual_param_list()");
	return never_ptr<const const_param_expr_list>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS*
INSTANCE_COLLECTION_CLASS::make_array(
		const scopespace& o, const string& n, const size_t d) {
	switch(d) {
		case 0: return new instance_array<Tag,0>(o, n);
		case 1: return new instance_array<Tag,1>(o, n);
		case 2: return new instance_array<Tag,2>(o, n);
		case 3: return new instance_array<Tag,3>(o, n);
		case 4: return new instance_array<Tag,4>(o, n);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
persistent*
INSTANCE_COLLECTION_CLASS::construct_empty(const int i) {
	switch(i) {
		case 0: return new instance_array<Tag,0>();
		case 1: return new instance_array<Tag,1>();
		case 2: return new instance_array<Tag,2>();
		case 3: return new instance_array<Tag,3>();
		case 4: return new instance_array<Tag,4>();
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
	collection_parameter_persistence<Tag>::collect(m, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	parent_type::write_object_base(m, o);
	// specialization functor parameter writer
	collection_parameter_persistence<Tag>::write(m, o, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	parent_type::load_object_base(m, i);
	// specialization functor parameter loader
	collection_parameter_persistence<Tag>::load(m, i, *this);
}

//=============================================================================
// class instance_alias method definitions

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o, const instance_alias<Tag,D>& b) {
	INVARIANT(b.valid());
	return o << '(' << class_traits<Tag>::tag_name << "-alias-" << D << ")";
}

//=============================================================================
// class array method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array() : parent_type(D), collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::~instance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::dump_unrolled_instances(ostream& o) const {
	for_each(this->collection.begin(), this->collection.end(),
		key_dumper(o));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE 
ostream&
INSTANCE_ARRAY_CLASS::key_dumper::operator () (const value_type& p) {
	os << auto_indent << _Select1st<value_type>()(p) << " = ";
	p.get_next()->dump_alias(os);
	return os << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates integer parameters at the specified indices.
	\param i fully-specified range of indices to instantiate.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::instantiate_indices(
		const index_collection_item_ptr_type& i) {
	STACKTRACE("instance_array<Tag,D>::instantiate_indices()");
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
		const_iterator iter = this->collection.find(key_gen);
		if (iter == collection.end()) {
			// then we can insert a new one
			// create with back-ref!
			this->collection.insert(element_type(key_gen,
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
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
const_index_list
INSTANCE_ARRAY_CLASS::resolve_indices(const const_index_list& l) const {
	const size_t l_size = l.size();
	if (D == l_size) {
		// already fully specified
		return l;
	}
	// convert indices to pair of list of multikeys
	if (!l_size) {
		return const_index_list(l, this->collection.is_compact());
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
	\return valid instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
typename INSTANCE_ARRAY_CLASS::instance_alias_base_ptr_type
INSTANCE_ARRAY_CLASS::lookup_instance(const multikey_index_type& i) const {
	typedef	typename INSTANCE_ARRAY_CLASS::instance_alias_base_ptr_type
							return_type;
	INVARIANT(D == i.dimensions());
	const key_type index(i);
	const const_iterator it(this->collection.find(index));
	if (it == this->collection.end()) {
		this->type_dump(
			cerr << "ERROR: reference to uninstantiated ") <<
			" " << this->get_qualified_name() << " at index: " <<
			i << endl;
		return return_type(NULL);
	}
	const element_type& b(*it);
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return return_type(const_cast<element_type*>(&b));
	} else {
		// remove the blank we added?
		// not necessary, but could keep the collection "clean"
		this->type_dump(
			cerr << "ERROR: reference to uninstantiated ") <<
			" " << this->get_qualified_name() << " at index: " <<
			i << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l list in which to accumulate instance references.
	\param r the ranges, must be valid, and fully resolved.
	\return false on error, e.g. if value doesn't exist or
		is uninitialized; true on success.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::lookup_instance_collection(
		list<instance_alias_base_ptr_type>& l,
		const const_range_list& r) const {
	INVARIANT(!r.empty());
//	multikey_generator<D, pvalue_type> key_gen;
	key_generator_type key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const const_iterator it(this->collection.find(key_gen));
		if (it == collection.end()) {
			this->type_dump(
				cerr << "FATAL: reference to uninstantiated ")
					<< " index " << key_gen << endl;
			l.push_back(instance_alias_base_ptr_type(NULL));
			ret = false;
		} else {
		const element_type& pi(*it);
		// pi MUST be valid if it belongs to an array
		if (pi.valid()) {
			l.push_back(instance_alias_base_ptr_type(
				const_cast<element_type*>(&pi)));
		} else {
			this->type_dump(
				cerr << "FATAL: reference to uninstantiated ")
					<< " index " << key_gen << endl;
			l.push_back(instance_alias_base_ptr_type(NULL));
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
	\pre array a must already be sized properly, do not resize here.  
	\return true on error, else false.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::unroll_aliases(const multikey_index_type& l, 
		const multikey_index_type& u, alias_collection_type& a) const {
	typedef	typename alias_collection_type::key_type
						collection_key_type;
	typedef	typename alias_collection_type::iterator
						alias_collection_iterator;
	const key_type lower(l);	// this will assert dimension match!
	const key_type upper(u);	// this will assert dimension match!
	key_generator_type key_gen(lower, upper);
	key_gen.initialize();
	bool ret = false;
	alias_collection_iterator a_iter(a.begin());
	const const_iterator collection_end(this->collection.end());
	// maybe INVARIANT(sizes == iterations)
	do {
		// really is a monotonic incremental search, 
		// don't need log(N) lookup each time, fix later...
		const const_iterator it(this->collection.find(key_gen));
		if (it == collection_end) {
			this->type_dump(
			cerr << "FATAL: reference to uninstantiated ") <<
				" index " << key_gen << endl;
			*a_iter = never_ptr<element_type>(NULL);
			ret = true;
		} else {
			const element_type& pi(*it);
			*a_iter = never_ptr<element_type>(
				const_cast<element_type*>(&pi));
		}
		a_iter++;
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	INVARIANT(a_iter == a.end());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads a key from binary stream then returns a reference to the 
	indexed instance alias.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
typename INSTANCE_ARRAY_CLASS::instance_alias_base_type&
INSTANCE_ARRAY_CLASS::load_reference(istream& i) const {
	key_type k;
	value_reader<key_type> read_key(i);
	read_key(k);
	const iterator it(collection.find(k));
	INVARIANT(it != this->collection.end());
	// need const cast because set only returns const references/iterators
	return const_cast<element_type&>(*it);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Going to need some sort of element_reader counterpart.
	\param e is a reference to a INSTANCE_ALIAS_CLASS.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::element_writer::operator () (const element_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::element_writer::operator()");
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
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::element_loader::operator () (void) {
	key_type temp_key;
	value_reader<key_type> read_key(this->is);
	read_key(temp_key);
	element_type temp_elem(temp_key);
	temp_elem.load_object_base(this->pom, this->is);
	this->coll.insert(temp_elem);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::connection_writer::operator() (const element_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::connection_writer::operator()");
	const instance_alias_base_type* const next = e.get_next();
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
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::connection_loader::operator() (const element_type& e) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::connection_loader::operator()");
	char c;
	read_value(this->is, c);
	if (c) {
		element_type& elem(const_cast<element_type&>(e));
		// lookup the instance in the collection referenced
		// and connect them
		elem.load_next_connection(this->pom, this->is);
	}
	// else just leave it pointing to itself, 
	// which was how it was constructed
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<parent_type>::type_key, D)) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::collect_transients()");
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	parent_type::write_object_base(m, f);
	// need to know how many members to expect
	write_value(f, this->collection.size());
	for_each(this->collection.begin(), this->collection.end(),
		element_writer(m, f)
	);
#if 1
	for_each(this->collection.begin(), this->collection.end(), 
		connection_writer(m, f)
	);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);
	// procedure:
	// 1) load all instantiated indices *without* their connections
	//      let them start out pointing to themselves.  
	// 2) each element contains information to reconstruct, 
	//      we need temporary local storage for it.
	size_t collection_size;
	read_value(f, collection_size);
	size_t i = 0;
	element_loader load_element(m, f, this->collection);
	for ( ; i < collection_size; i++) {
		// this must perfectly complement element_writer::operator()
		// construct the element locally first, then insert it into set
		load_element();
	}
#if 1
	for_each(collection.begin(), collection.end(),
		connection_loader(m, f)
	);
#endif
}

//=============================================================================
// class array method definitions (specialized)

#if 0
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(bool_scalar, 256)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array() : parent_type(0), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array(const scopespace& o, const string& n) :
		parent_type(o, n, 0), the_instance() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::~instance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bool
INSTANCE_SCALAR_CLASS::is_partially_unrolled(void) const {
	return this->the_instance.valid();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::dump_unrolled_instances(ostream& o) const {
#if 0
	return o << auto_indent << the_instance << endl;
#else
	// no auto-indent
	this->the_instance.get_next()->dump_alias(o << " = ");
	return o;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
/**
	Instantiates the_instance of integer datatype.
	Ideally, the error should never trigger because
	re-instantiation / redeclaration of a scalar instance
	is easily detected (and actually detected) during the compile phase.
	\param i indices must be NULL because this is not an array.
 */
void
INSTANCE_SCALAR_CLASS::instantiate_indices(
		const index_collection_item_ptr_type& i) {
	STACKTRACE("instance_array<Tag,0>::instantiate_indices()");
	INVARIANT(!i);
	if (this->the_instance.valid()) {
		// should never happen, but just in case...
		this->type_dump(cerr << "ERROR: Scalar ") <<
			" already instantiated!" << endl;
		THROW_EXIT;
	}
//	the_instance.instantiate();
	this->the_instance.instantiate(never_ptr<const this_type>(this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This specialization isn't ever supposed to be called.
	\param l is list of indices, which may be under-specified,
		or even empty.
	\return empty index list, always.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
const_index_list
INSTANCE_SCALAR_CLASS::resolve_indices(const const_index_list& l) const {
	cerr << "WARNING: instance_array<Tag,0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_alias_base_ptr_type
INSTANCE_SCALAR_CLASS::lookup_instance(const multikey_index_type& i) const {
	typedef	typename INSTANCE_SCALAR_CLASS::instance_alias_base_ptr_type
						return_type;
	if (!this->the_instance.valid()) {
		this->type_dump(cerr << "ERROR: Reference to uninstantiated ")
			<< "!" << endl;
		return return_type(NULL);
	} else	return return_type(
		const_cast<instance_alias_base_type*>(
			&static_cast<const instance_alias_base_type&>(
				this->the_instance)));
	// ok to return non-const reference to the type, 
	// perhaps it should be declared mutable?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
	\return false to signal error.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bool
INSTANCE_SCALAR_CLASS::lookup_instance_collection(
		list<instance_alias_base_ptr_type>& l,
		const const_range_list& r) const {
	cerr << "WARNING: instance_array<Tag,0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error, false on success.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bool
INSTANCE_SCALAR_CLASS::unroll_aliases(const multikey_index_type& l, 
		const multikey_index_type& u, alias_collection_type& a) const {
	if (this->the_instance.valid()) {
		*(a.begin()) = instance_alias_base_ptr_type(
			const_cast<instance_alias_base_type*>(
				&static_cast<const instance_alias_base_type&>(
					this->the_instance)));
		return false;
	} else {
		this->type_dump(cerr << "ERROR: Reference to uninstantiated ")
			<< "!" << endl;
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_alias_base_type&
INSTANCE_SCALAR_CLASS::load_reference(istream& i) const {
	// no key to read!
	// const_cast: have to modify next pointers to re-establish connection, 
	// which is semantically allowed because we allow the alias pointers
	// to be mutable.  
	return const_cast<instance_type&>(this->the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<parent_type>::type_key, 0)) {
	STACKTRACE_PERSISTENT("instance_scalar::collect_transients()");
	parent_type::collect_transient_info_base(m);
	this->the_instance.collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	parent_type::write_object_base(m, f);
	this->the_instance.write_object(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);
	this->the_instance.load_object(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_COLLECTION_TCC__

