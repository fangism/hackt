/**
	\file "Object/ref/meta_instance_reference_base.h"
	Base class family for instance references in HAC.  
	This file was "Object/art_object_inst_ref_base.h"
		in a previous life.  
	$Id: meta_instance_reference_base.h,v 1.21.2.1 2010/08/18 23:39:47 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_META_INSTANCE_REFERENCE_BASE_H__

#include "util/persistent.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "Object/inst/substructure_alias_fwd.h"
#include "Object/lang/generic_attribute_fwd.h"
#include "Object/ref/reference_enum.h"
#include "Object/devel_switches.h"
#include "util/boolean_types.h"
#include "util/tokenize_fwd.h"		// for util::string_list

namespace HAC {
namespace entity {
class scopespace;
struct footprint_frame;
struct expr_dump_context;
struct global_entry_context;
class entry_collection;
class module;
class definition_base;
class fundamental_type_reference;
class instance_collection_base;
class physical_instance_collection;
class instance_management_base;
class aliases_connection_base;
class port_connection_base;
class const_range_list;
class unroll_context;
class footprint;
class aggregate_meta_instance_reference_base;
class nonmeta_expr_visitor;
using util::bad_bool;
using util::good_bool;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::memory::count_ptr;
using std::istream;
using std::ostream;
using util::persistent;

//=============================================================================
/**
	PHASE this back into what is currently simple_meta_indexed_reference_base.  
	Base class for anything that *refers* to an instance, 
	or collection thereof.  
	Instance reference should be cacheable?

	Why derived from object? to be pushable onto an object stack... arg!
	We need separate stacks...
	See NOTES.
 */
class meta_instance_reference_base : virtual public persistent {
	typedef	meta_instance_reference_base		this_type;
public:
	typedef	count_ptr<aliases_connection_base>
						alias_connection_ptr_type;
	typedef	count_ptr<port_connection_base>
						port_connection_ptr_type;
public:
	meta_instance_reference_base() : persistent() { }

virtual	~meta_instance_reference_base();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

virtual	ostream&
	dump_type_size(ostream& o) const = 0;

virtual	size_t
	dimensions(void) const = 0;

virtual	never_ptr<const definition_base>
	get_base_def(void) const = 0;

virtual	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const = 0;


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
	alias_connection_ptr_type
	make_aliases_connection(
		const count_ptr<const this_type>&);

	static
	count_ptr<aggregate_meta_instance_reference_base>
	make_aggregate_meta_instance_reference(
		const count_ptr<const this_type>&);

	static
	port_connection_ptr_type
	make_port_connection(
		const count_ptr<const this_type>&);

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
	connect_port(physical_instance_collection&, 			\
		const unroll_context&) const

virtual	CONNECT_PORT_PROTO = 0;

#define	LOOKUP_TOP_LEVEL_REFERENCE_PROTO				\
	global_indexed_reference					\
	lookup_top_level_reference(const global_entry_context&) const
#define	LOOKUP_TOP_LEVEL_REFERENCES_PROTO				\
	good_bool							\
	lookup_top_level_references(const global_entry_context&, 	\
		global_reference_array_type&) const
#define	LOOKUP_GLOBAL_REFERENCE_INDICES_PROTO				\
	good_bool							\
	lookup_global_reference_indices(const global_entry_context&,	\
		typed_index_array_reference&,				\
		const unroll_context* = NULL) const

virtual	LOOKUP_TOP_LEVEL_REFERENCE_PROTO = 0;
virtual	LOOKUP_TOP_LEVEL_REFERENCES_PROTO = 0;
#if PRIVATE_MEMBER_REFERENCES
virtual	LOOKUP_GLOBAL_REFERENCE_INDICES_PROTO = 0;
#endif

#define	CREATE_INSTANCE_ATTRIBUTE_PROTO					\
	count_ptr<const instance_management_base>			\
	create_instance_attribute(					\
		const count_ptr<const meta_instance_reference_base>&, 	\
		const generic_attribute_list_type&) const

virtual	CREATE_INSTANCE_ATTRIBUTE_PROTO = 0;

#define	UNROLL_RESOLVE_COPY_REFERENCE_PROTO				\
	count_ptr<const meta_instance_reference_base>			\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const meta_instance_reference_base>&) const

virtual	UNROLL_RESOLVE_COPY_REFERENCE_PROTO = 0;

virtual	void
	accept(nonmeta_expr_visitor&) const = 0;

private:
virtual	alias_connection_ptr_type
	make_aliases_connection_private(void) const = 0;

virtual	count_ptr<aggregate_meta_instance_reference_base>
	make_aggregate_meta_instance_reference_private(void) const = 0;

virtual	port_connection_ptr_type
	make_port_connection_private(
		const count_ptr<const this_type>&) const = 0;

};	// end class meta_instance_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_META_INSTANCE_REFERENCE_BASE_H__

