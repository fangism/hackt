/**
	\file "Object/def/definition_base.h"
	Base classes for definition objects.  
	This file used to be "Object/art_object_definition_base.h".
	$Id: definition_base.h,v 1.15.4.1 2010/01/25 23:50:14 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_DEFINITION_BASE_H__
#define	__HAC_OBJECT_DEF_DEFINITION_BASE_H__

#include "util/string_fwd.h"
#include "util/macros.h"
#include "Object/common/object_base.h"
#include "Object/common/util_types.h"

#include "util/boolean_types.h"
#include "util/persistent.h"		// for persistent object interface
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

#include "Object/def/template_formals_manager.h"
#include "Object/traits/type_tag_enum.h"
#include "Object/devel_switches.h"

namespace HAC {
// forward declarations from outside namespaces
namespace parser {
	// note: methods may specify string as formal types, 
	// but you can still pass token_identifiers and token_strings
	// because they are derived from string.
	class token_identifier;
}
using parser::token_identifier;

//=============================================================================
namespace entity {
class scopespace;
class instance_placeholder_base;
class physical_instance_placeholder;
class param_value_placeholder;
class fundamental_type_reference;
class template_actuals;
class const_param_expr_list;
class port_formals_manager;
class footprint;
struct dump_flags;
using std::string;
using std::istream;
using util::bad_bool;
using util::good_bool;
using util::persistent;
using util::persistent_object_manager;
using util::memory::count_ptr;
using util::memory::never_ptr;
using util::memory::excl_ptr;

//=============================================================================
/**
	Base class for definition objects.  
	Consider deriving from name_space to re-use the 
	name-resolving functionality.  
	All definitions are potentially templatable.  
 */
class definition_base :
		virtual public persistent, 
		public object {
public:
	typedef	template_formals_manager::template_formals_value_type
					template_formals_value_type;
	typedef	template_formals_manager::template_formals_map_type
					template_formals_map_type;
	typedef	template_formals_manager::template_formals_list_type
					template_formals_list_type;

	typedef	count_ptr<fundamental_type_reference>
					type_ref_ptr_type;

	typedef	template_actuals	make_type_ptr_type;
	typedef	template_actuals	make_type_arg_type;
	typedef	instance_placeholder_base	placeholder_base_type;
	typedef	param_value_placeholder		value_placeholder_type;
	typedef	physical_instance_placeholder	instance_placeholder_type;
protected:
	template_formals_manager	template_formals;

	/**
		Whether or not this definition is complete or only declared.  
		As soon as a definition is opened, mark it as defined
		to allow self-recursive template definitions.  
	 */
	bool				defined;
#if REQUIRE_DEFINITION_EXPORT
	/**
		If true, then this definition is eligible for use
		outside its namespace.  
		This is not really applicable to typedefs, as their
		exportedness depends strictly on the underlying definition.
		Does this need to be kept persistently?
	 */
	bool				exported;
#endif
protected:
	definition_base();
public:

virtual	~definition_base();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const;	// temporary

	const template_formals_manager&
	get_template_formals_manager(void) const { return template_formals; }

	size_t
	num_strict_formals(void) const {
		return template_formals.num_strict_formals();
	}

	size_t
	num_relaxed_formals(void) const {
		return template_formals.num_relaxed_formals();
	}

	ostream&
	dump_template_formals(ostream& o) const;

	ostream&
	pair_dump(ostream& o) const;

virtual	meta_type_tag_enum
	get_meta_type(void) const = 0;

virtual	const string&
	get_key(void) const = 0;

virtual	never_ptr<const scopespace>
	get_parent(void) const = 0;

	bool
	is_defined(void) const { return defined; }

	void
	mark_defined(void) { assert(!defined); defined = true; }

#if REQUIRE_DEFINITION_EXPORT
// typedefs invoke base-definition's ::is_exported
virtual	bool
	is_exported(void) const;

	void
	mark_export(void) { exported = true; }
#endif

/// overridden by definitions that contain footprint_managers
virtual	void
	commit_arity(void) { }

	never_ptr<const value_placeholder_type>
	lookup_template_formal(const string& id) const;

	bool
	probe_relaxed_template_formal(const string&) const;

	size_t
	lookup_template_formal_position(const string& id) const;

/** should be pure virtual, but let's default to NULL */
	// default NULL
	// TODO: future use policy-based structure to rewrite definition classes
virtual	never_ptr<const port_formals_manager>
	get_port_formals_manager(void) const;

virtual	never_ptr<const scopespace>
	get_scopespace(void) const = 0;

	// non-virtual
	never_ptr<const placeholder_base_type>
	lookup_port_formal(const string& id) const;

	size_t
	lookup_port_formal_position(const instance_placeholder_type&) const;

	never_ptr<const object>
	lookup_nonparameter_member(const string& id) const;

virtual	never_ptr<const object>
	lookup_member(const string& id) const;

	// incidentally, this is never overridden, need not be virtual
virtual	good_bool
	check_null_template_argument(void) const;


protected:
	// Q: what if expressions are involved, can't statically resolve?
	// e.g. with arrays of parameters... and referenced indices.
	// well, they must be at least initialized (usable).  
	// need notion of formal equivalence
	// MAY be equivalent
	bool
	equivalent_template_formals(
		const never_ptr<const definition_base> d) const;

protected:
	good_bool
	certify_template_arguments(template_actuals& ta) const;

public:
	make_type_ptr_type
	make_default_template_arguments(void) const;

#define	CERTIFY_PORT_ACTUALS_PROTO					\
	good_bool							\
	certify_port_actuals(const checked_refs_type&) const

	/** by default returns false */
virtual	CERTIFY_PORT_ACTUALS_PROTO = 0;

public:
#define	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO				\
	definition_base::type_ref_ptr_type				\
	make_fundamental_type_reference(make_type_arg_type ta) const

// proposing to replace set_context_fundamental_type with the following:
virtual MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO = 0;

	// overloaded for no template argument, for convenience, 
	// but must check that everything has default arguments!
	definition_base::type_ref_ptr_type
	make_fundamental_type_reference(void) const;
// why virtual? special cases for built-in types?

/**
	Create an empty, unresolved typedef, based on the 
	invoker's type.  
 */
virtual	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const = 0;

	const string&
	get_name(void) const { return get_key(); }

	string
	get_qualified_name(void) const;

	ostream&
	dump_qualified_name(ostream&, const dump_flags&) const;

/** definition signature comparison, true if equal */
virtual	good_bool
	require_signature_match(const never_ptr<const definition_base>) const
		{ return good_bool(false); }	// temporary, should be pure

/**
	f should be const and owned -- pointer type conflict...  
	virtual so that types without templates can assert NULL.  
	TODO: This function should be pure virtual and belong 
		to a different interface!
	TODO: shouldn't argument be a param_instantiation_statement?
	TODO: shouldn't return type be param_value_placeholder?
 */
virtual	never_ptr<const value_placeholder_type>
	add_strict_template_formal(
		const count_ptr<instantiation_statement_base>&, 
		const token_identifier& id);

virtual	never_ptr<const value_placeholder_type>
	add_relaxed_template_formal(
		const count_ptr<instantiation_statement_base>&, 
		const token_identifier& id);

#define	DEFINITION_ADD_PORT_FORMAL_PROTO				\
	never_ptr<const instance_placeholder_type>			\
	add_port_formal(const count_ptr<instantiation_statement_base>&,	\
		const token_identifier&)

/**
	Really, only some definitions should have ports...
 */
virtual	DEFINITION_ADD_PORT_FORMAL_PROTO;

#define	REGISTER_COMPLETE_TYPE_PROTO					\
	void								\
	register_complete_type(						\
		const count_ptr<const const_param_expr_list>&) const

/**
	Need footprint argument to pass top-level footprint.  
 */
#define	CREATE_COMPLETE_TYPE_PROTO					\
	good_bool							\
	create_complete_type(						\
		const count_ptr<const const_param_expr_list>&,		\
			const footprint&) const

protected:
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream&) const;

	static
	void
	write_object_base_fake(const persistent_object_manager& m, ostream&);

	void
	load_object_base(const persistent_object_manager& m, istream&);

public:
	static const never_ptr<const definition_base>	null;
};	// end class definition_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_DEFINITION_BASE_H__

