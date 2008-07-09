/**
	\file "Object/lang/PRS_footprint.h"
	$Id: PRS_footprint_expr.h,v 1.4.116.1 2008/07/09 04:34:42 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_EXPR_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_EXPR_H__

#include <iosfwd>
#include <valarray>
#include <vector>
#include "util/macros.h"
#include "Object/lang/SPEC_fwd.h"
#include "Object/lang/cflat_visitee.h"
#include "util/memory/count_ptr.h"

#include "sim/prsim/devel_switches.h"
#if PRSIM_UNIFY_GRAPH_STRUCTURES
#include "sim/common.h"
#endif

namespace HAC {
namespace entity {
namespace PRS {
using std::istream;
using std::ostream;
using util::persistent_object_manager;
#if PRSIM_UNIFY_GRAPH_STRUCTURES
using SIM::expr_index_type;
using SIM::expr_count_type;
#endif

//=============================================================================
#if PRSIM_UNIFY_GRAPH_STRUCTURES
/**
	Is there a need for footprint_literal_node for leaf expressions?
	Only this type would need to bear parameters.  
 */
class footprint_literal_node : public cflat_visitee {
	node_index_type			node;
	/**
		This field is only applicable to PRS_LITERALs.
		Technically, non-leaf expression nodes never 
		have literal parameters, but we can't turn this 
		into a union either because vectors have
		non-trivial dtors.  
		We need to pay the extra storage cost here.  :(
	 */
	params_type			params;

};	// end class footprint_literal_node
#endif	// PRSIM_UNIFY_GRAPH_STRUCTURES

//=============================================================================
/**
	A top-down only structure for maintaining unrolled PRS
	in the process footprint.  
	Implementation is defined in "Object/lang/PRS_footprint.cc".
	TODO: Make this more like SIM::PRSIM::ExprGraphNode.
	Q: should this netlist be "optimized" as it is constructed?
		See -O1 options in hacprsim.  
 */
class footprint_expr_node : public cflat_visitee {
	/**
		Why int and not size_t?
		Might consider making this a int-bool pair, 
		to indicate whether or child is literal (with params)
		or is subexpression.  
	 */
#if PRSIM_UNIFY_GRAPH_STRUCTURES
	typedef	pair<bool, int>			node_value_type;
#else
	typedef	int				node_value_type;
#endif
	typedef	std::valarray<node_value_type>	node_array_type;
	typedef	directive_base_params_type	params_type;
private:
	/**
		Whether or not this is AND or OR, NOT, literal....  
		This uses the enumerations according to
		PRS::{literal,not_expr,and_expr,or_expr}::print_stamp
		in "Object/lang/PRS_enum.h".
		TODO: unify one set of enums from "sim/prsim/Expr.h"
	 */
	char				type;
#if PRSIM_UNIFY_GRAPH_STRUCTURES
	/**
		Temporary: use the redundant bit-field enums
		from "sim/prsim/Expr.h"
	 */
	uchar				type_alt;
	/**
		The offset position in the parent expression's 
		list of subexpression children.  
		Only needed for top-down views of expressions.  
		This has the role of SIM::PRSIM::ExprGraphNode::offset.
	 */
	expr_count_type			offset;
	/**
		Pointer to local parent expression.  
		Only needed for bottom-up views of expressions, 
		such as in simulation propagation.  
		Assumes role of SIM::PRSIM::Expr::parent.
		This information is redundant and can be reconstructed
		from top-down through nodes' children.  
	 */
	expr_index_type			parent;
#endif
	/**
		If it is a literal, then nodes[0] is the bool index.
		Otherwise, index is used for other expr_nodes.  
		NOTE: valarray just contains size_t and pointer.  
		Values are 1-indexed.  
		This now has the role of SIM::PRSIM::ExprGraphNode::children.
	 */
	node_array_type			nodes;
#if !PRSIM_UNIFY_GRAPH_STRUCTURES
	/**
		This field is only applicable to PRS_LITERALs.
		Technically, non-leaf expression nodes never 
		have literal parameters, but we can't turn this 
		into a union either because vectors have
		non-trivial dtors.  
		We need to pay the extra storage cost here.  :(
	 */
	params_type			params;
#endif

public:
	footprint_expr_node();

	explicit
	footprint_expr_node(const char);

	footprint_expr_node(const char t, const size_t s);

	char
	get_type(void) const { return type; }

	void
	set_type(const char t) { type = t; }

	params_type&
	get_params(void) { return params; }

	const params_type&
	get_params(void) const { return params; }

	size_t
	size(void) const { return nodes.size(); }

	/**
		Subtract one because indicies are intentionally 
		off by one.  
	 */
	node_value_type&
	operator [] (const size_t i) {
		INVARIANT(i-1 < nodes.size());
		return nodes[i-1];
	}

	/**
		Subtract one because indicies are intentionally 
		off by one.  
	 */
	const node_value_type&
	operator [] (const size_t i) const {
		INVARIANT(i-1 < nodes.size());
		return nodes[i-1];
	}

	const node_value_type&
	only(void) const {
		return nodes[0];
	}

	void
	resize(const size_t s) { nodes.resize(s); }

	/// returns the 1-indexed position of first error, else 0
	size_t
	first_node_error(void) const;

	size_t
	first_param_error(void) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	void
	accept(cflat_visitor&) const;
};	// end struct foorprint_expr_node

}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_FOOTPRINT_EXPR_H__
