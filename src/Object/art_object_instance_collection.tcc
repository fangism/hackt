/**
	\file "art_object_instance_int.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	TODO: replace duplicate managed code with templates.
	$Id: art_object_instance_collection.tcc,v 1.1.2.1 2005/02/23 21:12:37 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_COLLECTION_TCC__
#define	__ART_OBJECT_INSTANCE_COLLECTION_TCC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_alias.h"
#include "art_object_instance_collection.h"

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"

// #include "multikey_qmap.tcc"
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
#if 0
// reserve these statements for type-specific translation unit
STATIC_TRACE_BEGIN("instance-int")

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_instance_collection, 
		DINT_INSTANCE_COLLECTION_TYPE_KEY)
}	// end namespace util
#endif


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
	if (instance)
		this->instance->collect_transient_info(m);
	if (container)
		this->container->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::dump_alias(ostream& o) const {
	DIE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::write_next_connection(
		const persistent_object_manager&, ostream&) const {
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
	const typename INSTANCE_ALIAS_INFO_CLASS::instance_alias_base& i) {
	return o << "int-alias @ " << &i;
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
	m.write_pointer(o, this->container);
#if 0
	value_writer<key_type> kw(os);
	kw(e.key);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("int_alias::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(m);
	// next->collect_transient_info_base(m)?        CYCLE!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 */
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT("int_alias::write_object()");
#if 0
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
	value_reader<key_type> kr(os);
	kr(e.key);
#endif
	INSTANCE_ALIAS_INFO_CLASS::load_object_base(m, i);
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
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("int_alias<0>::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(m);
	// next->collect_transient_info_base(m)?        CYCLE!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT("int_alias<0>::write_object()");
	INSTANCE_ALIAS_INFO_CLASS::write_object_base(m, o);
	// no key to write!
	// continuation pointer?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT("int_alias<0>::load_object()");
	INSTANCE_ALIAS_INFO_CLASS::load_object_base(m, i);
	// no key to load!
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
	STACKTRACE("~int_instance_collection()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
ostream&
INSTANCE_COLLECTION_CLASS::type_dump(ostream& o) const {
	return o << "int<" << type_parameter << ">^" << dimensions;
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
#if 0
	// this action is specialized!
	STACKTRACE("int_instance_collection::commit_type()");
	INVARIANT(t->get_base_def() == &int_def);
	const never_ptr<const param_expr_list>
		params(t->get_template_params());
	NEVER_NULL(params);
	// extract first and only parameter, the integer width
	const never_ptr<const const_param_expr_list>
		cparams(params.is_a<const const_param_expr_list>());
	NEVER_NULL(cparams);
	INVARIANT(cparams->size() == 1);
	const count_ptr<const const_param>&
		param1(cparams->front());
	NEVER_NULL(param1);
	const count_ptr<const pint_const>
		pwidth(param1.is_a<const pint_const>());
	NEVER_NULL(pwidth);
	const pint_value_type new_width = pwidth->static_constant_int();
	INVARIANT(new_width);
	if (is_partially_unrolled()) {
		INVARIANT(int_width);
		return (new_width != int_width);
	} else {
		int_width = new_width;
		return false;
	}
#endif
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
INSTANCE_COLLECTION_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	parent_type::write_object_base(m, o);
	// USE value_writer...
#if 0
	write_value(o, int_width);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	parent_type::load_object_base(m, i);
	// USE value_reader
#if 0
	read_value(i, int_width);
#endif
}

//=============================================================================
// class instance_alias method definitions

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o, const instance_alias<Tag,D>& b) {
	INVARIANT(b.valid());
	return o << "(int-alias-" << D << ")";
}

//=============================================================================
// class array method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::array() : parent_type(D), collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::~array() { }

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
	return o << "int-array<" << D << ">";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::dump_unrolled_instances(ostream& o) const {
	for_each(collection.begin(), collection.end(), key_dumper(o));
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
typename INSTANCE_ARRAY_CLASS::instance_ptr_type
INSTANCE_ARRAY_CLASS::lookup_instance(const multikey_index_type& i) const {
	INVARIANT(D == i.dimensions());
	const key_type index(i);
	const const_iterator it(this->collection.find(index));
	if (it == collection.end()) {
		cerr << "ERROR: reference to uninstantiated int " <<
			get_qualified_name() << " at index: " << i << endl;
		return instance_ptr_type(NULL);
	}
	const element_type& b(*it);
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return instance_ptr_type(const_cast<element_type*>(&b));
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
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	INVARIANT(!r.empty());
//	multikey_generator<D, pvalue_type> key_gen;
	key_generator_type key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const const_iterator it(this->collection.find(key_gen));
		if (it == collection.end()) {
			cerr << "FATAL: reference to uninstantiated int index "
				<< key_gen << endl;
			l.push_back(instance_ptr_type(NULL));
			ret = false;
		} else {
		const element_type& pi(*it);
		// pi MUST be valid if it belongs to an array
		if (pi.valid()) {
			l.push_back(instance_ptr_type(
				const_cast<element_type*>(&pi)));
		} else {
			cerr << "FATAL: reference to uninstantiated int index "
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
	} while (key_gen != key_gen.lower_corner);
	INVARIANT(a_iter == a.end());
	return ret;
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
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::connection_writer::operator() (const element_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::connection_writer::operator()");
	const instance_alias_base* const next = e.get_next();
	this->next->write_next_connection(pom, os);
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
#if 0
	collection.write(f);
#else
	// need to know how many members to expect
	write_value(f, this->collection.size());
	for_each(this->collection.begin(), this->collection.end(),
		element_writer(m, f)
	);
#endif
#if 0
	// punting connections...
	for_each(collection.begin(), collection.end(), 
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
#if 0
	collection.read(f);
#else
	size_t collection_size;
	read_value(f, collection_size);
	size_t i = 0;
	for ( ; i < collection_size; i++) {
		// this must perfectly complement element_writer::operator()
		// construct the element locally first, then insert it into set
		key_type temp_key;
		value_reader<key_type> read_key(f);
		read_key(temp_key);
		element_type temp_elem(temp_key);
		temp_elem.load_object_base(m, f);
		this->collection.insert(temp_elem);
	}
#endif
#if 0
	// punting connections...
	i = 0;
	for ( ; i < collection_size; i++) {
		// this must complement connection_writer::operator()
	}
#endif
}

//=============================================================================
// class array method definitions (specialized)

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
	return o << "<?>-scalar";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::dump_unrolled_instances(ostream& o) const {
#if 0
	return o << auto_indent << the_instance << endl;
#else
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
	STACKTRACE("INSTANCE_SCALAR_CLASS::instantiate_indices()");
	INVARIANT(!i);
	if (this->the_instance.valid()) {
		// should never happen, but just in case...
		cerr << "ERROR: Scalar int already instantiated!" << endl;
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
	cerr << "WARNING: instance_array<0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_ptr_type
INSTANCE_SCALAR_CLASS::lookup_instance(const multikey_index_type& i) const {
	if (!this->the_instance.valid()) {
		cerr << "ERROR: Reference to uninstantiated int!" << endl;
		return instance_ptr_type(NULL);
	} else	return instance_ptr_type(
		const_cast<instance_type*>(&this->the_instance));
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
		list<instance_ptr_type>& l, const const_range_list& r) const {
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
		*(a.begin()) = never_ptr<instance_type>(
			const_cast<instance_type*>(&this->the_instance));
		return false;
	} else {
		cerr << "ERROR: Reference to uninstantiated int!" << endl;
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<parent_type>::type_key, 0)) {
	STACKTRACE_PERSISTENT("scalar::collect_transients()");
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
#if 0
	write_value(f, the_instance);
#else
	this->the_instance.write_object(m, f);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);
#if 0
	read_value(f, the_instance);
#else
	this->the_instance.load_object(m, f);
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#if 0
STATIC_TRACE_END("instance-int")
#endif

#endif	// __ART_OBJECT_INSTANCE_COLLECTION_TCC__

