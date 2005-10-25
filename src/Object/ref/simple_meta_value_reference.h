/**
	\file "Object/ref/simple_meta_value_reference.h"
	Classes related to meta parameter instance reference expressions. 
	This file was reincarnated from "Object/art_object_value_reference.h".
	$Id: simple_meta_value_reference.h,v 1.4 2005/10/25 20:51:57 fang Exp $
 */

#ifndef __OBJECT_REF_SIMPLE_META_VALUE_REFERENCE_H__
#define __OBJECT_REF_SIMPLE_META_VALUE_REFERENCE_H__

#include "Object/expr/const_index_list.h"	// used in assigner, below
#include "Object/common/multikey_index.h"
#include "Object/ref/simple_param_meta_value_reference.h"
#include "Object/traits/class_traits_fwd.h"

//=============================================================================
namespace ART {
namespace entity {
class const_param;
class const_index_list;
class const_range_list;
class unroll_context;
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
	public simple_param_meta_value_reference, 
	public class_traits<Tag>::meta_instance_reference_parent_type, 
	public class_traits<Tag>::expr_base_type {
public:
	typedef	typename class_traits<Tag>::value_type	value_type;
private:
	typedef	SIMPLE_META_VALUE_REFERENCE_CLASS	this_type;
	typedef	typename class_traits<Tag>::meta_instance_reference_parent_type
							parent_type;
	typedef	typename class_traits<Tag>::expr_base_type
							expr_base_type;
	typedef	simple_param_meta_value_reference	common_base_type;
	typedef	common_base_type::parent_type		grandparent_type;
	typedef	expr_base_type				interface_type;
public:
	typedef	count_ptr<const interface_type>		init_arg_type;
	typedef	typename class_traits<Tag>::value_collection_parent_type
						value_collection_parent_type;
protected:
	typedef	typename class_traits<Tag>::template value_array<0>::type
							value_scalar_type;
	typedef	typename class_traits<Tag>::value_collection_generic_type
							value_collection_type;
	typedef	typename class_traits<Tag>::const_collection_type
							const_collection_type;
	typedef	typename class_traits<Tag>::const_expr_type
							const_expr_type;
	typedef	never_ptr<value_collection_type>
						value_collection_ptr_type;
	value_collection_ptr_type			value_collection_ref;
private:
	simple_meta_value_reference();
public:
	explicit
	simple_meta_value_reference(const value_collection_ptr_type);

	simple_meta_value_reference(const value_collection_ptr_type, 
		excl_ptr<index_list_type>&);

	~simple_meta_value_reference();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

#if 0
	using parent_type::dump;
#endif

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	never_ptr<const instance_collection_base>
	get_inst_base_subtype(void) const;

	never_ptr<const value_collection_parent_type>
	get_param_inst_base(void) const;

	size_t
	dimensions(void) const;

	bool
	has_static_constant_dimensions(void) const;

	const_range_list
	static_constant_dimensions(void) const;

	good_bool
	initialize(const init_arg_type& i);

	// try these
	// using simple_param_meta_value_reference::may_be_initialized;
	// using simple_param_meta_value_reference::must_be_initialized;

	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	bool
	is_template_dependent(void) const;

	bool
	is_unconditional(void) const;

	bool
	is_loop_independent(void) const;

	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const interface_type& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	// why is this not available to other meta-instance-references?
	// doesn't this need context?
	const_index_list
	resolve_dimensions(void) const;

	const_index_list
	unroll_resolve_dimensions(const unroll_context&) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

#if 0
	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;
#endif

	bad_bool
	assign_value_collection(const const_collection_type&, 
		const unroll_context&) const;

private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

	UNROLL_GENERIC_SCALAR_REFERENCE_PROTO;

	CONNECT_PORT_PROTO;
protected:
	using common_base_type::collect_transient_info_base;
	using common_base_type::write_object_base;
	using common_base_type::load_object_base;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class simple_meta_value_reference

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_REF_SIMPLE_META_VALUE_REFERENCE_H__

