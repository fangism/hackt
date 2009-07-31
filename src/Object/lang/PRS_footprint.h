/**
	\file "Object/lang/PRS_footprint.h"
	$Id: PRS_footprint.h,v 1.14.2.1 2009/07/31 00:22:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_H__

#include <iosfwd>
#include <vector>
#include <map>
#include <set>		// for collecting unique node indices
#include <string>
#include "Object/inst/instance_pool_fwd.h"
#include "Object/lang/PRS_footprint_expr.h"
#include "Object/lang/PRS_footprint_rule.h"
#include "Object/lang/PRS_footprint_macro.h"
#include "Object/lang/PRS_footprint_expr_pool_fwd.h"
#include "util/macros.h"
#include "util/boolean_types.h"
#include "util/offset_array.h"
#include "util/persistent_fwd.h"

/**
	Define to 1 to include additional structures and members for
	maintaining subcircuit hierarchy.  
	For now subircuits are 1-level, and thus, cannot be nested. 
	Goal: 1
 */
#define	PRS_FOOTPRINT_SUBCKT			1

namespace HAC {
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
using util::persistent_object_manager;
using util::good_bool;
using std::map;
using std::string;

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
class footprint : public cflat_visitee {
	friend class cflat_visitor;
public:
	typedef	footprint_expr_node		expr_node;
	typedef	size_t				invariant_type;
	typedef	footprint_rule			rule;
	typedef	footprint_macro			macro;
	/**
		Expression pull direction for internal node.
		pull-up is true, pull-down if false.
	 */
	typedef	std::pair<size_t, bool>		node_expr_type;
	/**
		This map keeps track of internal nodes defined in 
		terms of one-sided guard expressions.  
		String should be of the form: x[...]+.
		Each pull may only be defined once.  
		value_type is index into expression pool.  
		TODO: Is there a way to store refrence object instead
		of their string representations?  (yes, but not critical now)
	 */
	typedef	map<string, node_expr_type>	internal_node_expr_map_type;
	/// list of root expression indices
	typedef	vector<invariant_type>		invariant_pool_type;
#if PRS_FOOTPRINT_SUBCKT
	/**
		This structure keeps a map of which rules/macros (by index)
		belong to which subcircuit.  
		Each pair represents the set [i,j)
		Indices not found in this set are presumed to be in
		the outer-most level, designated index 0.  
	 */
	typedef	std::pair<size_t, size_t>	index_range;
	/**
		Each subcircuit may contain rules and macros, 
		so we need to keep these sets coherent.
	 */
	struct subcircuit_map_entry {
		index_range			rules;
		index_range			macros;
	};	// end struct subcircuit_map_entry
	/**
		This will resemble a discrete_interval_set except that
		there can be blank entries, which are skipped.  
		Algorithm: since this is maintained sorted, 
			can use binary search algorithms, such as
			lower_bound, upper_bound.  
	 */
	typedef	vector<subcircuit_map_entry>	subcircuit_map_type;
#endif
private:
	typedef	state_instance<bool_tag>	bool_instance_type;
	typedef	instance_pool<bool_instance_type>
						node_pool_type;
	typedef	vector<rule>			rule_pool_type;
	typedef	PRS_footprint_expr_pool_type	expr_pool_type;
	typedef	vector<macro>			macro_pool_type;

	rule_pool_type				rule_pool;
	expr_pool_type				expr_pool;
	macro_pool_type				macro_pool;
	internal_node_expr_map_type		internal_node_expr_map;
	invariant_pool_type			invariant_pool;
#if PRS_FOOTPRINT_SUBCKT
	subcircuit_map_type			subcircuit_map;
#endif
public:
	footprint();
	~footprint();

	ostream&
	dump(ostream&, const entity::footprint&) const;

	const expr_pool_type&
	get_expr_pool(void) const { return expr_pool; }

	expr_pool_type&
	get_expr_pool(void) { return expr_pool; }

	const rule_pool_type&
	get_rule_pool(void) const { return rule_pool; }

	const macro_pool_type&
	get_macro_pool(void) const { return macro_pool; }

	const invariant_pool_type&
	get_invariant_pool(void) const { return invariant_pool; }

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
	ostream&
	dump_macro(const macro&, ostream&, const node_pool_type&);

public:
	// a method for registering internal nodes and expressions
	good_bool
	register_internal_node_expr(const string&, const size_t, const bool);

	size_t
	lookup_internal_node_expr(const string&, const bool) const;

	// returns reference to new expression node
	expr_node&
	push_back_expr(const char, const size_t);

	rule&
	push_back_rule(const int, const int, const bool);

	macro&
	push_back_macro(const string&);

	void
	push_back_invariant(const invariant_type t) {
		invariant_pool.push_back(t);
	}

#if PRS_FOOTPRINT_SUBCKT
	void
	push_back_subcircuit(const subcircuit_map_entry& t) {
		subcircuit_map.push_back(t);
	}
#endif

	size_t
	current_expr_index(void) const {
		return expr_pool.size();
	}

	void
	collect_literal_indices(std::set<size_t>&, // node_index_type
		const size_t) const;	// should be expr_index_type

public:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	void
	accept(cflat_visitor&) const;

};	// end class footprint

//=============================================================================
}	// end namepace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_FOOTPRINT_H__

