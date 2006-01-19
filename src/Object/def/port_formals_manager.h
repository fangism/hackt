/**
	\file "Object/def/port_formals_manager.h"
	Definition port formal instance manager class.  
	This file was "Object/def/port_formals_manager.h"
		in a previous life.  
	$Id: port_formals_manager.h,v 1.4.2.1 2006/01/19 07:42:41 fang Exp $
 */

#ifndef	__OBJECT_DEF_PORT_FORMALS_MANAGER_H__
#define	__OBJECT_DEF_PORT_FORMALS_MANAGER_H__

#include <iosfwd>
#include <vector>
#include "util/macros.h"
#include "Object/common/util_types.h"	// for checked_refs_type

#include "util/boolean_types.h"
#include "util/persistent_fwd.h"
#include "util/hash_qmap.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace parser {
	class token_identifier;
}
//=============================================================================
namespace entity {
class instance_collection_base;
class physical_instance_collection;
class unroll_context;
class subinstance_manager;		// basically, the port_actuals
using std::string;
using std::istream;
using std::ostream;
using std::vector;
using parser::token_identifier;
using util::bad_bool;
using util::good_bool;
using util::hash_qmap;
using util::persistent;
using util::persistent_object_manager;
using util::memory::excl_ptr;
using util::memory::never_ptr;

//=============================================================================
/**
	General definition public port instance manager.  
	Note: these are just placeholders for definitions;
	unrolling and creation are done with complete_types' private
	copies of the collections maintained in the footprint.  
	TODO: upgrade port_formals_value to physical_instance_collection?
 */
class port_formals_manager {
	typedef	physical_instance_collection	instance_collection_type;
public:
	typedef	never_ptr<const instance_collection_type>
						port_formals_value_type;
	/**
		Table of port formals.
		The types can be data-types or channel-types, 
		either base-types or user-defined types.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
		Implemented as a hash_qmap and list.  
	**/
	typedef vector<port_formals_value_type>	port_formals_list_type;
	typedef hash_qmap<string, port_formals_value_type>
						port_formals_map_type;
	typedef	port_formals_list_type::const_iterator
						const_list_iterator;
	// List of language bodies, separate or merged?

	enum {
		/**
			Because we disdain magic numbers...
		 */
		INVALID_POSITION = 0
	};

protected:
	port_formals_list_type			port_formals_list;
	port_formals_map_type			port_formals_map;
public:
	port_formals_manager();
	~port_formals_manager();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	size(void) const { return port_formals_list.size(); }

	const_list_iterator
	begin(void) const { return port_formals_list.begin(); }

	const_list_iterator
	end(void) const { return port_formals_list.end(); }

	/** overrides definition_base's */
	port_formals_value_type
	lookup_port_formal(const string& id) const;

	size_t
	lookup_port_formal_position(const string& id) const;

	void
	add_port_formal(const port_formals_value_type);

	good_bool
	certify_port_actuals(const checked_refs_type& ol) const;

	bool
	equivalent_port_formals(const port_formals_manager&) const;

	void
	unroll_ports(const unroll_context&, subinstance_manager&) const;

public:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class port_formals_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_DEF_PORT_FORMALS_MANAGER_H__

