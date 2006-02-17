/**
	\file "Object/ref/meta_instance_reference_base.h"
	Base class family for instance references in HAC.  
	This file was "Object/art_object_inst_ref_base.h"
		in a previous life.  
	$Id: meta_instance_reference_base.h,v 1.7.16.1.2.1 2006/02/17 05:07:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_BASE_H__

#include "Object/devel_switches.h"
#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
#include "util/persistent.h"
#else
#include "Object/ref/nonmeta_instance_reference_base.h"
#endif
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "Object/inst/substructure_alias_fwd.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
class scopespace;
struct footprint_frame;
struct expr_dump_context;
class state_manager;
class definition_base;
class fundamental_type_reference;
class instance_collection_base;
class aliases_connection_base;
class port_connection_base;
class const_range_list;
class unroll_context;
class aggregate_meta_instance_reference_base;
using util::bad_bool;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::memory::count_ptr;
using std::istream;
using std::ostream;
using util::persistent;

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
#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
		virtual public persistent
#else
		virtual public nonmeta_instance_reference_base
#endif
{
#if !DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
	typedef	nonmeta_instance_reference_base		parent_type;
#endif
	typedef	meta_instance_reference_base		this_type;
public:
#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
	meta_instance_reference_base() : persistent() { }
#else
	meta_instance_reference_base() : parent_type() { }
#endif

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

#if 0
virtual	bool
	may_be_densely_packed(void) const = 0;

virtual	bool
	must_be_densely_packed(void) const = 0;
#endif

#if 0
// is his actually needed
virtual	bool
	has_static_constant_dimensions(void) const = 0;
#endif

#if 0
virtual	const_range_list
	static_constant_dimensions(void) const = 0;
#endif

// what kind of type equivalence?
virtual	bool
	may_be_type_equivalent(const this_type&) const = 0;

// what kind of type equivalence?
virtual	bool
	must_be_type_equivalent(const this_type&) const = 0;

	/**
		Start an aliases connection list based on the referenced type.  
		We have the option of adding the first element to the list...
		NOTE: connections are only made in the meta-language.  
	 */
	static
	excl_ptr<aliases_connection_base>
	make_aliases_connection(
		const count_ptr<const this_type>&);

	static
	count_ptr<aggregate_meta_instance_reference_base>
	make_aggregate_meta_instance_reference(
		const count_ptr<const this_type>&);

#if SUBTYPE_PORT_CONNECTION
	static
	excl_ptr<port_connection_base>
	make_port_connection(
		const count_ptr<const this_type>&);
#endif

/**
	The implementation of this will be policy-determined, 
	by substructure_alias_base<bool>.  
	\return a pointer to an alias with substructure.  
 */
#define	UNROLL_SCALAR_SUBSTRUCTURE_REFERENCE_PROTO			\
	never_ptr<substructure_alias>					\
	unroll_scalar_substructure_reference(const unroll_context&) const

virtual	UNROLL_SCALAR_SUBSTRUCTURE_REFERENCE_PROTO = 0;

#define	CONNECT_PORT_PROTO						\
	bad_bool							\
	connect_port(instance_collection_base&, 			\
		const unroll_context&) const

virtual	CONNECT_PORT_PROTO = 0;

#define	LOOKUP_FOOTPRINT_FRAME_PROTO					\
	const footprint_frame*						\
	lookup_footprint_frame(const state_manager&) const

virtual	LOOKUP_FOOTPRINT_FRAME_PROTO = 0;

private:
virtual	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const = 0;

virtual	count_ptr<aggregate_meta_instance_reference_base>
	make_aggregate_meta_instance_reference_private(void) const = 0;

#if SUBTYPE_PORT_CONNECTION
virtual	excl_ptr<port_connection_base>
	make_port_connection_private(
		const count_ptr<const this_type>&) const = 0;
#endif

};	// end class meta_instance_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_META_INSTANCE_REFERENCE_BASE_H__

