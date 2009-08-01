/**
	\file "net/netgraph.cc"
	$Id: netgraph.cc,v 1.1.2.2 2009/08/01 00:13:26 fang Exp $
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
#include "Object/inst/port_alias_tracker.h"
#include "util/stacktrace.h"

namespace HAC {
namespace NET {
#include "util/using_ostream.h"
using entity::port_alias_tracker;
using entity::footprint_frame_map_type;
using std::pair;
using util::value_saver;

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
	INVARIANT(f->is_created());
	netlist_map_type::iterator mi(netmap.find(f));
	const bool first_time = (mi == netmap.end());
	const bool top_level = !current_netlist;
if (first_time) {
	netlist* nl = &netmap[f];	// insert default constructed
	// set current netlist:
	const value_saver<netlist*> __tmp(current_netlist, nl);
	// should not invalidate existing iterators
	const footprint_frame_map_type&
		pfm(p._frame.get_frame_map<process_tag>());
	footprint_frame_map_type::const_iterator i(pfm.begin()), e(pfm.end());
	for ( ; i!=e; ++i) {
		const global_entry<process_tag>&
			subp(sm->get_pool<process_tag>()[*i]);
		// no need to set footprint frames (global use only)
		visit(subp);	// recursion
		// guarantee that dependent type is processed with netlist
		nl->append_instance(subp);	// call subcircuit
	}
	// process local production rules and macros
	f->get_prs_footprint().accept(*this);
	// f->get_spec_footprint().accept(*this);	// ?
	if (!top_level) {
		// create_port_summary from footprint
		// const port_alias_tracker& pa(f->get_port_alias_tracker());
		// use an alias visitor?
		// port_summarizer ps(nl, ...);
		// f->accept(ps);
	}
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
	Traverse footprint in *subcircuit* order first.  
	Would this traversal be useful to other classes?
 */
void
netlist_generator::visit(const entity::PRS::footprint& r) {
	STACKTRACE_VERBOSE;
	typedef	entity::PRS::footprint	prs_footprint;
	typedef	pair<string, netlist> local_subnet_type;	// can be anon
	typedef	vector<local_subnet_type> local_subnet_map_type;
	// no local name translation needed for subcircuits
	typedef	prs_footprint::subcircuit_map_type	subckt_map_type;
	typedef	subckt_map_type::const_iterator		const_iterator;
	local_subnet_map_type local_subcircuits;
	const subckt_map_type& subc_map(r.get_subcircuit_map());
	// must be sorted ranges
{
	// TODO: internal nodes
}{
	const_iterator si(subc_map.begin()), se(subc_map.end());
	// rules
	const prs_footprint::rule_pool_type& rpool(r.get_rule_pool());
	const size_t s = rpool.size();
	size_t i = 0;
	for ( ; i<s; ++i) {
	if (si!=se && (i >= si->rules.first)) {
		// start of a subcircuit range, can be empty
		do {
			local_subcircuits.push_back(local_subnet_type());
			local_subnet_type& n(local_subcircuits.back());
			n.first = si->get_name();
			const value_saver<netlist*>
				__tmp(current_netlist, &n.second);
			rpool[i].accept(*this);
			++i;
		} while (i < si->rules.second);
			--i;	// back-adjust before continue
	} else {
		// rule is outside of subcircuits
		rpool[i].accept(*this);
	}
	}	// end for
}{
	const_iterator si(subc_map.begin()), se(subc_map.end());
	local_subnet_map_type::iterator mi(local_subcircuits.begin());
	// macros
	const prs_footprint::macro_pool_type& mpool(r.get_macro_pool());
	const size_t s = mpool.size();
	size_t i = 0;
	for ( ; i<s; ++i) {
	if (si!=se && (i >= si->macros.first)) {
		// start of a subcircuit range, can be empty
		do {
			INVARIANT(mi != local_subcircuits.end());
			local_subnet_type& n(*mi);
			const value_saver<netlist*>
				__tmp(current_netlist, &n.second);
			mpool[i].accept(*this);
			++mi;
			++i;
		} while (i < si->macros.second);
			--i;	// back-adjust before continue
	} else {
		// macro is outside of subcircuits
		mpool[i].accept(*this);
	}
	}	// end for
}
	// process all subcircuits first, then remaining local rules/macros
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

