/**
	\file "Object/lang/cflat_printer.cc"
	Implementation of cflattening visitor.
	$Id: cflat_printer.cc,v 1.35 2011/05/02 21:27:18 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <algorithm>
#include <set>
#include <sstream>
#include <numeric>
#include "Object/lang/cflat_printer.hh"
#include "Object/lang/cflat_printer.tcc"
#include "Object/lang/PRS_enum.hh"
#include "Object/lang/PRS_footprint.hh"
#include "Object/lang/PRS_attribute_registry.hh"
#include "Object/lang/PRS_macro_registry.hh"
#include "Object/lang/SPEC_footprint.hh"
#include "Object/lang/SPEC_registry.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/inst/state_instance.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/connection_policy.hh"
#include "Object/global_entry.hh"
#include "Object/global_channel_entry.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/traits/proc_traits.hh"
#include "main/cflat_options.hh"
#include "common/ICE.hh"
#include "common/TODO.hh"
#include "util/stacktrace.hh"
#include "util/offset_array.hh"
#include "util/member_saver.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/preal_const.hh"
#include "util/numeric/functional.hh"	// for reciprocate

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.hh"
using std::accumulate;
using std::transform;
using util::numeric::reciprocate;

//=============================================================================
// class cflat_prs_printer method definitions

cflat_prs_printer::~cflat_prs_printer() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat_prs_printer::__visit(const entity::footprint& f) {
	STACKTRACE_VERBOSE;
	// visit rules and spec directives, locally
if (cfopts.include_prs && f.has_prs_footprint()) {
	f.get_prs_footprint().accept(*this);
}
if (f.has_spec_footprint()) {
	f.get_spec_footprint().accept(*this);
}
	// f.get_chp_footprint().accept(*this);
	parent_type::visit(f);	// visit_recursive
	visit_local<bool_tag>(f, at_top());	// for bool attributes

	// for channel-terminals need to check both ports and locals
	visit_ports<process_tag>(f);
	visit_local<process_tag>(f, at_top());
	// exception printing handled by caller
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat_prs_printer::visit(const entity::footprint& f) {
	const bool b = (f.get_meta_type() == META_TYPE_PROCESS);
if (cfopts.show_hierarchy && current_gpid() && b) {
	const size_t gpid = current_gpid();
	std::ostringstream oss;
	oss << "$process: ";
	__dump_resolved_canonical_literal<process_tag>(oss, gpid);
	f.dump_type(oss << " (type: ") << ')';
	os << oss.str() << " {" << endl;
	__visit(f);
	os << "} // end " << oss.str() << endl;
} else {
	__visit(f);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat_prs_printer::visit(const PRS::footprint& p) {
	STACKTRACE_VERBOSE;
	cflat_visitor::visit(p);
	// now handle invariant expressions
// if (cfopts.primary_tool == cflat_options::TOOL_LVS) {
// ah, hell, just print it for everything, it's easy to grep out
	typedef	PRS::footprint::invariant_pool_type::const_iterator
						const_iterator;
	const expr_type_setter tmp(*this, PRS_LITERAL_TYPE_ENUM);
	const PRS::footprint::invariant_pool_type& ip(p.get_invariant_pool());
	const PRS_footprint_expr_pool_type& ep(p.get_expr_pool());
	const_iterator i(ip.begin()), e(ip.end());
	for ( ; i!=e; ++i) {
		os << "invariant ";
		// ep[*i].accept(*this);
#if INVARIANT_BACK_REFS
		visit(ep[i->first]);
#else
		visit(ep[*i]);
#endif
		os << endl;
	}
// }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Process attributes through the rule-attribute registry.
 */
static
void
__print_prefix_rule_attributes(const resolved_attribute_list_type& attrs, 
		cflat_prs_printer& v) {
	typedef	resolved_attribute_list_type::const_iterator
					const_iterator;
	const_iterator i(attrs.begin());
	const const_iterator e(attrs.end());
	resolved_attribute::values_type empty;
	for ( ; i!=e; ++i) {
		// already checked registered
		cflat_rule_attribute_registry.find(i->key)->second
			.main(v, (i->values ? *i->values : empty));
		// fake an empty list if necessary
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out the entire rule.  
	Adapted from footprint::cflat_rule().  
	Q1: why is the expression outside of the conditional?
	Q2: why is the conditional checked here? (more efficient in parent)
	TODO: support attributes (or use a visitor?)
 */
void
cflat_prs_printer::visit(const footprint_rule& r) {
	STACKTRACE_VERBOSE;
	const expr_type_setter tmp(*this, PRS_LITERAL_TYPE_ENUM);
if (!cfopts.check_prs) {
	if (r.attributes.size()) {
		__print_prefix_rule_attributes(r.attributes, *this);
	}
	const PRS_footprint_expr_pool_type* const expr_pool = 
		&get_current_footprint().get_prs_footprint().get_expr_pool();
	(*expr_pool)[r.expr_index].accept(*this);
	os << " -> ";
	// r.output_index gives the local unique ID,
	// which needs to be translated to global ID.
	// bfm[...] refers to a state_instance<bool_tag> (1-indexed)
	// const size_t j = bfm[r.output_index-1];
	__dump_canonical_literal<bool_tag>(r.output_index);
	os << (r.dir ? '+' : '-');
	if (cfopts.show_supply_nodes) {
		// FINISH ME
		// ick: computing effective index by address subtraction!
		const PRS::footprint& pfp(fpf->_footprint->get_prs_footprint());
		const size_t ri = std::distance(&pfp.get_rule_pool()[0], &r);
		const PRS::footprint::supply_map_type::const_iterator
			f(pfp.lookup_rule_supply(ri));
		const size_t sn = (r.dir ? f->Vdd : f->GND);
		os << " {";
		__dump_canonical_literal<bool_tag>(sn);
		os << "}";
	}
	if (cfopts.compute_conductances) {
		// min/mxa_conductance was evaluated from expression
		os << "\t(G min,one,max = " << min_conductance << ", " <<
			one_conductance << ", " <<
			max_conductance << ")";
	}
	os << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out the expression. 
	Adapted from footprint::cflat_expr().  
 */
void
cflat_prs_printer::visit(const footprint_expr_node& e) {
	STACKTRACE_VERBOSE;
	const footprint& pfp(get_current_footprint().get_prs_footprint());
	const size_t sz = e.size();
	const char type = e.get_type();
	const char ptype = parent_expr_type;
	const expr_type_setter __tmp(*this, type);
	const PRS_footprint_expr_pool_type* const expr_pool = 
		&pfp.get_expr_pool();
	switch (type) {
		case PRS_LITERAL_TYPE_ENUM: {
			INVARIANT(sz == 1);
			__dump_canonical_literal<bool_tag>(e.only());
			const directive_base_params_type& par(e.params);
			if (cfopts.size_prs) {
				directive_base::dump_params(par, os);
			}
			// PRS literal attributes
			if (cfopts.literal_attributes) {
	// NOTE: this is inconsistent with __print_prefix_rule_attributes
			if (e.attributes.size()) {
				e.attributes.dump(os << '[') << ']';
			}
			}
			// computing conductances
			preal_value_type width = 0.0;
			preal_value_type length = 0.0;
			const size_t ps = par.size();
			size_t i = 0;
			// take first two non-negative values as W/L
			for ( ; i<ps; ++i) {
				const preal_value_type tmp =
					par[i]->to_real_const();
				if (tmp > 0.0) {
				if (width <= 0.0) {
					width = tmp;
				} else if (length <= 0.0) {
					length = tmp;
				}
				}
			}
			// hard-coded default values
			if (width <= 0.0) width = 5.0;
			if (length <= 0.0) length = 2.0;
			max_conductance = min_conductance = width/length;
			one_conductance = max_conductance;
			break;
		}
		case PRS_NOT_EXPR_TYPE_ENUM:
			INVARIANT(sz == 1);
			os << '~';
			(*expr_pool)[e.only()].accept(*this);
			// conductances ignore negations;
			// just forward return value to caller.
			break;
		case PRS_AND_EXPR_TYPE_ENUM:
			// yes, fall-through
		case PRS_OR_EXPR_TYPE_ENUM: {
			const bool paren = ptype && (type != ptype);
			if (paren) os << '(';
			if (sz) {
				vector<preal_value_type> max_G, one_G, min_G;
				max_G.reserve(sz);
				one_G.reserve(sz);
				min_G.reserve(sz);
				(*expr_pool)[e.only()].accept(*this);
				max_G.push_back(max_conductance);
				one_G.push_back(one_conductance);
				min_G.push_back(min_conductance);
				const char* const op =
					(type == PRS_AND_EXPR_TYPE_ENUM) ?
						" &" : " |";
				// optionally print precharge here
				const footprint_expr_node::precharge_map_type&
					pm(e.get_precharges());
				typedef footprint_expr_node::precharge_map_type::const_iterator
						const_iterator;
				const_iterator pi(pm.begin()), pe(pm.end());
				// recall: precharges (map) are 0-indexed
				size_t i = 2;
				for ( ; i<=sz; ++i) {
					os << op;
					if (cfopts.show_precharges) {
					if ((pi != pe) && (i-2 == pi->first)) {
						os << '{' << 
							(pi->second.second ?
							'+' : '-');
						(*expr_pool)[pi->second.first].accept(*this);
						os << '}';
						++pi;
					}
					}
					os << ' ';
					(*expr_pool)[e[i]].accept(*this);
					max_G.push_back(max_conductance);
					one_G.push_back(one_conductance);
					min_G.push_back(min_conductance);
				}
				// compute new max/min conductances and return
				if (type == PRS_AND_EXPR_TYPE_ENUM) {
					// convert conductances to resistances
					// overwrite in-place to aliases
					vector<preal_value_type>& max_R(min_G);
					vector<preal_value_type>& one_R(one_G);
					vector<preal_value_type>& min_R(max_G);
					reciprocate<preal_value_type> recip;
					transform(min_G.begin(), min_G.end(), 
						max_R.begin(), recip);
					transform(one_G.begin(), one_G.end(), 
						one_R.begin(), recip);
					transform(max_G.begin(), max_G.end(), 
						min_R.begin(), recip);
					max_conductance = 1.0 /
						accumulate(min_R.begin(),
							min_R.end(), 0.0);
					one_conductance = 1.0 /
						accumulate(one_R.begin(),
							one_R.end(), 0.0);
					min_conductance = 1.0 /
						accumulate(max_R.begin(),
							max_R.end(), 0.0);
				} else {	// type is OR
					// min is smallest
					// max is sum
					min_conductance = *std::min_element(
						min_G.begin(), min_G.end());
					one_conductance = *std::max_element(
						one_G.begin(), one_G.end());
					max_conductance = accumulate(
						max_G.begin(), max_G.end(),
						0.0);
				}
			}
			else if (cfopts.compute_conductances) {
				cerr << "FATAL: Cannot compute conductance on "
					"an empty expression!" << endl;
				THROW_EXIT;
			}
			// null/empty expressions may arise from loop expansion
			if (paren) os << ')';
			break;
		}
		case PRS_NODE_TYPE_ENUM: {
			// we've already matched the direction of the rule
			// so we should just be able to print the expression.
			INVARIANT(sz == 1);
			const footprint_expr_node& en((*expr_pool)[e.only()]);
			// if there are any attributes associated with the
			// internal node, then parenthesize and post-fix
			// the list of attributes.
#if PRS_INTERNAL_NODE_ATTRIBUTES
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE
			const footprint::internal_node_pool_type&
				inp(pfp.get_internal_node_pool());
			const footprint::internal_node_pool_type::const_iterator
				f(pfp.find_internal_node(e.only()));
			INVARIANT(f != inp.end());
			const resolved_attribute_list_type& att(f->attributes);
#elif PRS_INTERNAL_NODE_ATTRIBUTES_AT_EXPR
			const resolved_attribute_list_type& att(en.attributes);
#endif
			const bool wrap = cfopts.literal_attributes &&
				att.size();
			const bool leaf =
				en.get_type() == PRS_LITERAL_TYPE_ENUM;
			// leaf literals will want to be parenthesized
			if (wrap && leaf) os << '(';
#endif
			en.accept(*this);
#if PRS_INTERNAL_NODE_ATTRIBUTES
	// NOTE: this is inconsistent with __print_prefix_rule_attributes
			if (wrap) {
				if (leaf) os << ')';
				att.dump(os << '[') << ']';
			}
#endif
			// other normal expression types do not bear attributes
			break;
		}
		default:
			ICE(cerr,
			cerr << "Invalid PRS expr type enumeration: "
				<< type << endl;
			)
	}	// end switch
}	// end method cflat_prs_printer::visit(const footprint_expr_node&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Here's the magic!
	This is where it looks up and calls a custom-defined 
	macro function.  
	Consider folding the checking functionality into main
		instead of calling separately.  (Especially, if they are
		all happening at the same call site, seems rather redundant.)
 */
void
cflat_prs_printer::visit(const footprint_macro& m) {
	STACKTRACE_VERBOSE;
	const cflat_macro_registry_type::const_iterator
		f(cflat_macro_registry.find(m.name));
	INVARIANT(f != cflat_macro_registry.end());
	const cflat_macro_definition_entry& d(f->second);
	INVARIANT(d);		// was already checked during unroll
	if (!d.check_param_args(m.params).good
			|| !d.check_node_args(m.nodes).good) {
		cerr << "Error with PRS macro." << endl;
		// m.dump(cerr) << endl;	// unimplemented
		THROW_EXIT;
	} else {
		d.main(*this, m.params, m.nodes);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat_prs_printer::visit(const SPEC::footprint_directive& d) {
	STACKTRACE_VERBOSE;
	const SPEC::cflat_spec_registry_type::const_iterator
		f(SPEC::cflat_spec_registry.find(d.name));
	INVARIANT(f != SPEC::cflat_spec_registry.end());
	const SPEC::cflat_spec_definition_entry& s(f->second);
	if (!s.check_param_args(d.params).good
			|| !s.check_node_args(d.nodes).good) {
		cerr << "Error with spec directive." << endl;
		// s.dump(cerr) << endl;	// unimplemented
		THROW_EXIT;
	} else {
		s.main(*this, d.params, d.nodes);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print attributes for nodes with non-default attribute values.  
 */
void
cflat_prs_printer::visit(const state_instance<bool_tag>& i) {
if (cfopts.node_attributes) {
	const instance_alias_info<bool_tag>& a(*i.get_back_ref());
if (a.has_nondefault_attributes()) {
	std::ostringstream oss;
	__dump_canonical_literal<bool_tag>(oss, a.instance_index);
	const string& n(oss.str());
	if (cfopts.split_instance_attributes) {
		a.dump_split_attributes(os, n);
	} else {
		os << "@ " << n;
		a.dump_flat_attributes(os) << endl;
	}
}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat_prs_printer::visit(const state_instance<process_tag>& gp) {
	STACKTRACE_VERBOSE;
if (cfopts.show_channel_terminals) {
	// get this instance name and type name
	const footprint_frame& gpff(gp._frame);
	const entity::footprint& f(*gpff._footprint);
	if (f.get_meta_type() != META_TYPE_PROCESS) {
		const instance_alias_info<process_tag>& a(*gp.get_back_ref());
		if (a.is_terminal_producer()) {
			os << "$channel: ";
			__dump_canonical_literal<process_tag>(os,
				a.instance_index);
			os << '!' << endl;
		}
		if (a.is_terminal_consumer()) {
			os << "$channel: ";
			__dump_canonical_literal<process_tag>(os,
				a.instance_index);
			os << '?' << endl;
		}
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat_prs_printer::visit(const RTE::footprint&) {
	FINISH_ME(Fang);
}

void
cflat_prs_printer::visit(const RTE::footprint_assignment&) {
	FINISH_ME(Fang);
}

void
cflat_prs_printer::visit(const RTE::footprint_expr_node&) {
	FINISH_ME(Fang);
}

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

