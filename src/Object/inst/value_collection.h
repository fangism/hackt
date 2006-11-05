/**
	\file "Object/inst/value_collection.h"
	Parameter instance collection classes for HAC.  
	This file was "Object/art_object_value_collection.h"
		in a previous life.  
	$Id: value_collection.h,v 1.20.2.4 2006/11/05 01:23:12 fang Exp $
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
#include "Object/devel_switches.h"

#include "util/memory/count_ptr.h"
#include "util/inttypes.h"
#include "util/persistent_fwd.h"

/**
	Define to 1 to enable pool-allocations of value-arrays and scalars.  
 */
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
#define	POOL_ALLOCATE_VALUE_COLLECTIONS			0
#else
#define	POOL_ALLOCATE_VALUE_COLLECTIONS			1
#endif

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
// template <class> class param_instantiation_statement;
using std::list;
using std::istream;
using std::ostream;
using std::string;
using util::memory::count_ptr;	// for experimental pointer classes
using util::bad_bool;
using util::good_bool;
using util::persistent;
using util::persistent_object_manager;


//=============================================================================
// foward declaration of general template
template <class, size_t>
class value_array;

// forward declaration of partial specialization
template <class Tag>
class value_array<Tag,0>;

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

	typedef	typename traits_type::instance_placeholder_type
						value_placeholder_type;
	typedef	never_ptr<const value_placeholder_type>	value_placeholder_ptr_type;

	typedef typename traits_type::instantiation_statement_type
					initial_instantiation_statement_type;
	typedef	never_ptr<const initial_instantiation_statement_type>
				initial_instantiation_statement_ptr_type;
	typedef	typename traits_type::value_reference_collection_type
					value_reference_collection_type;
protected:
	/**
		This is a back-reference to the placeholder that resides in the 
		enclosing scopespace, that contains the basic collection information, 
		prior to unrolling.  
	 */
	value_placeholder_ptr_type	source_placeholder;
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

protected:
	value_collection();

	explicit
	value_collection(const value_placeholder_ptr_type);

#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
public:
#endif
virtual	~value_collection();

public:
virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	type_dump(ostream& o) const;

	never_ptr<const param_value_placeholder>
	get_placeholder_base(void) const;

	value_placeholder_ptr_type
	get_placeholder(void) const {
		return this->source_placeholder;
	}

virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

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

	good_bool
	may_type_check_actual_param_expr(const param_expr&) const;

	good_bool
	must_type_check_actual_param_expr(const const_param&,
		const unroll_context&) const;

virtual	good_bool
	instantiate_indices(const const_range_list& i) = 0;

// possibly DEPRECATED
// is resolved by context elsewhere, now that we have placeholders
#define	LOOKUP_VALUE_INDEXED_PROTO					\
	good_bool							\
	lookup_value(value_type& v, const multikey_index_type& i) const

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

public:
	static
	this_type*
	make_array(const value_placeholder_ptr_type);
public:
	void
	collect_transient_info_base(persistent_object_manager& m) const;

#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	void
	collect_transient_info(persistent_object_manager& m) const;
#endif

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

	// subclasses are responsible for implementing:
	// write_object and load_object.
};	// end class value_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_VALUE_COLLECTION_H__

