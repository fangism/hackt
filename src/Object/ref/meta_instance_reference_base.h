/**
	\file "Object/ref/meta_instance_reference_base.h"
	Base class family for instance references in HAC.  
	This file was "Object/art_object_inst_ref_base.h"
		in a previous life.  
	$Id: meta_instance_reference_base.h,v 1.4.12.1 2005/12/11 00:45:45 fang Exp $
 */

#ifndef	__OBJECT_REF_META_INSTANCE_REFERENCE_BASE_H__
#define	__OBJECT_REF_META_INSTANCE_REFERENCE_BASE_H__

#include "Object/ref/nonmeta_instance_reference_base.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "Object/inst/substructure_alias_fwd.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
class scopespace;
class definition_base;
class fundamental_type_reference;
class instance_collection_base;
class aliases_connection_base;
class const_range_list;
class unroll_context;
using util::bad_bool;
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
	dump(ostream& o, const expr_dump_context&) const = 0;

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

// what kind of type equivalence?
virtual	bool
	may_be_type_equivalent(
		const meta_instance_reference_base& i) const = 0;

// what kind of type equivalence?
virtual	bool
	must_be_type_equivalent(
		const meta_instance_reference_base& i) const = 0;

	/**
		Start an aliases connection list based on the referenced type.  
		We have the option of adding the first element to the list...
		NOTE: connections are only made in the meta-language.  
	 */
	static
	excl_ptr<aliases_connection_base>
	make_aliases_connection(
		const count_ptr<const meta_instance_reference_base>&);

/**
	The implementation of this will be policy-determined, 
	by substructure_alias_base<bool>.  
 */
#define	UNROLL_GENERIC_SCALAR_REFERENCE_PROTO				\
	never_ptr<substructure_alias>					\
	unroll_generic_scalar_reference(const unroll_context&) const

virtual	UNROLL_GENERIC_SCALAR_REFERENCE_PROTO = 0;

#define	CONNECT_PORT_PROTO						\
	bad_bool							\
	connect_port(instance_collection_base&, 			\
		const unroll_context&) const

virtual	CONNECT_PORT_PROTO = 0;

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
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_REF_META_INSTANCE_REFERENCE_BASE_H__

