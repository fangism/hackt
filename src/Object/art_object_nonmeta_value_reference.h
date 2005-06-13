/**
	\file "Object/art_object_nonmeta_value_reference.h"
	Classes related to nonmeta (data) instance reference expressions. 
	$Id: art_object_nonmeta_value_reference.h,v 1.1.2.3 2005/06/13 17:52:13 fang Exp $
 */

#ifndef __OBJECT_ART_OBJECT_NONMETA_VALUE_REFERENCE_H__
#define __OBJECT_ART_OBJECT_NONMETA_VALUE_REFERENCE_H__

#include <iosfwd>
#include "util/STL/list_fwd.h"
#include "util/boolean_types.h"
#include "Object/art_object_index.h"
#include "Object/art_object_data_expr_base.h"
#include "Object/art_object_nonmeta_inst_ref_base.h"
#include "Object/art_object_classification_fwd.h"
#include "util/persistent.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

//=============================================================================
namespace ART {
namespace entity {
class const_index_list;
USING_LIST
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
#if 0
public:
	typedef	typename class_traits<Tag>::value_collection_parent_type
						value_collection_parent_type;
#endif
protected:
#if 0
	typedef	typename class_traits<Tag>::template value_array<0>::type
							value_scalar_type;
#endif
	typedef	typename class_traits<Tag>::instance_collection_generic_type
							value_collection_type;
#if 0
	typedef	typename class_traits<Tag>::const_collection_type
							const_collection_type;
	typedef	typename class_traits<Tag>::const_expr_type
							const_expr_type;
#endif
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

#if 0
	using parent_type::dump;
#endif

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	value_collection_ptr_type
	get_inst_base_subtype(void) const;

#if 0
	never_ptr<const value_collection_parent_type>
	get_param_inst_base(void) const;
#endif

	size_t
	dimensions(void) const;

	GET_DATA_TYPE_REF_PROTO;

#if 0
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
	is_unconditional(void) const;

	bool
	is_loop_independent(void) const;

	data_value_type
	static_constant_value(void) const;
#endif

	bool
	must_be_equivalent(const interface_type& ) const;

#if 0
	good_bool
	resolve_value(data_value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, data_value_type& i) const;
#endif

#if 0
	const_index_list
	resolve_dimensions(void) const;
#endif

#if 0
	good_bool
	resolve_values_into_flat_list(list<data_value_type>& l) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;
#endif

#if 0
	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;
#endif

#if 0
public:
	/**
		Helper class for assigning values to instances.
	 */
	class assigner {
	protected:
		/** reference to the source of values */
		const interface_type&	src;
		/** resolved range list */
		const_index_list	ranges;
		/** flat list of unrolled values */
		list<data_value_type>		vals;
	public:
		assigner(const interface_type& p);
		// default destructor

		bad_bool
		operator () (const bad_bool b,
			const SIMPLE_NONMETA_VALUE_REFERENCE_CLASS& p) const;

		template <template <class> class P>
		bad_bool
		operator () (const bad_bool b,
			const P<const SIMPLE_NONMETA_VALUE_REFERENCE_CLASS >& p) const {
			assert(p);
			return this->operator()(b, *p);
		}
	};	// end class assigner

private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;
#endif

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

#endif	// __OBJECT_ART_OBJECT_NONMETA_VALUE_REFERENCE_H__

