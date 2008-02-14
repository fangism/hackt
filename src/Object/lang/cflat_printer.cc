/**
	\file "Object/lang/cflat_printer.cc"
	Implementation of cflattening visitor.
	$Id: cflat_printer.cc,v 1.15.8.2 2008/02/14 18:23:00 fang Exp $
 */

#include <iostream>
#include <algorithm>
#include <set>
#include "Object/lang/cflat_printer.h"
#include "Object/lang/PRS_enum.h"
#include "Object/lang/PRS_footprint_expr.h"
#include "Object/lang/PRS_footprint_rule.h"
#include "Object/lang/PRS_footprint_macro.h"
#include "Object/lang/PRS_attribute_registry.h"
#include "Object/lang/PRS_macro_registry.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/lang/SPEC_registry.h"
#include "Object/global_entry.h"
#include "Object/state_manager.h"
#include "Object/traits/bool_traits.h"
#include "main/cflat_options.h"
#include "common/ICE.h"
#include "common/TODO.h"
#include "util/offset_array.h"
#include "util/member_saver.h"
#include "util/qmap.tcc"		// for const_assoc_query symbols??
#if CFLAT_WITH_CONDUCTANCES
#include "Object/expr/pint_const.h"
#include "Object/expr/preal_const.h"
#include "util/numeric/functional.h"	// for reciprocate
#endif

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.h"
#if CFLAT_WITH_CONDUCTANCES
using std::accumulate;
using std::transform;
using util::numeric::reciprocate;
#endif

//=============================================================================
// class cflat_prs_printer method definitions

/**
	Prints out the entire rule.  
	Adapted from footprint::cflat_rule().  
	Q1: why is the expression outside of the conditional?
	Q2: why is the conditional checked here? (more efficient in parent)
	TODO: support attributes (or use a visitor?)
 */
void
cflat_prs_printer::visit(const footprint_rule& r) {
	const expr_type_setter tmp(*this, PRS_LITERAL_TYPE_ENUM);
if (!cfopts.check_prs) {
	if (r.attributes.size()) {
		typedef	footprint_rule::attributes_list_type::const_iterator
						const_iterator;
		const_iterator i(r.attributes.begin());
		const const_iterator e(r.attributes.end());
		for ( ; i!=e; ++i) {
			cflat_attribute_registry[i->key].main(*this, *i->values);
		}
	}
	(*expr_pool)[r.expr_index].accept(*this);
	os << " -> ";
	// r.output_index gives the local unique ID,
	// which needs to be translated to global ID.
	// bfm[...] refers to a global_entry<bool_tag> (1-indexed)
	// const size_t j = bfm[r.output_index-1];
	if (cfopts.enquote_names) os << '\"';
	const size_t global_bool_index =
		parent_type::__lookup_global_bool_id(r.output_index);
	sm->get_pool<bool_tag>()[global_bool_index]
		.dump_canonical_name(os, *topfp, *sm);
	if (cfopts.enquote_names) os << '\"';
	os << (r.dir ? '+' : '-');
#if CFLAT_WITH_CONDUCTANCES
	if (cfopts.compute_conductances) {
		// min/mxa_conductance was evaluated from expression
		os << "\t(G min,one,max = " << min_conductance << ", " <<
			one_conductance << ", " <<
			max_conductance << ")";
	}
#endif
	os << endl;
}
}

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
#if CFLAT_WITH_CONDUCTANCES
/// helper function for extracting sizing parameter
static
float
extract_float_param(const count_ptr<const const_param>& p) {
	const count_ptr<const pint_const> i(p.is_a<const pint_const>());
	if (i) {
		return float(i->static_constant_value());
	} else {
		const count_ptr<const preal_const>
			r(p.is_a<const preal_const>());
		if (r)
			return r->static_constant_value();
	}
	return 0.0;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out the expression. 
	Adapted from footprint::cflat_expr().  
 */
void
cflat_prs_printer::visit(const footprint_expr_node& e) {
	const size_t sz = e.size();
	const char type = e.get_type();
	const char ptype = parent_expr_type;
	const expr_type_setter __tmp(*this, type);
	switch (type) {
		case PRS_LITERAL_TYPE_ENUM: {
			INVARIANT(sz == 1);
			__dump_canonical_literal(e.only());
			const directive_base_params_type& par(e.get_params());
			if (cfopts.size_prs) {
				directive_base::dump_params(par, os);
			}
#if CFLAT_WITH_CONDUCTANCES
			float width = 0.0;
			float length = 0.0;
			const size_t ps = par.size();
			size_t i = 0;
			// take first two non-negative values as W/L
			for ( ; i<ps; ++i) {
				float tmp = extract_float_param(par[i]);
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
#endif
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
#if CFLAT_WITH_CONDUCTANCES
				vector<float> max_G, one_G, min_G;
				max_G.reserve(sz);
				one_G.reserve(sz);
				min_G.reserve(sz);
#endif
				(*expr_pool)[e.only()].accept(*this);
#if CFLAT_WITH_CONDUCTANCES
				max_G.push_back(max_conductance);
				one_G.push_back(one_conductance);
				min_G.push_back(min_conductance);
#endif
				const char* const op =
					(type == PRS_AND_EXPR_TYPE_ENUM) ?
						" & " : " | ";
				size_t i = 2;
				for ( ; i<=sz; i++) {
					os << op;
					(*expr_pool)[e[i]].accept(*this);
#if CFLAT_WITH_CONDUCTANCES
					max_G.push_back(max_conductance);
					one_G.push_back(one_conductance);
					min_G.push_back(min_conductance);
#endif
				}
#if CFLAT_WITH_CONDUCTANCES
				// compute new max/min conductances and return
				if (type == PRS_AND_EXPR_TYPE_ENUM) {
					// convert conductances to resistances
					// overwrite in-place to aliases
					vector<float>& max_R(min_G);
					vector<float>& one_R(one_G);
					vector<float>& min_R(max_G);
					transform(min_G.begin(), min_G.end(), 
						max_R.begin(),
						reciprocate<float>());
					transform(one_G.begin(), one_G.end(), 
						one_R.begin(),
						reciprocate<float>());
					transform(max_G.begin(), max_G.end(), 
						min_R.begin(),
						reciprocate<float>());
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
#endif
			}
#if CFLAT_WITH_CONDUCTANCES
			else if (cfopts.compute_conductances) {
				cerr << "FATAL: Cannot compute conductance on "
					"an empty expression!" << endl;
				THROW_EXIT;
			}
#endif
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
	const cflat_macro_definition_entry& d(cflat_macro_registry[m.name]);
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
	const SPEC::cflat_spec_definition_entry&
		s(SPEC::cflat_spec_registry[d.name]);
	INVARIANT(s);		// was already checked during unroll
	if (!s.check_param_args(d.params).good
			|| !s.check_node_args(d.nodes).good) {
		cerr << "Error with spec directive." << endl;
		// s.dump(cerr) << endl;	// unimplemented
		THROW_EXIT;
	} else {
		s.main(*this, d.params, d.nodes);
	}
}

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

