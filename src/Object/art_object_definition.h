/**
	\file "Object/art_object_definition.h"
	Definition-related ART object classes.  
	$Id: art_object_definition.h,v 1.35.2.1.2.1 2005/07/14 23:15:50 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DEFINITION_H__
#define	__OBJECT_ART_OBJECT_DEFINITION_H__

#include "Object/art_object_definition_base.h"
#include "Object/art_object_namespace.h"
#include "Object/art_object_instance_management_base.h"

/*********** note on use of data structures ***************
Lists are needed for sets that need to maintain sequence, such as
formal declarations in definitions.  Type-checking is done in order
of elements, comparing actuals against formals one-by-one.  
For some lists, however, we'd like constant time access to 
elements in the sequence by hashing indices.  Hashlist provides
this added functionality by associating a key to each element in the 
list.  

Maps...

********************** end note **************************/

namespace ART {
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
// class param_expr_list;
using std::ostream;
USING_LIST
using util::memory::count_ptr;
using util::memory::excl_ptr;
using util::memory::never_ptr;

// class definition_base declared in "art_object_base.h"

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

	string
	get_qualified_name(void) const;

	ostream&
	dump_qualified_name(ostream&) const;

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
// process_definition and friends have been relocated to 
//	"Object/art_object_definition_proc.h"
//=============================================================================
// datatype_definition and friends have been relocated to 
//	"Object/art_object_definition_data.h"
//=============================================================================

/**
	Consider templating this to make it extensible to other types.

	Reserved for special built-in parameter types, pint and pbool.  
	Nothing can really be derived from them... yet.  
	Note that there is no intermediate param_definition class, 
	because parameter types can only be built in; there are no
	user-defined parameter types, for now...
	Doesn't have a param_type_reference pointer member, because
	type is hard-wired to the built-ins.  
 */
class built_in_param_def : public definition_base {
private:
	typedef	definition_base			parent_type;
	typedef	built_in_param_def		this_type;
protected:
	const string				key;
	const never_ptr<const name_space>	parent;
public:
	built_in_param_def(never_ptr<const name_space> p, const string& n);
	~built_in_param_def();

	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;

	const string&
	get_key(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	/** can't alias built-in param types, would be confusing */
	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const;

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

private:
	PERSISTENT_METHODS_DECLARATIONS

};	// end class built_in_param_def

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
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DEFINITION_H__

