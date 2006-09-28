/**
	\file "Object/def/process_definition.h"
	Process-definition-related HAC object classes.  
	This file came from "Object/art_object_definition_proc.h".
	$Id: process_definition.h,v 1.10.10.2 2006/09/28 23:55:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_PROCESS_DEFINITION_H__
#define	__HAC_OBJECT_DEF_PROCESS_DEFINITION_H__

#include "Object/def/process_definition_base.h"
#include "Object/common/scopespace.h"
#include "Object/unroll/sequential_scope.h"
#include "Object/lang/PRS_base.h"
#include "Object/lang/SPEC.h"
#include "Object/def/port_formals_manager.h"
#include "Object/def/footprint_manager.h"
#include "Object/lang/CHP.h"

namespace HAC {
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
protected:
	const string		key;		// inherited
//	used_id_map_type	used_id_map;	// inherited
	const never_ptr<const name_space>	parent;
	port_formals_manager			port_formals;
	// list language bodies
	PRS::rule_set				prs;
	CHP::concurrent_actions			chp;
	SPEC::directives_set			spec;
	mutable footprint_manager		footprint_map;
protected:
	process_definition();

#if MODULE_PROCESS
	// only intended for module
	explicit
	process_definition(const string& s); 
#endif
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
	dump_qualified_name(ostream&, const dump_flags&) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const scopespace>
	get_scopespace(void) const;

	const port_formals_manager&
	get_port_formals(void) const { return port_formals; }

	void
	commit_arity(void);

	// override
	never_ptr<const port_formals_manager>
	get_port_formals_manager(void) const;

	never_ptr<const object>
	lookup_member(const string& id) const;

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_PROCESS_TYPE_PROTO;

	DEFINITION_ADD_PORT_FORMAL_PROTO;

	CERTIFY_PORT_ACTUALS_PROTO;

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

	SPEC::directives_set&
	get_spec_directives_set(void) { return spec; }

	const footprint&
	get_footprint(const count_ptr<const const_param_expr_list>&) const;

protected:
	footprint&
	get_footprint(const count_ptr<const const_param_expr_list>&);

public:
	good_bool
	__unroll_complete_type(const count_ptr<const const_param_expr_list>&, 
		footprint&) const;

	good_bool
	__create_complete_type(const count_ptr<const const_param_expr_list>&, 
		footprint&) const;

	REGISTER_COMPLETE_TYPE_PROTO;
	UNROLL_COMPLETE_TYPE_PROTO;
	CREATE_COMPLETE_TYPE_PROTO;
// methods for object file I/O
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);

};	// end class process_definition

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_PROCESS_DEFINITION_H__

