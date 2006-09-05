/**
	\file "Object/inst/int_collection_type_manager.h"
	Template class for instance_collection's type manager.  
	$Id: int_collection_type_manager.h,v 1.9.8.1.2.1 2006/09/05 23:32:18 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_H__
#define	__HAC_OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_H__

#include <iostream>
#include "Object/type/canonical_type_fwd.h"	// for conditional
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"
#include "Object/expr/types.h"		// for pint_value_type
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
using std::istream;
using std::ostream;
#include "util/using_ostream.h"
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
class int_collection_type_manager {
private:
	typedef	int_collection_type_manager<Tag>	this_type;
	typedef	class_traits<Tag>			traits_type;
protected:
	typedef typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
#if USE_INSTANCE_PLACEHOLDERS
	typedef typename traits_type::instance_placeholder_type
					instance_placeholder_type;
#endif
	typedef typename traits_type::type_ref_ptr_type
					type_ref_ptr_type;

	/**
		Type is a constant integer.  
	 */
	instance_collection_parameter_type		type_parameter;

	/**
		Width is zero before it is determined at unroll-time.
		Is this the correct thing to do?
		TODO: fix me for good.
	 */
	int_collection_type_manager() : type_parameter(0) { }

	struct dumper;

	void
	collect_transient_info_base(persistent_object_manager&) const { }

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

#if 1
	type_ref_ptr_type
	get_type(void) const;
	
	// because we may need to extract from the index_collection
	type_ref_ptr_type
	get_type(const instance_collection_generic_type&) const;
#endif

public:
#if USE_RESOLVED_DATA_TYPES
	const instance_collection_parameter_type&
	__get_raw_type(void) const { return type_parameter; }
#else
	const instance_collection_parameter_type&
	get_canonical_type(void) const { return type_parameter; }
#endif

	bool
	is_relaxed_type(void) const { return false; }

	/**
		ints don't have footprints.
	 */
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
	must_be_collectibly_type_equivalent(const this_type& t) const {
		if (type_parameter == t.type_parameter) {
			return true;
		} else {
			cerr << "ERROR: integer widths do not match!  "
				"have: " << type_parameter << " vs. " <<
				t.type_parameter << endl;
			return false;
		}
	}


	bad_bool
	check_type(const instance_collection_parameter_type&) const;

	/**
		\param t type must be resolved constant.
		\pre first time called for the collection.  
	 */
	good_bool
	commit_type_first_time(const instance_collection_parameter_type& t);

private:
	static
	pint_value_type
	get_int_width(const type_ref_ptr_type&);

};	// end struct int_collection_type_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_H__

