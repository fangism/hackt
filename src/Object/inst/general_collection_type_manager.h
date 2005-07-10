/**
	\file "Object/inst/general_collection_type_manager.h"
	Template class for instance_collection's type manager.  
	$Id: general_collection_type_manager.h,v 1.1.2.1 2005/07/10 19:37:27 fang Exp $
 */

#ifndef	__OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_H__
#define	__OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
using std::istream;
using std::ostream;
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
class general_collection_type_manager {
private:
	typedef	general_collection_type_manager<Tag>	this_type;
protected:
	typedef typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef typename class_traits<Tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef typename class_traits<Tag>::type_ref_ptr_type
					type_ref_ptr_type;

	/**
		General type reference pointer for the collection.  
	 */
	instance_collection_parameter_type		type_parameter;

	struct dumper;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	const type_ref_ptr_type&
	get_type(void) const { return this->type_parameter; }
	
	/**
		Workaround for int case.  
	 */
	const type_ref_ptr_type&
	get_type(const instance_collection_generic_type&) const {
		return this->type_parameter;
	}
	
	bool
	is_relaxed_type(void) const;

	/**
		NOTE: called during connection checking.  
	 */
	bool
	must_match_type(const this_type&) const;

	// TODO: rename me!!!
	bad_bool
	commit_type(const type_ref_ptr_type&) const;

	/**
		\param t type must be resolved constant.
		\pre first time called for the collection.  
	 */
	void
	commit_type_first_time(const type_ref_ptr_type& t);

};	// end struct general_collection_type_manager

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_H__

