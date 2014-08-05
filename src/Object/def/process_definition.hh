/**
	\file "Object/def/process_definition.hh"
	Process-definition-related HAC object classes.  
	This file came from "Object/art_object_definition_proc.h".
	$Id: process_definition.hh,v 1.18 2010/05/26 00:46:48 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_PROCESS_DEFINITION_H__
#define	__HAC_OBJECT_DEF_PROCESS_DEFINITION_H__

#include "Object/devel_switches.hh"

#include "Object/def/process_definition_base.hh"
#if PROCESS_DEFINITION_IS_NAMESPACE
#include "Object/common/namespace.hh"
#else
#include "Object/common/scopespace.hh"
#endif
#include "Object/unroll/sequential_scope.hh"
#include "Object/lang/PRS_base.hh"
#include "Object/lang/RTE_base.hh"
#include "Object/lang/SPEC.hh"
#include "Object/def/port_formals_manager.hh"
#include "Object/def/footprint_manager.hh"
#include "Object/lang/CHP.hh"


namespace HAC {
namespace entity {
class process_type_reference;
#if PROCESS_DEFINITION_IS_NAMESPACE
#define	PD_VIRTUAL		virtual
#else
#define	PD_VIRTUAL
#endif
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
class process_definition : public process_definition_base,
#if PROCESS_DEFINITION_IS_NAMESPACE
	public name_space,
#else
	public scopespace, 
#endif
	public sequential_scope {
private:
	typedef	process_definition		this_type;
protected:
#if PROCESS_DEFINITION_IS_NAMESPACE
	typedef	name_space			scope_parent_type;
	// key and parent are inherited
#else
	typedef	scopespace			scope_parent_type;
	const string		key;
//	used_id_map_type	used_id_map;	// inherited
	const never_ptr<const name_space>	parent;
#endif
	meta_type_tag_enum			meta_type;
	port_formals_manager			port_formals;
	// list language bodies
/**
	Really, only needs to be a list/array of rule_set, but is currently
	list of rule_set_base (polymorphic).  Meh.
 */
	PRS::rule_set_base			prs;
	RTE::assignment_set_base		rte;
	CHP::concurrent_actions			chp;
	SPEC::directives_set			spec;
	mutable footprint_manager		footprint_map;
protected:
	process_definition();

	// only intended for module
	explicit
	process_definition(const string& s); 
public:
	process_definition(const never_ptr<const name_space> o,
		const string& s, 
		const meta_type_tag_enum t = META_TYPE_PROCESS); 
PD_VIRTUAL
	~process_definition();

PD_VIRTUAL
	ostream&
	what(ostream& o) const;

PD_VIRTUAL
	ostream&
	dump(ostream& o) const;

	const process_definition&
	get_canonical_proc_def(void) const;

	meta_type_tag_enum
	get_meta_type(void) const;

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

	PRS::rule_set_base&
	get_prs(void) { return prs; }

	void
	expand_prs_complements(void);

	void
	compact_prs_references(void);

	RTE::assignment_set_base&
	get_rte(void) { return rte; }

	using scopespace::add_node_instance_idempotent;

	void
	add_concurrent_chp_body(const count_ptr<CHP::action>&);

	SPEC::directives_set&
	get_spec_directives_set(void) { return spec; }

	const footprint*
	lookup_footprint(const count_ptr<const const_param_expr_list>&) const;

	const footprint&
	get_footprint(const count_ptr<const const_param_expr_list>&) const;

protected:
	footprint&
	get_footprint(const count_ptr<const const_param_expr_list>&);

	good_bool
	__unroll_complete_type(const count_ptr<const const_param_expr_list>&, 
		footprint&, const footprint&) const;

	// overrides definition_base::unroll_lang()
	good_bool
	unroll_lang(const unroll_context&) const;

public:
	// public for module::allocate...
	good_bool
	__create_complete_type(const count_ptr<const const_param_expr_list>&, 
		footprint&, const footprint&) const;

	REGISTER_COMPLETE_TYPE_PROTO;
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
#if PROCESS_DEFINITION_IS_NAMESPACE
	using scope_parent_type::load_used_id_map_object;
#else
	void
	load_used_id_map_object(excl_ptr<persistent>& o);
#endif

};	// end class process_definition

#undef	PD_VIRTUAL

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_PROCESS_DEFINITION_H__

