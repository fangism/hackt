/**
	\file "Object/inst/port_formal_array.h"
	$Id: port_formal_array.tcc,v 1.5 2006/11/27 08:29:16 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PORT_FORMAL_ARRAY_TCC__
#define	__HAC_OBJECT_INST_PORT_FORMAL_ARRAY_TCC__

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

#include <iostream>
#include <iterator>
#include <algorithm>

#include "Object/inst/port_formal_array.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/port_context.h"
#include "Object/def/footprint.h"
#include "common/ICE.h"

#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"
#include "util/packed_array.tcc"
#include "Object/inst/collection_traits.h"
#include "Object/inst/collection_pool.tcc"	// for lookup_index
#include "util/compose.h"
#include "util/dereference.h"
#include "util/indent.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::transform;
using std::mem_fun_ref;
using std::back_inserter;
USING_UTIL_COMPOSE
using util::dereference;
using util::write_value;
using util::read_value;
using util::value_writer;
using util::value_reader;
using util::auto_indent;

//=============================================================================
// class port_formal_array method definitions

PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
PORT_FORMAL_ARRAY_CLASS::port_formal_array() :
		parent_type(), value_array() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
PORT_FORMAL_ARRAY_CLASS::port_formal_array(const footprint& f, 
		const instance_placeholder_ptr_type p) :
		parent_type(f, p),
		value_array() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	All-in-one constructor.  
	Recursively instantiates ports.  
	\param p instance placeholder pointer.
	\param k the dimensions of the array to instantiate.
	\param t the collection type information.
	\param r relaxed actuals to attach to all elements, if applicable.
	\param c unroll-context for recursive port instantiation.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
PORT_FORMAL_ARRAY_CLASS::port_formal_array(
		const instance_placeholder_ptr_type p,
		const key_type& k, 
		const instance_collection_parameter_type& t, 
		const count_ptr<const const_param_expr_list>& r, 
		const unroll_context& c) :
		parent_type(p, t),
		value_array(k) {
	// should instantiate all members without relaxed actuals
	// TODO: instantiate!
	// and complete type at the same time?
	iterator i(this->begin()), e(this->end());
	for ( ; i!=e; ++i) {
		i->attach_actuals(r);
		i->instantiate(never_ptr<const this_type>(this), c);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
PORT_FORMAL_ARRAY_CLASS::~port_formal_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
ostream&
PORT_FORMAL_ARRAY_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
typename PORT_FORMAL_ARRAY_CLASS::iterator
PORT_FORMAL_ARRAY_CLASS::begin(void) {
	return this->value_array.begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
typename PORT_FORMAL_ARRAY_CLASS::const_iterator
PORT_FORMAL_ARRAY_CLASS::begin(void) const {
	return this->value_array.begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
typename PORT_FORMAL_ARRAY_CLASS::iterator
PORT_FORMAL_ARRAY_CLASS::end(void) {
	return this->value_array.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
typename PORT_FORMAL_ARRAY_CLASS::const_iterator
PORT_FORMAL_ARRAY_CLASS::end(void) const {
	return this->value_array.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
bool
PORT_FORMAL_ARRAY_CLASS::is_partially_unrolled(void) const {
	// we use 0-dimensions to indicate that the array
	// has not yet been populated.  
	return this->value_array.dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true.  By definition, this class is reserver for formals.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
bool
PORT_FORMAL_ARRAY_CLASS::is_formal(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param a reference to alias belonging to this collection.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
ostream&
PORT_FORMAL_ARRAY_CLASS::dump_element_key(ostream& o, 
		const instance_alias_info_type& a) const {
	STACKTRACE_VERBOSE;
	// internally uses 0-based indices, no correction needed.
	const key_type k(this->value_array.index_to_key(
		this->value_array.lookup_index(a)));
	return o << k;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i 1-based indexed into collection.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
ostream&
PORT_FORMAL_ARRAY_CLASS::dump_element_key(ostream& o, 
		const size_t i) const {
	STACKTRACE_VERBOSE;
	// internally 0-based index, adjusted for 1-based parameter
	const key_type k(this->value_array.index_to_key(i -1));
	return o << k;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
multikey_index_type
PORT_FORMAL_ARRAY_CLASS::lookup_key(const instance_alias_info_type& a) const {
	STACKTRACE_VERBOSE;
	// internally uses 0-based indices, no correction needed.
	return this->value_array.index_to_key(
		this->value_array.lookup_index(a));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i 1-based index into collection.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
multikey_index_type
PORT_FORMAL_ARRAY_CLASS::lookup_key(const size_t i) const {
	STACKTRACE_VERBOSE;
	INVARIANT(i);
	return this->value_array.index_to_key(i -1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre a's address must fall in the range spanned by the value_array,
		i.e. it must be an element of the array.  
	Very efficient, pointer subtraction.  
	\return 1-based internal index.
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
size_t
PORT_FORMAL_ARRAY_CLASS::lookup_index(const instance_alias_info_type& a) const {
	STACKTRACE_VERBOSE;
	return this->value_array.lookup_index(a) +1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Error handling?
	\pre k must be valid.
	\return 1-based index.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
size_t
PORT_FORMAL_ARRAY_CLASS::lookup_index(const multikey_index_type& k) const {
	STACKTRACE_VERBOSE;
//	return this->value_array.key_to_index(k) +1;	// protected
	return distance(this->begin(), const_iterator(&this->value_array[k])) +1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
size_t
PORT_FORMAL_ARRAY_CLASS::collection_size(void) const {
	return array_type::sizes_product(this->value_array.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: what if param p is of sparse collection type?
	\pre this and p must be same type and same size (all dimensions).  
	To be more efficient, should use iterator subtraction...
		avoid translating to key and back to index.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
typename PORT_FORMAL_ARRAY_CLASS::instance_alias_info_type&
PORT_FORMAL_ARRAY_CLASS::get_corresponding_element(
		const collection_interface_type& p, 
		const instance_alias_info_type& a) {
	STACKTRACE_VERBOSE;
	INVARIANT(this->value_array.dimensions());
	// why not just p.lookup_index(a); ?
	const this_type* t(IS_A(const this_type*, &p));
	if (t) {
		const size_t o = t->value_array.lookup_index(a);
#if 0
		const key_type k(t.index_to_key(o));
		return this->value_array[k];
#else
		return *(this->begin() +o);
#endif
	} else {
		// is a sparse collection
		const multikey_index_type k(p.lookup_key(a));
		INVARIANT(k.size());
		STACKTRACE_INDENT_PRINT("key = " << k << endl);
		return this->value_array[k];
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off of instance_array::key_dumper.
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
ostream&
PORT_FORMAL_ARRAY_CLASS::dump_unrolled_instances(ostream& o, 
		const dump_flags& df) const {
	for_each(this->begin(), this->end(), 
		typename parent_type::key_dumper(o, df));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should only ever be called once per port formal.
	This is a result of not being able to (conveniently)
	initialize the entire collection in this constructor... yet.
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
good_bool
PORT_FORMAL_ARRAY_CLASS::instantiate_indices(const const_range_list& ranges, 
		const instance_relaxed_actuals_type& actuals, 
		const unroll_context& c) {
	INVARIANT(!this->value_array.dimensions());
	const key_type k(ranges.resolve_sizes());
	this->value_array.resize(k);
	iterator i(this->begin()), e(this->end());
	if (actuals) {
		// if ports are ever allowed to depend on relaxed parameters,
		// then must attach actuals first before instantiating.  
		for ( ; i!=e; ++i) {
			i->instantiate(never_ptr<this_type>(this), c);
			const bool attached = i->attach_actuals(actuals);
			NEVER_NULL(attached);
		}
	} else {
		for ( ; i!=e; ++i) {
			i->instantiate(never_ptr<this_type>(this), c);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
good_bool
PORT_FORMAL_ARRAY_CLASS::allocate_local_instance_ids(footprint& f) {
	STACKTRACE_VERBOSE;
	iterator i(this->begin()), e(this->end());
	for ( ; i!=e; ++i) {
		if (!i->assign_local_instance_id(f)) {
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just need to make sure indices are in range.
	Since this is a dense collection, should always succeed if
	input range is valid.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
const_index_list
PORT_FORMAL_ARRAY_CLASS::resolve_indices(const const_index_list& l) const {
	STACKTRACE_VERBOSE;
	const size_t l_size = l.size();
	const size_t D = this->get_dimensions();
	if (D == l_size) {
		// already fully specified
		return l;
	}
	// TODO: factor out value-independent code from template class
	// else construct slice
	list<pint_value_type> lower_list, upper_list;
	transform(l.begin(), l.end(), back_inserter(lower_list),
		unary_compose(
			mem_fun_ref(&const_index::lower_bound),
			dereference<const_index_list::value_type>()
		)
	);
	transform(l.begin(), l.end(), back_inserter(upper_list),
		unary_compose(
			mem_fun_ref(&const_index::upper_bound),
			dereference<const_index_list::value_type>()
		)
	);
	// first key is always all zeros in port packed arrays
	const key_type lk(this->value_array.last_key());
	size_t j = l_size;
	for ( ; j < D; ++j) {
		lower_list.push_back(0);
		upper_list.push_back(lk[j]);
	}
	// convert indices to pair of list of multikeys
	return const_index_list(l,
		std::make_pair(lower_list, upper_list));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// operator []

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
typename PORT_FORMAL_ARRAY_CLASS::instance_alias_info_ptr_type
PORT_FORMAL_ARRAY_CLASS::lookup_instance(const multikey_index_type& i) const {
	typedef	instance_alias_info_ptr_type		return_type;
	STACKTRACE_VERBOSE;
	if (this->value_array.range_check(i)) {
		return return_type(&const_cast<instance_alias_info_type&>(
			value_array[i]));
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l list in which to accumulate instance references.  
	\return false to signal error, true to signal success.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
bool
PORT_FORMAL_ARRAY_CLASS::lookup_instance_collection(
		typename default_list<instance_alias_info_ptr_type>::type& l,
		const const_range_list& r) const {
	STACKTRACE_VERBOSE;
	INVARIANT(!r.empty());
	key_generator_type key_gen(r.lower_multikey(), r.upper_multikey());
	if (!value_array.range_check(key_gen.lower_corner)
			|| !value_array.range_check(key_gen.upper_corner)) {
		this->type_dump(cerr << "FATAL: reference to ") <<
			" with out-of-bounds indices " <<
			key_gen.lower_corner << ":" << key_gen.upper_corner
			<< endl;
		return false;
	}
	key_gen.initialize();
	// TODO: consider a subroutine to grab entire subslice efficiently
	do {
		const instance_alias_info_ptr_type
			pi(&const_cast<instance_alias_info_type&>(
				value_array[key_gen]));
		NEVER_NULL(pi);
		INVARIANT(pi->valid());	// port must already be instantiated
		l.push_back(pi);
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
bad_bool
PORT_FORMAL_ARRAY_CLASS::unroll_aliases(const multikey_index_type& l, 
		const multikey_index_type& u, alias_collection_type& a) const {
	STACKTRACE_VERBOSE;
	typedef	typename alias_collection_type::key_type
						collection_key_type;
	typedef	typename alias_collection_type::iterator
						alias_collection_iterator;
	const key_type lower(l), upper(u);
	if (!value_array.range_check(lower)
			|| !value_array.range_check(upper)) {
		this->type_dump(cerr << "FATAL: reference to ") <<
			" with out-of-bounds indices " << lower << ":" << upper
			<< endl;
		return bad_bool(true);
	}
	// else we know that every key is valid (since whole range is valid)
	key_generator_type key_gen(lower, upper);
	key_gen.initialize();
	alias_collection_iterator a_iter(a.begin());
//	const const_iterator collection_end(this->value_array.end());
	do {
		const instance_alias_info_ptr_type
			pi(&const_cast<instance_alias_info_type&>(
				value_array[key_gen]));
		NEVER_NULL(pi);
		INVARIANT(pi->valid());	// port must already be instantiated
		*a_iter = pi;
		++a_iter;
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	INVARIANT(a_iter == a.end());
	return bad_bool(false);
}	// end method unroll_aliases

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
good_bool
PORT_FORMAL_ARRAY_CLASS::connect_port_aliases_recursive(
		physical_instance_collection& p) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	INVARIANT(this->value_array.size() == t.value_array.size());
	iterator i(this->begin());
	iterator j(t.begin());
	const iterator e(this->value_array.end());
	for ( ; i!=e; ++i, ++j) {
		// unfortunately, set iterators only return const refs
		// we only intend to modify the value without modifying the key
		element_type& ii(*i);
		element_type& jj(*j);
		// possibly redundant port type checking is unnecessary
		if (!instance_alias_info_type::checked_connect_port(
				ii, jj).good) {
			// error message?
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
good_bool
PORT_FORMAL_ARRAY_CLASS::create_dependent_types(const footprint& top) {
	STACKTRACE_VERBOSE;
	iterator i(this->begin());
	const iterator e(this->end());
if (i == e) {
	// no instances in this collection were instantiated (conditional)
	return good_bool(true);
}
if (this->has_relaxed_type()) {
	for ( ; i!=e; i++) {
		if (!element_type::create_dependent_types(*i, top).good)
			return good_bool(false);
		element_type& ii(*i);
		// this call will deduce the canonical type from ii
		if (!internal_alias_policy::connect(ii).good) {
			return good_bool(false);
		}
	}
} else {
	// type of container is already strict, 
	// evaluate it once and re-use it when replaying internal aliases
	const typename parent_type::instance_collection_parameter_type
		t(collection_type_manager_parent_type::__get_raw_type());
	if (!create_definition_footprint(t, top).good) {
		return good_bool(false);
	}
	for ( ; i!=e; i++) {
		element_type& ii(*i);
		if (!internal_alias_policy::connect(ii, t).good) {
			return good_bool(false);
		}
	}
}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::collect_port_aliases(port_alias_tracker& t) const {
	STACKTRACE_VERBOSE;
	// TODO fix const_cast
	const_iterator i(this->begin());
	const const_iterator e(this->end());
	for ( ; i!=e; i++) {
		element_type& ii(const_cast<element_type&>(*i));
		INVARIANT(ii.instance_index);
		// 0 is not an acceptable index
		t.template get_id_map<Tag>()[ii.instance_index]
			.push_back(never_ptr<element_type>(&ii));
#if RECURSE_COLLECT_ALIASES
		ii.collect_port_aliases(t);
#else
		// no need to recurse because pool_manager visits
		// every instance collection already
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
typename PORT_FORMAL_ARRAY_CLASS::instance_alias_info_type&
PORT_FORMAL_ARRAY_CLASS::load_reference(istream& i) {
	STACKTRACE_VERBOSE;
	size_t index;		// 1-indexed
	read_value(i, index);
//	return this->value_array.find(this->value_array.index_to_key(index));
	INVARIANT(index);
	return *(this->begin() +(index -1));	// array-access
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::construct_port_context(port_collection_context& pcc, 
		const footprint_frame& ff) const {
	STACKTRACE_VERBOSE;
	const_iterator i(this->begin());
	const const_iterator e(this->end());
	pcc.resize(this->collection_size());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		i->construct_port_context(pcc, ff, j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::assign_footprint_frame(footprint_frame& ff, 
		const port_collection_context& pcc) const {
	STACKTRACE_VERBOSE;
	INVARIANT(this->collection_size() == pcc.size());
	const_iterator i(this->begin());
	const const_iterator e(this->end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		i->assign_footprint_frame(ff, pcc, j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
good_bool
PORT_FORMAL_ARRAY_CLASS::set_alias_connection_flags(const unsigned char f) {
	return for_each(this->begin(), this->end(),
		typename element_type::connection_flag_setter(f)).status;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PROPAGATE_CHANNEL_CONNECTIONS_HIERARCHICALLY
/**
	Since the formal collection is dense and lexicographically ordered
	by index, we can quickly iterate over the formal and actual 
	collections for instantiation-transformation.  
	\pre dependent types are already created
		(and any errors already caught)
		Is currently done in canonical_type::unroll_port_instances()
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::instantiate_actuals_from_formals(
		port_actuals_type& p, const unroll_context& c) const {
	INVARIANT(p.collection_size() == this->collection_size());
#if 0
	if (!create_dependent_types(*c.get_top_footprint()).good) {
		// error message, already have?
		THROW_EXIT;
	}
#endif
	typename port_actuals_type::iterator i(p.begin()), e(p.end());
	const_iterator j(this->begin());
	// only one element to instantiate
	for ( ; i!=e; ++i, ++j) {
		i->instantiate_actual_from_formal(
			never_ptr<const port_actuals_type>(&p), c, *j);
	}
	// propagate actuals from formal to actual
	// propagate direction connection information from formal to actual
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visitor.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::accept(alias_visitor& v) const {
	for_each(this->begin(), this->end(),
		bind2nd_argval(mem_fun_ref(&element_type::accept), v));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
	for_each(this->begin(), this->end(),
		typename parent_type::element_collector(m));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::write_pointer(ostream& o, 
		const collection_pool_bundle_type& pb) const {
	const unsigned char e = collection_traits<this_type>::ENUM_VALUE;
	write_value(o, e);
	const collection_index_entry::index_type index =
		pb.template get_collection_pool<this_type>()
			.lookup_index(*this);
	write_value(o, index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No need to write keys in dense collection!
	Very efficient.  
 */
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::write_object(const footprint& fp, 
		const persistent_object_manager& m, ostream& f) const {
	parent_type::write_object_base(m, f);
	const key_type& k(this->value_array.size());
	value_writer<key_type> write_key(f);
	write_key(k);
	const const_iterator b(this->begin()), e(this->end());
	for_each(b, e, typename parent_type::element_writer(fp, m, f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::write_connections(
		const collection_pool_bundle_type& m, ostream& f) const {
	for_each(this->begin(), this->end(), 
		typename parent_type::connection_writer(m, f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::load_object(footprint& fp, 
		const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(fp, m, f);
	// placeholder MUST already be loaded now, can use its key
	fp.register_collection_map_entry(
		this->source_placeholder->get_footprint_key(),
		lookup_collection_pool_index_entry(
		fp.template get_instance_collection_pool_bundle<Tag>()
			.template get_collection_pool<this_type>(), *this));
{
	key_type k;
	value_reader<key_type> read_key(f);
	read_key(k);
	this->value_array.resize(k);
}
	const iterator b(this->begin()), e(this->end());
	for_each(b, e, typename parent_type::element_loader(
			fp, m, f, never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_FORMAL_ARRAY_TEMPLATE_SIGNATURE
void
PORT_FORMAL_ARRAY_CLASS::load_connections(
		const collection_pool_bundle_type& m, istream& f) {
	for_each(this->begin(), this->end(), 
		typename parent_type::connection_loader(m, f));
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PORT_FORMAL_ARRAY_TCC__

