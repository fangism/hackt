/**
	\file "Object/ref/simple_nonmeta_value_reference.h"
	Classes related to nonmeta (data) instance reference expressions. 
	This file was reincarnated from
		"Object/art_object_nonmeta_value_reference.h"
	$Id: simple_nonmeta_value_reference.h,v 1.1.2.2 2005/07/23 01:06:00 fang Exp $
 */

#ifndef __OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_H__
#define __OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_H__

#include "util/boolean_types.h"
#include "Object/common/multikey_index.h"
#include "Object/expr/data_expr.h"
#include "Object/ref/simple_datatype_nonmeta_value_reference.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

//=============================================================================
namespace ART {
namespace entity {
class const_index_list;
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
};	// end struct data_type_resolver

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
	public simple_datatype_nonmeta_value_reference, 
	public class_traits<Tag>::nonmeta_instance_reference_parent_type, 
	// will be something like int_expr or bool_expr
	public class_traits<Tag>::data_expr_base_type {
friend struct data_type_resolver<Tag>;
public:
	typedef	typename class_traits<Tag>::data_value_type
							data_value_type;
private:
	typedef	SIMPLE_NONMETA_VALUE_REFERENCE_CLASS	this_type;
	typedef	typename class_traits<Tag>::nonmeta_instance_reference_parent_type
							parent_type;
	typedef	typename class_traits<Tag>::data_expr_base_type
							data_expr_base_type;
	typedef	simple_datatype_nonmeta_value_reference	common_base_type;
	typedef	common_base_type::parent_type		grandparent_type;
	typedef	data_expr_base_type			interface_type;
protected:
	typedef	typename class_traits<Tag>::instance_collection_generic_type
							value_collection_type;
	// NOTE: this is const, unlike simple_meta_value_reference
	typedef	never_ptr<const value_collection_type>
						value_collection_ptr_type;
	value_collection_ptr_type			value_collection_ref;
private:
	simple_nonmeta_value_reference();
public:
	explicit
	simple_nonmeta_value_reference(const value_collection_ptr_type);

	simple_nonmeta_value_reference(const value_collection_ptr_type, 
		excl_ptr<index_list_type>&);

	~simple_nonmeta_value_reference();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	value_collection_ptr_type
	get_inst_base_subtype(void) const;

	size_t
	dimensions(void) const;

	GET_DATA_TYPE_REF_PROTO;

	bool
	must_be_equivalent(const interface_type& ) const;

#if 0
protected:
	using common_base_type::collect_transient_info_base;
	using common_base_type::write_object_base;
	using common_base_type::load_object_base;
#endif

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class simple_nonmeta_value_reference

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_H__

