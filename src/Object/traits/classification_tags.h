/**
	\file "Object/traits/classification_tags.h"
	Classification of types in ART.
	These type tags may be used for specialization of many
	features of the language.  
	This file used to be "Object/art_object_classification_tags.h".
	$Id: classification_tags.h,v 1.1.2.1 2005/07/11 03:27:15 fang Exp $
 */

#ifndef	__OBJECT_TRAITS_CLASSIFICATION_TAGS_H__
#define	__OBJECT_TRAITS_CLASSIFICATION_TAGS_H__

namespace ART {
namespace entity {

/**
	Mother tag for all instance- and value- related classes.  
 */
struct object_entity_tag { };

/**
	Root tag for objects that correspond to physical entities.
	Such objects have reference-semantics in the language.  
 */
struct physical_instance_tag : public object_entity_tag { };

/**
	Root tag for object that correspond to values seen by the compiler.  
	Such objects have value-semantics in the language.  
 */
struct parameter_value_tag : public object_entity_tag { };

/**
	This tag is pertains to channel entities.  
	Channels have communication semantics in the language.  
 */
struct channel_tag : public physical_instance_tag { };

/**
	Processes are independent, concurrent functional units, 
	and are used to compose systems.  
 */
struct process_tag : public physical_instance_tag { };

/**
	Datatypes carry values as interpreted by some physical representation.
 */
struct datatype_tag : public physical_instance_tag { };

/**
	Boolean compiler parameters.  
 */
struct pbool_tag : public parameter_value_tag { };

/**
	Integer compiler parameters.  
 */
struct pint_tag : public parameter_value_tag { };

/**
	Floating-point compiler parameters.  
 */
struct pfloat_tag : public parameter_value_tag { };

/**
	Basic building block unit of data: a single node.  
 */
struct bool_tag : public datatype_tag { };

/**
	Integer interpreted data.  
 */
struct int_tag : public datatype_tag { };

/**
	Enumerated data type.  
 */
struct enum_tag : public datatype_tag { };

/**
	Composite (struct) data type.  
 */
struct datastruct_tag : public datatype_tag { };

/**
	User-defined channel type.  
 */
struct user_channel_tag : public channel_tag { };

/**
	Intrinsi base channel type.  
 */
struct builtin_channel_tag : public channel_tag { };

}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TRAITS_CLASSIFICATION_TAGS_H__

