/**
	\file "net/netgraph.h"
	$Id: netgraph.h,v 1.1.2.1 2009/07/28 23:51:37 fang Exp $
 */

#ifndef	__HAC_NET_NETGRAPH_H__
#define	__HAC_NET_NETGRAPH_H__

#include <iosfwd>
#include <vector>
#include <string>
#include <map>
#include "Object/lang/cflat_context_visitor.h"

namespace HAC {
namespace NET {
using std::ostream;
using std::vector;
using std::string;
using entity::cflat_context_visitor;
using entity::state_manager;
using entity::footprint;
using entity::global_entry;
using entity::bool_tag;
using entity::process_tag;

typedef	size_t		index_type;
typedef	double		real_type;

//=============================================================================
/**
	Extension of (local) node information.  
	Corresponds to an electrical node in the netlist. 
 */
struct node {
	/**
		Index may correspond to index in local bool instance pool.
	 */
	index_type			index;
	/**
		Name of node.
		Named nodes inherit their names from canonical names
		in the original local definition, possibly mangled.  
		Need disambioguous ways of naming internal and 
		auxiliary nodes.
	 */
	string				name;
	// flags:
	// is_named -- if this was a named node in original source, 
	//	otherwise is internal, auxiliary node.
	char				flags;
	// connectivity information needed? would be redundant with devices

	bool
	is_named_node(void) const;

	bool
	is_internal_node(void) const;

};	// end struct node

//-----------------------------------------------------------------------------
/**
	Standard 4-terminal device for transistor.
 */
struct transistor {
	/**
		Devices are somehow named after the rules from which
		they are derived.  
		TODO: need auto-enumeration
	 */
	string				name;
	index_type			source;
	index_type			gate;
	index_type			drain;
	/**
		Substrate contact.
	 */
	index_type			body;
	/// device width parameter
	real_type			width;
	/// device length parameter
	real_type			length;
	/// device type: nfet, pfet
	char				type;
	// TODO: support overriding of device type
	// e.g. hvt, lvt, svt...

	// allow a conf/tech file to define/enumerate additional types
	// attributes:
	// is_standard_keeper
	// is_combination_feedback_keeper
	char				attributes;

};	// end struct transistor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic 2-terminal device.
	Capacitor, resistor, or inductor.
 */
struct passive_device {
	/**
		Points to the two terminals.  
	 */
	index_type			t[2];
	// enum type: (R) resistor, (L) inductor, (C) capacitor
	// (D) diode should be separate as it can be tied to a model
	char				type;
};	// end struct device

//-----------------------------------------------------------------------------
/**
	Instantiation of a particular substructure type.
	spice card: x
 */
struct instance {
	typedef	vector<index_type>	actuals_list_type;
};	// end struct instance
//-----------------------------------------------------------------------------
/**
	A graph.
	One netlist corresponds to a subcircuit, or in HAC a unique footprint.  
	TODO: support for special global nodes like Vdd, GND?
		can be automatic or overridden.
 */
class netlist {
	typedef	vector<node>		node_pool_type;
	/**
		TODO: group transistors in a way that reflects
		original rule in PRS, if applicable.
		For now, flat list.
		TODO: print comment with origin rule before each
		group of transistors.
	 */
	typedef	vector<transistor>	transistor_pool_type;
	typedef	vector<passive_device>	passive_device_pool_type;
	typedef	vector<instance>	instance_pool_type;
	/**
		Indices refer to local nodes which need not be the 
		exact same set of local port aliases.
		May include Vdd, GND.
		Internal nodes cannot be ports.  
	 */
	typedef	vector<index_type>	port_list_type;

	/**
		All local nodes, including ports and internal nodes.
	 */
	node_pool_type			node_pool;
	transistor_pool_type		transistor_pool;
	instance_pool_type		instance_pool;
	/**
		NOTE: there cannot be any aliases in the ports.  
	 */
	port_list_type			port_list;
	// internal counters?

public:
	netlist();
	~netlist();

	void
	append_instance(const global_entry<process_tag>&);

	ostream&
	emit(ostream&, const bool s) const;

};	// end class netlist

//=============================================================================
struct netlist_options {
};	// end struct netlist_options

//=============================================================================
/**
	Visitor to do all the heavy-lifting and traversal.  
 */
class netlist_generator : public cflat_context_visitor {
	/**
		Primary structure for maintaining prerequisite ordering
		of emitting dependent subcircuits before they are used.
		Don't actually know if spice requires subcircuits
		to be defined before used, but this is safe.
		This way, only circuits that are used are emitted.
	 */
	typedef	std::map<const footprint*, netlist>
					netlist_map_type;
private:
	ostream& 			os;
	const netlist_options&		opt;
	netlist_map_type		netmap;
	netlist*			current_netlist;
public:
	netlist_generator(const state_manager& _sm, 
		const footprint& _topfp, ostream& o, const netlist_options p);
	~netlist_generator();

	// go!!!
	void
	operator () (void);

	void
	visit(const global_entry<process_tag>&);

	using cflat_context_visitor::visit;

	void
	visit(const global_entry<bool_tag>&);		// do nothing?
	void
	visit(const state_manager&);		// only visit processes
	void
	visit(const footprint&);
	void
	visit(const entity::PRS::footprint_rule&);
	void
	visit(const entity::PRS::footprint_expr_node&);		// do nothing?
	void
	visit(const entity::PRS::footprint_macro&);

	void
	visit(const entity::SPEC::footprint&);			// do nothing
	void
	visit(const entity::SPEC::footprint_directive&);	// do nothing

};	// end class netlist_generator

//=============================================================================

}	// end namespace NET
}	// end namespace HAC

#endif	// __HAC_NET_NETGRAPH_H__

