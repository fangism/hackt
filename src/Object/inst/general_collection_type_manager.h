/**
	\file "Object/inst/general_collection_type_manager.h"
	Template class for instance_collection's type manager.  
	$Id: general_collection_type_manager.h,v 1.9.8.1 2006/09/06 04:19:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_H__
#define	__HAC_OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_H__

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

#include <iosfwd>
#include "Object/type/canonical_type.h"
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"
#include "Object/devel_switches.h"

#if	ENABLE_STACKTRACE
#include "util/stacktrace.h"
#endif

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
class general_collection_type_manager {
private:
	typedef	general_collection_type_manager<Tag>	this_type;
	typedef	class_traits<Tag>			traits_type;
protected:
	typedef typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef typename traits_type::type_ref_ptr_type
					type_ref_ptr_type;
	typedef	typename type_ref_ptr_type::element_type
					type_ref_type;
#if USE_RESOLVED_DATA_TYPES
	typedef typename traits_type::resolved_type_ref_type
					resolved_type_ref_type;
#endif

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

#if 0
	type_ref_ptr_type
	get_type(void) const {
		return this->type_parameter.make_type_reference();
	}
#endif

#if !USE_INSTANCE_PLACEHOLDERS
	// problem: channels have both built-in and user-defined types.  
	type_ref_ptr_type
	get_type(const instance_collection_generic_type&) const {
		return this->type_parameter.make_type_reference();
	}
#endif

public:
#if USE_RESOLVED_DATA_TYPES
	const instance_collection_parameter_type&
	__get_raw_type(void) const {
#if ENABLE_STACKTRACE
		const instance_collection_parameter_type&
			ret(this->type_parameter);
		ret.dump(STACKTRACE_INDENT << "canonical type: ") << endl;
		return ret;
#else
		return this->type_parameter;
#endif
	}

	resolved_type_ref_type
	get_resolved_canonical_type(void) const {
		return this->type_parameter;
	}
#else	// USE_RESOLVED_DATA_TYPES
	const instance_collection_parameter_type&
	get_canonical_type(void) const {
#if ENABLE_STACKTRACE
		const instance_collection_parameter_type&
			ret(this->type_parameter);
		ret.dump(STACKTRACE_INDENT << "canonical type: ") << endl;
		return ret;
#else
		return this->type_parameter;
#endif
	}
#endif	// USE_RESOLVED_DATA_TYPES

	bool
	is_relaxed_type(void) const;

	static
	good_bool
	create_definition_footprint(
			const instance_collection_parameter_type& t) {
		return t.create_definition_footprint();
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

};	// end struct general_collection_type_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_H__

