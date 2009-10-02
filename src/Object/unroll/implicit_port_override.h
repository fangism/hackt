/**
	\file "Object/unroll/implicit_port_override.h"
	Class for setting implicit port connections.
	$Id: implicit_port_override.h,v 1.2 2009/10/02 01:57:18 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_IMPLICIT_PORT_OVERRIDE_H__
#define	__HAC_OBJECT_UNROLL_IMPLICIT_PORT_OVERRIDE_H__

#include "Object/unroll/port_connection_base.h"
#include "Object/ref/references_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {

//=============================================================================
/**
	I've decided to not make this a class template because
	only process instances use this.
 */
class implicit_port_override : public port_connection_base {
	typedef	implicit_port_override		this_type;
	typedef	port_connection_base		parent_type;
public:
/**
	Restriction that port_connection_base's reference list
	consists of scalar bools.  
 */
	typedef	simple_bool_meta_instance_reference	port_type;
	typedef	simple_process_meta_instance_reference	instance_type;
	typedef	count_ptr<const port_type>		port_ptr_type;
	typedef	count_ptr<const instance_type>		instance_ptr_type;
private:
	instance_ptr_type				inst;
public:
	implicit_port_override();
	~implicit_port_override();

	explicit
	implicit_port_override(const instance_ptr_type&);

	void
	append_bool_port(const port_ptr_type&);

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	good_bool
	unroll(const unroll_context&) const;

	void
	collect_transient_info(persistent_object_manager&) const;
	
	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

private:
	// use append_bool_port instead
	using port_connection_base::append_meta_instance_reference;

};	// end class implicit_port_override

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_IMPLICIT_PORT_OVERRIDE_H__
