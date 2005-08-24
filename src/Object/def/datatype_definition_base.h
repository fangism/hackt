/**
	\file "Object/def/datatype_definition_base.h"
	Definition-related ART object classes.  
	This file came from "Object/art_object_definition_data.h".
	$Id: datatype_definition_base.h,v 1.2.8.3 2005/08/24 22:36:58 fang Exp $
 */

#ifndef	__OBJECT_DEF_DATATYPE_DEFINITION_BASE_H__
#define	__OBJECT_DEF_DATATYPE_DEFINITION_BASE_H__

#include "Object/def/definition_base.h"
#include "Object/type/canonical_type_fwd.h"

namespace ART {
namespace entity {
class data_type_reference;
//=============================================================================
/**
	Base class interface for data type definitions.  
 */
class datatype_definition_base : virtual public definition_base {
private:
	typedef	definition_base				parent_type;
public:
	typedef	data_type_reference			type_reference_type;
public:
	datatype_definition_base() : definition_base() { }

virtual	~datatype_definition_base();

	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const;

virtual	ostream&
	what(ostream& o) const = 0;

	using parent_type::dump;

	/**
		Resolves the underlying type, without regard to the 
		template arguments of any typedefs, for the sake of 
		resolving the category: built-in, enum, or struct.
	 */
virtual	never_ptr<const datatype_definition_base>
	resolve_canonical_datatype_definition(void) const = 0;

virtual	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO = 0;

#define	MAKE_CANONICAL_DATA_TYPE_REFERENCE_PROTO			\
	count_ptr<const data_type_reference>				\
	make_canonical_fundamental_type_reference(const template_actuals&) const

virtual	MAKE_CANONICAL_DATA_TYPE_REFERENCE_PROTO = 0;

#define	MAKE_CANONICAL_DATA_TYPE_PROTO					\
	canonical_generic_datatype					\
	make_canonical_type(const template_actuals&) const

virtual	MAKE_CANONICAL_DATA_TYPE_PROTO = 0;

virtual	good_bool
	require_signature_match(
		const never_ptr<const definition_base> d) const = 0;

virtual	REGISTER_COMPLETE_TYPE_PROTO = 0;
virtual	UNROLL_COMPLETE_TYPE_PROTO = 0;
virtual	CREATE_COMPLETE_TYPE_PROTO = 0;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class datatype_definition_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_DEF_DATATYPE_DEFINITION_BASE_H__

