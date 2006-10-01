/**
	\file "Object/inst/instance_placeholder.h"
	Instance placeholders are used to represent instantiated collections
	that actually reside in footprints and other allocated locations.  
	$Id: instance_placeholder.h,v 1.1.2.9 2006/10/01 21:14:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_H__
#define	__HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_H__

#include <iosfwd>
#include <set>

#include "Object/type/canonical_type_fwd.h"	// for conditional
#include "Object/traits/class_traits_fwd.h"
#include "Object/inst/physical_instance_placeholder.h"	// for macros
// #include "Object/inst/instance_placeholder_base.h"
#include "Object/common/multikey_index.h"
#include "util/STL/list_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"
#include "util/multikey_set.h"
#include "util/boolean_types.h"
#include "util/memory/chunk_map_pool_fwd.h"
#include "util/inttypes.h"

/**
	Define to 1 if you want instance_arrays and scalars pool-allocated.  
	Causes regression in one strange test case, needs debugging.  
 */
#define	POOL_ALLOCATE_INSTANCE_PLACEHOLDERS		1

namespace HAC {
namespace entity {
using std::list;
using std::default_list;
using std::istream;
using std::ostream;
using std::set;
using std::string;
using util::memory::count_ptr;
using util::memory::never_ptr;
using util::good_bool;
using util::bad_bool;
using util::default_multikey_set;
using util::multikey_set_element_derived;
using util::persistent;
using util::persistent_object_manager;

class scopespace;
class footprint;
class physical_instance_placeholder;
class meta_instance_reference_base;
class nonmeta_instance_reference_base;
class const_index_list;
class const_range_list;
class const_param_expr_list;
class unroll_context;
class subinstance_manager;
template <bool> class internal_aliases_policy;
template <class> class instantiation_statement;

//=============================================================================
#define	INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	INSTANCE_PLACEHOLDER_CLASS					\
instance_placeholder<Tag>

/**
	Interface to collection of instance aliases.  
	This abstract base class is dimension-generic.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
class instance_placeholder :
	public class_traits<Tag>::instance_placeholder_parent_type
#if 0
	public class_traits<Tag>::collection_type_manager_parent_type
#endif
{
public:
	typedef	class_traits<Tag>			traits_type;
private:
	typedef	Tag					category_type;
	typedef	typename traits_type::instance_placeholder_parent_type
							parent_type;
	typedef	INSTANCE_PLACEHOLDER_CLASS		this_type;
	FRIEND_PERSISTENT_TRAITS
public:
	typedef	typename traits_type::type_ref_type	type_ref_type;
	typedef	typename traits_type::type_ref_ptr_type	type_ref_ptr_type;
// ? do placeholders need type-managers?
#if 0
	typedef	typename traits_type::collection_type_manager_parent_type
					collection_type_manager_parent_type;
#endif
	// placeholders need not know about instance aliases
#if 0
	typedef	typename traits_type::instance_alias_info_type
						instance_alias_info_type;
	typedef	typename traits_type::instance_alias_base_type
						instance_alias_base_type;
	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;
	typedef	typename traits_type::alias_collection_type
						alias_collection_type;
#endif
#if 0
	typedef	typename traits_type::instance_placeholder_parameter_type
					instance_placeholder_parameter_type;
#endif
	typedef	typename traits_type::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
	typedef	typename traits_type::simple_nonmeta_instance_reference_type
					simple_nonmeta_instance_reference_type;
	typedef	typename traits_type::member_simple_meta_instance_reference_type
				member_simple_meta_instance_reference_type;
//	typedef	meta_instance_reference_base		meta_instance_reference_base_type;
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
// public:
protected:
	typedef	typename parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	typename parent_type::member_inst_ref_ptr_type
						member_inst_ref_ptr_type;
#if 0
	typedef	typename parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
	// type parameter, if applicable is inherited from
	// collection_type_manager_parent_type
	typedef	internal_aliases_policy<traits_type::can_internally_alias>
						internal_alias_policy;
#endif
public:
	typedef	typename traits_type::instantiation_statement_type
					initial_instantiation_statement_type;
	typedef	never_ptr<const initial_instantiation_statement_type>
				initial_instantiation_statement_ptr_type;
protected:
	/**
		All collections track the first instantiation statement,
		for the sake of deducing the type.  
		Scalars instance collections need this too because
		of the possibility of relaxed template arguments.  
	 */
	initial_instantiation_statement_ptr_type
					initial_instantiation_statement_ptr;
protected:
	instance_placeholder();

	explicit
	instance_placeholder(const size_t d) :
		parent_type(d), 
#if 0
		collection_type_manager_parent_type(),
#endif
		initial_instantiation_statement_ptr(NULL) { }

	instance_placeholder(const this_type&, const footprint&);

private:
	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO;

public:
	instance_placeholder(const scopespace& o, const string& n, 
		const size_t d);

	~instance_placeholder();

	instance_collection_generic_type*
	make_collection(void) const;

	ostream&
	what(ostream&) const;

	ostream&
	type_dump(ostream&) const;

	ostream&
	dump_formal(ostream&) const;

	void
	attach_initial_instantiation_statement(
		const initial_instantiation_statement_ptr_type i) {
		NEVER_NULL(i);
		if (!this->initial_instantiation_statement_ptr)
			this->initial_instantiation_statement_ptr = i;
		// else skip
	}

	/**
		CAVEAT: this statement could be conditional.  
	 */
	initial_instantiation_statement_ptr_type
	get_initial_instantiation_statement(void) const {
		return this->initial_instantiation_statement_ptr;
	}

	index_collection_item_ptr_type
	get_initial_instantiation_indices(void) const;

	// inappropriate for placeholders
#if 0
virtual	bool
	is_partially_unrolled(void) const = 0;
#endif

	// this could just return hard-coded built-in type...
	// this returns the type as given by the first instantiation statement
	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const;

	// same thing, but covariant return type
	type_ref_ptr_type
	get_unresolved_type_ref_subtype(void) const;

	bool
	must_be_collectibly_type_equivalent(const this_type&) const;

	bool
	has_relaxed_type(void) const;

	// inappropriate for placeholders
#if 0
	// 2005-07-07: intended for first-time type establishment, 
	// which determines whether or not the collection is relaxed or 
	// strictly typed.  
	good_bool
	establish_collection_type(const instance_placeholder_parameter_type&);


	// 2005-07-07: now intended for use AFTER collection type is established
	bad_bool
	check_established_type(
		const instance_placeholder_parameter_type&) const;
#endif

	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const;

	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const;

	member_inst_ref_ptr_type
	make_member_meta_instance_reference(const inst_ref_ptr_type&) const;

#if 0
/**
	Prototype for instantiating alias indices during unroll phase.  
 */
#define	INSTANTIATE_INDICES_PROTO					\
	good_bool							\
	instantiate_indices(const const_range_list& i, 			\
		const instance_relaxed_actuals_type&, 			\
		const unroll_context&)

virtual	INSTANTIATE_INDICES_PROTO = 0;

virtual	CONNECT_PORT_ALIASES_RECURSIVE_PROTO = 0;

protected:
virtual	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO = 0;
#endif

public:
// not appropriate for placeholders
#if 0
	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;

virtual instance_alias_base_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual	bool
	lookup_instance_placeholder(
		typename default_list<instance_alias_base_ptr_type>::type& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;
#endif

	UNROLL_PORT_ONLY_PROTO;

#if 0
#define	UNROLL_ALIASES_PROTO						\
	bad_bool							\
	unroll_aliases(const multikey_index_type&, 			\
		const multikey_index_type&, 				\
		alias_collection_type&) const

virtual	UNROLL_ALIASES_PROTO = 0;

virtual	COLLECT_PORT_ALIASES_PROTO = 0;

virtual	CONSTRUCT_PORT_CONTEXT_PROTO = 0;

virtual	ASSIGN_FOOTPRINT_FRAME_PROTO = 0;

virtual	void
	accept(alias_visitor&) const = 0;
#endif

public:
#if 0
virtual	instance_alias_base_type&
	load_reference(istream& i) const = 0;
#endif

	static
	// won't be this_type anymore!
	instance_collection_generic_type*
	make_array(const scopespace& o, const string& n, const size_t d);

	static
	persistent*
	construct_empty(const int);

public:
	void
	collect_transient_info(persistent_object_manager&) const;

protected:
	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class instance_placeholder

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#if 0
#undef	UNROLL_ALIASES_PROTO
#undef	INSTANTIATE_INDICES_PROTO
#endif

#endif	// __HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_H__

