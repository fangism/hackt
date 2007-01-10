/**
	\file "Object/inst/channel_collection_type_manager.h"
	Template class for instance_collection's type manager.  
	$Id: channel_collection_type_manager.h,v 1.1.2.1 2007/01/10 20:14:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CHANNEL_COLLECTION_TYPE_MANAGER_H__
#define	__HAC_OBJECT_INST_CHANNEL_COLLECTION_TYPE_MANAGER_H__

#include <iosfwd>
#include "Object/type/canonical_type_fwd.h"	// for conditional
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"
#include "util/memory/pointer_classes_fwd.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
class const_param_expr_list;
using std::istream;
using std::ostream;
using util::good_bool;
using util::bad_bool;
using util::persistent_object_manager;
using util::memory::count_ptr;
class footprint;
template <class> class class_traits;

//=============================================================================
/**
	Generic instance collection type manager for classes with
	full type references as type information.  
	Not appropriate for built-in types.  
 */
template <class Tag>
class channel_collection_type_manager {
private:
	typedef	channel_collection_type_manager<Tag>	this_type;
	typedef	class_traits<Tag>			traits_type;
protected:
	typedef typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef typename traits_type::type_ref_ptr_type
					type_ref_ptr_type;
	typedef typename traits_type::resolved_type_ref_type
					resolved_type_ref_type;

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

public:
	const instance_collection_parameter_type&
	__get_raw_type(void) const { return this->type_parameter; }

	resolved_type_ref_type
	get_resolved_canonical_type(void) const;

	/**
		No-op, channels don't have process footprints.  
	 */
	good_bool
	complete_type_definition_footprint(
			const count_ptr<const const_param_expr_list>& r) const {
		return good_bool(true);
	}

	bool
	is_complete_type(void) const { return this->type_parameter; }

	bool
	is_relaxed_type(void) const { return false; }

	/// built-in channels don't have process footprints
	static
	good_bool
	create_definition_footprint(
			const instance_collection_parameter_type& t, 
			const footprint& top) {
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
	good_bool
	commit_type_first_time(const instance_collection_parameter_type& t);

};	// end struct channel_collection_type_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_CHANNEL_COLLECTION_TYPE_MANAGER_H__

