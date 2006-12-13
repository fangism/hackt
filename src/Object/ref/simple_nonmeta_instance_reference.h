/**
	\file "Object/ref/simple_nonmeta_instance_reference.h"
	Class template for nonmeta instance references in HAC.  
	This file originated from "Object/art_object_nonmeta_inst_ref.h"
		in a previous life.  
	$Id: simple_nonmeta_instance_reference.h,v 1.11.4.2 2006/12/13 02:29:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_H__
#define	__HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_H__

#include "Object/ref/simple_nonmeta_instance_reference_base.h"
#include "Object/inst/instance_collection_base.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/packed_array_fwd.h"
#include "util/STL/vector_fwd.h"

namespace HAC {
namespace entity {
class data_expr;
class unroll_context;
class nonmeta_expr_visitor;
class state_manager;
class footprint;
class footprint_frame;
using util::packed_array_generic;
template <class> class simple_meta_instance_reference;

//=============================================================================
#define	SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE		\
template <class Tag>

#define	SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS				\
simple_nonmeta_instance_reference<Tag>

/**
	Class template for physical instance references.
	Needs to be virtual so that member_nonmeta_instance_reference may safely
	derive from this class.  
	\param Collection the instance collection type.
	\param Parent the type from which this is derived, 
		probably simple_nonmeta_instance_reference or descendant.  
	NOTE: nothing is virtual, this is a final class until
		otherwise changed.  
	NOTE: we may need to support nonmeta member references soon, 
		in which case, the contents of this class may be refactored.  
 */
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class simple_nonmeta_instance_reference :
	public simple_nonmeta_instance_reference_base, 
	public class_traits<Tag>::nonmeta_instance_reference_base_type {
	typedef	SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS	this_type;
public:
	typedef	class_traits<Tag>		traits_type;
protected:
	typedef	simple_nonmeta_instance_reference_base
						common_base_type;
	typedef	typename traits_type::nonmeta_instance_reference_base_type
						parent_type;
public:
	/// the instance collection base type
	typedef	typename traits_type::instance_placeholder_type
					instance_placeholder_type;
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	simple_meta_instance_reference<Tag>
					simple_meta_instance_reference_type;
	typedef	typename traits_type::alias_collection_type
						alias_collection_type;
	/// pointer type for instance collections
	typedef	never_ptr<const instance_placeholder_type>
						instance_placeholder_ptr_type;
private:
	const instance_placeholder_ptr_type	inst_collection_ref;
protected:
	simple_nonmeta_instance_reference();
public:
	explicit
	simple_nonmeta_instance_reference(const instance_placeholder_ptr_type);

	~simple_nonmeta_instance_reference();

	size_t
	dimensions(void) const;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	instance_placeholder_ptr_type
	get_inst_base_subtype(void) const {
		return this->inst_collection_ref;
	}

	good_bool
	attach_indices(excl_ptr<index_list_type>&);

	count_ptr<const this_type>
	unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const this_type>&) const;

	good_bool
	lookup_may_reference_global_indices(const state_manager&, 
		const footprint&, const footprint_frame* const,
		std::default_vector<size_t>::type&) const;

	void
	accept(nonmeta_expr_visitor&) const;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class nonmeta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_H__

