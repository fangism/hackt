/**
	\file "Object/ref/simple_nonmeta_instnace_reference.h"
	Class template for nonmeta instance references in HAC.  
	This file originated from "Object/art_object_nonmeta_inst_ref.h"
		in a previous life.  
	$Id: simple_nonmeta_instance_reference.h,v 1.6 2006/03/16 03:40:27 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_H__
#define	__HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_H__

#include "Object/ref/simple_nonmeta_instance_reference_base.h"
#include "Object/inst/instance_collection_base.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/packed_array_fwd.h"

namespace HAC {
namespace entity {
class unroll_context;
using util::packed_array_generic;

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
 */
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class simple_nonmeta_instance_reference :
	public simple_nonmeta_instance_reference_base, 
	public class_traits<Tag>::nonmeta_instance_reference_parent_type {
	typedef	SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS	this_type;
public:
	typedef	class_traits<Tag>		traits_type;
protected:
	typedef	simple_nonmeta_instance_reference_base
						common_base_type;
	typedef	typename traits_type::nonmeta_instance_reference_parent_type
						parent_type;
public:
	/// the instance collection base type
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	/// pointer type for instance collections
	typedef	never_ptr<const instance_collection_generic_type>
						instance_collection_ptr_type;
private:
	const instance_collection_ptr_type	inst_collection_ref;
protected:
	simple_nonmeta_instance_reference();
public:
	explicit
	simple_nonmeta_instance_reference(const instance_collection_ptr_type);

	~simple_nonmeta_instance_reference();

	size_t
	dimensions(void) const;

	ostream&
	what(ostream&) const;

#if 0
	using parent_type::dump;
#else
	ostream&
	dump(ostream&, const expr_dump_context&) const;
#endif

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	instance_collection_ptr_type
	get_inst_base_subtype(void) const;

	good_bool
	attach_indices(excl_ptr<index_list_type>&);

#if 0
// only needed if included statically, such as in member_instance_reference
// however, we don't support members references of 
// non-meta instance references... yet.
protected:
	void
	collect_transient_info_base(persistent_object_manager& ) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
#endif

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class nonmeta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_H__

