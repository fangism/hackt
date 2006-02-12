/**
	\file "Object/ref/aggregate_meta_value_reference.h"
	This is going to be exciting...
	$Id: aggregate_meta_value_reference.h,v 1.1.2.1 2006/02/12 06:15:33 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_H__
#define	__HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_H__

#include <iosfwd>
#include <vector>
// #include "Object/ref/simple_param_meta_value_reference.h"
#include "Object/expr/const_index_list.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
class expr_dump_context;
class const_param;
class const_range_list;
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
	public class_traits<Tag>::meta_value_reference_parent_type,
	public class_traits<Tag>::expr_base_type {
	typedef	AGGREGATE_META_VALUE_REFERENCE_CLASS		this_type;
public:
	typedef	class_traits<Tag>				traits_type;
	typedef	typename traits_type::value_type		value_type;
private:
	typedef	typename traits_type::meta_value_reference_parent_type
							parent_type;
	typedef	typename traits_type::expr_base_type	expr_base_type;
	typedef	expr_base_type					interface_type;
public:
	// aggregates probably won't take initializers
	// typedef	count_ptr<const interface_type>		init_arg_type;
	typedef	typename traits_type::value_collection_parent_type
						value_collection_parent_type;
	typedef	count_ptr<const parent_type>	subreference_ptr_type;
	typedef	std::vector<subreference_ptr_type>
						subreferences_array_type;
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
public:
	aggregate_meta_value_reference();

	explicit
	aggregate_meta_value_reference(const subreference_ptr_type&);

	~aggregate_meta_value_reference();

	ostream&
	what(ostream&) const;

	ostream&
	what(ostream&, const expr_dump_context&) const;

	size_t
	dimensions(void) const;

	/// probably just conservatively return false for simplicity
	bool
	has_static_constant_dimensions(void) const;

	/// probably leave unimplemented, if we're going to punt check
	const_range_list
	static_constant_dimensions(void) const;

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

	bool
	is_template_dependent(void) const;

	bool
	is_unconditional(void) const;

	bool
	is_loop_independent(void) const;

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

	/// should this be called? actually, yeah, possibly early check
	const_index_list
	resolve_dimensions(void) const;

	const_index_list
	unroll_resolve_dimensions(const unroll_context&) const;

	count_ptr<const_param>
	uroll_resolve(const unroll_context&) const;

	bad_bool
	assign_value_collection(const const_collection_type&, 
		const unroll_context&) const;

#if 0
private:
	// these should never be called, they are for the sake of 
	// completing an instance_reference interface, which
	// really should've been factored into a physical_instance_reference
	// class hierarchy
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

	UNROLL_SCALAR_SUBSTRUCTURE_REFERENCE_PROTO;

	CONNECT_PORT_PROTO;
#endif
public:
	PERSISTENT_METHODS_DECLARATIONS

};	// end class aggregate_meta_value_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_H__

