/**
	\file "Object/lang/PRS_footprint.h"
	$Id: PRS_footprint.h,v 1.22 2010/09/29 00:13:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_H__

#include <iosfwd>
#include <vector>
#include <map>
#include <set>		// for collecting unique node indices
#include <string>

/**
	Define to 1 to support internal node attributes.
	Goal: 1
	Status: beginning
	Rationale: for correct propagation of attributes when 
		expanding internal nodes.  
 */
#define	PRS_INTERNAL_NODE_ATTRIBUTES		1
/**
	The flag determines *where* internal node attributes are kept.
	If 0, store the attributes with the internal_node_pool nodes.
	If 1, store the attributes with the associated *expression*, 
		which already has attribute support.  
	Goal: 0 (_AT_NODE)
	0 (at node) is probably cleaner because it avoids the scenario of
	attaching internal expression attributes to a singleton
	literal that already has attributes specified.
 */
#define	PRS_INTERNAL_NODE_ATTRIBUTES_AT_EXPR	(0 && PRS_INTERNAL_NODE_ATTRIBUTES)
#define	PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE	(1 && PRS_INTERNAL_NODE_ATTRIBUTES)

#include "Object/inst/instance_pool_fwd.h"
#include "Object/lang/PRS_footprint_expr.h"
#include "Object/lang/PRS_footprint_rule.h"
#include "Object/lang/PRS_footprint_macro.h"
#if PRS_INTERNAL_NODE_ATTRIBUTES
#include "Object/lang/generic_attribute.h"
#endif
#include "Object/lang/PRS_footprint_expr_pool_fwd.h"
#include "Object/devel_switches.h"
#include "util/macros.h"
#include "util/boolean_types.h"
#include "util/offset_array.h"
#include "util/persistent_fwd.h"
#include "util/memory/excl_ptr.h"	// for never_ptr

/**
	Define to 1 to include additional structures and members for
	maintaining subcircuit hierarchy.  
	For now subircuits are 1-level, and thus, cannot be nested. 
	Goal: 1
	Status: done, tested, perm'd
 */
// #define	PRS_FOOTPRINT_SUBCKT			1

/**
	Define to 1 to use a pooled storage for internal nodes, 
	instead of map<string, internal_node> directly.  
	Goal: 1
	Rationale: allows for efficient lookup and reverse lookup
		by name/index, indices are cheaper to store than strings, 
		which is critical for subcircuit entries, and in turn, 
		helpful for netlist generation structures.  
	Status: basically tested, perm'd
 */
// #define	PRS_INTERNAL_NODE_POOL			1

/**
	Define to 1 to have invariant expression entries point back
	to their source.
	Rationale: source has assert-fail string message now.
	Goal: 1
	Status: done, tested
 */
#define	INVARIANT_BACK_REFS				1

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

namespace SPEC {
class invariant;
}

namespace PRS {
class subcircuit;
using std::ostream;
using std::istream;
using util::persistent_object_manager;
using util::good_bool;
using std::map;
using std::pair;
using std::string;
using util::memory::never_ptr;
using SPEC::invariant;

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
	typedef	size_t				node_index_type;
	typedef	size_t				expr_index_type;
	typedef	size_t				rule_index_type;
	typedef	rule_index_type			macro_index_type;
	typedef	footprint_expr_node		expr_node;
#if INVARIANT_BACK_REFS
	// now contains a weak-back-ref pointer
	typedef	never_ptr<const invariant>	invariant_source_ptr_type;
	typedef	pair<expr_index_type, invariant_source_ptr_type>
						invariant_base;

	struct invariant_type : public invariant_base {
		invariant_type() { }	// default ctor: uninitialized

		invariant_type(const expr_index_type e, 
			const invariant* _this) : invariant_base(e,
				never_ptr<const invariant>(_this)) { }
		void
		collect_transient_info_base(persistent_object_manager&) const {
		}

		void
		write_object_base(const persistent_object_manager&,
			ostream&) const;
		void
		write_object(const persistent_object_manager& m,
				ostream& o) const {
			write_object_base(m, o);
		}
		void
		load_object_base(const persistent_object_manager&, istream&);
		void
		load_object(const persistent_object_manager& m, istream& i) {
			load_object_base(m, i);
		}

	};	// end struct invariant_type
#else
	typedef	expr_index_type			invariant_type;
#endif
	typedef	footprint_rule			rule;
	typedef	footprint_macro			macro;
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE
	typedef	resolved_attribute_list_type	attributes_list_type;
#endif
	/**
		Expression pull direction for internal node.
		pull-up is true, pull-down if false.
		first: expression index
		second: direction
	 */
	struct node_expr_type : public pair<expr_index_type, bool> {
		typedef	pair<expr_index_type, bool>	parent_type;
		// redundant, but relying on copy-on-write memory efficiency
		string				name;
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE
		// TODO: could just use the attributes in footprint_expr_node
		// that which is indexed by the 'first' key.
		// but there are fewer of these, less memory overhead?
		attributes_list_type		attributes;
#endif

		node_expr_type() { }	// uninitialized
		node_expr_type(const expr_index_type i,
			const bool d, const string& n) :
			parent_type(i, d), name(n)
#if PRS_INTERNAL_NODE_ATTRIBUTES_AT_NODE
			, attributes()
#endif
			{ }

#if PRS_INTERNAL_NODE_ATTRIBUTES
			void
			collect_transient_info_base(
				persistent_object_manager&) const;
#endif

			void
			write_object_base(const persistent_object_manager&, 
				ostream&) const;

			void
			write_object(const persistent_object_manager& m, 
					ostream& o) const {
				write_object_base(m, o);
			}

			void
			load_object_base(const persistent_object_manager&,
				istream&);

			void
			load_object(const persistent_object_manager& m,
					istream& i) {
				load_object_base(m, i);
			}
	};	// end struct node_expr_type
	typedef	vector<node_expr_type>		internal_node_pool_type;
	/**
		This map keeps track of internal nodes defined in 
		terms of one-sided guard expressions.  
		String should be of the form: x[...]+.
		Each pull may only be defined once.  
		value_type is index into expression pool.  
		TODO: Is there a way to store reference object instead
		of their string representations?  (yes, but not critical now)

		this is a redundant map, 
		key is same as node_expr_type::name
		value is index into internal_node_pool
	 */
	typedef	map<string, node_index_type>	internal_node_expr_map_type;
	/// list of root expression indices
	typedef	vector<invariant_type>		invariant_pool_type;

	/**
		This structure keeps a map of which rules/macros (by index)
		belong to which subcircuit.  
		Each pair represents the set [i,j)
		Indices not found in this set are presumed to be in
		the outer-most level, designated index 0.  
	 */
	typedef	std::pair<rule_index_type, rule_index_type>
						index_range;

	static
	ostream&
	print_range(ostream&, const index_range&);

	static
	bool
	range_empty(const index_range& r) {
		return r.first == r.second;
	}

	/**
		Information common to all mappings of side structures.  
	 */
	struct resource_map_entry {
		index_range			rules;
		index_range			macros;
		index_range			int_nodes;

		bool
		rules_empty(void) const {
			return rules.first == rules.second;
		}

		bool
		macros_empty(void) const {
			return macros.first == macros.second;
		}

		bool
		nodes_empty(void) const {
			return int_nodes.first == int_nodes.second;
		}

		ostream&
		dump(ostream&) const;

	};
	/**
		Each subcircuit may contain rules and macros, 
		so we need to keep these sets coherent.
	 */
	struct subcircuit_map_entry : public resource_map_entry {
		/**
			Back-reference to original subcircuit.
			Saves from copying string name, or other info.
			Ideally, this should be reconstructed without
			having to save the pointer persistently.
		 */
		never_ptr<const subcircuit>	back_ref;
		subcircuit_map_entry() { }
		subcircuit_map_entry(const subcircuit* b) : back_ref(b) { }

		const string&
		get_name(void) const;

		void
		collect_transient_info_base(persistent_object_manager&) const;

		void
		write_object(const persistent_object_manager&, ostream&) const;
		void
		load_object(const persistent_object_manager&, istream&);
	};	// end struct subcircuit_map_entry
	/**
		This will resemble a discrete_interval_set except that
		there can be blank entries, which are skipped.  
		Algorithm: since this is maintained sorted, 
			can use binary search algorithms, such as
			lower_bound, upper_bound.  
	 */
	typedef	vector<subcircuit_map_entry>	subcircuit_map_type;
#if PRS_SUPPLY_OVERRIDES
	/**
		Structure for tracking which supplies drive which rules.  
	 */
	struct supply_override_entry : public resource_map_entry {
		// never_ptr<const rule_set>		back_ref;
		node_index_type			Vdd;
		node_index_type			GND;
#if PRS_SUBSTRATE_OVERRIDES
		node_index_type			Vdd_substrate;
		node_index_type			GND_substrate;
#endif

		void
		write_object(const persistent_object_manager&, ostream&) const;
		void
		load_object(const persistent_object_manager&, istream&);
	};	// end struct supply_override_entry
	/**
		More space-efficient to keep supply information aside
		instead of per rule because this is usually coarse-grained.  
	 */
	typedef	vector<supply_override_entry>	supply_map_type;
#endif
	typedef	state_instance<bool_tag>	bool_instance_type;
	typedef	instance_pool<bool_instance_type>
						node_pool_type;
	typedef	vector<rule>			rule_pool_type;
	typedef	PRS_footprint_expr_pool_type	expr_pool_type;
	typedef	vector<macro>			macro_pool_type;

private:
	rule_pool_type				rule_pool;
	expr_pool_type				expr_pool;
	macro_pool_type				macro_pool;
	internal_node_pool_type			internal_node_pool;
	internal_node_expr_map_type		internal_node_expr_map;
	invariant_pool_type			invariant_pool;
	subcircuit_map_type			subcircuit_map;
#if PRS_SUPPLY_OVERRIDES
	supply_map_type				supply_map;
public:
	/**
		HACK: these members are only used during unroll construction, 
		and need not persist.
	 */
	node_index_type				current_Vdd;
	node_index_type				current_GND;
#if PRS_SUBSTRATE_OVERRIDES
	node_index_type				current_Vdd_substrate;
	node_index_type				current_GND_substrate;
#endif
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

// private:
public:
	ostream&
	dump_expr(const expr_node&, ostream&, 
		const entity::footprint&, const char) const;

	ostream&
	dump_rule(const rule&, ostream&, const entity::footprint&) const;

	ostream&
	dump_macro(const macro&, ostream&, const entity::footprint&) const;

public:
	// a method for registering internal nodes and expressions
	good_bool
	register_internal_node_expr(const string&, 
		const expr_index_type, const bool);

	expr_index_type
	lookup_internal_node_expr(const string&, const bool) const;

	// ALERT: O(n) linear search
	internal_node_pool_type::const_iterator
	find_internal_node(const expr_index_type) const;

	internal_node_pool_type&
	get_internal_node_pool(void) {
		return internal_node_pool;
	}

	const internal_node_pool_type&
	get_internal_node_pool(void) const {
		return internal_node_pool;
	}

	const node_expr_type&
	get_internal_node(const node_index_type i) const {
		INVARIANT(i < internal_node_pool.size());
		return internal_node_pool[i];
	}

	// returns reference to new expression node
	expr_node&
	push_back_expr(const char, const expr_index_type);

	rule&
	push_back_rule(const int, const int, const bool);

	macro&
	push_back_macro(const string&);

	void
	push_back_invariant(const invariant_type& t) {
		invariant_pool.push_back(t);
	}

	const subcircuit_map_type&
	get_subcircuit_map(void) const { return subcircuit_map; }

	void
	push_back_subcircuit(const subcircuit_map_entry& t) {
		subcircuit_map.push_back(t);
	}

	expr_index_type
	current_expr_index(void) const {
		return expr_pool.size();
	}

#if PRS_SUPPLY_OVERRIDES
	const supply_map_type&
	get_supply_map(void) const { return supply_map; }

	supply_map_type&
	get_supply_map(void) { return supply_map; }

private:
	// for use with std::upper_bound
	static
	bool
	rule_supply_map_compare(const rule_index_type v,
			const supply_map_type::value_type& i) {
		return v < i.rules.first;
	}

	static
	bool
	macro_supply_map_compare(const macro_index_type v,
			const supply_map_type::value_type& i) {
		return v < i.macros.first;
	}

	static
	bool
	internal_node_supply_map_compare(const node_index_type v,
			const supply_map_type::value_type& i) {
		return v < i.int_nodes.first;
	}

public:
	supply_map_type::const_iterator
	lookup_rule_supply(const rule_index_type) const;

	supply_map_type::const_iterator
	lookup_macro_supply(const macro_index_type) const;

	supply_map_type::const_iterator
	lookup_internal_node_supply(const node_index_type) const;
#endif

	void
	collect_literal_indices(std::set<node_index_type>&, // node_index_type
		const expr_index_type) const;	// should be expr_index_type

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

