/**
	\file "Object/traits/classification_tags.hh"
	Classification of types in HAC.
	These type tags may be used for specialization of many
	features of the language.  
	This file used to be "Object/art_object_classification_tags.h".
	$Id: classification_tags.hh,v 1.10 2010/09/21 00:18:27 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_CLASSIFICATION_TAGS_H__
#define	__HAC_OBJECT_TRAITS_CLASSIFICATION_TAGS_H__

#include "Object/traits/classification_tags_fwd.hh"

namespace HAC {
namespace entity {
template <class Tag> class instance_collection;
template <class Tag> class value_collection;

/***
	Rationale for NOT making the tag classes the actual traits class?
	We don't want an actual inheritance relationship between
	the individual traits classes; it would lead to potential
	confusion inheriting traits (mistakenly) from parents.  
	We only want tag inheritance relationships.  
***/

/**
	Mother tag for all instance- and value- related classes.  
 */
struct object_entity_tag { };

/**
	Root tag for objects that correspond to physical entities.
	Such objects have reference-semantics in the language.  
 */
struct physical_instance_tag : public object_entity_tag {
	typedef	object_entity_tag	parent_tag;

	template <class Tag>
	struct collection {
		typedef	instance_collection<Tag>	type;
	};
};	// end struct physical_instance_tag

/**
	Root tag for object that correspond to values seen by the compiler.  
	Such objects have value-semantics in the language.  
 */
struct parameter_value_tag : public object_entity_tag {
	typedef	object_entity_tag	parent_tag;

	template <class Tag>
	struct collection {
		typedef	value_collection<Tag>		type;
	};
};	// end struct parameter_value_tag

/**
	This tag is pertains to channel entities.  
	Channels have communication semantics in the language.  
 */
struct channel_tag : public physical_instance_tag {
	typedef	physical_instance_tag	parent_tag;
};	// end struct channel_tag

/**
	Processes are independent, concurrent functional units, 
	and are used to compose systems.  
 */
struct process_tag : public physical_instance_tag {
	typedef	physical_instance_tag	parent_tag;
};	// end struct process_tag

/**
	Datatypes carry values as interpreted by some physical representation.
 */
struct datatype_tag : public physical_instance_tag {
	typedef	physical_instance_tag	parent_tag;
};	// end struct datatype_tag

/**
	Boolean compiler parameters.  
 */
struct pbool_tag : public parameter_value_tag {
	typedef	parameter_value_tag	parent_tag;
};	// end struct pbool_tag

/**
	Integer compiler parameters.  
 */
struct pint_tag : public parameter_value_tag {
	typedef	parameter_value_tag	parent_tag;
};	// end struct pint_tag

/**
	Floating-point compiler parameters.  
 */
struct preal_tag : public parameter_value_tag {
	typedef	parameter_value_tag	parent_tag;
};	// end struct preal_tag

/**
	String parameters.
 */
struct pstring_tag : public parameter_value_tag {
	typedef	parameter_value_tag	parent_tag;
};	// end struct pstring_tag

/**
	Basic building block unit of data: a single node.  
 */
struct bool_tag : public datatype_tag {
	typedef	datatype_tag		parent_tag;
};	// end struct bool_tag

/**
	Integer interpreted data.  
 */
struct int_tag : public datatype_tag {
	typedef	datatype_tag		parent_tag;
};	// end struct int_tag

/**
	Real-valued interpreted data.  
 */
struct real_tag : public datatype_tag {
	typedef	datatype_tag		parent_tag;
};	// end struct real_tag

/**
	String-valued interpreted data.  
 */
struct string_tag : public datatype_tag {
	typedef	datatype_tag		parent_tag;
};	// end struct string_tag

/**
	Enumerated data type.  
 */
struct enum_tag : public datatype_tag {
	typedef	datatype_tag		parent_tag;
};	// end struct enum_tag

/**
	Composite (struct) data type.  
 */
struct datastruct_tag : public datatype_tag {
	typedef	datatype_tag		parent_tag;
};	// end struct datastruct_tag

/**
	User-defined channel type.  
 */
struct user_channel_tag : public channel_tag {
	typedef	channel_tag		parent_tag;
};	// end struct user_channel_tag

/**
	Intrinsi base channel type.  
 */
struct builtin_channel_tag : public channel_tag {
	typedef	channel_tag		parent_tag;
};	// end struct builtin_channel_tag

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_CLASSIFICATION_TAGS_H__

