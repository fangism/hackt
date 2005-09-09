/**
	\file "Object/port_context.h"
	$Id: port_context.h,v 1.1.2.3 2005/09/09 20:12:30 fang Exp $
 */

#ifndef	__OBJECT_PORT_CONTEXT_H__
#define	__OBJECT_PORT_CONTEXT_H__

#include <iosfwd>
// #include <valarray>
#include <vector>

namespace ART {
namespace entity {
using std::ostream;
struct port_context;

//=============================================================================
struct port_member_context;
struct port_collection_context;

//-----------------------------------------------------------------------------
/**
	One of these for each instance_collection. 
	Each member collection may contain several array members.  
 */
struct port_member_context {
	typedef	std::vector<port_collection_context>	member_array_type;
	member_array_type				member_array;

	typedef	member_array_type::const_iterator	const_iterator;
	typedef	member_array_type::iterator		iterator;

	port_member_context();
	~port_member_context();

	size_t
	size(void) const { return member_array.size(); }

	void
	resize(const size_t);

	port_collection_context&
	operator [] (const size_t);

	const port_collection_context&
	operator [] (const size_t) const;

	ostream&
	dump(ostream&) const;

};	// end struct port_member_context

//-----------------------------------------------------------------------------
/**
	Array for each member of a collection.  
	TODO: consider using vector of pair<size_t, port_member_context>
 */
struct port_collection_context {
	typedef	std::vector<size_t>			id_map_type;
	/**
		Only used if type has substructure.  
	 */
	typedef	std::vector<port_member_context>	substructure_array_type;

	id_map_type					id_map;
	substructure_array_type				substructure_array;

	port_collection_context();
	~port_collection_context();

	size_t
	size(void) const { return id_map.size(); }

	void
	resize(const size_t);

	ostream&
	dump(ostream&) const;

};	// end struct port_collection_context

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_PORT_CONTEXT_H__

