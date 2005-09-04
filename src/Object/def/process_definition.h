/**
	\file "Object/def/process_definition.h"
	Process-definition-related ART object classes.  
	This file came from "Object/art_object_definition_proc.h".
	$Id: process_definition.h,v 1.3 2005/09/04 21:14:43 fang Exp $
 */

#ifndef	__OBJECT_DEF_PROCESS_DEFINITION_H__
#define	__OBJECT_DEF_PROCESS_DEFINITION_H__

#include "Object/def/process_definition_base.h"
#include "Object/common/scopespace.h"
#include "Object/unroll/sequential_scope.h"
#include "Object/lang/PRS_base.h"
#include "Object/def/port_formals_manager.h"
#include "Object/def/footprint_manager.h"
#include "Object/lang/CHP.h"

namespace ART {
namespace entity {
class process_type_reference;
//=============================================================================
/**
	Process definition.  
	All processes are user-defined.  
	Contains optional set of template formals, 
	set of port formals, and body of instantiations and language bodies.  
	Is there a way to re-use name resolution code
	from class name_space without copying?  
	No other class derives from this?
 */
class process_definition : public process_definition_base, public scopespace, 
	public sequential_scope {
private:
	typedef	process_definition		this_type;
public:
protected:
	const string		key;		// inherited
//	used_id_map_type	used_id_map;	// inherited
	const never_ptr<const name_space>	parent;
	port_formals_manager			port_formals;
	// list language bodies
	PRS::rule_set				prs;
	CHP::concurrent_actions			chp;
	mutable footprint_manager		footprint_map;
private:
	process_definition();
public:
	process_definition(const never_ptr<const name_space> o,
		const string& s); 
	~process_definition();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	const string&
	get_key(void) const;

	string
	get_qualified_name(void) const;

	ostream&
	dump_qualified_name(ostream&) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	const port_formals_manager&
	get_port_formals(void) const { return port_formals; }

	void
	commit_arity(void);

	/** overrides definition_base's */
	never_ptr<const instance_collection_base>
	lookup_port_formal(const string& id) const;

	size_t
	lookup_port_formal_position(const instance_collection_base&) const;

	never_ptr<const object>
	lookup_object_here(const string& id) const;

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_PROCESS_TYPE_PROTO;

	DEFINITION_ADD_PORT_FORMAL_PROTO;

	good_bool
	certify_port_actuals(const checked_refs_type& ol) const;

	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const;

	void
	add_production_rule(excl_ptr<PRS::rule>&);

	void
	expand_prs_complements(void);

	void
	compact_prs_references(void);

	void
	add_concurrent_chp_body(const count_ptr<CHP::action>&);

	const footprint&
	get_footprint(const count_ptr<const const_param_expr_list>&) const;

	REGISTER_COMPLETE_TYPE_PROTO;
	UNROLL_COMPLETE_TYPE_PROTO;
	CREATE_COMPLETE_TYPE_PROTO;
// methods for object file I/O
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);

};	// end class process_definition

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_DEF_PROCESS_DEFINITION_H__

