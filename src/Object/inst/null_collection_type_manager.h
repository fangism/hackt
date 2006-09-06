/**
	\file "Object/inst/null_collection_type_manager.h"
	Template class for instance_collection's type manager.  
	$Id: null_collection_type_manager.h,v 1.8.8.1 2006/09/06 04:19:48 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_H__
#define	__HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_H__

#include <iosfwd>
#include "Object/type/canonical_type_fwd.h"	// just for conditional
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"
#include "Object/devel_switches.h"

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
	TODO: write a quick raw-type comparison without having to construct
		canonical type.  
 */
template <class Tag>
class null_collection_type_manager {
private:
	typedef	null_collection_type_manager<Tag>	this_type;
	typedef	class_traits<Tag>			traits_type;
protected:
	typedef typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef typename traits_type::type_ref_ptr_type
					type_ref_ptr_type;
#if USE_RESOLVED_DATA_TYPES
	typedef typename traits_type::resolved_type_ref_type
					resolved_type_ref_type;
#endif

	// has no type parameter

	struct dumper;

	void
	collect_transient_info_base(persistent_object_manager&) const { }

	void
	write_object_base(const persistent_object_manager&, ostream&) const { }

	void
	load_object_base(const persistent_object_manager&, istream&) { }

#if 0
	const type_ref_ptr_type&
	get_type(void) const;
#endif

#if !USE_INSTANCE_PLACEHOLDERS
	const type_ref_ptr_type&
	get_type(const instance_collection_generic_type&) const {
		return this->get_type();
	}
#endif

public:
#if USE_RESOLVED_DATA_TYPES
	// distinguish internal type from canonical_type

	/**
		Only used internally with collections.  
	 */
	instance_collection_parameter_type
	__get_raw_type(void) const {
		return instance_collection_parameter_type();
	}

	resolved_type_ref_type
	get_resolved_canonical_type(void) const;
#else
	instance_collection_parameter_type
	get_canonical_type(void) const {
		return instance_collection_parameter_type();
	}
#endif

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
	must_be_collectibly_type_equivalent(const this_type&) const {
		return true;
	}

	bad_bool
	check_type(const instance_collection_parameter_type&) const
		{ return bad_bool(false); }

	/**
		\param t type must be resolved constant.
		\pre first time called for the collection.  
	 */
	good_bool
	commit_type_first_time(
		const instance_collection_parameter_type& t) const {
		return good_bool(true);
	}

};	// end struct null_collection_type_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_H__

