/**
	\file "Object/art_object_definition_base.h"
	Base classes for definition objects.  
	$Id: art_object_definition_base.h,v 1.23.2.5 2005/07/15 03:49:02 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DEFINITION_BASE_H__
#define	__OBJECT_ART_OBJECT_DEFINITION_BASE_H__

#include "util/string_fwd.h"

#include "util/macros.h"
#include "Object/art_object_base.h"
#include "Object/art_object_util_types.h"

#include "util/boolean_types.h"
#include "util/persistent.h"		// for persistent object interface
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

#include "Object/art_object_template_formals_manager.h"

namespace ART {
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
class instance_collection_base;
class fundamental_type_reference;
class template_actuals;
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

protected:
	template_formals_manager	template_formals;

	/**
		Whether or not this definition is complete or only declared.  
		As soon as a definition is opened, mark it as defined
		to allow self-recursive template definitions.  
	 */
	bool				defined;
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

	ostream&
	dump_template_formals(ostream& o) const;

	ostream&
	pair_dump(ostream& o) const;

virtual	const string&
	get_key(void) const = 0;

virtual	never_ptr<const scopespace>
	get_parent(void) const = 0;

	bool
	is_defined(void) const { return defined; }

	void
	mark_defined(void) { assert(!defined); defined = true; }

	never_ptr<const param_instance_collection>
	lookup_template_formal(const string& id) const;

	bool
	probe_relaxed_template_formal(const string&) const;

	size_t
	lookup_template_formal_position(const string& id) const;

/** should be pure virtual, but let's default to NULL */
virtual	never_ptr<const instance_collection_base>
	lookup_port_formal(const string& id) const;

virtual	never_ptr<const object>
	lookup_object_here(const string& id) const;

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

	/** by default returns false */
virtual	good_bool
	certify_port_actuals(const checked_refs_type&) const;

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

// need not be virtual?
virtual	string
	get_qualified_name(void) const;

	ostream&
	dump_qualified_name(ostream&) const;

/** definition signature comparison, true if equal */
virtual	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const
		{ return good_bool(false); }	// temporary, should be pure

/**
	f should be const and owned -- pointer type conflict...  
	virtual so that types without templates can assert NULL.  
	TODO: This function should be pure virtual and belong 
		to a different interface!
	TODO: shouldn't argument be a param_instantiation_statement?
 */
virtual	never_ptr<const instance_collection_base>
	add_strict_template_formal(
		const never_ptr<instantiation_statement_base> f, 
		const token_identifier& id);

virtual	never_ptr<const instance_collection_base>
	add_relaxed_template_formal(
		const never_ptr<instantiation_statement_base> f, 
		const token_identifier& id);

#define	DEFINITION_ADD_PORT_FORMAL_PROTO				\
	never_ptr<const instance_collection_base>			\
	add_port_formal(const never_ptr<instantiation_statement_base>, 	\
		const token_identifier&)

/**
	Really, only some definitions should have ports...
 */
virtual	DEFINITION_ADD_PORT_FORMAL_PROTO;

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
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DEFINITION_BASE_H__

