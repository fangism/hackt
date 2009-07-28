/**
	\file "net/netgraph.cc"
	$Id: netgraph.cc,v 1.1.2.1 2009/07/28 23:51:37 fang Exp $
 */

#define	ENABLE_STACKTRACE		1

#include <iostream>
#include "net/netgraph.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/global_entry_context.h"
#include "Object/def/footprint.h"
#include "Object/traits/instance_traits.h"
#include "Object/lang/PRS_footprint.h"
#include "util/stacktrace.h"

namespace HAC {
namespace NET {
#include "util/using_ostream.h"
using entity::footprint_frame_map_type;

//=============================================================================
// class netlist method definitions

netlist::netlist() : node_pool(), transistor_pool(), 
		instance_pool(), port_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist::~netlist() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist::append_instance(const global_entry<process_tag>& p) {
	// FINISH_ME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre all dependent subinstances have been processed.
		This should really only be called from netlist_generator.
	\param s is true if this is a subckt and should be wrapped
	in .subckt/.ends.
 */
ostream&
netlist::emit(ostream& o, const bool sub) const {
	// FINISH_ME
	return o;
}

//=============================================================================
// class netlist_generator method definitions

netlist_generator::netlist_generator(const state_manager& _sm,
		const footprint& _topfp, ostream& o, const netlist_options p) :
		cflat_context_visitor(_sm, _topfp), os(o), opt(p), netmap(),
		current_netlist(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
netlist_generator::~netlist_generator() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Main function for starting hierarchy traversal for netlist generation.
	\pre top-level footprint contains top-level instances in
		reserved process slot [0] in the state_manager.
	Algorithm: starting with top-level footprint
		for all dependent subcircuit types
			recurse (pre-order)
		process local rules
		create port map and summary
 */
void
netlist_generator::operator () (void) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(sm);
	const global_entry<process_tag>& ptop(sm->get_pool<process_tag>()[0]);
	visit(ptop);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't know if anything needs to be done here.  
 */
void
netlist_generator::visit(const global_entry<bool_tag>& p) {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visit all dependent subcircuits (processes) *first*.
	Each unique type is only visited once.  
	Remember that top-most level should not be wrapped in subcircuit.  
 */
void
netlist_generator::visit(const global_entry<process_tag>& p) {
	STACKTRACE_VERBOSE;
	// don't need to temporarily set the footprint_frame
	// because algorithm is completely hierarchical, no flattening
	// will need p._frame when emitting subinstances
	const footprint* f(p._frame._footprint);
	NEVER_NULL(f);
	netlist_map_type::iterator mi(netmap.find(f));
const bool first_time = (mi == netmap.end());
const bool top_level = !current_netlist;
if (first_time) {
	netlist* nl = &netmap[f];	// insert default constructed
	// set current netlist:
	const util::value_saver<netlist*> __tmp(current_netlist, nl);
	// should not invalidate existing iterators
	const footprint_frame_map_type&
		pfm(p._frame.get_frame_map<process_tag>());
	footprint_frame_map_type::const_iterator i(pfm.begin()), e(pfm.end());
	for ( ; i!=e; ++i) {
		const global_entry<process_tag>&
			subp(sm->get_pool<process_tag>()[*i]);
		visit(subp);	// recursion
		// guarantee that dependent type is processed with netlist
		nl->append_instance(subp);	// call subcircuit
	}
	// process local production rules and macros
	f->get_prs_footprint().accept(*this);
	// finally, emit this process
	nl->emit(os, !top_level) << endl;
}
	// if this is not top-level, wrap emit in .subckt/.ends
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::visit(const state_manager& s) {
	STACKTRACE_VERBOSE;
	// never called, do nothing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Construct subgraph from LHS expression, and connect final
	output(s) to output node.
	\pre current_netlist is set
 */
void
netlist_generator::visit(const entity::PRS::footprint_rule& r) {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TODO: handle internal node definitions and precharges!
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Walk expressions from left to right, emitting transistors.
	\pre current_netlist is set
 */
void
netlist_generator::visit(const entity::PRS::footprint_expr_node& e) {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expand a macro into netlist.
	Examples: passn and passp for pass-gates.
	\pre current_netlist is set
 */
void
netlist_generator::visit(const entity::PRS::footprint_macro& e) {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::visit(const entity::SPEC::footprint&) {
	// nothing... yet
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
netlist_generator::visit(const entity::SPEC::footprint_directive&) {
	// nothing... yet
}

//=============================================================================
}	// end namespace NET
}	// end namespace HAC

