/**
	\file "art_object_instance_proc.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	TODO: replace duplicate managed code with templates.
	$Id: art_object_instance_proc.cc,v 1.8.2.5.4.1.2.2 2005/02/20 06:36:31 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_PROC_CC__
#define	__ART_OBJECT_INSTANCE_PROC_CC__

#define	ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_proc.h"
#include "art_object_inst_stmt_base.h"
#include "art_object_inst_ref.h"
#include "art_object_type_ref.h"
#include "art_object_expr_const.h"
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
	ART::entity::proc_instance, UNIQUE_PROCESS_INSTANCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_instance_collection, 
		PROCESS_INSTANCE_COLLECTION_TYPE_KEY)
}	// end namespace util

namespace ART {
namespace entity {
#include "using_ostream.h"
using std::string;
using util::multikey_generator;
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
USING_STACKTRACE;
using util::write_value;
using util::read_value;
using util::indent;
using util::auto_indent;

//=============================================================================
// class proc_instance method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_instance::proc_instance() : state(0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_instance::~proc_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
proc_instance::what(ostream& o) const {
	return o << "proc_instance";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
proc_instance::construct_empty(const int) {
	return new proc_instance;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_instance::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, UNIQUE_PROCESS_INSTANCE_TYPE_KEY)) {
	// walk vector of pointers...
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_instance::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, state);
	// write pointer sequence...
}

void
proc_instance::load_object(const persistent_object_manager& m, istream& f) {
	read_value(f, state);
	// read pointer sequence...
}

//=============================================================================
// class proc_instance_alias method definitions

// don't register as persistent type...

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_instance_alias::proc_instance_alias() :
		instance(NULL), alias(NULL), instantiated(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_instance_alias::~proc_instance_alias() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
proc_instance_alias::what(ostream& o) const {
	return o << "proc_instance_alias";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_instance_alias::collect_transient_info(
		persistent_object_manager& m) const {
	// don't register itself, not dynamic, and isn't persistent! (yet)
	if (instance)
		instance->collect_transient_info(m);
	if (alias)
		alias->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only intended for use by process_instance_collections, 
	not by the persistent object manager.  
 */
void
proc_instance_alias::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	m.write_pointer(f, instance);
	m.write_pointer(f, alias);
	write_value(f, instantiated);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only intended for use by process_instance_collections, 
	not by the persistent object manager.  
 */
void
proc_instance_alias::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, instance);
	m.read_pointer(f, alias);
	read_value(f, instantiated);
}

//=============================================================================
// class process_instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance_collection::process_instance_collection(const size_t d) :
		parent_type(d), proc_type(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance_collection::process_instance_collection(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d), proc_type(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance_collection::~process_instance_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre must be already partially unrolled, already has a comitted type.  
 */
ostream&
process_instance_collection::type_dump(ostream& o) const {
	STACKTRACE("proc_inst_coll::type_dump()");
	INVARIANT(proc_type);
	return proc_type->dump(o) << '^' << dimensions;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instance_collection::dump(ostream& o) const {
	parent_type::dump(o);
	if (is_partially_unrolled()) {
		if (dimensions) {
			indent indenter(o);
			o << auto_indent << "unrolled indices: {" << endl;
			{
				indent indenter(o);
				dump_unrolled_instances(o);
			}
			o << auto_indent << "}";        // << endl;
		} else {
			// else nothing to say, just one scalar instance
			o << " (instantiated)";
		}
	}
        return o;
}

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
	\param t resolved type-reference parameter, must reference 
		canonical definition, and not an alias.  
	\return false on success, true on error.
 */
bool
process_instance_collection::commit_type(const final_ptr_type& t) {
	STACKTRACE("proc_inst_coll::commit_type()");
	if (is_partially_unrolled()) {
		STACKTRACE("already committed");
		// type-check
		INVARIANT(proc_type);
		if (!t->must_be_equivalent(*proc_type)) {
			// more descriptive error message later...
			cerr << "ERROR: process-types do not match "
				"in unrolling instantiation." << endl;
			return true;
		} else {
			return false;
		}
	} else {
		STACKTRACE("new commit");
		proc_type = t;
		return false;
	}
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
			never_ptr<const process_instance_collection>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a member reference to a process, 
	and pushes it onto the context's object_stack.  
	\param b is the parent owner of this instantiation referenced.  
 */
process_instance_collection::member_inst_ref_ptr_type
process_instance_collection::make_member_instance_reference(
		const inst_ref_ptr_type& b) const {
	NEVER_NULL(b);
	// maybe verify that b contains this, as sanity check
	return member_inst_ref_ptr_type(
		new process_member_instance_reference(
			b, never_ptr<const process_instance_collection>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance_collection::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
	if (proc_type)
		proc_type->collect_transient_info(m);
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
	m.write_pointer(o, proc_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance_collection::load_object_base(
		const persistent_object_manager& m, istream& i) {
	parent_type::load_object_base(m, i);
	m.read_pointer(i, proc_type);
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
proc_array<D>::proc_array() : parent_type(D), collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE
proc_array<D>::proc_array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PROC_ARRAY_TEMPLATE_SIGNATURE
proc_array<D>::~proc_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
PROC_ARRAY_TEMPLATE_SIGNATURE
bool
proc_array<D>::is_partially_unrolled(void) const {
	return !collection.empty();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
PROC_ARRAY_TEMPLATE_SIGNATURE
size_t
proc_array<D>::dimensions(void) const {
	return D;
}
#endif

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
		proc_instance_alias& pi(collection[key_gen]);
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
PROC_ARRAY_TEMPLATE_SIGNATURE
const_index_list
proc_array<D>::resolve_indices(const const_index_list& l) const {
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
	\return valid proc_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
PROC_ARRAY_TEMPLATE_SIGNATURE
typename proc_array<D>::instance_ptr_type
proc_array<D>::lookup_instance(const multikey_index_type& i) const {
	INVARIANT(D == i.dimensions());
	// will create and return an "uninstantiated" instance if not found
	const key_type index(i);
	const proc_instance_alias&
		b(collection[index]);
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
	multikey_generator<D, pint_value_type> key_gen;
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
/**
	TODO:
	\todo visit all members of collection and collect pointers.  
 */
PROC_ARRAY_TEMPLATE_SIGNATURE
void
proc_array<D>::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		PROCESS_INSTANCE_COLLECTION_TYPE_KEY, dimensions)) {
	parent_type::collect_transient_info_base(m);
//	cerr << "FANG: finish proc_array<D>::collect_transient_info()" << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: walk the collection.
	Can't just .write because contains pointers.  
 */
PROC_ARRAY_TEMPLATE_SIGNATURE
void
proc_array<D>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	parent_type::write_object_base(m, f);
#if 0
	cerr << "FANG: finish proc_array<D>::write_object()" << endl;
	collection.write(f);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: walk the collection.
	Can't just read because contains pointers.  
 */
PROC_ARRAY_TEMPLATE_SIGNATURE
void
proc_array<D>::load_object(const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);
#if 0
	cerr << "FANG: finish proc_array<D>::load_object()" << endl;
	collection.read(f);
#endif
}

//=============================================================================
// class proc_array method definitions (specialized)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_array<0>::proc_array() : parent_type(0), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_array<0>::proc_array(const scopespace& o, const string& n) :
		parent_type(o, n, 0), the_instance() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_array<0>::~proc_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
bool
proc_array<0>::is_partially_unrolled(void) const {
	return the_instance.valid();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
size_t
proc_array<0>::dimensions(void) const {
	return 0;
}
#endif

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
proc_array<0>::lookup_instance(const multikey_index_type& i) const {
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
/**
	TODO:
	\todo visit all members of collection and collect pointers.  
 */
void
proc_array<0>::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		PROCESS_INSTANCE_COLLECTION_TYPE_KEY, dimensions)) {
	parent_type::collect_transient_info_base(m);
//	cerr << "FANG: finish proc_array<0>::collect_transient_info()" << endl;
	the_instance.collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_array<0>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	parent_type::write_object_base(m, f);

//	cerr << "FANG: finish proc_array<0>::write_object()" << endl;
	the_instance.write_object(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_array<0>::load_object(const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);

//	cerr << "FANG: finish proc_array<0>::load_object()" << endl;
	the_instance.load_object(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#undef	ENABLE_STACKTRACE

#endif	// __ART_OBJECT_INSTANCE_PROC_CC__

