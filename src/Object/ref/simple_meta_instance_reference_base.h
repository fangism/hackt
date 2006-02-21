/**
	\file "Object/ref/simple_meta_instance_reference_base.h"
	Base class family for instance references in HAC.  
	This file was "Object/art_object_inst_ref_base.h"
		in a previous life.  
	$Id: simple_meta_instance_reference_base.h,v 1.9 2006/02/21 04:48:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_BASE_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"
#include "util/memory/excl_ptr.h"
#include "Object/common/util_types.h"

namespace HAC {
namespace entity {
class scopespace;
class meta_index_list;
class const_index_list;
struct footprint_frame;
class state_manager;
class definition_base;
class fundamental_type_reference;
class instance_collection_base;
struct expr_dump_context;
class const_range_list;
using std::ostream;
using std::istream;
using util::good_bool;
using util::memory::excl_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	20060213: this class is now an implementation base for
		any reference that may be indexed, be it instance or value.  
	Plan: remove anything from this class that ties it to instances.  
	Possibly define template helper methods.  
	TODO: many of the functions no longer belong here.  
	Most methods in here should be helpers.  

	PHASE THIS back into meta_instance_reference_base.
	OR... call this "simple_meta_instance_reference_base" instead.  
		and replace collective_meta_instance_reference with
		complex_aggregate_meta_instance_reference...
	Base class for a reference to a particular instance.  
	Where a particular instance, either array or single, is 
	connected or aliased, this object refers to a simple instance
	of a datatype, channel, or process.  
	To check, that the instance references was actually in the 
	dimension range of the array declared.  
	Collection, bundle?
	Sub-classes must contain a const pointer to the appropriate
	subclass of fundamental_type_reference.  
	Should these be hashed into used_id_map?
		Will there be identifier conflicts?
 */
class simple_meta_instance_reference_base {
private:
	typedef	simple_meta_instance_reference_base		this_type;

	template <bool>
	struct has_substructure { };
public:
	typedef	meta_index_list			index_list_type;
protected:
	/**
		The indices (optional) for this particular reference.
		Why modifiable pointer?
		May need method to deep-copy the indices, unless
		this pointer becomes counted.  

		This pointer may be null, when no explicit indices are given, 
		not necessarily zero-dimensional (scalar).
		Could be implicit reference to entire collection.  
	 */
	excl_ptr<index_list_type>		array_indices;

// for subclasses:
//	never_ptr<const instance_collection_base>	inst_ref;

protected:
	// constructors for children only
	simple_meta_instance_reference_base();

	explicit
	simple_meta_instance_reference_base(excl_ptr<index_list_type>&);

public:

virtual	~simple_meta_instance_reference_base();

virtual	good_bool
	attach_indices(excl_ptr<index_list_type>&) = 0;

	ostream&
	dump_indices(ostream&, const expr_dump_context&) const;

protected:		// for children only
	// persistent object IO helper methods
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

};	// end class simple_meta_instance_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_BASE_H__

