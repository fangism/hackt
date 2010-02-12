/**
	\file "Object/lang/cflat_printer.cc"
	Implementation of cflattening visitor.
	$Id: cflat_printer.cc,v 1.24.2.6 2010/02/12 18:20:36 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <algorithm>
#include <set>
#include <sstream>
#include "Object/lang/cflat_printer.h"
#include "Object/lang/PRS_enum.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/PRS_attribute_registry.h"
#include "Object/lang/PRS_macro_registry.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/lang/SPEC_registry.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/connection_policy.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/def/footprint.h"
#else
#include "Object/state_manager.h"
#endif
#include "Object/traits/bool_traits.h"
#include "main/cflat_options.h"
#include "common/ICE.h"
#include "common/TODO.h"
#include "util/stacktrace.h"
#include "util/offset_array.h"
#include "util/member_saver.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/preal_const.h"
#include "util/numeric/functional.h"	// for reciprocate

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.h"
using std::accumulate;
using std::transform;
using util::numeric::reciprocate;

//=============================================================================
// class cflat_prs_printer method definitions

cflat_prs_printer::~cflat_prs_printer() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
void
cflat_prs_printer::visit(const entity::footprint& f) {
	STACKTRACE_VERBOSE;
	// visit rules and spec directives, locally
	f.get_prs_footprint().accept(*this);
	f.get_spec_footprint().accept(*this);
	// f.get_chp_footprint().accept(*this);
	parent_type::visit(f);	// visit_recursive
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat_prs_printer::visit(const PRS::footprint& p) {
	STACKTRACE_VERBOSE;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	cflat_visitor::visit(p);
#else
	parent_type::visit(p);
#endif
	// now handle invariant expressions
// if (cfopts.primary_tool == cflat_options::TOOL_LVS) {
// ah, hell, just print it for everything, it's easy to grep out
	typedef	PRS::footprint::invariant_pool_type::const_iterator
						const_iterator;
	const expr_type_setter tmp(*this, PRS_LITERAL_TYPE_ENUM);
	const PRS::footprint::invariant_pool_type& ip(p.get_invariant_pool());
	const PRS_footprint_expr_pool_type& ep(p.get_expr_pool());
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	const expr_pool_setter __p(*this, ep);
	NEVER_NULL(expr_pool);
#endif
	const_iterator i(ip.begin()), e(ip.end());
	for ( ; i!=e; ++i) {
		os << "invariant ";
		// ep[*i].accept(*this);
		visit(ep[*i]);
		os << endl;
	}
// }
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
		typedef	footprint_rule::attributes_list_type
						attributes_list_type;
		typedef	attributes_list_type::const_iterator
						const_iterator;
		const_iterator i(r.attributes.begin());
		const const_iterator e(r.attributes.end());
		resolved_attribute::values_type empty;
		for ( ; i!=e; ++i) {
			// already checked registered
			cflat_rule_attribute_registry.find(i->key)->second
				.main(*this, (i->values ? *i->values : empty));
			// fake an empty list if necessary
		}
	}
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const PRS_footprint_expr_pool_type* const expr_pool = 
		&get_current_footprint().get_prs_footprint().get_expr_pool();
#endif
	(*expr_pool)[r.expr_index].accept(*this);
	os << " -> ";
	// r.output_index gives the local unique ID,
	// which needs to be translated to global ID.
	// bfm[...] refers to a GLOBAL_ENTRY<bool_tag> (1-indexed)
	// const size_t j = bfm[r.output_index-1];
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	__dump_canonical_literal(r.output_index);
#else
	const size_t global_bool_index =
		parent_type::__lookup_global_bool_id(r.output_index);
	print_node_name(os, sm->get_pool<bool_tag>()[global_bool_index]);
#endif
	os << (r.dir ? '+' : '-');
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
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	Automatically adds quotes if the options desire it.  
	redundant with __dump_resolved_canonical_literal, deprecated.
 */
ostream&
cflat_prs_printer::print_node_name(ostream& o, 
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		const size_t bi
#else
		const GLOBAL_ENTRY<bool_tag>& b
#endif
		) const {
	if (cfopts.enquote_names) o << '\"';
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	topfp->dump_canonical_name<bool_tag>(o, bi);
#else
	b.dump_canonical_name(o, *topfp, *sm);
#endif
	if (cfopts.enquote_names) o << '\"';
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ni the global node ID.  
 */
void
cflat_prs_printer::__dump_resolved_canonical_literal(const size_t ni) const {
	if (cfopts.enquote_names) { os << '\"'; }
	parent_type::__dump_resolved_canonical_literal(os, ni);
	if (cfopts.enquote_names) { os << '\"'; }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates local node reference to its canonical name.
	\param lni is the local node id, which needs to be resolved
		into the globally allocated id.  
 */
void
cflat_prs_printer::__dump_canonical_literal(const size_t lni) const {
	__dump_resolved_canonical_literal(
		parent_type::__lookup_global_bool_id(lni));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print using default wrapper and delimiter.  
 */
void
cflat_prs_printer::__dump_canonical_literal_group(
		const directive_node_group_type& g) const {
	__dump_canonical_literal_group(g, "{", ",", "}");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat_prs_printer::__dump_resolved_literal_group(
		const directive_node_group_type& g) const {
	__dump_resolved_literal_group(g, "{", ",", "}");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The node list argument contains already unresolved local IDs.  
	\param g group argument
	\param l left wrapper, optional
	\param r right group wrapper, optional
	\param d group element delimiter
 */
void
cflat_prs_printer::__dump_canonical_literal_group(
		const directive_node_group_type& g, 
		const char* l, const char* d, const char* r) const {
	NEVER_NULL(d);
	// collect resolved (unique) node IDs here:
	directive_node_group_type s;
	__resolve_unique_literal_group(g, s);
	__dump_resolved_literal_group(s, l, d, r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints a group of nodes.  
	Nodes in argument list are already resolved to global IDs.  
	By default, single nodes are not wrapped in braces, 
	and commas are used as delimiters.  
	Duplicate nodes are also eliminated.  
	\param g group argument
	\param l left wrapper, optional
	\param r right group wrapper, optional
	\param d group element delimiter
 */
void
cflat_prs_printer::__dump_resolved_literal_group(
		const directive_node_group_type& s, 
		const char* l, const char* d, const char* r) const {
	typedef	directive_node_group_type::const_iterator	const_iterator;
	if (s.size() > 1) {
		const_iterator i(s.begin()), e(s.end());
		if (l)	os << l;
		__dump_resolved_canonical_literal(*i);
		for (++i; i!=e; ++i) {
			os << d;
			__dump_resolved_canonical_literal(*i);
		}
		if (r)	os << r;
	} else {
		// only one element
		INVARIANT(!s.empty());
		__dump_resolved_canonical_literal(*s.begin());
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
	const size_t sz = e.size();
	const char type = e.get_type();
	const char ptype = parent_expr_type;
	const expr_type_setter __tmp(*this, type);
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const PRS_footprint_expr_pool_type* const expr_pool = 
		&get_current_footprint().get_prs_footprint().get_expr_pool();
#endif
	switch (type) {
		case PRS_LITERAL_TYPE_ENUM: {
			INVARIANT(sz == 1);
			__dump_canonical_literal(e.only());
			const directive_base_params_type& par(e.params);
			if (cfopts.size_prs) {
				directive_base::dump_params(par, os);
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
		case PRS_NODE_TYPE_ENUM:
			// we've already matched the direction of the rule
			// so we should just be able to print the expression.
			INVARIANT(sz == 1);
			(*expr_pool)[e.only()].accept(*this);
			break;
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
cflat_prs_printer::visit(const GLOBAL_ENTRY<bool_tag>& b) {
if (cfopts.node_attributes) {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const state_instance<bool_tag>& i(b);
#else
	const state_instance<bool_tag>& i(b.get_canonical_instance(*this));
#endif
	const instance_alias_info<bool_tag>& a(*i.get_back_ref());
if (a.has_nondefault_attributes()) {
	std::ostringstream oss;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	__dump_canonical_literal(i.get_back_ref()->instance_index);
	// print_node_name(oss, i.get_back_ref()->instance_index);
#else
	print_node_name(oss, b);	// auto-quote
#endif
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

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

