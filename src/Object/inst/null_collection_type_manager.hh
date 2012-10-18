/**
	\file "Object/inst/null_collection_type_manager.hh"
	Template class for instance_collection's type manager.  
	$Id: null_collection_type_manager.hh,v 1.12 2007/04/15 05:52:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_H__
#define	__HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_H__

#include <iosfwd>
#include "Object/type/canonical_type_fwd.hh"	// just for conditional
#include "util/persistent_fwd.hh"
#include "util/boolean_types.hh"
#include "util/memory/pointer_classes_fwd.hh"

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
template <class> struct class_traits;

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
	typedef typename traits_type::resolved_type_ref_type
					resolved_type_ref_type;

	// has no type parameter

	struct dumper;

	void
	collect_transient_info_base(persistent_object_manager&) const { }

	void
	write_object_base(const persistent_object_manager&, ostream&) const { }

	void
	load_object_base(const persistent_object_manager&, istream&) { }

public:
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

	good_bool
	complete_type_definition_footprint(
			const count_ptr<const const_param_expr_list>&) const {
		return good_bool(true);
	}

	bool
	is_complete_type(void) const { return true; }

	bool
	is_relaxed_type(void) const { return false; }

	// bool doesn't have a footprint
	static
	good_bool
	create_definition_footprint(
			const instance_collection_parameter_type&, 
			const footprint& /* top */) {
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
		const instance_collection_parameter_type&) const {
		return good_bool(true);
	}

};	// end struct null_collection_type_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_NULL_COLLECTION_TYPE_MANAGER_H__

