/**
	\file "Object/lang/cflat_printer.cc"
	$Id: cflat_printer.cc,v 1.6.14.1 2006/04/09 04:34:02 fang Exp $
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
#include "util/member_saver.h"

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.h"
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
	Translates local node reference to its canonical name.
 */
void
cflat_prs_printer::__dump_canonical_literal(const size_t lni) const {
	if (cfopts.enquote_names) { os << '\"'; }
	sm->get_pool<bool_tag>()[__lookup_global_bool_id(lni)]
		.dump_canonical_name(os, *fp, *sm);
	if (cfopts.enquote_names) { os << '\"'; }
}

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

