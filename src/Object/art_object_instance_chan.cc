/**
	\file "art_object_instance_chan.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	TODO: replace duplicate managed code with templates.
	$Id: art_object_instance_chan.cc,v 1.8.2.1 2005/02/03 03:34:52 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_CHAN_CC__
#define	__ART_OBJECT_INSTANCE_CHAN_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_chan.h"
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

#include "ptrs_functional.h"
#include "compose.h"
#include "binders.h"

namespace ART {
namespace entity {
using std::string;
using namespace MULTIKEY_NAMESPACE;
USING_UTIL_COMPOSE
using std::dereference;
using std::mem_fun_ref;

//=============================================================================
// class channel_instance_collection method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(channel_instance_collection,
	CHANNEL_INSTANCE_COLLECTION_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

channel_instance_collection::channel_instance_collection(const scopespace& o, 
		const string& n, const size_t d) : parent_type(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance_collection::~channel_instance_collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instance_collection::type_dump(ostream& o) const {
	cerr << "FANG!  write chan_inst_coll::type_dump()!" << endl;
	return o << "chan ???^" << dimensions;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The type of the first instantiation statement.  
 */
count_ptr<const fundamental_type_reference>
channel_instance_collection::get_type_ref(void) const {
	INVARIANT(!index_collection.empty());
	return (*index_collection.begin())->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create an channel reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
count_ptr<instance_reference_base>
channel_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	return count_ptr<channel_instance_reference>(
		new channel_instance_reference(
			never_ptr<const channel_instance_collection>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a member reference to a channel, 
	and pushes it onto the context's object_stack.  
	\param b is the parent owner of this instantiation referenced.  
 */
count_ptr<member_instance_reference_base>
channel_instance_collection::make_member_instance_reference(
		const count_ptr<const simple_instance_reference>& b) const {
	NEVER_NULL(b);
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
if (!m.register_transient_object(this, 
		CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, dimensions)) {
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance_collection*
channel_instance_collection::make_chan_array(
		const scopespace& o, const string& n, const size_t d) {
	switch(d) {
		case 0: return new chan_array<0>(o, n);
		case 1: return new chan_array<1>(o, n);
		case 2: return new chan_array<2>(o, n);
		case 3: return new chan_array<3>(o, n);
		case 4: return new chan_array<4>(o, n);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
channel_instance_collection::construct_empty(const int i) {
	switch(i) {
		case 0: return new chan_array<0>();
		case 1: return new chan_array<1>();
		case 2: return new chan_array<2>();
		case 3: return new chan_array<3>();
		case 4: return new chan_array<4>();
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance_collection::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	parent_type::write_object_base(m, o);
	// until new members added...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance_collection::load_object_base(
		const persistent_object_manager& m, istream& i) {
	parent_type::load_object_base(m, i);
	// until new members added...
}

//=============================================================================
// class chan_instance_alias method definitions

ostream&
operator << (ostream& o, const chan_instance_alias& b) {
	INVARIANT(b.valid());
	return o << "(chan-alias)";
}

//=============================================================================
// class chan_array method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHAN_ARRAY_TEMPLATE_SIGNATURE
chan_array<D>::chan_array() : parent_type(D), collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHAN_ARRAY_TEMPLATE_SIGNATURE
chan_array<D>::chan_array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHAN_ARRAY_TEMPLATE_SIGNATURE
chan_array<D>::~chan_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHAN_ARRAY_TEMPLATE_SIGNATURE
bool
chan_array<D>::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHAN_ARRAY_TEMPLATE_SIGNATURE
ostream&
chan_array<D>::what(ostream& o) const {
	return o << "chan-array<" << D << ">";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHAN_ARRAY_TEMPLATE_SIGNATURE
ostream&
chan_array<D>::dump_unrolled_instances(ostream& o) const {
	for_each(collection.begin(), collection.end(), key_dumper(o));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHAN_ARRAY_TEMPLATE_SIGNATURE 
ostream&
chan_array<D>::key_dumper::operator () (
		const typename collection_type::value_type& p) {
	return os << auto_indent << p.first << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates channel instances at the specified indices.
	\param i fully-specified range of indices to instantiate.
 */
CHAN_ARRAY_TEMPLATE_SIGNATURE
void
chan_array<D>::instantiate_indices(const index_collection_item_ptr_type& i) {
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
		chan_instance_alias& pi(collection[key_gen]);
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
CHAN_ARRAY_TEMPLATE_SIGNATURE
const_index_list
chan_array<D>::resolve_indices(const const_index_list& l) const {
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
	\return valid chan_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
CHAN_ARRAY_TEMPLATE_SIGNATURE
typename chan_array<D>::instance_ptr_type
chan_array<D>::lookup_instance(const unroll_index_type& i) const {
	INVARIANT(D == i.dimensions());
	// will create and return an "uninstantiated" instance if not found
	const chan_instance_alias&
		b(collection[i]);
//		b(AS_A(const collection_type&, collection)[i]);
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return instance_ptr_type(const_cast<chan_instance_alias*>(&b));
	} else {
		// remove the blank we added?
		// not necessary, but could keep the collection "clean"
		cerr << "ERROR: reference to uninstantiated channel " <<
			// channel type name, please?
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
CHAN_ARRAY_TEMPLATE_SIGNATURE
bool
chan_array<D>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	INVARIANT(!r.empty());
	multikey_generator<D, pint_value_type> key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const chan_instance_alias& pi(collection[key_gen]);
		if (pi.valid()) {
			l.push_back(instance_ptr_type(
				const_cast<chan_instance_alias*>(&pi)));
		} else {
			cerr << "FATAL: reference to uninstantiated channel index "
				<< key_gen << endl;
			// channel type name, please?
			l.push_back(instance_ptr_type(NULL));
		}
		ret &= pi.valid();
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHAN_ARRAY_TEMPLATE_SIGNATURE
void
chan_array<D>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	parent_type::write_object_base(m, f);
	collection.write(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHAN_ARRAY_TEMPLATE_SIGNATURE
void
chan_array<D>::load_object(const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);
	collection.read(f);
}

//=============================================================================
// class chan_array method definitions (specialized)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
chan_array<0>::chan_array() : parent_type(0), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
chan_array<0>::chan_array(const scopespace& o, const string& n) :
		parent_type(o, n, 0), the_instance() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
chan_array<0>::~chan_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
chan_array<0>::is_partially_unrolled(void) const {
	return the_instance.valid();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
chan_array<0>::what(ostream& o) const {
	return o << "chan-scalar";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
chan_array<0>::dump_unrolled_instances(ostream& o) const {
	return o << auto_indent << the_instance << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates the_instance of channel datatype.
	Ideally, the error should never trigger because
	re-instantiation / redeclaration of a scalar instance
	is easily detected (and actually detected) during the compile phase.
	\param i indices must be NULL because this is not an array.
 */
void
chan_array<0>::instantiate_indices(const index_collection_item_ptr_type& i) {
	INVARIANT(!i);
	if (the_instance.valid()) {
		// should never happen, but just in case...
		cerr << "ERROR: Scalar channel already instantiated!" << endl;
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
chan_array<0>::resolve_indices(const const_index_list& l) const {
	cerr << "WARNING: chan_array<0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid chan_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
chan_array<0>::instance_ptr_type
chan_array<0>::lookup_instance(const unroll_index_type& i) const {
	if (!the_instance.valid()) {
		cerr << "ERROR: Reference to uninstantiated channel!" << endl;
		return instance_ptr_type(NULL);
	} else	return instance_ptr_type(
		const_cast<chan_instance_alias*>(&the_instance));
	// ok to return non-const reference to the type, 
	// perhaps it should be declared mutable?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
	\return false to signal error.  
 */
bool
chan_array<0>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	cerr << "WARNING: chan_array<0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chan_array<0>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	parent_type::write_object_base(m, f);
	write_value(f, the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chan_array<0>::load_object(const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);
	read_value(f, the_instance);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_CHAN_CC__

