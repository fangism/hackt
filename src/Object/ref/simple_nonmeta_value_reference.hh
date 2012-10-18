/**
	\file "Object/ref/simple_nonmeta_value_reference.hh"
	Classes related to nonmeta (data) instance reference expressions. 
	This file was reincarnated from
		"Object/art_object_nonmeta_value_reference.h"
	$Id: simple_nonmeta_value_reference.hh,v 1.16 2011/02/25 23:19:36 fang Exp $
 */

#ifndef __HAC_OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_H__
#define __HAC_OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_H__

#include "util/boolean_types.hh"
#include "Object/common/multikey_index.hh"
#include "Object/expr/data_expr.hh"
#include "Object/ref/simple_nonmeta_instance_reference_base.hh"
#include "Object/ref/data_nonmeta_instance_reference.hh"
#include "Object/traits/class_traits_fwd.hh"
#include "Object/devel_switches.hh"
#include "util/memory/excl_ptr.hh"
#include "util/memory/count_ptr.hh"
#include "util/STL/vector_fwd.hh"

//=============================================================================
namespace HAC {
namespace entity {
class const_index_list;
class unroll_context;
class nonmeta_context_base;
class nonmeta_expr_visitor;
struct global_entry_context;
class state_manager;
class footprint;
class footprint_frame;
class const_param;
#if NONMETA_MEMBER_REFERENCES
class meta_value_reference_base;
template <class> class simple_meta_value_reference;
#endif
using std::ostream;
using util::good_bool;
using util::bad_bool;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::memory::count_ptr;

//=============================================================================
/**
	Generic template for the functor that resolves the 
	data type reference.  
	(This generalized declaration remains undefined.)
	Specializations should follow this form.  
 */
template <class Tag>
struct data_type_resolver {
	typedef	typename class_traits<Tag>::simple_nonmeta_instance_reference_type
						data_value_reference_type;
	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&) const;

	count_ptr<const data_type_reference>
	operator () (const data_value_reference_type&, 
		const unroll_context&) const;

};	// end struct data_type_resolver

//=============================================================================
/**
	Policy class implementing unroll-resolving. 
	\param TagDupe is true if type_tag has corresponding meta_type_tag,
		e.g. bool_traits::has_meta_equivalent.
 */
template <class Tag, class TagParent>
struct nonmeta_unroll_resolve_copy_policy;

//=============================================================================
#define	SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE		\
template <class Tag>

#define	SIMPLE_NONMETA_VALUE_REFERENCE_CLASS				\
simple_nonmeta_value_reference<Tag>

/**
	A reference to a instance of built-in type pbool.  
	This is intended as a replacement for 
	simple_nonmeta_instance_reference.
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
class simple_nonmeta_value_reference :
	public class_traits<Tag>::nonmeta_instance_reference_base_type, 
	public simple_nonmeta_instance_reference_base, 
	// will be something like int_expr or bool_expr
	public class_traits<Tag>::data_expr_base_type {
friend struct data_type_resolver<Tag>;
public:
	typedef	class_traits<Tag>			traits_type;
private:
friend struct nonmeta_unroll_resolve_copy_policy<Tag, typename Tag::parent_tag>;
	typedef	SIMPLE_NONMETA_VALUE_REFERENCE_CLASS	this_type;
	typedef	typename traits_type::nonmeta_instance_reference_base_type
							parent_type;
	typedef	typename traits_type::data_expr_base_type
							data_expr_base_type;
	typedef	typename traits_type::const_expr_type	const_expr_type;
	typedef	simple_nonmeta_instance_reference_base	common_base_type;
	typedef	data_expr_base_type			interface_type;
	typedef	typename parent_type::assign_update_arg_type
							assign_update_arg_type;
#if NONMETA_MEMBER_REFERENCES
public:
	typedef	simple_meta_value_reference<Tag>	meta_reference_type;
#endif
protected:
	typedef	typename traits_type::instance_placeholder_type
							value_collection_type;
	// NOTE: this is const, unlike simple_meta_value_reference
	typedef	never_ptr<const value_collection_type>
						value_collection_ptr_type;
	value_collection_ptr_type			value_collection_ref;
protected:
	simple_nonmeta_value_reference();
public:
	explicit
	simple_nonmeta_value_reference(const value_collection_ptr_type);

	simple_nonmeta_value_reference(const value_collection_ptr_type, 
		excl_ptr<index_list_type>&);

virtual	~simple_nonmeta_value_reference();

virtual	ostream&
	what(ostream&) const;

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const;

// doesn't need to be virtual
	never_ptr<const definition_base>
	get_base_def(void) const;

	value_collection_ptr_type
	get_inst_base_subtype(void) const;

	good_bool
	attach_indices(excl_ptr<index_list_type>&);

	size_t
	dimensions(void) const;

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
	GET_RESOLVED_DATA_TYPE_REF_PROTO;

	bool
	is_lvalue(void) const;

	bool
	must_be_equivalent(const interface_type& ) const;

virtual	count_ptr<const data_expr_base_type>
	unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const data_expr_base_type>&) const;

virtual	count_ptr<const const_param>
	nonmeta_resolve_copy(const nonmeta_context_base&, 
		const count_ptr<const data_expr_base_type>&) const;

	count_ptr<const const_expr_type>
	__nonmeta_resolve_rvalue(const nonmeta_context_base&, 
		const count_ptr<const data_expr_base_type>&) const;

virtual	good_bool
	lookup_may_reference_global_indices(
		const global_entry_context&, 
		std::default_vector<size_t>::type&) const;

virtual	size_t
	lookup_nonmeta_global_index(const nonmeta_context_base&) const;

#if NONMETA_MEMBER_REFERENCES
virtual count_ptr<meta_reference_type>
        resolve_meta_reference(const nonmeta_context_base&) const;
#endif

virtual	NONMETA_ASSIGN_PROTO;
virtual	DIRECT_ASSIGN_PROTO;

virtual	void
	accept(nonmeta_expr_visitor&) const;

protected:
	using data_expr_base_type::unroll_resolve_copy;
	using data_expr_base_type::nonmeta_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class simple_nonmeta_value_reference

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_H__

