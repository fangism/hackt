/**
	\file "Object/def/typedef_base.h"
	Definition-related HAC object classes.  
	This file originated from "Object/art_object_definition.h",  
	$Id: typedef_base.h,v 1.6 2006/01/22 18:19:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_TYPEDEF_BASE_H__
#define	__HAC_OBJECT_DEF_TYPEDEF_BASE_H__

#include "Object/def/definition_base.h"
#include "Object/common/scopespace.h"
#include "Object/unroll/sequential_scope.h"

namespace HAC {
//=============================================================================
// forward declarations from outside namespaces
namespace parser {
	// note: methods may specify string as formal types, 
	// but you can still pass token_identifiers and token_strings
	// because they are derived from string.
	class token_string;
	class token_identifier;
}
using parser::token_identifier;

//=============================================================================
namespace entity {
class data_type_reference;
using std::ostream;
using util::memory::count_ptr;
using util::memory::excl_ptr;
using util::memory::never_ptr;

//=============================================================================
/**
	Common interface for parameterizable (template) definitions.  
	? Are all definitions with scopespaces templatable ?
 */
class templatable_definition {	// : public scopespace?
public:

};	// end class templatable_definition

//=============================================================================
/**
	Abstract base class interface for typedef alias definitions.  
	All typedefs are templateable, and thus have their own little
	scope space for template parameters.  
	Awkward?
	Why sequential scope? for the template parameters
 */
class typedef_base : virtual public definition_base, public scopespace, 
	public sequential_scope {
private:
	typedef	definition_base		definition_parent_type;
	typedef	scopespace		scope_parent_type;
	typedef	sequential_scope	sequential_parent_type;
protected:
	// no new members
public:
	typedef_base() : definition_base(), scopespace(), sequential_scope() { }

virtual	~typedef_base();

virtual	const string&
	get_key(void) const = 0;

virtual	never_ptr<const scopespace>
	get_scopespace(void) const = 0;

	string
	get_qualified_name(void) const;

	ostream&
	dump_qualified_name(ostream&, const dump_flags&) const;

virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	dump(ostream& o) const;

virtual never_ptr<const fundamental_type_reference>
	get_base_type_ref(void) const = 0;

virtual	bool
	assign_typedef(excl_ptr<const fundamental_type_reference>& f) = 0;

#if 0
	excl_ptr<const fundamental_type_reference>
	resolve_complete_type(never_ptr<const param_expr_list> p) const;
#endif

private:
virtual	void
	load_used_id_map_object(excl_ptr<persistent>& o) = 0;
};	// end class typedef_base

//=============================================================================
/// Type aliases are analogous to typedefs in C (not yet implemented)
// for renaming convenience
// going to use this mechanism to inherit built-in types into each namespace
//	to accelerate their resolution, not having to search sequentially
//	up to the global namespace.  
// add an alias into each scope's used_id_map...
// also templatable, for partial template spcifications?  YES
// should we sub-type? hold off...
// new definition class hierarchy sub-types!

// class type_alias was here.

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_TYPEDEF_BASE_H__

