/**
	\file "art_object_value_reference.h"
	Classes related to parameter instance reference expressions. 
	$Id: art_object_value_reference.h,v 1.1.6.1 2005/03/11 01:16:21 fang Exp $
 */

#ifndef __ART_OBJECT_VALUE_REFERENCE_H__
#define __ART_OBJECT_VALUE_REFERENCE_H__

#include "boolean_types.h"
#include "art_object_fwd.h"

//=============================================================================
namespace ART {
namespace entity {
USING_LIST
using std::string;
using std::ostream;

#define	VALUE_REFERENCE_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	VALUE_REFERENCE_CLASS						\
value_reference<Tag>

//=============================================================================
/**
	A reference to a instance of built-in type pbool.  
	Consider multiply deriving from pbool_expr, 
	and replacing pbool_literal.  
 */
VALUE_REFERENCE_TEMPLATE_SIGNATURE
class value_reference :
	public class_traits<Tag>::instance_reference_parent_type, 
	public class_traits<Tag>::expr_base_type {
public:
	typedef	typename class_traits<Tag>::value_type	value_type;
private:
	typedef	VALUE_REFERENCE_CLASS			this_type;
	typedef	typename class_traits<Tag>::instance_reference_parent_type
							parent_type;
	typedef	typename class_traits<Tag>::expr_base_type
							expr_base_type;
	typedef	typename parent_type::parent_type	grandparent_type;
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
	never_ptr<value_collection_type>		value_collection_ref;
private:
	value_reference();
public:
	explicit
	value_reference(const never_ptr<value_collection_type> pi);

	value_reference(const never_ptr<value_collection_type> pi, 
		excl_ptr<index_list>& i);

	~value_reference();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	using parent_type::dump;

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

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

	string
	hash_string(void) const;

	// try these
	// using param_instance_reference::may_be_initialized;
	// using param_instance_reference::must_be_initialized;

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
			const VALUE_REFERENCE_CLASS& p) const;

		template <template <class> class P>
		bad_bool
		operator () (const bad_bool b,
			const P<const VALUE_REFERENCE_CLASS >& p) const {
			assert(p);
			return this->operator()(b, *p);
		}
	};	// end class assigner

#if 1
private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;
#endif

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class value_reference

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __ART_OBJECT_VALUE_REFERENCE_H__

