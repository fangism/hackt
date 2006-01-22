/**
	\file "Object/inst/null_collection_type_manager.h"
	Template class for instance_collection's type manager.  
	$Id: null_collection_type_manager.h,v 1.5 2006/01/22 18:20:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_H__
#define	__HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_H__

#include <iosfwd>
#include "Object/type/canonical_type_fwd.h"	// just for conditional
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
	Appropriate for built-in types with no template parameters.  
	Pretty much only useful to bool.  
 */
template <class Tag>
class null_collection_type_manager {
private:
	typedef	null_collection_type_manager<Tag>	this_type;
protected:
	typedef typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef typename class_traits<Tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef typename class_traits<Tag>::type_ref_ptr_type
					type_ref_ptr_type;

	// has no type parameter

	struct dumper;

	void
	collect_transient_info_base(persistent_object_manager&) const { }

	void
	write_object_base(const persistent_object_manager&, ostream&) const { }

	void
	load_object_base(const persistent_object_manager&, istream&) { }

	const type_ref_ptr_type&
	get_type(void) const;
	
	const type_ref_ptr_type&
	get_type(const instance_collection_generic_type&) const {
		return this->get_type();
	}

public:
	instance_collection_parameter_type
	get_canonical_type(void) const {
		return instance_collection_parameter_type();
	}

	bool
	is_relaxed_type(void) const { return false; }

	// bool doesn't have a footprint
	static
	good_bool
	create_definition_footprint(
			const instance_collection_parameter_type& t) {
		return good_bool(true);
	}

protected:
	bool
	must_match_type(const this_type&) const { return true; }

	bad_bool
	check_type(const instance_collection_parameter_type&) const
		{ return bad_bool(false); }

	/**
		\param t type must be resolved constant.
		\pre first time called for the collection.  
	 */
	void
	commit_type_first_time(
		const instance_collection_parameter_type& t) const { }

};	// end struct null_collection_type_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_H__

