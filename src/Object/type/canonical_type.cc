/**
	\file "Object/type/canonical_type.cc"
	Explicit template instantiation of canonical type classes.  
	Probably better to include the .tcc where needed, 
	as this is just temporary and convenient.  
	$Id: canonical_type.cc,v 1.1.4.2 2005/08/15 21:12:22 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "Object/type/canonical_type.tcc"
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/channel_type_reference.h"
#include "Object/type/process_type_reference.h"
#include "Object/inst/subinstance_manager.h"
#include "common/TODO.h"

namespace ART {
namespace entity {
//=============================================================================
// specialized definitions

void
canonical_definition_load_policy<datatype_definition_base>::operator ()
		(const persistent_object_manager& m,
		never_ptr<const definition_type>& d) const {
	data_type_reference::intercept_builtin_definition_hack(m, d);
}

//-----------------------------------------------------------------------------
template <>
struct unroll_port_instances_policy<datatype_definition_base> {
	void
	operator () (const canonical_generic_datatype& d, 
			const unroll_context& c,
			subinstance_manager& sub) const {
		// temporary
		// eventually will need template arguments
		data_type_reference::unroll_port_instances(
			d.get_base_def(), c, sub);
	}
};	// end struct unroll_port_instances_policy

//-----------------------------------------------------------------------------
template <>
struct unroll_port_instances_policy<user_def_datatype> {
	void
	operator () (const canonical_user_def_data_type& d, 
			const unroll_context& c,
			subinstance_manager& sub) const {
		// temporary
		FINISH_ME(Fang);
	}
};	// end struct unroll_port_instances_policy

//-----------------------------------------------------------------------------
template <>
struct unroll_port_instances_policy<user_def_chan> {
	void
	operator () (const canonical_user_def_chan_type& d, 
			const unroll_context& c,
			subinstance_manager& sub) const {
		// temporary
		FINISH_ME(Fang);
	}
};	// end struct unroll_port_instances_policy

//-----------------------------------------------------------------------------
template <>
struct unroll_port_instances_policy<process_definition> {
	void
	operator () (const canonical_process_type& p, 
			const unroll_context& c,
			subinstance_manager& sub) const {
		// modeled after process_type_reference::unroll_port_instances()
		const port_formals_manager&
			pf(p.canonical_definition_ptr->get_port_formals());
		const unroll_context cc(p.make_unroll_context());
		pf.unroll_ports(cc, sub);
	}
};	// end struct unroll_port_instances_policy

//=============================================================================
#if 0
template
canonical_type<user_def_datatype>::canonical_type(
	const canonical_type<datatype_definition_base>&);
#else
template
canonical_user_def_data_type::canonical_type(const canonical_generic_datatype&);
#endif

template class canonical_type<datatype_definition_base>;
template class canonical_type<user_def_datatype>;
template class canonical_type<user_def_chan>;
template class canonical_type<process_definition>;
// specialized, defined in "Object/type/canonical_generic_chan_type.cc"
// template class canonical_type<channel_definition_base>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

