/**
	\file "Object/inst/collection_traits.h"
	Instance and value-collection traits.  
	$Id: collection_traits.h,v 1.1.2.1 2006/11/03 05:22:22 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_COLLECTION_TRAITS_H__
#define	__HAC_OBJECT_INST_COLLECTION_TRAITS_H__

#include "util/size_t.h"
#include "Object/inst/collection_fwd.h"

namespace HAC {
namespace entity {
//=============================================================================
typedef	enum {
	INSTANCE_COLLECTION_TYPE_NULL = 0,
	INSTANCE_COLLECTION_TYPE_SCALAR = 1, 
	INSTANCE_COLLECTION_TYPE_1D,
	INSTANCE_COLLECTION_TYPE_2D,
	INSTANCE_COLLECTION_TYPE_3D,
	INSTANCE_COLLECTION_TYPE_4D,
	INSTANCE_COLLECTION_TYPE_PORT_FORMAL,
	INSTANCE_COLLECTION_TYPE_PORT_ACTUAL
} instance_collection_type_enum ;

typedef	enum {
	VALUE_COLLECTION_TYPE_NULL = 0,
	VALUE_COLLECTION_TYPE_SCALAR = 1, 
	VALUE_COLLECTION_TYPE_1D,
	VALUE_COLLECTION_TYPE_2D,
	VALUE_COLLECTION_TYPE_3D,
	VALUE_COLLECTION_TYPE_4D,
	VALUE_COLLECTION_TYPE_TEMPLATE_FORMAL
} value_collection_type_enum ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This maps between collection types and enumerations.  
 */
template <class>
struct collection_traits;

template <class Tag, size_t D>
struct collection_traits<instance_array<Tag, D> > {
	// add 1 to intentionally reserve 0 as NULL
	enum { ENUM_VALUE = D+1 };
};

template <class Tag>
struct collection_traits<port_formal_array<Tag> > {
	enum { ENUM_VALUE = INSTANCE_COLLECTION_TYPE_PORT_FORMAL };
};

template <class Tag>
struct collection_traits<port_actual_collection<Tag> > {
	enum { ENUM_VALUE = INSTANCE_COLLECTION_TYPE_PORT_ACTUAL };
};

template <class Tag, size_t D>
struct collection_traits<value_array<Tag, D> > {
	// add 1 to intentionally reserve 0 as NULL
	enum { ENUM_VALUE = D+1 };
};

#if 0
template <class Tag>
struct collection_traits<value_formal_array<Tag> > {
	enum { ENUM_VALUE = VALUE_COLLECTION_TYPE_TEMPLATE_FORMAL };
};
#endif

//-----------------------------------------------------------------------------
template <unsigned char>
struct instance_collection_type_map;

template <>
struct instance_collection_type_map<INSTANCE_COLLECTION_TYPE_SCALAR> {
	template <class Tag>
	struct collection {
		typedef	instance_array<Tag, 0>		type;
	};
};

template <>
struct instance_collection_type_map<INSTANCE_COLLECTION_TYPE_1D> {
	template <class Tag>
	struct collection {
		typedef	instance_array<Tag, 1>		type;
	};
};

template <>
struct instance_collection_type_map<INSTANCE_COLLECTION_TYPE_2D> {
	template <class Tag>
	struct collection {
		typedef	instance_array<Tag, 2>		type;
	};
};

template <>
struct instance_collection_type_map<INSTANCE_COLLECTION_TYPE_3D> {
	template <class Tag>
	struct collection {
		typedef	instance_array<Tag, 3>		type;
	};
};

template <>
struct instance_collection_type_map<INSTANCE_COLLECTION_TYPE_4D> {
	template <class Tag>
	struct collection {
		typedef	instance_array<Tag, 4>		type;
	};
};

template <>
struct instance_collection_type_map<INSTANCE_COLLECTION_TYPE_PORT_FORMAL> {
	template <class Tag>
	struct collection {
		typedef	port_formal_array<Tag>		type;
	};
};

template <>
struct instance_collection_type_map<INSTANCE_COLLECTION_TYPE_PORT_ACTUAL> {
	template <class Tag>
	struct collection {
		typedef	port_actual_collection<Tag>	type;
	};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <unsigned char>
struct value_collection_type_map;

template <>
struct value_collection_type_map<VALUE_COLLECTION_TYPE_SCALAR> {
	template <class Tag>
	struct collection {
		typedef	value_array<Tag, 0>		type;
	};
};

template <>
struct value_collection_type_map<VALUE_COLLECTION_TYPE_1D> {
	template <class Tag>
	struct collection {
		typedef	value_array<Tag, 1>		type;
	};
};

template <>
struct value_collection_type_map<VALUE_COLLECTION_TYPE_2D> {
	template <class Tag>
	struct collection {
		typedef	value_array<Tag, 2>		type;
	};
};

template <>
struct value_collection_type_map<VALUE_COLLECTION_TYPE_3D> {
	template <class Tag>
	struct collection {
		typedef	value_array<Tag, 3>		type;
	};
};

template <>
struct value_collection_type_map<VALUE_COLLECTION_TYPE_4D> {
	template <class Tag>
	struct collection {
		typedef	value_array<Tag, 4>		type;
	};
};

#if 0
template <>
struct value_collection_type_map<VALUE_COLLECTION_TYPE_TEMPLATE_FORMAL> {
	template <class Tag>
	struct collection {
		typedef	value_formal_array<Tag>		type;
	};
};
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_COLLECTION_TRAITS_H__

