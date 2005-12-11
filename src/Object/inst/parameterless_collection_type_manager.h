/**
	\file "Object/inst/parameterless_collection_type_manager.h"
	Template class for instance_collection's type manager.  
	$Id: parameterless_collection_type_manager.h,v 1.3.20.1 2005/12/11 00:45:38 fang Exp $
 */

#ifndef	__OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_H__
#define	__OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_H__

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
protected:
	typedef typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef typename class_traits<Tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef typename class_traits<Tag>::type_ref_ptr_type
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
	must_match_type(const this_type&) const;

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

#endif	// __OBJECT_INST_PARAMETERLESS_COLLECTION_TYPE_MANAGER_H__

