/**
	\file "Object/art_object_inst_ref_base.h"
	Base class family for instance references in ART.  
	$Id: art_object_inst_ref_base.h,v 1.12.4.4 2005/06/06 09:25:58 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_REF_BASE_H__
#define	__OBJECT_ART_OBJECT_INST_REF_BASE_H__

#include "util/persistent.h"
#include "Object/art_object_nonmeta_inst_ref_base.h"
#include "Object/art_object_expr_const.h"	// for const_range_list
#include "Object/art_object_util_types.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class definition_base;
class fundamental_type_reference;
class instance_collection_base;
class param_instance_collection;
using std::istream;
using std::ostream;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::memory::count_ptr;

//=============================================================================
/**
	PHASE this back into what is currently simple_meta_instance_reference_base.  
	Base class for anything that *refers* to an instance, 
	or collection thereof.  
	Instance reference should be cacheable?

	Why derived from object? to be pushable onto an object stack... arg!
	We need separate stacks...
	See NOTES.
 */
class meta_instance_reference_base : 
		virtual public nonmeta_instance_reference_base {
	typedef	nonmeta_instance_reference_base		parent_type;
public:
	meta_instance_reference_base() : parent_type() { }

virtual	~meta_instance_reference_base() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	ostream&
	dump_type_size(ostream& o) const = 0;

#if 0
// only simple instance reference have a single base collection
virtual never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;
#endif

virtual	size_t
	dimensions(void) const = 0;

virtual	never_ptr<const definition_base>
	get_base_def(void) const = 0;

virtual	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const = 0;

virtual	bool
	may_be_densely_packed(void) const = 0;

virtual	bool
	must_be_densely_packed(void) const = 0;

virtual	bool
	has_static_constant_dimensions(void) const = 0;

virtual	const_range_list
	static_constant_dimensions(void) const = 0;

virtual	bool
	may_be_type_equivalent(const meta_instance_reference_base& i) const = 0;

virtual	bool
	must_be_type_equivalent(const meta_instance_reference_base& i) const = 0;

	/**
		Start an aliases connection list based on the referenced type.  
		We have the option of adding the first element to the list...
		NOTE: connections are only made in the meta-language.  
	 */
	static
	excl_ptr<aliases_connection_base>
	make_aliases_connection(
		const count_ptr<const meta_instance_reference_base>&);

private:
virtual	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const = 0;
};	// end class meta_instance_reference_base

//=============================================================================
#if 0
PHASE OUT, or needs a facelift
	EVOLVE INTO: complex_aggregate_meta_instance_reference, muhahahaha!
/// in favor of using generic (simple/complex_aggregate) instance references
//	all have potential indices, forget hierarchy
// scheme has much changed since this idea was proposed...
/**
	Reference to an array (one-level) of instances.  
	Self-reference is acceptable and intended for multidimensional
	array element references.  
 */
class collective_meta_instance_reference : public meta_instance_reference_base {
protected:
	// owned? no belongs to cache, even if multidimensional
	// may also be collective
	never_ptr<const meta_instance_reference_base>	base_array;
	never_ptr<const param_expr>			lower_index;
	never_ptr<const param_expr>			upper_index;
public:
	collective_meta_instance_reference(
		never_ptr<const meta_instance_reference_base> b, 
		const param_expr* l = NULL, const param_expr* r = NULL);

virtual	~collective_meta_instance_reference();

virtual	ostream&
	what(ostream& o) const;

virtual	ostream&
	dump(ostream& o) const;

virtual	string
	hash_string(void) const;
};	// end class collective_meta_instance_reference
#endif

//=============================================================================
#if 0
// EXPERIMENTAL

class indexable_meta_instance_reference : virtual public meta_instance_reference_base {

};	// end class indexable_meta_instance_reference

//=============================================================================
class process_meta_instance_reference_base : virtual public meta_instance_reference_base {
protected:
	never_ptr<const process_instance_collection>	proc_inst;
public:

};	// end class process_meta_instance_reference_base
#endif

//=============================================================================
/**
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
class simple_meta_instance_reference_base :
		virtual public meta_instance_reference_base {
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
	excl_ptr<meta_index_list>		array_indices;

	/**
		The current state of the instantiation collection
		at the point of reference.
		Important because the state of an instantiation
		collection may change, so implicit collection or
		sub-collection references with the same indices may 
		refer to different sets.  
	 */
	const instantiation_state		inst_state;

// for subclasses:
//	never_ptr<const instance_collection_base>	inst_ref;

protected:
	simple_meta_instance_reference_base();
public:
	explicit
	simple_meta_instance_reference_base(const instantiation_state& st);

	simple_meta_instance_reference_base(excl_ptr<meta_index_list>& i, 
		const instantiation_state& st);

virtual	~simple_meta_instance_reference_base();

	size_t
	dimensions(void) const;

	bool
	may_be_densely_packed(void) const;

	bool
	must_be_densely_packed(void) const;

	bool
	is_static_constant_collection(void) const;

	bool
	has_static_constant_dimensions(void) const;

	const_range_list
	static_constant_dimensions(void) const;

	const_index_list
	implicit_static_constant_indices(void) const;

	good_bool
	attach_indices(excl_ptr<meta_index_list>& i);

virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	dump_briefer(ostream& o, const never_ptr<const scopespace>) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

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

private:
	// compute static index coverage
	excl_ptr<mset_base>
	unroll_static_instances(const size_t dim) const;

protected:		// for children only
	// persistent object IO helper methods
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

private:
	// need help with instantiation state, count?
	void
	write_instance_collection_state(ostream& f) const;

	void
	load_instance_collection_state(istream& f);
};	// end class simple_meta_instance_reference_base

//=============================================================================
/**
	A reference to a simple instance of parameter.  
	Abstract base class.  
 */
class param_meta_instance_reference : public simple_meta_instance_reference_base {
protected:
	typedef	simple_meta_instance_reference_base	parent_type;
//	excl_ptr<meta_index_list>		array_indices;	// inherited

protected:
	param_meta_instance_reference();
public:
	explicit
	param_meta_instance_reference(const instantiation_state& st);

virtual	~param_meta_instance_reference() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;

virtual	never_ptr<const param_instance_collection>
	get_param_inst_base(void) const = 0;

	// consider moving these functions into meta_instance_reference_base
	//	where array_indices are inherited from.  
	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

	bool
	is_static_constant(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

};	// end class param_meta_instance_reference

//=============================================================================
// classes pint_meta_instance_reference and pbool_meta_instance_reference
//	are in "art_object_expr_param_ref.*"

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_REF_BASE_H__

