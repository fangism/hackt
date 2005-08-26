/**
	\file "Object/inst/port_alias_signature.cc"
	$Id: port_alias_signature.cc,v 1.1.2.1 2005/08/26 21:11:04 fang Exp $
 */

#include "Object/inst/port_alias_signature.h"
#include "Object/inst/instance_collection_base.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/def/footprint.h"
#include "Object/def/port_formals_manager.h"
#include "util/reserve.h"
#include "util/memory/count_ptr.tcc"

namespace ART {
namespace entity {
//=============================================================================
// class port_alias_signature method definitions

/**
	\param pfm the port formals manager from which to acquire ports.
	\param fp the footprint from which to get the actual
		definition-local instance collections (of the ports)
 */
port_alias_signature::port_alias_signature(
		const port_formals_manager& pfm, const footprint& fp) :
		port_array() {
	util::reserve(port_array, pfm.size());
	typedef	port_formals_manager::const_list_iterator
			const_port_iterator;
	const_port_iterator i(pfm.begin());
	const const_port_iterator e(pfm.end());
	for ( ; i!=e; i++) {
		port_array.push_back(
			fp[(*i)->get_name()]
				.is_a<const physical_instance_collection>()
		);
		NEVER_NULL(port_array.back());
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
port_alias_signature::~port_alias_signature() { }

//=============================================================================
}	// end namespace entity
}	// end namespace ART

