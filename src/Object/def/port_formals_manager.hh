/**
	\file "Object/def/port_formals_manager.hh"
	Definition port formal instance manager class.  
	This file was "Object/def/port_formals_manager.h"
		in a previous life.  
	$Id: port_formals_manager.hh,v 1.13 2009/10/02 01:56:49 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_PORT_FORMALS_MANAGER_HH__
#define	__HAC_OBJECT_DEF_PORT_FORMALS_MANAGER_HH__

#include <iosfwd>
#include <vector>
#include <string>
#include <map>

#include "util/macros.h"
#include "Object/common/util_types.hh"	// for checked_refs_type
#include "Object/unroll/target_context.hh"
#include "Object/devel_switches.hh"

#include "util/boolean_types.hh"
#include "util/persistent_fwd.hh"
#include "util/memory/excl_ptr.hh"

namespace HAC {
namespace parser {
	class token_identifier;
}
//=============================================================================
namespace entity {
class instance_placeholder_base;
class physical_instance_placeholder;
class physical_instance_collection;
class footprint;
using std::string;
using std::istream;
using std::ostream;
using std::vector;
using parser::token_identifier;
using util::bad_bool;
using util::good_bool;
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
	typedef	physical_instance_placeholder	instance_placeholder_type;
public:
	typedef	never_ptr<const instance_placeholder_type>
						port_formals_value_type;
	/**
		Table of port formals.
		The types can be data-types or channel-types, 
		either base-types or user-defined types.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
		Implemented as a map and list.  
		The list and map contain both normally declared ports
		AND now implicitly declared global ports.  
		Need to be careful to distinguish between the two.
	**/
	typedef vector<port_formals_value_type>	port_formals_list_type;
	typedef std::map<string, port_formals_value_type>
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
#if IMPLICIT_SUPPLY_PORTS
private:
	// cached counts of number of implicit ports (globals)
	size_t					__implicit_ports;
	// cached counts of number of explicit ports
	size_t					__explicit_ports;
#endif
public:
	port_formals_manager();
	~port_formals_manager();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

private:
	// counts both implicit and explicit ports
	size_t
	size(void) const { return port_formals_list.size(); }

public:
#if IMPLICIT_SUPPLY_PORTS
	size_t
	implicit_ports(void) const { return __implicit_ports; }
	size_t
	explicit_ports(void) const { return __explicit_ports; }
#endif

	// note: these iterate over ALL ports, implicit and explicit
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

	good_bool
	unroll_ports(const target_context&,
		vector<never_ptr<physical_instance_collection> >&) const;

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

#endif	// __HAC_OBJECT_DEF_PORT_FORMALS_MANAGER_HH__

