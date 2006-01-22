/**
	\file "Object/lang/PRS_footprint.h"
	$Id: PRS_footprint.h,v 1.5 2006/01/22 18:20:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_H__

#include <iosfwd>
#include <valarray>
#include <vector>
#include "Object/inst/instance_pool_fwd.h"
#include "Object/lang/PRS_footprint_expr.h"
#include "Object/lang/PRS_footprint_rule.h"
#include "Object/lang/PRS_footprint_expr_pool_fwd.h"
#include "util/macros.h"
#include "util/list_vector.h"
#include "util/offset_array.h"

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
class footprint : public cflat_visitee {
	friend class cflat_visitor;
public:
	typedef	footprint_expr_node		expr_node;
	typedef	footprint_rule			rule;

	// typedef	std::vector<>
private:
	typedef	state_instance<bool_tag>	bool_instance_type;
	typedef	instance_pool<bool_instance_type>
						node_pool_type;
	typedef	util::list_vector<rule>		rule_pool_type;
#if 0
	typedef	util::offset_array<util::list_vector<expr_node>, 1>
						expr_pool_type;
#else
	typedef	PRS_footprint_expr_pool_type	expr_pool_type;
#endif

	rule_pool_type				rule_pool;
	expr_pool_type				expr_pool;

public:
	footprint();
	~footprint();

	ostream&
	dump(ostream&, const entity::footprint&) const;

	const expr_pool_type&
	get_expr_pool(void) const { return expr_pool; }

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

	void
	accept(cflat_visitor&) const;

};	// end class footprint

//=============================================================================
}	// end namepace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_FOOTPRINT_H__

