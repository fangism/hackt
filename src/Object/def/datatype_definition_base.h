/**
	\file "Object/def/datatype_definition_base.h"
	Definition-related HAC object classes.  
	This file came from "Object/art_object_definition_data.h".
	$Id: datatype_definition_base.h,v 1.7.70.1 2009/09/11 00:05:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_DATATYPE_DEFINITION_BASE_H__
#define	__HAC_OBJECT_DEF_DATATYPE_DEFINITION_BASE_H__

#include "Object/def/definition_base.h"
#include "Object/type/canonical_type_fwd.h"


namespace HAC {
namespace entity {
class data_type_reference;
class fundamental_channel_footprint;
class channel_data_reader;	// from "Object/nonmeta_channel_manipulator.h"

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

virtual	void
	count_channel_member(fundamental_channel_footprint&) const = 0;

/**
	Same idea as simple_nonmeta_value_reference:direct_assign(),
	where the actual type determines which sub-iterator to read
	from and advance.  
 */
#define	DUMP_CHANNEL_FIELD_ITERATE_PROTO				\
	ostream&							\
	dump_channel_field_iterate(ostream&, channel_data_reader&) const

virtual	DUMP_CHANNEL_FIELD_ITERATE_PROTO = 0;

virtual	meta_type_tag_enum
	get_meta_type(void) const = 0;

public:
	bool
	less_ordering(const datatype_definition_base&) const;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class datatype_definition_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_DATATYPE_DEFINITION_BASE_H__

