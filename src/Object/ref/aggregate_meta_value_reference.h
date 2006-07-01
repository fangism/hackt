/**
	\file "Object/ref/aggregate_meta_value_reference.h"
	This is going to be exciting...
	$Id: aggregate_meta_value_reference.h,v 1.4.2.2 2006/07/01 03:42:21 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_H__
#define	__HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_H__

#include <iosfwd>
#include <vector>
#include "Object/expr/const_index_list.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/ref/meta_value_reference.h"
#include "Object/ref/aggregate_meta_value_reference_base.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
class definition_base;
class fundamental_type_reference;
class expr_dump_context;
class const_param;
class const_range_list;
class param_value_collection;
using std::istream;
using std::ostream;
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::good_bool;
using util::bad_bool;

#define	AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE		\
template <class Tag>

#define AGGREGATE_META_VALUE_REFERENCE_CLASS				\
aggregate_meta_value_reference<Tag>

//=============================================================================
/**
	A collection of values constructed from subarrays, 
	either by concatenation or array construction.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
class aggregate_meta_value_reference :
	public aggregate_meta_value_reference_base, 
	public class_traits<Tag>::meta_value_reference_parent_type {
	typedef	AGGREGATE_META_VALUE_REFERENCE_CLASS		this_type;
public:
	typedef	class_traits<Tag>				traits_type;
	typedef	typename traits_type::value_type		value_type;
private:
	typedef	typename traits_type::meta_value_reference_parent_type
							parent_type;
	typedef	typename traits_type::simple_meta_value_reference_type
						simple_reference_type;
	typedef	typename traits_type::expr_base_type	expr_base_type;
	typedef	typename traits_type::data_expr_base_type
							data_expr_base_type;
	typedef	expr_base_type				interface_type;
	typedef	count_ptr<const interface_type>		init_arg_type;
public:
	// aggregates probably won't take initializers
	// typedef	count_ptr<const interface_type>		init_arg_type;
	typedef	typename traits_type::value_collection_parent_type
						value_collection_parent_type;
	typedef	count_ptr<const expr_base_type>	subreference_ptr_type;
	typedef	std::vector<subreference_ptr_type>
						subreferences_array_type;
	typedef	typename traits_type::value_reference_collection_type
						value_reference_collection_type;
private:
	typedef	typename traits_type::template value_array<0>::type
						value_scalar_type;
	typedef	typename traits_type::value_collection_generic_type
						value_collection_type;
	typedef	typename traits_type::const_collection_type
						const_collection_type;
	typedef	typename traits_type::const_expr_type
						const_expr_type;
	typedef	never_ptr<value_collection_type>
						value_collection_ptr_type;
	typedef	typename subreferences_array_type::const_iterator
						const_iterator;
private:
	subreferences_array_type		subreferences;
public:
	aggregate_meta_value_reference();

	explicit
	aggregate_meta_value_reference(const subreference_ptr_type&);

	~aggregate_meta_value_reference();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	size_t
	dimensions(void) const;

	never_ptr<const param_value_collection>
	get_coll_base(void) const;

	good_bool
	initialize(const init_arg_type&);

	/// conservatively return true for simplicity
	bool
	may_be_initialized(void) const;

	/// conservatively return false for simplicity
	bool
	must_be_initialized(void) const;

	/// conservatively return false?
	bool
	is_static_constant(void) const;

	// recursive
	bool
	is_relaxed_formal_dependent(void) const;

	/// should never be called
	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const interface_type&) const;

	/// should never be called
	good_bool
	resolve_value(value_type&) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type&) const;

	count_ptr<const const_expr_type>
	__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const expr_base_type>&) const;

#if COW_UNROLL_RESOLVE_RVALUES
	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const expr_base_type>&) const;
#else
	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&) const;
#endif

	/// should this be called? actually, yeah, possibly early check
	const_index_list
	resolve_dimensions(void) const;

	const_index_list
	unroll_resolve_dimensions(const unroll_context&) const;

	bad_bool
	unroll_lvalue_references(const unroll_context&, 
		value_reference_collection_type&) const;

	count_ptr<const expr_base_type>
	unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const expr_base_type>&) const;
protected:
#if COW_UNROLL_RESOLVE_RVALUES
	using expr_base_type::unroll_resolve_rvalues;
#endif
	using expr_base_type::unroll_resolve_copy;

public:
	good_bool
	append_meta_value_reference(const count_ptr<const param_expr>&);

	good_bool
	append_meta_value_reference(const count_ptr<const expr_base_type>&);

public:
	PERSISTENT_METHODS_DECLARATIONS

};	// end class aggregate_meta_value_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_H__

