/**
	\file "Object/ref/simple_meta_value_reference.hh"
	Classes related to meta parameter instance reference expressions. 
	This file was reincarnated from "Object/art_object_value_reference.h".
	$Id: simple_meta_value_reference.hh,v 1.18 2007/01/21 05:59:35 fang Exp $
 */

#ifndef __HAC_OBJECT_REF_SIMPLE_META_VALUE_REFERENCE_H__
#define __HAC_OBJECT_REF_SIMPLE_META_VALUE_REFERENCE_H__

#include "Object/expr/const_index_list.hh"	// used in assigner, below
#include "Object/common/multikey_index.hh"
#include "Object/ref/meta_value_reference_base.hh"
#include "Object/ref/simple_meta_indexed_reference_base.hh"
	// transformed to not be instance-specific
#include "Object/traits/class_traits_fwd.hh"

//=============================================================================
namespace HAC {
namespace entity {
class const_param;
class const_index_list;
class const_range_list;
class unroll_context;
class nonmeta_context_base;
class param_expr;
class template_formals_manager;
class dynamic_param_expr_list;
class nonmeta_expr_visitor;
using util::good_bool;
using util::bad_bool;

//=============================================================================
#define	SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	SIMPLE_META_VALUE_REFERENCE_CLASS				\
simple_meta_value_reference<Tag>

/**
	A reference to a instance of built-in type pbool.  
	Consider multiply deriving from pbool_expr, 
	and replacing pbool_literal.  
 */
SIMPLE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
class simple_meta_value_reference :
	public simple_meta_indexed_reference_base, 
	public class_traits<Tag>::meta_value_reference_parent_type {
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::value_type	value_type;
private:
	typedef	SIMPLE_META_VALUE_REFERENCE_CLASS	this_type;
	typedef	typename traits_type::meta_value_reference_parent_type
							parent_type;
	typedef	typename parent_type::expr_base_type	expr_base_type;
	typedef	typename traits_type::data_expr_base_type
							data_expr_base_type;
	// is not actually specific to instances
	typedef	simple_meta_indexed_reference_base	common_base_type;
	typedef	expr_base_type				interface_type;
public:
	typedef	common_base_type::indices_ptr_arg_type	indices_ptr_arg_type;
	typedef	count_ptr<const interface_type>		init_arg_type;
	typedef	typename traits_type::value_placeholder_parent_type
						value_placeholder_parent_type;
	typedef	typename traits_type::value_collection_parent_type
						value_collection_parent_type;
	typedef	typename traits_type::value_reference_collection_type
						value_reference_collection_type;
protected:
	typedef	typename traits_type::template value_array<0>::type
							value_scalar_type;
	typedef	typename traits_type::instance_placeholder_type
							value_placeholder_type;
	/**
		Reference never modifies the source placeholder, hence const.  
	 */
	typedef	never_ptr<const value_placeholder_type>
						value_placeholder_ptr_type;
	typedef	typename traits_type::value_collection_generic_type
							value_collection_type;
	typedef	typename traits_type::const_collection_type
							const_collection_type;
	typedef	typename traits_type::const_expr_type
							const_expr_type;
	typedef	never_ptr<value_collection_type>
						value_collection_ptr_type;
	value_placeholder_ptr_type			value_collection_ref;
private:
	simple_meta_value_reference();
public:
	explicit
	simple_meta_value_reference(const value_placeholder_ptr_type);

	simple_meta_value_reference(const value_placeholder_ptr_type, 
		indices_ptr_arg_type);

	~simple_meta_value_reference();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	good_bool
	attach_indices(indices_ptr_arg_type);

	never_ptr<const param_value_placeholder>
	get_coll_base(void) const;

	never_ptr<const value_placeholder_parent_type>
	get_param_inst_base(void) const;

	size_t
	dimensions(void) const;

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const interface_type& ) const;

	good_bool
	unroll_resolve_defined(const unroll_context&, pbool_value_type&) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	unroll_resolve_dimensions(const unroll_context&) const;

	count_ptr<const const_expr_type>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const expr_base_type>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const expr_base_type>&) const;

protected:
	count_ptr<const const_param>
	__unroll_resolve_rvalues(const unroll_context&) const;

public:
#if 0
	count_ptr<const_index>
	unroll_resolve_rvalues_index(const unroll_context&) const;
#endif

	bad_bool
	unroll_lvalue_references(const unroll_context&, 
		value_reference_collection_type&) const;

	count_ptr<const expr_base_type>
	unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const expr_base_type>&) const;

        count_ptr<const expr_base_type>
        substitute_default_positional_parameters(
                const template_formals_manager&,
                const dynamic_param_expr_list&,
                const count_ptr<const expr_base_type>&) const;
	using parent_type::substitute_default_positional_parameters;

	void
	accept(nonmeta_expr_visitor&) const;

protected:
	using parent_type::unroll_resolve_rvalues;
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;

protected:
	using common_base_type::collect_transient_info_base;
	using common_base_type::write_object_base;
	using common_base_type::load_object_base;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class simple_meta_value_reference

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_REF_SIMPLE_META_VALUE_REFERENCE_H__

