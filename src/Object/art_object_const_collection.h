/**
	\file "art_object_const_collection.h"
	Classes related to constant expressions, symbolic and parameters.  
	$Id: art_object_const_collection.h,v 1.1.2.1 2005/03/09 22:46:35 fang Exp $
 */

#ifndef __ART_OBJECT_CONST_COLLECTION_H__
#define __ART_OBJECT_CONST_COLLECTION_H__

#include "art_object_fwd.h"
// #include "art_object_expr_base.h"
#include "packed_array.h"
// #include "memory/list_vector_pool_fwd.h"

//=============================================================================
namespace ART {
namespace entity {

USING_CONSTRUCT
using std::ostream;
using std::istream;
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
	typedef	typename class_traits<Tag>::value_type	value_type;
	typedef	util::packed_array_generic<pint_value_type, value_type>
							array_type;
	typedef	typename array_type::iterator		iterator;
	typedef	typename array_type::const_iterator	const_iterator;
protected:
	array_type					values;
public:
	explicit
	const_collection(const size_t d);

	explicit
	const_collection(const typename array_type::key_type&);

	~const_collection();

	iterator
	begin(void) { return values.begin(); }

	const_iterator
	begin(void) const { return values.begin(); }

	iterator
	end(void) { return values.end(); }

	const_iterator
	end(void) const { return values.end(); }

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	string
	hash_string(void) const;

	size_t
	dimensions(void) const;

	bool
	is_static_constant(void) const { return true; }

	count_ptr<const parent_const_type>
	static_constant_param(void) const;

	bool
	has_static_constant_dimensions(void) const;

	const_range_list
	static_constant_dimensions(void) const;

	bool
	may_be_initialized(void) const { return true; }

	bool
	must_be_initialized(void) const { return true; }

	bool
	may_be_equivalent(const param_expr& ) const;

	bool
	must_be_equivalent(const param_expr& ) const;

#if 1
	bool
	must_be_equivalent_pint(const pint_expr& ) const;
#endif

	bool
	is_loop_independent(void) const { return true; }

	bool
	is_unconditional(void) const { return true; }

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

	// flat-list needs to be replaced
	good_bool
	resolve_values_into_flat_list(list<value_type>& ) const;

	count_ptr<parent_const_type>
	unroll_resolve(const unroll_context&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS

};	// end class const_collection

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __ART_OBJECT_CONST_COLLECTION_H__

