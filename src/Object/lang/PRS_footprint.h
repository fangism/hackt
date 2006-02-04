/**
	\file "Object/lang/PRS_footprint.h"
	$Id: PRS_footprint.h,v 1.7.2.1 2006/02/04 01:33:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_H__

#include <iosfwd>
#include <valarray>
#include <vector>
#include "Object/inst/instance_pool_fwd.h"
#include "Object/lang/PRS_footprint_expr.h"
#include "Object/lang/PRS_footprint_rule.h"
#include "Object/lang/PRS_footprint_macro.h"
#include "Object/lang/PRS_footprint_expr_pool_fwd.h"
#include "util/macros.h"
#include "util/list_vector.h"
#include "util/offset_array.h"
#include "util/persistent_fwd.h"

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
	typedef	footprint_macro			macro;

private:
	typedef	state_instance<bool_tag>	bool_instance_type;
	typedef	instance_pool<bool_instance_type>
						node_pool_type;
	typedef	util::list_vector<rule>		rule_pool_type;
	typedef	PRS_footprint_expr_pool_type	expr_pool_type;
	typedef	util::list_vector<macro>	macro_pool_type;

	rule_pool_type				rule_pool;
	expr_pool_type				expr_pool;
	macro_pool_type				macro_pool;

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
	ostream&
	dump_macro(const macro&, ostream&, const node_pool_type&);

public:
	// returns reference to new expression node
	expr_node&
	push_back_expr(const char, const size_t);

	rule&
	push_back_rule(const int, const int, const bool);

	macro&
	push_back_macro(const string&);

	size_t
	current_expr_index(void) const {
		return expr_pool.size();
	}

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

