/**
	\file "Object/art_object_definition_base.h"
	Base classes for definition objects.  
	$Id: art_object_definition_base.h,v 1.20.2.1.4.1 2005/06/12 21:27:56 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DEFINITION_BASE_H__
#define	__OBJECT_ART_OBJECT_DEFINITION_BASE_H__

#include <vector>
#include <string>		// needed by gcc-3.2 :S

#include "util/macros.h"
#include "Object/art_object_base.h"
#include "Object/art_object_util_types.h"

#include "util/boolean_types.h"
#include "util/persistent.h"		// for persistent object interface
#include "util/hash_qmap.h"		// need complete definition
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/STL/vector_fwd.h"

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
class param_expr;
class param_expr_list;
class scopespace;
class instance_collection_base;
class fundamental_type_reference;
using std::string;
using std::istream;
using std::vector;
using util::bad_bool;
using util::good_bool;
using util::hash_qmap;
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

	/** map from param_instance_collection to actual value passed */
	typedef	hash_qmap<string, count_ptr<const param_expr> >
					template_actuals_map_type;
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

	void
	fill_template_actuals_map(template_actuals_map_type& am, 
		const param_expr_list& al) const;

	never_ptr<const param_instance_collection>
	lookup_template_formal(const string& id) const;

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
	certify_template_arguments(
		const never_ptr<dynamic_param_expr_list> ta) const;

public:
	excl_ptr<dynamic_param_expr_list>
	make_default_template_arguments(void) const;

	/** by default returns false */
virtual	good_bool
	certify_port_actuals(const checked_refs_type&) const;

public:
// proposing to replace set_context_fundamental_type with the following:
virtual count_ptr<const fundamental_type_reference>
	make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const = 0;

	// overloaded for no template argument, for convenience, 
	// but must check that everything has default arguments!
	count_ptr<const fundamental_type_reference>
	make_fundamental_type_reference(void) const;
// why virtual? special cases for built-in types?

/**
	Create an empty, unresolved typedef, based on the 
	invoker's type.  
 */
virtual	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const = 0;

// need not be virtual?
virtual	string
	get_name(void) const;

// need not be virtual?
virtual	string
	get_qualified_name(void) const;

/** definition signature comparison, true if equal */
virtual	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const
		{ return good_bool(false); }	// temporary, should be pure

/**
	f should be const and owned -- pointer type conflict...  
	virtual so that types without templates can assert NULL.  
	TO DO: This function should be pure virtual and belong 
		to a different interface!
 */
virtual	never_ptr<const instance_collection_base>
	add_strict_template_formal(
		const never_ptr<instantiation_statement_base> f, 
		const token_identifier& id);

virtual	never_ptr<const instance_collection_base>
	add_relaxed_template_formal(
		const never_ptr<instantiation_statement_base> f, 
		const token_identifier& id);

/**
	Really, only some definitions should have ports...
 */
virtual	never_ptr<const instance_collection_base>
	add_port_formal(const never_ptr<instantiation_statement_base> f, 
		const token_identifier& id);

#if 0
virtual	bool
	exclude_object(const used_id_map_type::value_type& i) const;
#endif

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
/**
	actual values passed
	This needs to be updated to include a pair of paramater lists.
	For strict and relaxed template arguments.  
	(Although an argument is an argument.)
 */
typedef	definition_base::template_actuals_map_type
		template_actuals_map_type;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DEFINITION_BASE_H__

