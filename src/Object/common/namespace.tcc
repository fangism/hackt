/**
	\file "Object/common/namespace.tcc"
	Template method definitions for class name_space.  
	This file was "Object/common/namespace.tcc"
		in a previous life.  
	$Id: namespace.tcc,v 1.2 2005/07/23 06:52:22 fang Exp $
 */

#ifndef	__OBJECT_COMMON_NAMESPACE_TCC__
#define	__OBJECT_COMMON_NAMESPACE_TCC__

#include "util/hash_specializations.h"	// needs to be first
#include "Object/common/namespace.h"

namespace ART {
namespace entity {
//=============================================================================
// class namespace method definitions

/**
	General collector of things over namespaces.  
	Accumulates matches in the list l.
	Things may be definitionas, instance collections, etc...
	\param L the list type to accumulate.  
		The list type requires only a push_back interface.  
		The type of object to collect is deduced from L::value_type.  
		Constraint: L::value_type must be a never_ptr.
			I should enforce this with a concept-check...  
	\param l the list in which to accumulate matches.  
 */
template <class L>
void
name_space::collect(L& l) const {
	typedef	typename L::value_type			pointer_type;
	typedef	typename pointer_type::element_type	element_type;
	used_id_map_type::const_iterator i = used_id_map.begin();
	const used_id_map_type::const_iterator e = used_id_map.end();
	for ( ; i!=e; i++) {
		// const here will check for const-copy-constructibility
		// where push_back is called.  
		const never_ptr<object> o(i->second);
		const pointer_type p(o.template is_a<element_type>());
		// template keyword needed by g++-3.3, I believe.  
		if (p)  l.push_back(p);
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_COMMON_NAMESPACE_TCC__

