/**
	\file "Object/ref/simple_meta_instance_reference_base.h"
	Base class family for instance references in HAC.  
	This file was "Object/art_object_inst_ref_base.h"
		in a previous life.  
	$Id: simple_meta_instance_reference_base.h,v 1.8.18.1 2006/02/17 05:07:46 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_BASE_H__

#include "Object/devel_switches.h"
#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
#include <iosfwd>
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"
#include "util/memory/excl_ptr.h"
#else
#include "Object/ref/meta_instance_reference_base.h"
#endif
#include "Object/common/util_types.h"

namespace HAC {
namespace entity {
class scopespace;
class meta_index_list;
class const_index_list;
struct footprint_frame;
class state_manager;
#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
class definition_base;
class fundamental_type_reference;
class instance_collection_base;
struct expr_dump_context;
class const_range_list;
using std::ostream;
using std::istream;
#endif
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
class simple_meta_instance_reference_base
#if !DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
		: virtual public meta_instance_reference_base
#endif
{
#if ENABLE_STATIC_COMPILE_CHECKS
private:
	typedef	simple_meta_instance_reference_base		this_type;
	/**
		Helper class for evaluating sparse, multidimensional
		collections.  
		Virtual base class wrapper around sparse set.  
	 */
	class mset_base;

	template <size_t>
	class mset;
#endif

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

#if ENABLE_STATIC_COMPILE_CHECKS
	/**
		TODO: get rid of this crap in the far future (20060213).
		The current state of the instantiation collection
		at the point of reference.
		Important because the state of an instantiation
		collection may change, so implicit collection or
		sub-collection references with the same indices may 
		refer to different sets.  
	 */
	const instantiation_state		inst_state;
#endif

// for subclasses:
//	never_ptr<const instance_collection_base>	inst_ref;

protected:
	// constructors for children only
	simple_meta_instance_reference_base();

#if ENABLE_STATIC_COMPILE_CHECKS
	explicit
	simple_meta_instance_reference_base(const instantiation_state& st);

	simple_meta_instance_reference_base(excl_ptr<index_list_type>& i, 
		const instantiation_state& st);
#else
	explicit
	simple_meta_instance_reference_base(excl_ptr<index_list_type>&);
#endif
public:

virtual	~simple_meta_instance_reference_base();

#if !DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
	size_t
	dimensions(void) const;

	bool
	may_be_densely_packed(void) const;

	bool
	must_be_densely_packed(void) const;

	bool
	is_static_constant_collection(void) const;

#if ENABLE_STATIC_DIMENSION_ANALYSIS
	bool
	has_static_constant_dimensions(void) const;

	const_range_list
	static_constant_dimensions(void) const;
#endif

	bool
	is_relaxed_formal_dependent(void) const;

	bool
	is_template_dependent(void) const;

	const_index_list
	implicit_static_constant_indices(void) const;
#endif

#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
virtual	good_bool
	attach_indices(excl_ptr<index_list_type>&) = 0;
#else
	good_bool
	attach_indices(excl_ptr<index_list_type>& i);
#endif

#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
	ostream&
	dump_indices(ostream&, const expr_dump_context&) const;
#else
virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	ostream&
	dump_type_size(ostream& o) const;

virtual never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	// need not be virtual
	bool
	may_be_type_equivalent(const meta_instance_reference_base& i) const;

	bool
	must_be_type_equivalent(const meta_instance_reference_base& i) const;
#endif

#if !DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
virtual	LOOKUP_FOOTPRINT_FRAME_PROTO = 0;
#endif

#if ENABLE_STATIC_COMPILE_CHECKS
private:
	// compute static index coverage
	excl_ptr<mset_base>
	unroll_static_instances(const size_t dim) const;
#endif

protected:		// for children only
	// persistent object IO helper methods
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

#if ENABLE_STATIC_COMPILE_CHECKS
private:
	// need help with instantiation state, count?
	void
	write_instance_collection_state(ostream& f) const;

	void
	load_instance_collection_state(istream& f);
#endif
};	// end class simple_meta_instance_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_BASE_H__

