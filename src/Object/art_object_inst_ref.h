// "art_object_inst_ref.h"

#ifndef	__ART_OBJECT_INST_REF_H__
#define	__ART_OBJECT_INST_REF_H__

#include "art_macros.h"
#include "art_object_base.h"
#include "multidimensional_sparse_set_fwd.h"
#include "count_ptr.h"

namespace ART {
//=============================================================================
// forward declarations from outside namespaces
namespace parser {
	// note: methods may specify string as formal types, 
	// but you can still pass token_identifiers and token_strings
	// because they are derived from string.
	class token_string;
	class token_identifier;
	class qualified_id_slice;
	class qualified_id;
	class context;
}
using namespace parser;

//=============================================================================
/**
	The namespace of objects that will be returned by the type-checker, 
	and includes the various hierarchical symbol tables in their 
	respective scopes.  
 */
namespace entity {
//=============================================================================
	using namespace std;
	using namespace PTRS_NAMESPACE;	// for experimental pointer classes
	using namespace COUNT_PTR_NAMESPACE;
	using namespace MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE;

//=============================================================================
/**
	PHASE this back into what is currently simple_instance_reference.  
	Base class for anything that *refers* to an instance, 
	or collection thereof.  
	Instance reference should be cacheable?
 */
class instance_reference_base : virtual public object {
public:
	instance_reference_base() : object() { }
virtual	~instance_reference_base() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual	ostream& dump_type_size(ostream& o) const = 0;
virtual never_const_ptr<instantiation_base> get_inst_base(void) const = 0;
virtual	string hash_string(void) const = 0;
virtual	size_t dimensions(void) const = 0;
virtual	never_const_ptr<definition_base> get_base_def(void) const = 0;
virtual	count_const_ptr<fundamental_type_reference>
		get_type_ref(void) const = 0;
virtual	bool may_be_densely_packed(void) const = 0;
virtual	bool must_be_densely_packed(void) const = 0;
virtual	bool has_static_constant_dimensions(void) const = 0;
virtual	const_range_list static_constant_dimensions(void) const = 0;
virtual	bool may_be_type_equivalent(
		const instance_reference_base& i) const = 0;
virtual	bool must_be_type_equivalent(
		const instance_reference_base& i) const = 0;
};	// end class instance_reference_base

//=============================================================================
#if 0
PHASE OUT, or needs a facelift
	EVOLVE INTO: complex_aggregate_instance_reference, muhahahaha!
/// in favor of using generic (simple/complex_aggregate) instance references
//	all have potential indices, forget hierarchy
// scheme has much changed since this idea was proposed...
/**
	Reference to an array (one-level) of instances.  
	Self-reference is acceptable and intended for multidimensional
	array element references.  
 */
class collective_instance_reference : public instance_reference_base {
protected:
	// owned? no belongs to cache, even if multidimensional
	// may also be collective
	never_const_ptr<instance_reference_base>	base_array;
	never_const_ptr<param_expr>			lower_index;
	never_const_ptr<param_expr>			upper_index;
public:
	collective_instance_reference(
		never_const_ptr<instance_reference_base> b, 
		const param_expr* l = NULL, const param_expr* r = NULL);
virtual	~collective_instance_reference();

virtual	ostream& what(ostream& o) const;
virtual	ostream& dump(ostream& o) const;
virtual	string hash_string(void) const;
};	// end class collective_instance_reference
#endif

//=============================================================================
#if 0
// EXPERIMENTAL

class indexable_instance_reference : virtual public instance_reference_base {

};	// end class indexable_instance_reference

//=============================================================================
class process_instance_reference_base : virtual public instance_reference_base {
protected:
	never_const_ptr<process_instantiation>	proc_inst;
public:

};	// end class process_instance_reference_base
#endif

//=============================================================================
/**
	PHASE THIS back into instance_reference_base.
	OR... call this "simple_instance_reference" instead.  
		and replace collective_instance_reference with
		complex_aggregate_instance_reference...
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
class simple_instance_reference : virtual public instance_reference_base {
private:
	/**
		Helper class for evaluating sparse, multidimensional
		collections.  
	 */
	typedef	base_multidimensional_sparse_set<int, const_range>
						mset_base;
protected:
	/**
		The indices (optional) for this particular reference.
		Why modifiable pointer?
		May need method to deep-copy the indices, unless
		this pointer becomes counted.  
	 */
	excl_ptr<index_list>			array_indices;
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
//	never_const_ptr<instantiation_base>	inst_ref;

protected:
	simple_instance_reference();
public:
	simple_instance_reference(excl_ptr<index_list> i, 
		const instantiation_state& st);
virtual	~simple_instance_reference();

	size_t dimensions(void) const;
	bool may_be_densely_packed(void) const;
	bool must_be_densely_packed(void) const;
	bool is_static_constant_collection(void) const;
	bool has_static_constant_dimensions(void) const;
	const_range_list static_constant_dimensions(void) const;
	const_index_list implicit_static_constant_indices(void) const;

	bool attach_indices(excl_ptr<index_list> i);

virtual	ostream& what(ostream& o) const = 0;
	ostream& dump(ostream& o) const;
	ostream& dump_type_size(ostream& o) const;
virtual never_const_ptr<instantiation_base> get_inst_base(void) const = 0;
	count_const_ptr<fundamental_type_reference> get_type_ref(void) const;
	never_const_ptr<definition_base> get_base_def(void) const;
virtual	string hash_string(void) const;
	// need not be virtual
	bool may_be_type_equivalent(const instance_reference_base& i) const;
	bool must_be_type_equivalent(const instance_reference_base& i) const;

private:
	// compute static index coverage
	excl_ptr<mset_base> unroll_static_instances(const size_t dim) const;
public:
	// persistent object IO helper methods
	// need help with instantiation state, count?
	void write_instantiation_state(ostream& f) const;
	void load_instantiation_state(istream& f);
};	// end class simple_instance_reference

//=============================================================================
/**
	Abstract interface class for member instance references.  
	Make type-specific {process,data,channel}?
	Don't need instantiation_state because members, can only
	refer to ports, which cannot be appended with more instances
	and indices.  
 */
class member_instance_reference_base : virtual public instance_reference_base {
protected:
	/** The owning base instance, 
		must have dimension-0, scalar... for now
	 */
	const count_const_ptr<simple_instance_reference>	base;
protected:
	member_instance_reference_base();
public:
	member_instance_reference_base(
		count_const_ptr<simple_instance_reference> b);
virtual	~member_instance_reference_base();

#if 0
	size_t dimensions(void) const;
	bool may_be_densely_packed(void) const;
	bool must_be_densely_packed(void) const;
	bool is_static_constant_collection(void) const;
	bool has_static_constant_dimensions(void) const;
	const_range_list static_constant_dimensions(void) const;
	const_index_list implicit_static_constant_indices(void) const;

	bool attach_indices(excl_ptr<index_list> i);

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
//	string hash_string(void) const;
	bool may_be_type_equivalent(const instance_reference_base& i) const;
	bool must_be_type_equivalent(const instance_reference_base& i) const;
#endif

};	// end class member_instance_reference

//=============================================================================
/**
	A reference to a simple instance of datatype.  
 */
class datatype_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	const never_const_ptr<datatype_instantiation>	data_inst_ref;

protected:
	datatype_instance_reference();
public:
	datatype_instance_reference(never_const_ptr<datatype_instantiation> di, 
		excl_ptr<index_list> i);
virtual	~datatype_instance_reference();

virtual	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;

public:
	// need to be virtual? for member_instance_reference?
	ART_OBJECT_IO_METHODS
};	// end class datatype_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a simple instance of channel.  
 */
class channel_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	const never_const_ptr<channel_instantiation>	channel_inst_ref;

protected:
	channel_instance_reference();
public:
	channel_instance_reference(never_const_ptr<channel_instantiation> ci, 
		excl_ptr<index_list> i);
virtual	~channel_instance_reference();

virtual	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;

public:
	// need to be virtual? for member_instance_reference?
	ART_OBJECT_IO_METHODS
};	// end class channel_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a simple instance of process.  
 */
class process_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	const never_const_ptr<process_instantiation>	process_inst_ref;

protected:
	process_instance_reference();
public:
	process_instance_reference(never_const_ptr<process_instantiation> pi, 
		excl_ptr<index_list> i);
virtual	~process_instance_reference();

virtual	ostream& what(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;

public:
	// need to be virtual? for member_instance_reference?
	ART_OBJECT_IO_METHODS
};	// end class process_instance_reference

//=============================================================================
/**
	Reference to a process instance member of something else.
	Derive from some generic member_instance_reference?
 */
class process_member_instance_reference :
		public member_instance_reference_base, 
		public process_instance_reference {
protected:
// inherited:
//	excl_ptr<index_list>			array_indices;
//	const never_const_ptr<process_instantiation>	process_inst_ref;
//	const count_const_ptr<simple_instance_reference>	base;
private:
	process_member_instance_reference();
public:
	process_member_instance_reference(
		count_const_ptr<simple_instance_reference> b, 
		never_const_ptr<process_instantiation> m);
	~process_member_instance_reference();

	ostream& what(ostream& o) const;
// can also attach indices!

public:
	ART_OBJECT_IO_METHODS
};	// end class process_member_instance_reference

//=============================================================================
/**
	Reference to a datatype instance member of another struct.  
 */
class datatype_member_instance_reference : 
		public member_instance_reference_base, 
		public datatype_instance_reference {
protected:
// inherited:
//	excl_ptr<index_list>			array_indices;
//	const never_const_ptr<datatype_instantiation>	data_inst_ref;
//	const count_const_ptr<simple_instance_reference>	base;
private:
	datatype_member_instance_reference();
public:
	datatype_member_instance_reference(
		count_const_ptr<simple_instance_reference> b, 
		never_const_ptr<datatype_instantiation> m);
	~datatype_member_instance_reference();

	ostream& what(ostream& o) const;
// can also attach indices!

public:
	ART_OBJECT_IO_METHODS

};	// end class datatype_member_instance_reference

//=============================================================================
/**
	Reference to a channel instance member of another struct.  
 */
class channel_member_instance_reference : 
		public member_instance_reference_base, 
		public channel_instance_reference {
protected:
// inherited:
//	excl_ptr<index_list>			array_indices;
//	const never_const_ptr<channel_instantiation>	channel_inst_ref;
//	const count_const_ptr<simple_instance_reference>	base;
private:
	channel_member_instance_reference();
public:
	channel_member_instance_reference(
		count_const_ptr<simple_instance_reference> b, 
		never_const_ptr<channel_instantiation> m);
	~channel_member_instance_reference();

	ostream& what(ostream& o) const;
// can also attach indices!

public:
	ART_OBJECT_IO_METHODS

};	// end class channel_member_instance_reference

//=============================================================================
/**
	A reference to a simple instance of parameter.  
 */
class param_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited

// virtualized
//	never_ptr<param_instantiation>		param_inst_ref;

protected:
	param_instance_reference();
public:
	param_instance_reference(excl_ptr<index_list> i, 
		const instantiation_state& st);
virtual	~param_instance_reference() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	never_const_ptr<instantiation_base>
		get_inst_base(void) const = 0;
virtual	never_const_ptr<param_instantiation>
		get_param_inst_base(void) const = 0;

	// consider moving these functions into instance_reference_base
	//	where array_indices are inherited from.  
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;

virtual	bool initialize(count_const_ptr<param_expr> i) = 0;

};	// end class param_instance_reference

//=============================================================================
// classes pint_instance_reference and pbool_instance_reference
//	are in "art_object_expr.*"

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_H__

