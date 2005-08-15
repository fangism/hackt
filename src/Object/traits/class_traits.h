/**
	\file "Object/traits/class_traits.h"
	Traits and policy classes for instances.  
	This file is included by "Object/art_object_*_traits.h"
	This file used to be "Object/art_object_classification_details.h".
	$Id: class_traits.h,v 1.2.10.1 2005/08/15 21:12:20 fang Exp $
 */

#ifndef	__OBJECT_TRAITS_CLASS_TRAITS_H__
#define	__OBJECT_TRAITS_CLASS_TRAITS_H__

#include <iosfwd>
#include "Object/art_object_fwd.h"
#include "util/memory/pointer_classes_fwd.h"
#include "util/packed_array_fwd.h"
#include "util/ring_node_fwd.h"

namespace util {
	class persistent_object_manager;
}

namespace ART {
namespace entity {
using std::istream;
using std::ostream;
using util::packed_array_generic;
using util::ring_node_derived;
using util::persistent_object_manager;
using util::memory::never_ptr;
using util::memory::count_ptr;

class unroll_context;

// defined in "Object/inst/alias_empty.h"
class instance_alias_info_empty;

// defined in "Object/inst/alias_actuals.h"
class instance_alias_info_actuals;

/**
	Default definition is empty, must specialize for each class.  
	The definitions contained herein serve no purpose but to 
	document the design pattern used.  
	A few default patterns are provided.  
	The typedefs defined in specalizations of this are intended
	to describe the relationship between various classes.  
	This serves as a centralized location for determining
	implementation details.  
 */
template <class Tag>
struct class_traits {
	typedef	Tag				tag_type;
	/**
		The type of uniquely created object with possible aliases
	 */
	typedef	void				instance_type;

	/**
		Unique alias placeholder pointing to a instance.
		This is most likely a base type from which 
		dimension-specific subclasses are derived.  
	 */
	typedef	void				instance_alias_base_type;
	/// a non-owned, modifiable pointer reference to instance_alias_base_type
	typedef	never_ptr<instance_alias_base_type>
						instance_alias_base_ptr_type;

	/**
		This determines which alias classes contain relaxed
		actual parameters.  
		Two base classes are provided:
			instance_alias_info_empty, 
			and instance_alias_info_actuals.
		Each policy that corresponds to a collection type
			will define this type.  
	 */
	typedef	void			instance_alias_relaxed_actuals_type;

	/**
		This is the base type of the instance collection, 
		the home location of the instance_aliases.  
		This class will be sub-typed into dimension-specific
		arrays and a scalar instance.  
		This tag is only applicable to physical instance types.  
	 */
	typedef	void			instance_collection_generic_type;
	/**
		The parent type of instance_collection_generic_type.
	 */
	typedef	void				instance_collection_parent_type;

	/**
		Parameter value collection types.  
		This is only applicable to parameter instance classes.  
	 */
	typedef	void				value_collection_generic_type;
	typedef	void				value_collection_parent_type;

	/**
		Before unrolling, this is the simple kind of 
		reference object that will be unrolled into
		(possibly) a collection of instance_aliases.  
		NOTE: this should be a simple_meta_instance_reference.
		(The 'simple' qualifier is implied.)
	 */
	typedef	void			simple_meta_instance_reference_type;
	typedef	void			simple_nonmeta_instance_reference_type;
	typedef	void			meta_instance_reference_parent_type;

	/**
		This sub-type is for member references that also
		resolve to the same kind of instance aliases.  
	 */
	typedef	void		member_simple_meta_instance_reference_type;

	/**
		Instantiation statement type.
	 */
	typedef	void				instantiation_statement_type;
	/**
		Instantiation statement parent type.
	 */
	typedef	void			instantiation_statement_parent_type;

	/**
		This is the type of aggregate object that contains
		(pointers) to aliases resolved by unrolling references.  
		These are passed around during unroll time to
		manipulate references and connections.  
	 */
	typedef	packed_array_generic<pint_value_type, instance_alias_base_type>
						alias_collection_type;

	/**
		This type represent a connection statement to alias
		instance_aliases.  
	 */
	typedef	void				alias_connection_type;
	typedef	void				alias_connection_parent_type;

	/**
		An optional parameter to distinguish between
		subtypes of the same base, or variations of one type.  
		This may be an empty class, which opens opportunities
		for the Empty Base Class Optimization (EBCO).  
	 */
	typedef	void			instance_collection_parameter_type;

	/**
		The type reference class used to distinguish types
		of the instances referenced.  
	 */
	typedef	void				type_ref_type;
	typedef	void				type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;

};	// end struct class_traits

//=============================================================================
// some useful forward declarations, because I can't find a better place...

// defined in "Object/art_object_inst_stmt_type_ref_default.h"
template <class>
class instantiation_statement_type_ref_default;

// for classes without parameter (bool)
struct null_parameter_type {
	/// type is never null, added since canonical_type was used
	operator bool () const { return true; }

	unroll_context
	make_unroll_context(void) const;
};	// end struct null_parameter_type

typedef	instance_alias_info<bool_tag>		bool_instance_alias_info;
typedef	instance_alias_info<int_tag>		int_instance_alias_info;
typedef	instance_alias_info<enum_tag>		enum_instance_alias_info;
typedef	instance_alias_info<datastruct_tag>	datastruct_instance_alias_info;
typedef	instance_alias_info<channel_tag>	channel_instance_alias_info;
typedef	instance_alias_info<process_tag>	process_instance_alias_info;

typedef ring_node_derived<bool_instance_alias_info>
						bool_instance_alias_base;
typedef ring_node_derived<int_instance_alias_info>
						int_instance_alias_base;
typedef ring_node_derived<enum_instance_alias_info>
						enum_instance_alias_base;
typedef ring_node_derived<datastruct_instance_alias_info>
						struct_instance_alias_base;
typedef ring_node_derived<channel_instance_alias_info>
						channel_instance_alias_base;
typedef ring_node_derived<process_instance_alias_info>
						process_instance_alias_base;

//=============================================================================
// useful base templates

/**
	Handy empty class that does nothing but meet a convenient interface
	for persistent object management of type references.  
 */
class empty_instantiation_statement_type_ref_base {
public:

	void 
	collect_transient_info_base(persistent_object_manager&) const {
		// nothing to collect
	}

	void
	write_object_base(const persistent_object_manager&, ostream&) const {
		// nothing to write
	}

	void
	load_object_base(const persistent_object_manager&, istream&) {
		// nothing to load
	} 

};	// end class empty_instantiation_statement_type_ref_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TRAITS_CLASS_TRAITS_H__

