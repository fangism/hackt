/**
	\file "art_object_instance_int.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	TODO: replace duplicate managed code with templates.
	$Id: art_object_instance_int.cc,v 1.12.2.5.2.3.2.4 2005/02/26 04:56:44 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_INT_CC__
#define	__ART_OBJECT_INSTANCE_INT_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_int.h"
#include "art_object_inst_ref_data.h"
#include "art_object_member_inst_ref.h"
#include "art_object_expr_const.h"
#include "art_object_connect.h"
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_type_hash.h"
#include "art_built_ins.h"

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
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

// conditional defines, after including "stacktrace.h"
#if !USE_INSTANCE_COLLECTION_TEMPLATE
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

//=============================================================================
STATIC_TRACE_BEGIN("instance-int")

//=============================================================================
// module-local specializations

#if !USE_INSTANCE_COLLECTION_TEMPLATE
namespace std {
using ART::entity::int_instance_alias;

template <size_t D>
struct _Select1st<int_instance_alias<D> > :
	public _Select1st<typename int_instance_alias<D>::parent_type> {
};      // end struct _Select1st

template <size_t D>
struct _Select2nd<int_instance_alias<D> > :
	public _Select2nd<typename int_instance_alias<D>::parent_type> {
};
}	// end namespace std
#endif

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_instance_collection, 
		DINT_INSTANCE_COLLECTION_TYPE_KEY)
#if USE_INSTANCE_COLLECTION_TEMPLATE
	SPECIALIZE_UTIL_WHAT(ART::entity::int_scalar, "int_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_1D, "int_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_2D, "int_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_3D, "int_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_4D, "int_array_4D")
#endif
}	// end namespace util


namespace ART {
namespace entity {
#if !USE_INSTANCE_COLLECTION_TEMPLATE
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
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
// functor specializations
#if USE_INSTANCE_COLLECTION_TEMPLATE
template <>
struct type_dumper<int_tag> {
	typedef class_traits<int_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	ostream& os;
	type_dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		return os << "int<" << c.get_type_parameter() <<
			">^" << c.get_dimensions();
	}
};      // end struct type_dumper<int_tag>

//-----------------------------------------------------------------------------
template <>
struct collection_parameter_persistence<int_tag> {
	typedef class_traits<int_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<int_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;

	static
	void
	collect(persistent_object_manager& m, 
		const instance_collection_generic_type& c) {
		// c.type_parameter contains no pointers
	}

	static
	void
	write(const persistent_object_manager&, ostream& o,
		const instance_collection_generic_type& c) {
		// parameter is just an int
		write_value(o, c.type_parameter);
	}

	static
	void
	load(const persistent_object_manager&, istream& i,
		instance_collection_generic_type& c) {
		// parameter is just an int
		read_value(i, c.type_parameter);
	}

};      // end struct collection_parameter_persistence

//-----------------------------------------------------------------------------

template <>
struct collection_type_committer<int_tag> {
	typedef class_traits<int_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<int_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	/**
		During unroll phase, this commits the type of the collection.  
		\param t the data integer type reference, containing width, 
			must already be resolved to a const_param_expr_list.  
		\return false on success, true on error.  
		\post the integer width is fixed for the rest of the program.  
	 */
	bool
	operator () (instance_collection_generic_type& c,
		const type_ref_ptr_type& t) const {
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
		if (c.is_partially_unrolled()) {
			INVARIANT(c.type_parameter);
			return (new_width != c.type_parameter);
		} else { 
			c.type_parameter = new_width;
			return false;
		}
	}
};      // end struct collection_type_committer
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
// class int_instance_alias_info method definitions

#if !USE_INSTANCE_COLLECTION_TEMPLATE
// inline
int_instance_alias_info::~int_instance_alias_info() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias_info::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (instance)
		instance->collect_transient_info(m);
	if (container)
		container->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias_info::dump_alias(ostream& o) const {
	DIE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias_info::write_next_connection(
		const persistent_object_manager&, ostream&) const {
	DIE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias_info::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, instance);
	m.write_pointer(o, container);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias_info::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, instance);
	m.read_pointer(i, container);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias_info::transient_info_collector::operator () (
		const int_instance_alias_info& i) {
	i.collect_transient_info_base(manager);
}
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
// typedef int_instance_alias_base function definitions

ostream&
operator << (ostream& o, const int_instance_alias_base& i) {
	return o << "int-alias @ " << &i;
}

//=============================================================================
#if !USE_INSTANCE_COLLECTION_TEMPLATE
// class int_instance_alias method definitions

template <size_t D>
int_instance_alias<D>::~int_instance_alias() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Prints out the next instance alias in the connected set.  
 */
template <size_t D>
void
int_instance_alias<D>::dump_alias(ostream& o) const {
	NEVER_NULL(container);
	o << container->get_qualified_name() <<
		multikey<D, pint_value_type>(key);
		// casting to multikey for the sake of printing [i] for D==1.
		// could use specialization to accomplish this...
		// bah, not important
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D>
void
int_instance_alias<D>::write_next_connection(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, container);
#if 0
	value_writer<key_type> kw(os);
	kw(e.key);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D>
void
int_instance_alias<D>::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("bool_alias::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	int_instance_alias_info::collect_transient_info_base(m);
	// next->collect_transient_info_base(m)?        CYCLE!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 */
template <size_t D>
void
int_instance_alias<D>::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT("int_alias::write_object()");
#if 0
	value_writer<key_type> write_key(os);
	write_key(key);
#endif
	int_instance_alias_info::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D>
void
int_instance_alias<D>::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT("int_alias::load_object()");
#if 0
	value_reader<key_type> kr(os);
	kr(e.key);
#endif
	int_instance_alias_info::load_object_base(m, i);
}

//=============================================================================
// class int_instance_alias<0> method definitions

int_instance_alias<0>::~int_instance_alias() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias<0>::dump_alias(ostream& o) const {
	NEVER_NULL(container);
	o << container->get_qualified_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias<0>::write_next_connection(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, container);
	// no key to write!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias<0>::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("int_alias<0>::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	int_instance_alias_info::collect_transient_info_base(m);
	// next->collect_transient_info_base(m)?        CYCLE!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias<0>::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT("int_alias<0>::write_object()");
	int_instance_alias_info::write_object_base(m, o);
	// no key to write!
	// continuation pointer?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_alias<0>::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT("int_alias<0>::load_object()");
	int_instance_alias_info::load_object_base(m, i);
	// no key to load!
}
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
#if !USE_INSTANCE_COLLECTION_TEMPLATE
// class int_instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int_instance_collection::int_instance_collection(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d), int_width(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_instance_collection::~int_instance_collection() {
	STACKTRACE("~int_instance_collection()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_instance_collection::type_dump(ostream& o) const {
	return o << "int<" << int_width << ">^" << dimensions;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	During unroll phase, this commits the type of the collection.  
	\param t the data integer type reference, containing width, 
		must already be resolved to a const_param_expr_list.  
	\return false on success, true on error.  
	\post the integer width is fixed for the rest of the program.  
 */
bool
int_instance_collection::commit_type(const type_ref_ptr_type& t) {
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
count_ptr<instance_reference_base>
int_instance_collection::make_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	return count_ptr<datatype_instance_reference>(
		new int_instance_reference(
			never_ptr<const int_instance_collection>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_instance_collection::member_inst_ref_ptr_type
int_instance_collection::make_member_instance_reference(
		const inst_ref_ptr_type& b) const {
	NEVER_NULL(b);
	return member_inst_ref_ptr_type(
		new int_member_instance_reference(
			b, never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to return a legitmate reference to a parameter list!
 */
never_ptr<const const_param_expr_list>
int_instance_collection::get_actual_param_list(void) const {
	STACKTRACE("int_instance_collection::get_actual_param_list()");
	return never_ptr<const const_param_expr_list>(NULL);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_collection::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	parent_type::write_object_base(m, o);
	write_value(o, int_width);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_collection::load_object_base(
		const persistent_object_manager& m, istream& i) {
	parent_type::load_object_base(m, i);
	read_value(i, int_width);
}

//=============================================================================
// class int_instance_alias method definitions

#if !USE_INSTANCE_COLLECTION_TEMPLATE
template <size_t D>
ostream&
operator << (ostream& o, const int_instance_alias<D>& b) {
	INVARIANT(b.valid());
	return o << "(int-alias-" << D << ")";
}
#endif

//=============================================================================
// class int_array method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
int_array<D>::int_array() : parent_type(D), collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
int_array<D>::int_array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection() {
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
int_array<D>::key_dumper::operator () (const value_type& p) {
	os << auto_indent << _Select1st<value_type>()(p) << " = ";
	p.get_next()->dump_alias(os);
	return os << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates integer parameters at the specified indices.
	\param i fully-specified range of indices to instantiate.
 */
INT_ARRAY_TEMPLATE_SIGNATURE
void
int_array<D>::instantiate_indices(const index_collection_item_ptr_type& i) {
	STACKTRACE("int_array<D>::instantiate_indices()");
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
INT_ARRAY_TEMPLATE_SIGNATURE
const_index_list
int_array<D>::resolve_indices(const const_index_list& l) const {
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
	\return valid int_instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INT_ARRAY_TEMPLATE_SIGNATURE
typename int_array<D>::instance_ptr_type
int_array<D>::lookup_instance(const multikey_index_type& i) const {
	INVARIANT(D == i.dimensions());
	const key_type index(i);
	const const_iterator it(collection.find(index));
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
INT_ARRAY_TEMPLATE_SIGNATURE
bool
int_array<D>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	INVARIANT(!r.empty());
//	multikey_generator<D, pint_value_type> key_gen;
	key_generator_type key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const const_iterator it(collection.find(key_gen));
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
INT_ARRAY_TEMPLATE_SIGNATURE
bool
int_array<D>::unroll_aliases(const multikey_index_type& l, 
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
	const const_iterator collection_end(collection.end());
	// maybe INVARIANT(sizes == iterations)
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
	} while (key_gen != key_gen.lower_corner);
	INVARIANT(a_iter == a.end());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Going to need some sort of element_reader counterpart.
	\param e is a reference to a int_instance_alias<D>.
 */
INT_ARRAY_TEMPLATE_SIGNATURE
void
int_array<D>::element_writer::operator () (const element_type& e) const {
	STACKTRACE_PERSISTENT("int_array<D>::element_writer::operator()");
	value_writer<key_type> write_key(os);
	write_key(e.key);
	e.write_object_base(pom, os);
	// postpone connection writing until next phase
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
void
int_array<D>::connection_writer::operator() (const element_type& e) const {
	STACKTRACE_PERSISTENT("int_array<D>::connection_writer::operator()");
	const int_instance_alias_base* const next = e.get_next();
	next->write_next_connection(pom, os);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
void
int_array<D>::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<parent_type>::type_key, D)) {
	STACKTRACE_PERSISTENT("int_array<D>::collect_transients()");
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INT_ARRAY_TEMPLATE_SIGNATURE
void
int_array<D>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	parent_type::write_object_base(m, f);
#if 0
	collection.write(f);
#else
	// need to know how many members to expect
	write_value(f, collection.size());
	for_each(collection.begin(), collection.end(),
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
INT_ARRAY_TEMPLATE_SIGNATURE
void
int_array<D>::load_object(const persistent_object_manager& m, istream& f) {
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
		collection.insert(temp_elem);
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
// class int_array method definitions (specialized)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_array<0>::int_array() : parent_type(0), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_array<0>::int_array(const scopespace& o, const string& n) :
		parent_type(o, n, 0), the_instance() {
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
ostream&
int_array<0>::what(ostream& o) const {
	return o << "int-scalar";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_array<0>::dump_unrolled_instances(ostream& o) const {
#if 0
	return o << auto_indent << the_instance << endl;
#else
	the_instance.get_next()->dump_alias(o << " = ");
	return o;
#endif
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
	STACKTRACE("int_array<0>::instantiate_indices()");
	INVARIANT(!i);
	if (the_instance.valid()) {
		// should never happen, but just in case...
		cerr << "ERROR: Scalar int already instantiated!" << endl;
		THROW_EXIT;
	}
//	the_instance.instantiate();
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
int_array<0>::lookup_instance(const multikey_index_type& i) const {
	if (!the_instance.valid()) {
		cerr << "ERROR: Reference to uninstantiated int!" << endl;
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
int_array<0>::lookup_instance_collection(
		list<instance_ptr_type>& l, const const_range_list& r) const {
	cerr << "WARNING: int_array<0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error, false on success.
 */
bool
int_array<0>::unroll_aliases(const multikey_index_type& l, 
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
void
int_array<0>::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<parent_type>::type_key, 0)) {
	STACKTRACE_PERSISTENT("int_scalar::collect_transients()");
	parent_type::collect_transient_info_base(m);
	the_instance.collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_array<0>::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	parent_type::write_object_base(m, f);
#if 0
	write_value(f, the_instance);
#else
	the_instance.write_object(m, f);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_array<0>::load_object(const persistent_object_manager& m, istream& f) {
	parent_type::load_object_base(m, f);
#if 0
	read_value(f, the_instance);
#else
	the_instance.load_object(m, f);
#endif
}
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
#if USE_INSTANCE_COLLECTION_TEMPLATE
template class instance_collection<int_tag>;
template class instance_array<int_tag, 0>;
template class instance_array<int_tag, 1>;
template class instance_array<int_tag, 2>;
template class instance_array<int_tag, 3>;
template class instance_array<int_tag, 4>;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

STATIC_TRACE_END("instance-int")

#endif	// __ART_OBJECT_INSTANCE_INT_CC__

