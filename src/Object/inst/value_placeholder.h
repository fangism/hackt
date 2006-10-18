/**
	\file "Object/inst/value_placeholder.h"
	Parameter instance placeholder classes for HAC.  
	$Id: value_placeholder.h,v 1.2 2006/10/18 01:19:42 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_VALUE_PLACEHOLDER_H__
#define	__HAC_OBJECT_INST_VALUE_PLACEHOLDER_H__

#include <iosfwd>
#include "util/string_fwd.h"
#include "util/STL/list_fwd.h"
#include "util/boolean_types.h"
// #include "Object/inst/instance_placeholder_base.h"	// for macros
#include "Object/inst/param_value_placeholder.h"
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
#define	POOL_ALLOCATE_VALUE_PLACEHOLDERS			1

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
#define	VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE				\
template <class Tag>

#define VALUE_PLACEHOLDER_CLASS						\
value_placeholder<Tag>

/**
	Value placeholder class template.  
	This class is final, no virtual functions here, no derivatives.  
 */
VALUE_PLACEHOLDER_TEMPLATE_SIGNATURE
class value_placeholder :
	public class_traits<Tag>::value_placeholder_parent_type {
// friend class simple_meta_instance_reference<Tag>;
friend class simple_meta_value_reference<Tag>;
public:
	typedef	class_traits<Tag>		traits_type;
private:
	typedef	VALUE_PLACEHOLDER_CLASS		this_type;
	typedef	typename traits_type::value_placeholder_parent_type
						parent_type;
	FRIEND_PERSISTENT_TRAITS
public:
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::value_collection_generic_type
					value_collection_generic_type;
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

	typedef typename traits_type::instantiation_statement_type
					initial_instantiation_statement_type;
#if REF_COUNT_INSTANCE_MANAGEMENT
	typedef	count_ptr<const initial_instantiation_statement_type>
#else
	typedef	never_ptr<const initial_instantiation_statement_type>
#endif
				initial_instantiation_statement_ptr_type;
	typedef	typename traits_type::value_reference_collection_type
					value_reference_collection_type;
protected:
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

	initial_instantiation_statement_ptr_type
					initial_instantiation_statement_ptr;

protected:
	value_placeholder();

	explicit
	value_placeholder(const size_t d);

	value_placeholder(const this_type& t, const footprint& f);

private:
	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO;

public:
	value_placeholder(const scopespace& o, const string& n, 
		const size_t d);

	~value_placeholder();

	value_collection_generic_type*
	make_collection(void) const;

	ostream&
	what(ostream& o) const;

	ostream&
	type_dump(ostream& o) const;

	void
	attach_initial_instantiation_statement(
#if REF_COUNT_INSTANCE_MANAGEMENT
		const count_ptr<const instantiation_statement_base>& i
#else
		const initial_instantiation_statement_ptr_type i
#endif
		);

	index_collection_item_ptr_type
	get_initial_instantiation_indices(void) const;

#if 0
virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;
#endif
	bool
	is_loop_variable(void) const;

	ostream&
	dump_formal(ostream&, const unroll_context&) const;

	ostream&
	dump_formal(ostream&) const;

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

	count_ptr<meta_value_reference_base>
	make_meta_value_reference(void) const;

	good_bool
	initialize(const init_arg_type& e);

	good_bool
	assign_default_value(const count_ptr<const param_expr>& p);

	count_ptr<const param_expr>
	default_value(void) const;

	count_ptr<const expr_type>
	initial_value(void) const;

	good_bool
	may_type_check_actual_param_expr(const param_expr&) const;

	good_bool
	must_type_check_actual_param_expr(const const_param&,
		const unroll_context&) const;

#if 0
virtual	good_bool
	instantiate_indices(const const_range_list& i) = 0;
#endif

// possibly DEPRECATED
#define	LOOKUP_VALUE_INDEXED_PROTO					\
	good_bool							\
	lookup_value(value_type& v, const multikey_index_type& i, 	\
		const unroll_context&) const

// virtual	LOOKUP_VALUE_INDEXED_PROTO = 0;
	LOOKUP_VALUE_INDEXED_PROTO;
	// need methods for looking up dense sub-collections of values?
	// what should they return?

#if RESOLVE_VALUES_WITH_FOOTPRINT
	good_bool
	unroll_assign_formal_parameter(const unroll_context&, 
		const count_ptr<const param_expr>&) const;
#endif

#if 0
virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

#define	UNROLL_LVALUE_REFERENCES_PROTO					\
	bad_bool							\
	unroll_lvalue_references(const multikey_index_type&, 		\
		const multikey_index_type&, 				\
		value_reference_collection_type&) const

virtual	UNROLL_LVALUE_REFERENCES_PROTO = 0;
#endif

protected:
	// never call!
	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const;

#if 0
public:
	static
	// won't be this_type anymore!
	value_collection_generic_type*
	make_array(const scopespace& o, const string& n, const size_t d);
#endif
public:
	void
	collect_transient_info(persistent_object_manager& m) const;

protected:
	void
	write_object(const persistent_object_manager& m, ostream& o) const;

	void
	load_object(const persistent_object_manager& m, istream& i);

	// subclasses are responsible for implementing:
	// write_object and load_object.
};	// end class value_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef	LOOKUP_VALUE_INDEXED_PROTO
#undef	UNROLL_LVALUE_REFERENCES_PROTO

#endif	// __HAC_OBJECT_INST_VALUE_PLACEHOLDER_H__

