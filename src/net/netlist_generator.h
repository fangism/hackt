/**
	\file "net/netlist_generator.h"
	$Id: netlist_generator.h,v 1.13 2011/03/31 01:21:49 fang Exp $
 */

#ifndef	__HAC_NET_NETLIST_GENERATOR_H__
#define	__HAC_NET_NETLIST_GENERATOR_H__

#include <vector>
#include <string>
#include <map>
#include "net/netgraph.h"
#include "Object/global_entry.h"
#include "Object/global_entry_context.h"
#include "Object/lang/cflat_visitor.h"
#include "Object/lang/PRS_footprint_expr.h"	// for precharge_ref_type

namespace HAC {
namespace NET {
using std::vector;
using std::string;
using std::map;
using entity::global_process_context;
using entity::global_entry_context;
using entity::PRS::cflat_visitor;
using entity::footprint_frame;
using entity::global_offset;
using entity::footprint;
using entity::state_instance;
using entity::bool_tag;
using entity::process_tag;
using entity::PRS::footprint_expr_node;
using entity::resolved_attribute_list_type;

/**
	Define to 1 to check for type-name collisions.
	Use global case_collision policy to determine whether or not
	to case slam to_lower.
	Status: done.
 */
#define	POST_MANGLE_TYPE_NAME_COLLISIONS	NETLIST_CHECK_NAME_COLLISIONS

//=============================================================================
/**
	Visitor to do all the heavy-lifting and traversal.  
	TODO: warn against instantiating port processes with production rules.
	Would the rules belong to the caller or the callee?
	Certainly should NOT be duplicated.
	This is the reason for deftype, defchan, etc.
 */
class netlist_generator :
	public global_process_context,
	public global_entry_context,
	public cflat_visitor
{
	typedef	entity::PRS::footprint	prs_footprint;
private:
	struct rule_attribute_functions;
	struct literal_attribute_functions;
#if POST_MANGLE_TYPE_NAME_COLLISIONS
	typedef	std::map<string, const netlist*>
					typename_map_type;
#endif
private:
	ostream& 			os;
	const netlist_options&		opt;
	netlist_map_type		netmap;
#if POST_MANGLE_TYPE_NAME_COLLISIONS
	typename_map_type		typename_map;
#endif

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
#if NETLIST_CACHE_ASSOC_UID
	/**
		For back-linking devices to their home local subcircuit,
		this 1-based indexis used to track.
	 */
	index_type			current_local_subckt_index;
#endif
	/**
		Usually GND, used for precharge, and local prs override.
		Overridden by prs <Vdd, GND>
	 */
	index_type			low_supply;
	/**
		Usually Vdd, used for precharge, and local prs override.
	 */
	index_type			high_supply;
#if PRS_SUBSTRATE_OVERRIDES
	/**
		Substrate contact for NFETs.
	 */
	index_type			low_substrate;
	/**
		Substrate contact for PFETs.
	 */
	index_type			high_substrate;
#endif
	/**
		The base node (initially, Vdd/GND) to connect to the source
		terminal of the next device/expression.  
		This is necessary to correctly connect OR'd expressions.  
		Paired with output_node member.
	 */
	index_type			foot_node;
	/**
		The output node (initially NULL) produced by the 
		last expression, paired against foot_node.  
	 */
	index_type			output_node;
#if NETLIST_GROUPED_TRANSISTORS
	index_type			current_assoc_node;
	bool				current_assoc_dir;
#endif
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
	transistor::attributes_type	fet_attr;
	/**
		In expression traversal, whether or not this level
		of expression is negated.  
		Use for automatic negation-normalization and
		CMOS-implementability checking.  
	 */
	bool				negated;
public:
	netlist_generator(const entity::module&,
		ostream& o, const netlist_options& p);
	~netlist_generator();

	// go!!!
	void
	operator () (void);

	const netlist&
	lookup_netlist(const footprint&) const;

private:
	void
	visit(const state_instance<process_tag>&);

	using global_entry_context::visit;

	void
	visit(const state_instance<bool_tag>&);		// do nothing?
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

	void
	inherit_transistor_attributes(const resolved_attribute_list_type&);

};	// end class netlist_generator

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

#endif	// __HAC_NET_NETLIST_GENERATOR_H__

