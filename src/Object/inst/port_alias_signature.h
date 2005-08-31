/**
	\file "Object/inst/port_alias_signature.h"
	$Id: port_alias_signature.h,v 1.1.2.3 2005/08/31 06:19:28 fang Exp $
 */

#ifndef	__OBJECT_INST_PORT_ALIAS_SIGNATURE_H__
#define	__OBJECT_INST_PORT_ALIAS_SIGNATURE_H__

#include <iosfwd>
#include <vector>
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class physical_instance_collection;
class footprint;
class port_formals_manager;
using std::ostream;
using util::memory::count_ptr;

//=============================================================================
/**
	Alias signature for ports.  
	Exported.  
	TODO: cache this somewhere in footprint, to save from
		having to reconstruct it.  
 */
class port_alias_signature {
	typedef	port_alias_signature				this_type;
public:
	typedef	count_ptr<const physical_instance_collection>	value_type;
private:
	typedef	std::vector<value_type>				array_type;
public:
	typedef	array_type::const_iterator			const_iterator;
private:
	array_type						port_array;
public:
	port_alias_signature(const port_formals_manager&, const footprint&);
	~port_alias_signature();

	size_t
	size(void) const { return port_array.size(); }

	const_iterator
	begin(void) const { return port_array.begin(); }

	const_iterator
	end(void) const { return port_array.end(); }

	ostream&
	dump(ostream&) const;

};	// end class port_alias_signature

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_PORT_ALIAS_SIGNATURE_H__

