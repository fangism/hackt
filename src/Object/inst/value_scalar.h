/**
	\file "Object/inst/value_scalar.h"
	$Id: value_scalar.h,v 1.2.2.5 2006/11/07 01:08:01 fang Exp $
	This file spawned from:
	Id: value_collection.h,v 1.19.2.1 2006/10/22 08:03:28 fang Exp
 */

#ifndef	__HAC_OBJECT_INST_VALUE_SCALAR_H__
#define	__HAC_OBJECT_INST_VALUE_SCALAR_H__

#include "Object/inst/value_collection.h"

namespace HAC {
namespace entity {
// template <class> class param_instantiation_statement;


//=============================================================================
#define	VALUE_SCALAR_TEMPLATE_SIGNATURE					\
template <class Tag>

#define	VALUE_SCALAR_CLASS						\
value_array<Tag,0>

/**
	Specialization of scalar parameter.
 */
VALUE_SCALAR_TEMPLATE_SIGNATURE
class VALUE_SCALAR_CLASS : public value_collection<Tag> {
private:
	typedef	value_collection<Tag>			parent_type;
	typedef	VALUE_SCALAR_CLASS			this_type;
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::instance_type	instance_type;
	typedef	instance_type				element_type;
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::expr_base_type	expr_type;
	typedef	typename traits_type::const_expr_type	const_expr_type;
	typedef	typename traits_type::value_reference_collection_type
					value_reference_collection_type;
	typedef	typename parent_type::value_placeholder_type
							value_placeholder_type;
	typedef typename parent_type::value_placeholder_ptr_type
						value_placeholder_ptr_type;
private:
	instance_type					the_instance;
	const_expr_type					cached_value;
	bool						cache_validity;
public:
	value_array();

	explicit
	value_array(const value_placeholder_ptr_type);
public:
	~value_array();

	instance_type&
	get_instance(void) { return this->the_instance; }

	const instance_type&
	get_instance(void) const { return this->the_instance; }

	ostream&
	what(ostream& ) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_values(ostream& o) const;

	good_bool
	lookup_value(value_type&) const;

// there are implemented to do nothing but sanity check, 
// since it doesn't even make sense to call these.  
	// update this to accept a const_range_list
	good_bool
	instantiate_indices(const const_range_list&);

	LOOKUP_VALUE_INDEXED_PROTO;
	// need methods for looking up dense sub-collections of values?
	// what should they return?

	UNROLL_LVALUE_REFERENCES_PROTO;

	const_index_list
	resolve_indices(const const_index_list& l) const;

public:
	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(footprint&, const persistent_object_manager&, istream&);

};	// end class value_array specialization

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_VALUE_SCALAR_H__

