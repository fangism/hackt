/**
	\file "Object/unroll/instantiation_statement_base.h"
	Instance statement base class.
	This file's previous revision history is in
		"Object/art_object_inst_stmt_base.h"
	$Id: instantiation_statement_base.h,v 1.12.36.1 2007/07/13 18:49:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_BASE_H__
#define	__HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_BASE_H__

#include "util/string_fwd.h"
#include "Object/unroll/instance_management_base.h"
#include "Object/common/util_types.h"
#include "Object/devel_switches.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class const_range_list;
class dynamic_param_expr_list;
class instance_placeholder_base;
class physical_instance_placeholder;
class fundamental_type_reference;
using std::string;
using std::istream;
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	This node retains the information for an instantiation statement.  
	This is what will be unrolled.  
	No parent, is a globally sequential item.  
	Every sub-class will contain a modifiable
	back-reference to an (sub-type of) 
	<strike>instance_collection_base</strike> instance_placeholder, 
	where the collection will be unrolled.  
	Should this point to an unrolled instance?
	No, it will be looked up.  
	TODO: this is where optional relaxed template arguments go.  
 */
class instantiation_statement_base : public instance_management_base {
public:
	typedef	count_ptr<const dynamic_param_expr_list>
						const_relaxed_args_type;
	typedef	count_ptr<dynamic_param_expr_list>	relaxed_args_type;
protected:
	index_collection_item_ptr_type		indices;
protected:
	instantiation_statement_base() : instance_management_base(), 
		indices(NULL) { }

	explicit
	instantiation_statement_base(const index_collection_item_ptr_type& i);

public:
virtual	~instantiation_statement_base();

	ostream&
	dump(ostream&, const expr_dump_context&) const;

virtual	void
	attach_collection(const never_ptr<const instance_placeholder_base>) = 0;

virtual	never_ptr<const instance_placeholder_base>
	get_inst_base(void) const = 0;

	string
	get_name(void) const;

	size_t
	dimensions(void) const;

	index_collection_item_ptr_type
	get_indices(void) const;

#if !ENABLE_RELAXED_TEMPLATE_PARAMETERS
virtual	const_relaxed_args_type
	get_relaxed_actuals(void) const = 0;
#endif

	good_bool
	resolve_instantiation_range(const_range_list&, 
		const unroll_context&) const;

virtual	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const = 0;

virtual	good_bool
	unroll(const unroll_context&) const = 0;

/**
	2005-07-13:
	After reworking class hierarchy, this should not be virtual.
	Don't know what to do here w.r.t. placeholders...
 */
#define	INSTANTIATE_PORT_PROTO						\
	good_bool							\
	instantiate_port(const unroll_context&, 			\
		physical_instance_collection&) const

/***
	case: A top-level instantiation is called.
	case: won't happen?  instantiation inside definition called
***/
	// unroll (context?)

protected:

	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& ) const;

	void
	load_object_base(const persistent_object_manager& m, istream& );

};	// end class instantiation_statement_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_BASE_H__

