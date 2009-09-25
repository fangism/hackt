/**
	\file "net/netlist_generator.h"
	$Id: netlist_generator.h,v 1.3.2.1 2009/09/25 01:21:42 fang Exp $
 */

#ifndef	__HAC_NET_NETLIST_GENERATOR_H__
#define	__HAC_NET_NETLIST_GENERATOR_H__

#include <vector>
#include <string>
#include <map>
#include "net/netgraph.h"
#include "Object/lang/cflat_context_visitor.h"
#include "Object/lang/PRS_footprint_expr.h"	// for precharge_ref_type

namespace HAC {
namespace NET {
using std::vector;
using std::string;
using std::map;
using entity::cflat_context_visitor;
using entity::state_manager;
using entity::footprint;
using entity::global_entry;
using entity::bool_tag;
using entity::process_tag;
using entity::PRS::footprint_expr_node;

//=============================================================================
/**
	Visitor to do all the heavy-lifting and traversal.  
	TODO: warn against instantiating port processes with production rules.
	Would the rules belong to the caller or the callee?
	Certainly should NOT be duplicated.
	This is the reason for deftype, defchan, etc.
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
	typedef	entity::PRS::footprint	prs_footprint;
private:
	ostream& 			os;
	const netlist_options&		opt;
	netlist_map_type		netmap;

// local data used only during traversal:
	/**
		Need this for back-reference to expression pool.
	 */
	const prs_footprint*		prs;
	// need a place to hold netlist that belong to subcircuits
	// which don't have associated footprints, for now might 
	// only keep them around temporarily and locally
	netlist*			current_netlist;
	/**
		Where devices are to be added.  
		Can point to either primary subcircuit
		or local subcircuit.  
	 */
	netlist_common*			current_local_netlist;
	/**
		Usually GND, used for precharge, and local prs override.
		Overridden by prs <Vdd, GND>
	 */
	index_type			low_supply;
	/**
		Usually Vdd, used for precharge, and local prs override.
	 */
	index_type			high_supply;
	/**
		The base node (initially, Vdd/GND) to connect to the source
		terminal of the next device/expression.  
		This is necessary to correctly connect OR'd expressions.  
	 */
	index_type			foot_node;
	/**
		The output node (initially NULL) produced by the 
		last expression.  
	 */
	index_type			output_node;
	/**
		For transistor sizing, let sizes be sticky, and
		propagate to latter unspecified series transistors.
		Q: do these really need to be separated by NFET/PFET?
		Perhaps, for different min/max?
		With exception of precharge and internal node, 
		expressions types can be walked one at a time.
		Can easily reset defaults each time.
	 */
	real_type			current_width;
	real_type			current_length;
	/**
		Default transistor is used unless overridden by directive.
		Usually determined by direction of pull of rule, 
		or pass-gate type.
	 */
	transistor::fet_type		fet_type;
	/**
		Current attributes to apply to generated FETs
		See transistor::attributes.
	 */
	// transistor::flags		fet_attr;
	char				fet_attr;
	/**
		In expression traversal, whether or not this level
		of expression is negated.  
		Use for automatic negation-normalization and
		CMOS-implementability checking.  
	 */
	bool				negated;
	/**
		The last specified device width, for automatic propagation.  
	 */
	real_type			last_width;
	/**
		The last specified device length, for automatic propagation.  
	 */
	real_type			last_length;
public:
	netlist_generator(const state_manager& _sm, 
		const footprint& _topfp, ostream& o, const netlist_options& p);
	~netlist_generator();

	// go!!!
	void
	operator () (void);

private:
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
	visit(const prs_footprint&);		// override
	void
	visit(const entity::PRS::footprint_rule&);
	void
	visit(const footprint_expr_node&);		// do nothing?
	void
	visit(const entity::PRS::footprint_macro&);

	void
	visit(const entity::SPEC::footprint&);			// do nothing
	void
	visit(const entity::SPEC::footprint_directive&);	// do nothing

private:
	void
	visit(const footprint_expr_node::precharge_pull_type&);

	template <class RP>
	void
	visit_rule(const RP&, const index_type);

	template <class MP>
	void
	visit_macro(const MP&, const index_type);

	void
	set_current_width(const real_type);

	void
	set_current_length(const real_type);

	index_type
	register_internal_node(const index_type);

	index_type
	register_named_node(const index_type);

};	// end class netlist_generator

//=============================================================================

}	// end namespace NET
}	// end namespace HAC

#endif	// __HAC_NET_NETLIST_GENERATOR_H__

