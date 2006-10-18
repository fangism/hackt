/**
	\file "Object/inst/value_collection.h"
	Parameter instance collection classes for HAC.  
	This file was "Object/art_object_value_collection.h"
		in a previous life.  
	$Id: value_collection.h,v 1.18 2006/10/18 01:19:41 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_VALUE_COLLECTION_H__
#define	__HAC_OBJECT_INST_VALUE_COLLECTION_H__

#include <iosfwd>
#include "util/string_fwd.h"
#include "util/STL/list_fwd.h"
#include "util/boolean_types.h"
#include "Object/inst/instance_collection_base.h"	// for macros
#include "Object/common/multikey_index.h"
#include "Object/traits/class_traits_fwd.h"

#include "util/memory/count_ptr.h"
#include "util/inttypes.h"
#include "util/persistent_fwd.h"
#include "util/new_functor_fwd.h"
#include "util/multikey_fwd.h"
#include "util/multikey_qmap_fwd.h"
#include "util/memory/chunk_map_pool_fwd.h"

/**
	Define to 1 to enable pool-allocations of value-arrays and scalars.  
 */
#define	POOL_ALLOCATE_VALUE_COLLECTIONS			1

namespace HAC {
namespace entity {
template <class>
class simple_meta_value_reference;
// template <class>
// class simple_meta_instance_reference;
class meta_instance_reference_base;
class meta_value_reference_base;
class nonmeta_instance_reference_base;
class fundamental_type_reference;
class param_type_reference;
class param_expr;
class const_param;
class const_range_list;
class const_index_list;
class scopespace;
class unroll_context;
class param_value_placeholder;
template <class> class param_instantiation_statement;
using std::list;
using std::istream;
using std::ostream;
using std::string;
using util::memory::count_ptr;	// for experimental pointer classes
using util::qmap;
using util::default_multikey_map;
using util::bad_bool;
using util::good_bool;
using util::persistent;
using util::persistent_object_manager;


//=============================================================================
#define	VALUE_COLLECTION_TEMPLATE_SIGNATURE				\
template <class Tag>

#define VALUE_COLLECTION_CLASS						\
value_collection<Tag>

/**
	Value collection class template.  
 */
VALUE_COLLECTION_TEMPLATE_SIGNATURE
class value_collection :
	public class_traits<Tag>::value_collection_parent_type {
// friend class simple_meta_instance_reference<Tag>;
friend class simple_meta_value_reference<Tag>;
public:
	typedef	class_traits<Tag>		traits_type;
private:
	typedef	VALUE_COLLECTION_CLASS		this_type;
	typedef	typename traits_type::value_collection_parent_type
						parent_type;
public:
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::simple_meta_value_reference_type
					simple_meta_value_reference_type;
	typedef	typename traits_type::simple_nonmeta_instance_reference_type
					simple_nonmeta_instance_reference_type;
	typedef	typename traits_type::expr_base_type
						expr_type;
	typedef	typename traits_type::const_expr_type
						const_expr_type;
	typedef	typename traits_type::const_collection_type
						const_collection_type;
	typedef	count_ptr<const expr_type>	init_arg_type;

#if USE_INSTANCE_PLACEHOLDERS
	// TODO: rename me!
	typedef	typename traits_type::instance_placeholder_type
						value_placeholder_type;
	typedef	never_ptr<const value_placeholder_type>	value_placeholder_ptr_type;
#endif

	typedef typename traits_type::instantiation_statement_type
					initial_instantiation_statement_type;
	typedef	never_ptr<const initial_instantiation_statement_type>
				initial_instantiation_statement_ptr_type;
	typedef	typename traits_type::value_reference_collection_type
					value_reference_collection_type;
protected:
#if USE_INSTANCE_PLACEHOLDERS
	/**
		This is a back-reference to the placeholder that resides in the 
		enclosing scopespace, that contains the basic collection information, 
		prior to unrolling.  
	 */
	value_placeholder_ptr_type	source_placeholder;
#endif
	/**
		TODO: 20060214: eliminate static initial value analysis?

		Expression or value with which parameter is initialized. 
		Recall that parameters are static -- written once only.  
		Not to be used by the hash_string.  
		In the formals context of a template signature, 
		ival is to be interpreted as a default value, in the 
		case where one is not supplied.  
		Or should this be never deleted? cache-owned expressions?
		Screw the cache.  
		Only applicable for simple instances.  
		Collectives won't be checked until unroll time.  
	 */
	count_ptr<const expr_type>		ival;

#if !USE_INSTANCE_PLACEHOLDERS
	initial_instantiation_statement_ptr_type
					initial_instantiation_statement_ptr;
#endif
protected:
#if USE_INSTANCE_PLACEHOLDERS
	value_collection();

	explicit
	value_collection(const value_placeholder_ptr_type);
#else
	explicit
	value_collection(const size_t d);

	value_collection(const this_type& t, const footprint& f);
#endif

private:
#if !USE_INSTANCE_PLACEHOLDERS
virtual	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO = 0;

public:
	value_collection(const scopespace& o, const string& n, 
		const size_t d);
#endif
public:

virtual	~value_collection();

virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	type_dump(ostream& o) const;

#if USE_INSTANCE_PLACEHOLDERS
	never_ptr<const param_value_placeholder>
	get_placeholder_base(void) const;

	value_placeholder_ptr_type
	get_placeholder(void) const {
		return this->source_placeholder;
	}

#if 0
	const string&
	get_name(void) const;

	never_ptr<const scopespace>
	get_owner(void) const;

	size_t
	get_dimensions(void) const;
#endif
#endif
#if !USE_INSTANCE_PLACEHOLDERS
	void
	attach_initial_instantiation_statement(
		const initial_instantiation_statement_ptr_type i) {
		NEVER_NULL(i);
		if (!initial_instantiation_statement_ptr) {
			initial_instantiation_statement_ptr = i;
		}
	}

	index_collection_item_ptr_type
	get_initial_instantiation_indices(void) const;
#endif

virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

#if !USE_INSTANCE_PLACEHOLDERS
	ostream&
	dump_formal(ostream&, const unroll_context&) const;

	ostream&
	dump_formal(ostream&) const;
#endif

protected:
	using parent_type::dump;

public:
	ostream&
	dump(ostream&, const dump_flags&) const;

	// PROBLEM: built-in? needs to be consistent
	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const;

	count_ptr<const param_type_reference>
	get_param_type_ref(void) const;

#if !USE_INSTANCE_PLACEHOLDERS
	count_ptr<meta_value_reference_base>
	make_meta_value_reference(void) const;
#endif

#if !USE_INSTANCE_PLACEHOLDERS
	good_bool
	initialize(const init_arg_type& e);

	good_bool
	assign_default_value(const count_ptr<const param_expr>& p);

	count_ptr<const param_expr>
	default_value(void) const;

	count_ptr<const expr_type>
	initial_value(void) const;
#endif

	good_bool
	may_type_check_actual_param_expr(const param_expr&) const;

	good_bool
	must_type_check_actual_param_expr(const const_param&,
		const unroll_context&) const;

virtual	good_bool
	instantiate_indices(const const_range_list& i) = 0;

// possibly DEPRECATED
#if USE_INSTANCE_PLACEHOLDERS
// is resolved by context elsewhere, now that we have placeholders
#define	LOOKUP_VALUE_INDEXED_PROTO					\
	good_bool							\
	lookup_value(value_type& v, const multikey_index_type& i) const
#else
#define	LOOKUP_VALUE_INDEXED_PROTO					\
	good_bool							\
	lookup_value(value_type& v, const multikey_index_type& i, 	\
		const unroll_context&) const
#endif

virtual	LOOKUP_VALUE_INDEXED_PROTO = 0;
	// need methods for looking up dense sub-collections of values?
	// what should they return?

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

#define	UNROLL_LVALUE_REFERENCES_PROTO					\
	bad_bool							\
	unroll_lvalue_references(const multikey_index_type&, 		\
		const multikey_index_type&, 				\
		value_reference_collection_type&) const

virtual	UNROLL_LVALUE_REFERENCES_PROTO = 0;

#if !USE_INSTANCE_PLACEHOLDERS
protected:
	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const;
#endif

public:
#if USE_INSTANCE_PLACEHOLDERS
	static
	this_type*
	make_array(const value_placeholder_ptr_type);
#else
	static
	this_type*
	make_array(const scopespace& o, const string& n, const size_t d);
#endif
public:
	void
	collect_transient_info(persistent_object_manager& m) const;

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

	// subclasses are responsible for implementing:
	// write_object and load_object.
};	// end class value_collection

//-----------------------------------------------------------------------------
#define VALUE_ARRAY_TEMPLATE_SIGNATURE					\
template <class Tag, size_t D>

#define	VALUE_ARRAY_CLASS						\
value_array<Tag,D>

/**
	Dimension-specific array of parameters.
 */
VALUE_ARRAY_TEMPLATE_SIGNATURE
class value_array : public value_collection<Tag> {
private:
	typedef VALUE_ARRAY_CLASS			this_type;
	typedef	value_collection<Tag>			parent_type;
friend class value_collection<Tag>;
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::instance_type	element_type;

	// later change this to multikey_set or not?
	/// Type for actual values, including validity and status.
private:
	typedef	default_multikey_map<D, pint_value_type, element_type>
							__helper_map_type;
	typedef	typename __helper_map_type::template rebind_default_map_type<
				util::default_qmap>::type
							qmap_type;
public:
	typedef	util::multikey_map<D, pint_value_type, element_type, qmap_type>
							collection_type;
	typedef	typename collection_type::key_type	key_type;
	typedef	typename traits_type::const_collection_type
							const_collection_type;
	typedef	typename traits_type::value_reference_collection_type
					value_reference_collection_type;
#if USE_INSTANCE_PLACEHOLDERS
	typedef	typename parent_type::value_placeholder_type
							value_placeholder_type;
	typedef typename parent_type::value_placeholder_ptr_type
						value_placeholder_ptr_type;
#endif
private:
	/// the collection of boolean instances
	collection_type					collection;
	// value cache is not persistent
	const_collection_type				cached_values;
	// tracking validity and density of the value cache?

	value_array();

#if !USE_INSTANCE_PLACEHOLDERS
	value_array(const this_type&, const footprint&);

	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO;

public:
	value_array(const scopespace& o, const string& n);
#else
public:
	explicit
	value_array(const value_placeholder_ptr_type);
#endif
	~value_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

#if !USE_INSTANCE_PLACEHOLDERS
	bool
	is_loop_variable(void) const;
#endif

	ostream&
	dump_unrolled_values(ostream& o) const;

	// update this to accept const_range_list instead
	good_bool
	instantiate_indices(const const_range_list&);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	LOOKUP_VALUE_INDEXED_PROTO;

	UNROLL_LVALUE_REFERENCES_PROTO;

	/// helper functor for dumping values
	struct key_value_dumper {
		ostream& os;
		key_value_dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const typename collection_type::value_type&);
	};      // end struct key_value_dumper

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
#if POOL_ALLOCATE_VALUE_COLLECTIONS
	enum {
#ifdef	HAVE_UINT64_TYPE
		pool_chunk_size = 64
#else
		pool_chunk_size = 32
#endif
	};
	CHUNK_MAP_POOL_ROBUST_STATIC_DECLARATIONS(pool_chunk_size)
#endif
};	// end class value_array

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	VALUE_SCALAR_TEMPLATE_SIGNATURE					\
template <class Tag>

#define	VALUE_SCALAR_CLASS						\
value_array<Tag,0>

/**
	Specialization of scalar parameter.
 */
VALUE_SCALAR_TEMPLATE_SIGNATURE
class VALUE_SCALAR_CLASS : public value_collection<Tag> {
private:
	typedef	value_collection<Tag>			parent_type;
	typedef	VALUE_SCALAR_CLASS			this_type;
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::instance_type	instance_type;
	typedef	instance_type				element_type;
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::expr_base_type	expr_type;
	typedef	typename traits_type::const_expr_type	const_expr_type;
	typedef	typename traits_type::value_reference_collection_type
					value_reference_collection_type;
#if USE_INSTANCE_PLACEHOLDERS
	typedef	typename parent_type::value_placeholder_type
							value_placeholder_type;
	typedef typename parent_type::value_placeholder_ptr_type
						value_placeholder_ptr_type;
#endif
private:
	instance_type					the_instance;
	const_expr_type					cached_value;
	bool						cache_validity;
public:
	value_array();

#if USE_INSTANCE_PLACEHOLDERS
	explicit
	value_array(const value_placeholder_ptr_type);
#else
	value_array(const scopespace& o, const string& n);

private:
	value_array(const this_type&, const footprint&);

	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO;
#endif
public:
	~value_array();

#if USE_INSTANCE_PLACEHOLDERS
	instance_type&
	get_instance(void) { return this->the_instance; }

	const instance_type&
	get_instance(void) const { return this->the_instance; }
#endif

	ostream&
	what(ostream& ) const;

	bool
	is_partially_unrolled(void) const;

#if !USE_INSTANCE_PLACEHOLDERS
	bool
	is_loop_variable(void) const;
#endif

	ostream&
	dump_unrolled_values(ostream& o) const;

	good_bool
	lookup_value(value_type& i
#if !USE_INSTANCE_PLACEHOLDERS
		, const unroll_context&
#endif
		) const;

// there are implemented to do nothing but sanity check, 
// since it doesn't even make sense to call these.  
	// update this to accept a const_range_list
	good_bool
	instantiate_indices(const const_range_list&);

	LOOKUP_VALUE_INDEXED_PROTO;
	// need methods for looking up dense sub-collections of values?
	// what should they return?

	UNROLL_LVALUE_REFERENCES_PROTO;

	const_index_list
	resolve_indices(const const_index_list& l) const;

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
#if POOL_ALLOCATE_VALUE_COLLECTIONS
	enum {
#ifdef	HAVE_UINT64_TYPE
		pool_chunk_size = 64
#else
		pool_chunk_size = 32
#endif
	};
	CHUNK_MAP_POOL_ROBUST_STATIC_DECLARATIONS(pool_chunk_size)
#endif
};	// end class value_array specialization

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef	LOOKUP_VALUE_INDEXED_PROTO
#undef	UNROLL_LVALUE_REFERENCES_PROTO

#endif	// __HAC_OBJECT_INST_VALUE_COLLECTION_H__

