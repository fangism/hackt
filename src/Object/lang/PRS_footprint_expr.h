/**
	\file "Object/lang/PRS_footprint.h"
	$Id: PRS_footprint_expr.h,v 1.2 2006/01/22 06:53:03 fang Exp $
 */

#ifndef	__OBJECT_LANG_PRS_FOOTPRINT_EXPR_H__
#define	__OBJECT_LANG_PRS_FOOTPRINT_EXPR_H__

#include <iosfwd>
#include <valarray>
#include "util/macros.h"
#include "Object/lang/cflat_visitee.h"

namespace HAC {
namespace entity {
namespace PRS {
using std::ostream;
using std::istream;
// using util::persistent_object_manager;

//=============================================================================
/**
	A top-down only structure for maintaining unrolled PRS
	in the process footprint.  
	Implementation is defined in "Object/lang/PRS_footprint.cc".
 */
class footprint_expr_node : public cflat_visitee {
	typedef	std::valarray<int>	node_array_type;
private:
	/**
		Whether or not this is AND or OR, NOT, literal....  
		This uses the enumerations according to
		PRS::{literal,not_expr,and_expr,or_expr}::print_stamp
		in "Object/lang/PRS.h".
	 */
	char				type;
	/**
		If it is a literal, then nodes[0] is the bool index.
		Otherwise, index is used for other expr_nodes.  
		NOTE: valarray just contains size_t and pointer.  
		Values are 1-indexed.  
	 */
	node_array_type			nodes;

public:
	footprint_expr_node() { }

	explicit
	footprint_expr_node(const char t) : type(t), nodes() { }

	footprint_expr_node(const char t, const size_t s) :
		type(t), nodes(s) { }

	char
	get_type(void) const { return type; }

	void
	set_type(const char t) { type = t; }

	size_t
	size(void) const { return nodes.size(); }

	/**
		Subtract one because indicies are intentionally 
		off by one.  
	 */
	int&
	operator [] (const size_t i) {
		INVARIANT(i-1 < nodes.size());
		return nodes[i-1];
	}

	/**
		Subtract one because indicies are intentionally 
		off by one.  
	 */
	const int&
	operator [] (const size_t i) const {
		INVARIANT(i-1 < nodes.size());
		return nodes[i-1];
	}

	const int&
	only(void) const {
		return nodes[0];
	}

	void
	resize(const size_t s) { nodes.resize(s); }

	/// returns the 1-indexed position of first error, else 0
	size_t
	first_error(void) const;

	void
	write_object_base(ostream&) const;

	void
	load_object_base(istream&);

	void
	accept(cflat_visitor&) const;
};	// end struct foorprint_expr_node

}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_LANG_PRS_FOOTPRINT_EXPR_H__
