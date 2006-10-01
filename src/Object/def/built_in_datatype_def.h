/**
	\file "Object/def/built_in_datatype_def.h"
	Definition-related HAC object classes.  
	This file came from "Object/art_object_definition_data.h".
	$Id: built_in_datatype_def.h,v 1.8.8.3 2006/10/01 21:13:49 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_BUILT_IN_DATATYPE_DEF_H__
#define	__HAC_OBJECT_DEF_BUILT_IN_DATATYPE_DEF_H__

#include "Object/def/datatype_definition_base.h"
#include "Object/common/scopespace.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Reserved for special built-in fundamental data types.  
	All user-defined data types will boil down to these types.  
	Final class.  
	Need to derive from scopespace because, built-in data type
	int's template parameter, pint, needs a valid owner scopespace.  
 */
class built_in_datatype_def : public datatype_definition_base, 
		public scopespace {
private:
	typedef	built_in_datatype_def			this_type;
protected:
	const string					key;
	const never_ptr<const name_space>		parent;
public:
	built_in_datatype_def(never_ptr<const name_space> o, const string& n);

	~built_in_datatype_def();

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

	never_ptr<const datatype_definition_base>
	resolve_canonical_datatype_definition(void) const;

	CERTIFY_PORT_ACTUALS_PROTO;

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;
	// overrides definition_base's, exception to rule
	// because this is not a scopespace
	// ah, but it is now!

	MAKE_CANONICAL_DATA_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_DATA_TYPE_PROTO;

	never_ptr<const value_placeholder_type>
	add_template_formal(excl_ptr<value_placeholder_type>& f);

	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const
		{ assert(d); return good_bool(key == d->get_name()); }
		// really, this should never be called...

	REGISTER_COMPLETE_TYPE_PROTO;
	UNROLL_COMPLETE_TYPE_PROTO;
	CREATE_COMPLETE_TYPE_PROTO;
public:
	// actually going to de/serialize built-in type, only to be
	// intercepted and replaced by data_type_reference::load_object
	PERSISTENT_METHODS_DECLARATIONS
	// thus we need only collect and write...
private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);
};	// end class built_in_datatype_def

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_BUILT_IN_DATATYPE_DEF_H__

