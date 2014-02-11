/**
	\file "Object/lang/PRS_base.hh"
	Structures for production rules.
	$Id: PRS_base.hh,v 1.12 2010/07/09 02:14:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_BASE_H__
#define	__HAC_OBJECT_LANG_PRS_BASE_H__

#include <list>
#include "util/memory/excl_ptr.hh"
#include "util/memory/count_ptr.hh"
#include "util/persistent.hh"
#include "util/boolean_types.hh"
#include "Object/lang/PRS_dump_context.hh"
#include "Object/inst/instance_pool_fwd.hh"

/**
	Define to 1 to support internal nodes.
	Goal: 1
	Priority: med (for ACT compatibility)
	Status: perm'd on branch ACT-00-01-04-main-00-81-74-ACT-01-11-PRS-08
#define	PRS_INTERNAL_NODES			1
**/

namespace HAC {
namespace entity {
class unroll_context;
class scopespace;
struct bool_tag;

template <class>
class state_instance;

/**
	Namespace for PRS objects.  
	There are classes that are stored in process definitions, 
		but not the final result of unroll-creation.  
 */
namespace PRS {
class footprint;		// defined in "Object/lang/PRS_footprint.h"
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

class rule;
class prs_expr;
typedef	count_ptr<prs_expr>			prs_expr_ptr_type;
typedef	count_ptr<const prs_expr>		const_prs_expr_ptr_type;

typedef	state_instance<bool_tag>		bool_instance_type;
typedef	instance_pool<bool_instance_type>	node_pool_type;

//=============================================================================
/**
	Abstract base class for a production rule.  
	TODO: parent link for upward structure?
 */
class rule : public persistent {
public:
	rule() { }
virtual	~rule() { }

virtual	ostream&
	dump(ostream&, const rule_dump_context&) const = 0;

#define	PRS_EXPAND_COMPLEMENT_PROTO					\
	excl_ptr<rule>							\
	expand_complement(void)
virtual	PRS_EXPAND_COMPLEMENT_PROTO = 0;

/**
	Prototype for unroll visiting.  
 */
#define	PRS_UNROLL_RULE_PROTO						\
	good_bool							\
	unroll(const unroll_context&) const

virtual	PRS_UNROLL_RULE_PROTO = 0;

#define	PRS_CHECK_RULE_PROTO						\
	void check(void) const

virtual	PRS_CHECK_RULE_PROTO = 0;

	struct checker;
	struct dumper;
};	// end class rule

//=============================================================================
/**
	A collection or production rules.  
	This class wants to be pure-virtual, except that it is 
	instantiated non-dynamically by process_definition.
 */
class rule_set_base : public list<sticky_ptr<rule> > {
protected:
	typedef	list<sticky_ptr<rule> >		parent_type;
public:
	typedef	parent_type::value_type		value_type;
public:
	rule_set_base();
	// dtor needs to be polymorphic to dynamic_cast to rule_set
virtual	~rule_set_base();

#if 0
private:
	// not copy-constructible, or should be restricted with run-time check
	explicit
	rule_set(_baseconst this_type&);
#endif

public:

	ostream&
	dump(ostream&, const rule_dump_context& = rule_dump_context()) const;

	void
	expand_complements(void);

	void
	compact_references(void);

	void
	append_rule(excl_ptr<rule>&);

	template <class R>
	void
	append_rule(excl_ptr<R>& r) {
		excl_ptr<rule> tr = r.template as_a_xfer<rule>();
		this->append_rule(tr);
	}

	// supply these for derived classes
	PRS_UNROLL_RULE_PROTO;
	PRS_CHECK_RULE_PROTO;
	PRS_EXPAND_COMPLEMENT_PROTO;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

private:
	// hide this from user
	using parent_type::push_back;

};	// end class rule_set_base

typedef	rule_set_base		nested_rules;

//=============================================================================
/**
	Abstract class of production rule expressions.  
	These expressions are not unrolled.  
 */
class prs_expr : public persistent {
public:
	/**
		Worry about implementation efficiency later...
		(Vector of raw pointers or excl_ptr with copy-constructor.)
	 */
	typedef	list<prs_expr_ptr_type>	expr_sequence_type;
public:
	prs_expr() { }
virtual	~prs_expr() { }

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

	ostream&
	dump(ostream& o) const { return dump(o, expr_dump_context()); }

virtual	prs_expr_ptr_type
	negate(void) const = 0;

virtual	prs_expr_ptr_type
	flip_literals(void) const = 0;

virtual	prs_expr_ptr_type
	negation_normalize(void) = 0;

virtual	void
	check(void) const = 0;

#define	PRS_UNROLL_EXPR_PROTO						\
	size_t								\
	unroll(const unroll_context&) const

virtual	PRS_UNROLL_EXPR_PROTO = 0;

#define	PRS_UNROLL_COPY_PROTO						\
	prs_expr_ptr_type						\
	unroll_copy(const unroll_context&, const prs_expr_ptr_type&) const

virtual	PRS_UNROLL_COPY_PROTO = 0;

protected:
	struct checker;
	struct negater;
	struct literal_flipper;
	struct negation_normalizer;
	struct unroller;
	struct unroll_copier;

};	// end class prs_expr

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_BASE_H__

