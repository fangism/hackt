/**
	\file "Object/lang/PRS_literal_attribute_registry.hh"
	Header file for PRS literal attributes.  
	$Id: PRS_literal_attribute_registry.hh,v 1.2 2009/09/14 21:17:02 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_LITERAL_ATTRIBUTE_REGISTRY_H__
#define	__HAC_OBJECT_LANG_PRS_LITERAL_ATTRIBUTE_REGISTRY_H__

#include "Object/lang/attribute_visitor_entry.hh"
#include "util/size_t.h"
#include "util/STL/map_fwd.hh"

namespace HAC {
namespace entity {
namespace PRS {
class cflat_prs_printer;
using entity::attribute_visitor_entry;

//=============================================================================
// TODO: factor these tool-dependent typedefs out to another header

typedef	attribute_visitor_entry<cflat_prs_printer>
				cflat_literal_attribute_definition_entry;
/**
	The global map type for attributes.
 */
typedef	std::default_map<string, cflat_literal_attribute_definition_entry>::type
				cflat_literal_attribute_registry_type;

/**
	The global PRS attribute registry.  
	This is globally/statically initialized in this corresponding .cc file. 
 */
extern const cflat_literal_attribute_registry_type&
cflat_literal_attribute_registry;

//=============================================================================
/**
	Macro for declaring PRS literal attribute classes.  
	The base classes are declared in "Object/lang/PRS_attribute_common.h".
	Here, the vistor_type is cflat_prs_printer.
	TODO: These classes should have hidden visibility.  
	TODO: could also push name[] into the base class, but would we be 
		creating an initialization order dependence?
 */
#define DECLARE_PRS_LITERAL_ATTRIBUTE_CLASS(class_name, visitor)	\
struct class_name : public entity::PRS::literal_attributes::class_name { \
	typedef	entity::PRS::literal_attributes::class_name parent_type; \
	typedef	visitor					visitor_type;	\
	typedef	entity::attribute_visitor_entry<visitor_type>		\
					definition_entry_type;		\
	typedef	definition_entry_type::values_type	values_type;	\
	typedef	values_type::value_type			value_type;	\
public:									\
	static const char				name[];		\
	static void main(visitor_type&, const values_type&);		\
	static good_bool check_vals(const values_type&);		\
private:								\
	static const size_t				id;		\
};	// end class class_name

/**
	\param registrar is the name of the template function used to register
		the class for a specific tool.  
 */
#define DEFINE_PRS_LITERAL_ATTRIBUTE_CLASS(class_name, att_name, registrar) \
const char class_name::name[] = att_name;				\
good_bool								\
class_name::check_vals(const values_type& v) {				\
	return parent_type::__check_vals(name, v);			\
}									\
const size_t class_name::id = registrar<class_name>();

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_LITERAL_ATTRIBUTE_REGISTRY_H__

