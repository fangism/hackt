/**
	\file "Object/lang/RTE_base.hh"
	Structures for production assignments.
	$Id: RTE_base.hh,v 1.12 2010/07/09 02:14:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_RTE_BASE_HH__
#define	__HAC_OBJECT_LANG_RTE_BASE_HH__

#include <list>
#include "util/memory/excl_ptr.hh"
#include "util/memory/count_ptr.hh"
#include "util/persistent.hh"
#include "util/boolean_types.hh"
#include "Object/inst/instance_pool_fwd.hh"
#include "Object/lang/PRS_dump_context.hh"

namespace HAC {
namespace entity {
class unroll_context;
class scopespace;
struct bool_tag;

template <class>
class state_instance;

/**
	Namespace for RTE objects.  
	There are classes that are stored in process definitions, 
		but not the final result of unroll-creation.  
 */
namespace PRS {
	struct rule_dump_context;
	struct expr_dump_context;
}
namespace RTE {
class footprint;		// defined in "Object/lang/RTE_footprint.h"
using std::list;
using std::istream;
using std::ostream;
using util::good_bool;
using util::memory::never_ptr;
using util::memory::excl_ptr;
using util::memory::count_ptr;
using util::memory::sticky_ptr;
using util::persistent;
using util::persistent_object_manager;
//=============================================================================

class atomic_assignment;
class rte_expr;
typedef	count_ptr<rte_expr>			rte_expr_ptr_type;
typedef	count_ptr<const rte_expr>		const_rte_expr_ptr_type;

typedef	state_instance<bool_tag>		bool_instance_type;
typedef	instance_pool<bool_instance_type>	node_pool_type;
typedef	size_t					node_index_type;

//=============================================================================
/**
	Dump modifier for RTE assignments.  
 */
typedef	PRS::rule_dump_context			assignment_dump_context;

/**
	Helper class for controlling RTE and expression dumps.  
	Reuse from PRS.
 */
typedef	PRS::expr_dump_context			expr_dump_context;

//=============================================================================
/**
	Abstract base class for a production atomic_assignment.  
	TODO: parent link for upward structure?
 */
class atomic_assignment : public persistent {
public:
	atomic_assignment() { }
virtual	~atomic_assignment() { }

virtual	ostream&
	dump(ostream&, const assignment_dump_context&) const = 0;

/**
	Prototype for unroll visiting.  
 */
#define	RTE_UNROLL_ASSIGN_PROTO						\
	good_bool							\
	unroll(const unroll_context&) const

virtual	RTE_UNROLL_ASSIGN_PROTO = 0;

#define	RTE_CHECK_ASSIGN_PROTO						\
	void check(void) const

virtual	RTE_CHECK_ASSIGN_PROTO = 0;

	struct checker;
	struct dumper;
};	// end class atomic_assignment

//=============================================================================
/**
	A collection or production assignments.  
	This class wants to be pure-virtual, except that it is 
	instantiated non-dynamically by process_definition.
 */
class assignment_set_base : public list<sticky_ptr<atomic_assignment> > {
protected:
	typedef	list<sticky_ptr<atomic_assignment> >		parent_type;
public:
	typedef	parent_type::value_type		value_type;
public:
	assignment_set_base();
	// dtor needs to be polymorphic to dynamic_cast to assignment_set
virtual	~assignment_set_base();

#if 0
private:
	// not copy-constructible, or should be restricted with run-time check
	explicit
	assignment_set(_baseconst this_type&);
#endif

public:

	ostream&
	dump(ostream&, const assignment_dump_context& = assignment_dump_context()) const;

	void
	append_assignment(excl_ptr<atomic_assignment>&);

	template <class R>
	void
	append_assignment(excl_ptr<R>& r) {
		excl_ptr<atomic_assignment> tr = r.template as_a_xfer<atomic_assignment>();
		this->append_assignment(tr);
	}

	// supply these for derived classes
	RTE_UNROLL_ASSIGN_PROTO;
	RTE_CHECK_ASSIGN_PROTO;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

private:
	// hide this from user
	using parent_type::push_back;

};	// end class assignment_set_base

typedef	assignment_set_base		nested_assignments;

//=============================================================================
/**
	Abstract class of atomic assignment expressions.  
	These expressions are not unrolled.  
 */
class rte_expr : public persistent {
public:
	/**
		Worry about implementation efficiency later...
		(Vector of raw pointers or excl_ptr with copy-constructor.)
	 */
	typedef	list<rte_expr_ptr_type>	expr_sequence_type;
public:
	rte_expr() { }
virtual	~rte_expr() { }

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

	ostream&
	dump(ostream& o) const { return dump(o, expr_dump_context()); }

virtual	void
	check(void) const = 0;

// accumulate set of used node indices
#define	RTE_UNROLL_EXPR_PROTO						\
	size_t								\
	unroll(const unroll_context&) const

virtual	RTE_UNROLL_EXPR_PROTO = 0;

#define	RTE_UNROLL_COPY_PROTO						\
	rte_expr_ptr_type						\
	unroll_copy(const unroll_context&, const rte_expr_ptr_type&) const

virtual	RTE_UNROLL_COPY_PROTO = 0;

protected:
	struct checker;
	struct unroller;
	struct unroll_copier;

};	// end class rte_expr

//=============================================================================
}	// end namespace RTE
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_RTE_BASE_HH__

