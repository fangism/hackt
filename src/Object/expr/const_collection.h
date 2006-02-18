/**
	\file "Object/expr/const_collection.h"
	Classes related to constant expressions, symbolic and parameters.  
	This file was "Object/expr/const_collection.h"
		in a previous life.  
	$Id: const_collection.h,v 1.7.4.2 2006/02/18 05:16:33 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_CONST_COLLECTION_H__
#define __HAC_OBJECT_EXPR_CONST_COLLECTION_H__

#include <iosfwd>
#include "Object/expr/types.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/devel_switches.h"
#include "util/STL/construct_fwd.h"
#include "util/packed_array.h"
#include "util/persistent.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"
// #include "util/memory/chunk_map_pool_fwd.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_index_list;
class const_range_list;
class unroll_context;
class const_param;
struct expr_dump_context;
USING_CONSTRUCT
using std::ostream;
using std::istream;
using util::good_bool;
using util::memory::count_ptr;
using util::persistent;
using util::persistent_object_manager;	// forward declared

#define	CONST_COLLECTION_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	CONST_COLLECTION_CLASS						\
const_collection<Tag>

//=============================================================================
/**
	Packed collection of constant integer values, arbitrary dimension.  
	Note: this is only usable for aggregates of constants.  
	Complex aggregates of non-const expressions will require
	a more advanced structure (dynamic_pint_collection?).  
 */
CONST_COLLECTION_TEMPLATE_SIGNATURE
class const_collection :
		public class_traits<Tag>::expr_base_type,
		public class_traits<Tag>::const_collection_parent_type {
	typedef	CONST_COLLECTION_CLASS			this_type;
	typedef	typename class_traits<Tag>::expr_base_type
							expr_base_type;
public:
	typedef	typename class_traits<Tag>::const_collection_parent_type
							parent_const_type;
	typedef	typename class_traits<Tag>::const_expr_type
							const_expr_type;
	typedef	typename class_traits<Tag>::value_type	value_type;
	typedef	util::packed_array_generic<pint_value_type, value_type>
							array_type;
	typedef	typename array_type::iterator		iterator;
	typedef	typename array_type::const_iterator	const_iterator;
	typedef	typename array_type::reference		reference;
	typedef	typename array_type::const_reference	const_reference;
protected:
	typedef	typename array_type::key_type		key_type;
protected:
	array_type					values;
public:
	explicit
	const_collection(const size_t d);

	explicit
	const_collection(const typename array_type::key_type&);

	~const_collection();

	const_reference
	front(void) const { return values.front(); }

	iterator
	begin(void) { return values.begin(); }

	const_iterator
	begin(void) const { return values.begin(); }

	iterator
	end(void) { return values.end(); }

	const_iterator
	end(void) const { return values.end(); }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream&) const;

	size_t
	dimensions(void) const;

	bool
	is_static_constant(void) const { return true; }

	bool
	is_relaxed_formal_dependent(void) const { return false; }

	count_ptr<const parent_const_type>
	static_constant_param(void) const;

	bool
	has_static_constant_dimensions(void) const;

	const_range_list
	static_constant_dimensions(void) const;

	value_type
	operator [] (const key_type&) const;

	bool
	may_be_initialized(void) const { return true; }

	bool
	must_be_initialized(void) const { return true; }

#if 0
	// required by const_param
	bool
	may_be_equivalent(const param_expr& ) const;

	// required by const_param
	bool
	must_be_equivalent(const param_expr& ) const;
#endif

	// required by pint_expr or pbool_expr
	bool
	must_be_equivalent(const expr_base_type& ) const;

	// only makes sense for scalars
	value_type
	static_constant_value(void) const;

	// only makes sense for scalars
	good_bool
	resolve_value(value_type& ) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	count_ptr<parent_const_type>
	unroll_resolve(const unroll_context&) const;

	this_type
	make_value_slice(const const_index_list&) const;

	bool
	operator < (const const_param&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS

};	// end class const_collection

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_CONST_COLLECTION_H__

