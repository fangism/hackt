/**
	\file "Object/art_object_definition_proc.h"
	Process-definition-related ART object classes.  
	$Id: art_object_definition_proc.h,v 1.7.2.1 2005/06/30 23:22:16 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DEFINITION_PROC_H__
#define	__OBJECT_ART_OBJECT_DEFINITION_PROC_H__

#include "Object/art_object_definition.h"
#include "Object/art_object_PRS_base.h"
#include "Object/art_object_port_formals_manager.h"
#include "Object/art_object_CHP.h"		// reduce later

namespace ART {
namespace entity {
class process_type_reference;
//=============================================================================
/**
	Process definition base class.  From this, there will arise: true
	process definitions, and typedef process definitions. 
 */
class process_definition_base : virtual public definition_base {
private:
	typedef	definition_base		parent_type;
protected:
	// no new members?
protected:
//	process_definition_base(const string& n);
	process_definition_base() : definition_base() { }
public:
virtual	~process_definition_base() { }

	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const;

#define	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO			\
	count_ptr<const process_type_reference>				\
	make_canonical_type_reference(const template_actuals&) const

virtual	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO = 0;

// inherited pure virtuals are still pure virtuals
protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class process_definition_base

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
private:
	process_definition();
public:
	process_definition(never_ptr<const name_space> o, const string& s); 
	~process_definition();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	const string&
	get_key(void) const;

	string
	get_qualified_name(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	/** overrides definition_base's */
	never_ptr<const instance_collection_base>
	lookup_port_formal(const string& id) const;

	never_ptr<const object>
	lookup_object_here(const string& id) const;

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO;

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

// methods for object file I/O
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);

};	// end class process_definition

//=============================================================================
/**
	A process-class typedef.  
	Is usable as a process_definition_base for complete types, 
	but not definable.  
	Has no contents, just a level of indirection to the encapsulated
	process type. 
	May be templated for partial type specifications.  
 */
class process_definition_alias : public process_definition_base, 
		public typedef_base {
private:
	typedef	process_definition_alias		this_type;
protected:
	const string					key;
	/** parent can be namespace or definition */
	never_ptr<const scopespace>			parent;
	excl_ptr<const process_type_reference>		base;
private:
	process_definition_alias();
public:
	process_definition_alias(const string& n, 
		const never_ptr<const scopespace> p);

	~process_definition_alias();

	ostream&
	what(ostream& o) const;

	const string&
	get_key(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const fundamental_type_reference>
	get_base_type_ref(void) const;

	bool
	assign_typedef(excl_ptr<const fundamental_type_reference>& f);

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);
};	// end class process_definition_alias

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DEFINITION_PROC_H__

