/**
	\file "Object/lang/cflat_printer.cc"
	$Id: cflat_printer.cc,v 1.6.16.2 2006/04/11 22:54:09 fang Exp $
 */

#include <iostream>
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
#if GROUPED_DIRECTIVE_ARGUMENTS
#include <set>
#endif

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.h"
//=============================================================================
// class cflat_prs_printer::expr_type_setter definition

class cflat_prs_printer::expr_type_setter {
private:
	cflat_prs_printer&		cpp;
	const char			ppt;
public:
	/// save away the previous value on stack
	expr_type_setter(cflat_prs_printer& _cpp, const char _pt) :
			cpp(_cpp), ppt(cpp.parent_expr_type) {
		cpp.parent_expr_type = _pt;
	}

	/// restore previous expr type from stack
	~expr_type_setter() {
		cpp.parent_expr_type = ppt;
	}

};	// end class cflat_prs_printer::expr_type_setter

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
			attribute_registry[i->key].main(*this, *i->values);
		}
	}
	(*expr_pool)[r.expr_index].accept(*this);
	os << " -> ";
	// r.output_index gives the local unique ID,
	// which needs to be translated to global ID.
	// bfm[...] refers to a global_entry<bool_tag> (1-indexed)
	// const size_t j = bfm[r.output_index-1];
	if (cfopts.enquote_names) os << '\"';
	sm->get_pool<bool_tag>()[
		fpf->get_frame_map<bool_tag>()[r.output_index-1]]
		.dump_canonical_name(os, *fp, *sm);
	if (cfopts.enquote_names) os << '\"';
	os << (r.dir ? '+' : '-') << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param lni is the *local* node index referenced by this
		literal reference in this process.
	\return The local node index is translated into a globally 
		allocated (bool) node index, using the 
		footprint_frame_map.  
 */
size_t
cflat_prs_printer::__lookup_global_bool_id(const size_t lni) const {
	INVARIANT(lni);
	return fpf->get_frame_map<bool_tag>()[lni-1];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ni the global node ID.  
 */
void
cflat_prs_printer::__dump_resolved_canonical_literal(const size_t ni) const {
	if (cfopts.enquote_names) { os << '\"'; }
	sm->get_pool<bool_tag>()[ni].dump_canonical_name(os, *fp, *sm);
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
#if 0
	if (cfopts.enquote_names) { os << '\"'; }
	sm->get_pool<bool_tag>()[__lookup_global_bool_id(lni)]
		.dump_canonical_name(os, *fp, *sm);
	if (cfopts.enquote_names) { os << '\"'; }
#else
	__dump_resolved_canonical_literal(__lookup_global_bool_id(lni));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if GROUPED_DIRECTIVE_ARGUMENTS
/**
	Translates set of local node IDs into unique set of 
	global IDs which may result in fewer nodes because duplicate
	aliases are dropped.  
 */
void
cflat_prs_printer::__resolve_unique_literal_group(
		const directive_node_group_type& s,
		directive_node_group_type& d) const {
	typedef	directive_node_group_type::const_iterator
					const_iterator;
	const_iterator i(s.begin()), e(s.end());
	for ( ; i!=e; ++i) {
		d.insert(__lookup_global_bool_id(*i));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints a group of nodes.  
	By default, single nodes are not wrapped in braces, 
	and commas are used as delimiters.  
	Duplicate nodes are also eliminated.  
	TODO: take wrapper and delimiters as arguments.  
 */
void
cflat_prs_printer::__dump_canonical_literal_group(
		const directive_node_group_type& g) const {
	typedef	directive_node_group_type::const_iterator
					const_iterator;
	// collect resolved (unique) node IDs here:
	directive_node_group_type s;
	__resolve_unique_literal_group(g, s);
	if (s.size() > 1) {
		const_iterator i(s.begin()), e(s.end());
		os << '{';
		__dump_resolved_canonical_literal(*i);
		for (++i; i!=e; ++i) {
			os << ',';
			__dump_resolved_canonical_literal(*i);
		}
		os << '}';
	} else {
		// only one element
		INVARIANT(!s.empty());
		__dump_resolved_canonical_literal(*s.begin());
	}
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
	const expr_type_setter tmp(*this, type);
	switch (type) {
		case PRS_LITERAL_TYPE_ENUM:
			INVARIANT(sz == 1);
			__dump_canonical_literal(e.only());
			if (cfopts.size_prs) {
				directive_base::dump_params(e.get_params(), os);
			}
			break;
		case PRS_NOT_EXPR_TYPE_ENUM:
			INVARIANT(sz == 1);
			os << '~';
			(*expr_pool)[e.only()].accept(*this);
			break;
		case PRS_AND_EXPR_TYPE_ENUM:
			// yes, fall-through
		case PRS_OR_EXPR_TYPE_ENUM: {
			const bool paren = ptype && (type != ptype);
			if (paren) os << '(';
			if (e.size()) {
				(*expr_pool)[e.only()].accept(*this);
				const char* const op =
					(type == PRS_AND_EXPR_TYPE_ENUM) ?
						" & " : " | ";
				size_t i = 2;
				for ( ; i<=sz; i++) {
					os << op;
					(*expr_pool)[e[i]].accept(*this);
				}
			}
			if (paren) os << ')';
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
	const macro_definition_entry& d(macro_registry[m.name]);
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
	const SPEC::spec_definition_entry& s(SPEC::spec_registry[d.name]);
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

