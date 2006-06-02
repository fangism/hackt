/**
	\file "Object/def/param_definition.h"
	Definition-related HAC object classes.  
	This file used to be "Object/art_object_definition.h".
	$Id: param_definition.h,v 1.6 2006/06/02 20:15:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_PARAM_DEFINITION_H__
#define	__HAC_OBJECT_DEF_PARAM_DEFINITION_H__

#include "Object/def/definition_base.h"
#include "Object/common/namespace.h"

namespace HAC {
//=============================================================================
// forward declarations from outside namespaces
namespace parser {
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

	never_ptr<const scopespace>
	get_scopespace(void) const;

	/** can't alias built-in param types, would be confusing */
	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const;

	CERTIFY_PORT_ACTUALS_PROTO;

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

private:
	PERSISTENT_METHODS_DECLARATIONS

};	// end class built_in_param_def

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_PARAM_DEFINITION_H__

