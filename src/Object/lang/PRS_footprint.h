/**
	\file "Object/lang/PRS_footprint.h"
	$Id: PRS_footprint.h,v 1.1.2.2 2005/10/05 23:10:20 fang Exp $
 */

#ifndef	__OBJECT_LANG_PRS_FOOTPRINT_H__
#define	__OBJECT_LANG_PRS_FOOTPRINT_H__

#include <iosfwd>
#include <valarray>
#include <vector>
#include "Object/inst/instance_pool_fwd.h"
#include "util/macros.h"
#include "util/list_vector.h"

namespace ART {
struct cflat_options;

namespace entity {
class footprint;
struct bool_tag;
class state_manager;

template <class Tag>
class state_instance;

template <class Tag>
class footprint_frame_map;

namespace PRS {
using std::ostream;
using std::istream;
// using util::persistent_object_manager;

//=============================================================================
/**
	A production rule set footprint contains a resolved representation
	of its production rules in terms of local and formal 
	boolean instance references.  
	The production rules that belong to a process definition
	are just compact representations that need to be unrolled.  
	Consider: break up into template-parameter dependent or independent?

	As this is constructed, each 'node' could conceivably keep track
	of its "local" fanin and fanout, to avoid recomputing it.  
 */
class footprint {
public:
	/**
		A top-down only structure for maintaining unrolled PRS
		in the process footprint.  
	 */
	struct expr_node {
		typedef	std::valarray<int>	node_array_type;
		/**
			Whether or not this is AND or OR, NOT, literal....  
			This uses the enumerations according to
			PRS::{literal,not_expr,and_expr,or_expr}::print_stamp
			in "Object/lang/PRS.h".
		 */
		char				type;
	private:
		/**
			If it is a literal, then nodes[0] is the bool index.
			Otherwise, index is used for other expr_nodes.  
			NOTE: valarray just contains size_t and pointer.  
			Values are 1-indexed.  
		 */
		node_array_type			nodes;

	public:
		expr_node() { }

		explicit
		expr_node(const char t) : type(t), nodes() { }

		expr_node(const char t, const size_t s) : type(t), nodes(s) { }

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

		void
		resize(const size_t s) { nodes.resize(s); }

		/// returns the 1-indexed position of first error, else 0
		size_t
		first_error(void) const;

		void
		write_object_base(ostream&) const;

		void
		load_object_base(istream&);
	};

	/**
		Compact and resolved representation of production rule.  
	 */
	struct rule {
		/**
			index to root expression for this node.
			1-indexed.
		 */
		int				expr_index;
		/**
			index to output node (local to this definition).
			1-indexed.  
		 */
		int				output_index;
		/**
			Whether or not is pull-up or down.
			Could use the sign of output index...
		 */
		bool				dir;

		rule() { }

		rule(const int e, const int o, const bool d) :
			expr_index(e), output_index(o), dir(d) { }

		void
		write_object_base(ostream&) const;

		void
		load_object_base(istream&);
	};

	// typedef	std::vector<>

private:
	typedef	state_instance<bool_tag>	bool_instance_type;
	typedef	instance_pool<bool_instance_type>
						node_pool_type;
	typedef	util::list_vector<rule>		rule_pool_type;
	typedef	util::list_vector<expr_node>	expr_pool_type;

	rule_pool_type				rule_pool;
	expr_pool_type				expr_pool;

public:
	footprint();
	~footprint();

	ostream&
	dump(ostream&, const entity::footprint&) const;

private:
	static
	ostream&
	dump_expr(const expr_node&, ostream&, const node_pool_type&,
		const expr_pool_type&, const char);

	static
	ostream&
	dump_rule(const rule&, ostream&, const node_pool_type&, 
		const expr_pool_type&);

	static
	void
	cflat_expr(const expr_node&, ostream&,
		const footprint_frame_map<bool_tag>&, 
		const entity::footprint&, const cflat_options&,
		const state_manager&, const expr_pool_type&, const char);

	static
	void
	cflat_rule(const rule&, ostream&, const footprint_frame_map<bool_tag>&, 
		const entity::footprint&, const cflat_options&, 
		const state_manager&, const expr_pool_type&);

public:
	// returns reference to new expression node
	expr_node&
	push_back_expr(const char, const size_t);

	rule&
	push_back_rule(const int, const int, const bool);

	size_t
	current_expr_index(void) const {
		return expr_pool.size();
	}

	void
	cflat_prs(ostream&, const footprint_frame_map<bool_tag>&, 
		const entity::footprint&, const cflat_options&, 
		const state_manager&) const;

public:
	// requires no persistent object manager
	void
	write_object_base(ostream&) const;

	// requires no persistent object manager
	void
	load_object_base(istream&);

};	// end class footprint

//=============================================================================
}	// end namepace PRS
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_LANG_PRS_FOOTPRINT_H__

