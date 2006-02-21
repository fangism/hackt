/**
	\file "Object/inst/parameterless_collection_type_manager.h"
	Template class for instance_collection's type manager.  
	$Id: parameterless_collection_type_manager.h,v 1.7 2006/02/21 21:33:02 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_H__
#define	__HAC_OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_H__

#include <iosfwd>
#include "Object/type/canonical_type_fwd.h"	// for conditional
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
using std::istream;
using std::ostream;
using util::good_bool;
using util::bad_bool;
using util::persistent_object_manager;
template <class> class class_traits;

//=============================================================================
/**
	Generic instance collection type manager for classes with
	full type references as type information.  
	Not appropriate for built-in types.  
 */
template <class Tag>
class parameterless_collection_type_manager {
private:
	typedef	parameterless_collection_type_manager<Tag>	this_type;
	typedef	class_traits<Tag>			traits_type;
protected:
	typedef typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef typename traits_type::type_ref_ptr_type
					type_ref_ptr_type;

	/**
		The type parameter is ONLY a definition.
	 */
	instance_collection_parameter_type		type_parameter;

	struct dumper;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

#if 0
	type_ref_ptr_type
	get_type(void) const;
#endif
	
	// workadound for int
	type_ref_ptr_type
	get_type(const instance_collection_generic_type&) const;

public:
	const instance_collection_parameter_type&
	get_canonical_type(void) const { return this->type_parameter; }

	bool
	is_relaxed_type(void) const { return false; }

	/// enums don't have footprints
	static
	good_bool
	create_definition_footprint(
			const instance_collection_parameter_type& t) {
		return good_bool(true);
	}

protected:
	/**
		NOTE: called during connection checking.  
	 */
	bool
	must_be_collectibly_type_equivalent(const this_type&) const;

	bad_bool
	check_type(const instance_collection_parameter_type&) const;

	/**
		\param t type must be resolved constant.
		\pre first time called for the collection.  
	 */
	void
	commit_type_first_time(const instance_collection_parameter_type& t);

};	// end struct parameterless_collection_type_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_H__

