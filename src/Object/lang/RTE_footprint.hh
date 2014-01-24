/**
	\file "Object/lang/RTE_footprint.hh"
	$Id: RTE_footprint.hh,v 1.22 2010/09/29 00:13:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_RTE_FOOTPRINT_HH__
#define	__HAC_OBJECT_LANG_RTE_FOOTPRINT_HH__

#include <iosfwd>
#include <vector>
#include <map>
#include <set>		// for collecting unique node indices
#include <string>

#include "Object/inst/instance_pool_fwd.hh"
#include "Object/lang/RTE_footprint_expr.hh"
#include "Object/lang/RTE_footprint_assignment.hh"
#include "Object/lang/RTE_footprint_expr_pool_fwd.hh"
#include "Object/devel_switches.hh"
#include "util/macros.h"
#include "util/boolean_types.hh"
#include "util/offset_array.hh"
#include "util/persistent_fwd.hh"
#include "util/memory/excl_ptr.hh"	// for never_ptr

namespace HAC {
struct cflat_options;

namespace entity {
class footprint;
struct bool_tag;
class state_manager;
class atomic_update_graph;

template <class Tag>
class state_instance;

template <class Tag>
class footprint_frame_map;

namespace RTE {
using std::ostream;
using std::istream;
using util::persistent_object_manager;
using util::good_bool;
using std::map;
using std::pair;
using std::string;
using util::memory::never_ptr;

//=============================================================================
/**
	An expression set footprint contains a resolved representation
	of its expression assignments in terms of local and formal 
	boolean instance references.  
	The production rules that belong to a process definition
	are just compact representations that need to be unrolled.  

	As this is constructed, each 'node' could conceivably keep track
	of its "local" fanin and fanout, to avoid recomputing it.  
 */
class footprint : public cflat_visitee {
	friend class cflat_visitor;
public:
	typedef	size_t				node_index_type;
	typedef	size_t				expr_index_type;
	typedef	size_t				assignment_index_type;
	typedef	footprint_expr_node		expr_node;
	typedef	footprint_assignment		assignment;
	typedef	state_instance<bool_tag>	bool_instance_type;
	typedef	instance_pool<bool_instance_type>
						node_pool_type;
	// unordered
	typedef	vector<assignment>		assignment_pool_type;
	typedef	RTE_footprint_expr_pool_type	expr_pool_type;
#if 0
	typedef	expr_index_type			node_expr_type;
	// need this?
	typedef	vector<node_expr_type>		assigned_node_pool_type;
	typedef map<node_index_type, expr_index_type>
						assignment_map_type;
#endif
private:
	assignment_pool_type			assignment_pool;
	expr_pool_type				expr_pool;
public:
	footprint();
	~footprint();

	ostream&
	dump(ostream&, const entity::footprint&) const;

	const expr_pool_type&
	get_expr_pool(void) const { return expr_pool; }

	expr_pool_type&
	get_expr_pool(void) { return expr_pool; }

	const assignment_pool_type&
	get_assignment_pool(void) const { return assignment_pool; }

// private:
public:
	ostream&
	dump_expr(const expr_node&, ostream&, 
		const entity::footprint&, const char) const;

	ostream&
	dump_assignment(const assignment&, ostream&, const entity::footprint&) const;

public:
	// returns reference to new expression node
	expr_node&
	push_back_expr(const char, const expr_index_type);

	assignment&
	push_back_assignment(const int expr, const int node);

	expr_index_type
	current_expr_index(void) const {
		return expr_pool.size();
	}

	void
	collect_literal_indices(std::set<node_index_type>&, // node_index_type
		const expr_index_type) const;	// should be expr_index_type

	void
	collect_atomic_dependencies(const node_pool_type&,
		atomic_update_graph&) const;

private:
	void
	__collect_atomic_dependencies_assign(const node_pool_type&,
		const assignment&, atomic_update_graph&) const;

	void
	__collect_atomic_dependencies_expr(const node_pool_type&,
		const expr_node&, std::set<node_index_type>&) const;

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
}	// end namepace RTE
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_RTE_FOOTPRINT_HH__

