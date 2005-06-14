/**
	\file "Object/art_object_value_reference.h"
	Classes related to meta parameter instance reference expressions. 
	$Id: art_object_value_reference.h,v 1.7.2.3 2005/06/14 05:38:38 fang Exp $
 */

#ifndef __OBJECT_ART_OBJECT_VALUE_REFERENCE_H__
#define __OBJECT_ART_OBJECT_VALUE_REFERENCE_H__

#include <iosfwd>
#include "util/STL/list_fwd.h"
#include "util/boolean_types.h"
#include "Object/art_object_index.h"
#include "Object/art_object_expr_const.h"	// for const_index_list
#include "Object/art_object_inst_ref_base.h"
#include "Object/art_object_classification_fwd.h"
#include "util/persistent.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

//=============================================================================
namespace ART {
namespace entity {
USING_LIST
using std::ostream;
using util::good_bool;
using util::bad_bool;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::memory::count_ptr;

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
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

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

	const_index_list
	resolve_dimensions(void) const;

	good_bool
	resolve_values_into_flat_list(list<value_type>& l) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

#if 0
	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;
#endif

public:
	/**
		TODO: consider separate file?
		Helper class for assigning values to instances.
	 */
	class assigner {
	protected:
		/** reference to the source of values */
		const interface_type&	src;
		/** resolved range list */
		const_index_list	ranges;
		/** flat list of unrolled values */
		list<value_type>		vals;
	public:
		assigner(const interface_type& p);
		// default destructor

		bad_bool
		operator () (const bad_bool b,
			const SIMPLE_META_VALUE_REFERENCE_CLASS& p) const;

		template <template <class> class P>
		bad_bool
		operator () (const bad_bool b,
			const P<const SIMPLE_META_VALUE_REFERENCE_CLASS >& p) const {
			assert(p);
			return this->operator()(b, *p);
		}
	};	// end class assigner

private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

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

#endif	// __OBJECT_ART_OBJECT_VALUE_REFERENCE_H__

