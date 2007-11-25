/**
	\file "Object/lang/PRS_base.h"
	Structures for production rules.
	$Id: PRS_base.h,v 1.9.6.2 2007/11/25 02:28:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_BASE_H__
#define	__HAC_OBJECT_LANG_PRS_BASE_H__

#include <list>
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"
#include "util/boolean_types.h"
#include "Object/inst/instance_pool_fwd.h"

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
	Dump modifier for PRS rules.  
 */
struct rule_dump_context {
	never_ptr<const scopespace>		parent_scope;

	rule_dump_context() : parent_scope(NULL) { }

	explicit
	rule_dump_context(const scopespace& s) : parent_scope(&s) { }

	explicit
	rule_dump_context(const never_ptr<const scopespace> s) :
		parent_scope(s) { }

	// default copy-constructor

};	// end struct rule_dump_context

//-----------------------------------------------------------------------------
/**
	Helper class for controlling PRS and expression dumps.  
 */
struct expr_dump_context : public rule_dump_context {
	int					expr_stamp;

	expr_dump_context() : rule_dump_context(), expr_stamp(0) { }

	explicit
	expr_dump_context(const never_ptr<const scopespace> s) :
		rule_dump_context(s), expr_stamp(0) { }

	// implicit OK
	expr_dump_context(const rule_dump_context& r) :
		rule_dump_context(r), expr_stamp(0) { }

	// default copy constructor

};	// end struct expr_dump_context

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

virtual	excl_ptr<rule>
	expand_complement(void) = 0;

/**
	Prototype for unroll visiting.  
 */
#define	PRS_UNROLL_RULE_PROTO						\
	good_bool							\
	unroll(const unroll_context&, const node_pool_type&, 		\
		PRS::footprint&) const

virtual	PRS_UNROLL_RULE_PROTO = 0;

virtual	void
	check(void) const = 0;

	struct checker;
	struct dumper;
};	// end class rule

//=============================================================================
/**
	A collection or production rules.  
 */
class rule_set : public rule, 
		public list<sticky_ptr<rule> > {
	typedef	rule_set			this_type;
protected:
	typedef	list<sticky_ptr<rule> >		parent_type;
public:
	typedef	parent_type::value_type		value_type;
public:
	rule_set();
	~rule_set();

#if 0
private:
	// not copy-constructible, or should be restricted with run-time check
	explicit
	rule_set(const rule_set&);
#endif

public:
	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context& = rule_dump_context()) const;

	void
	check(void) const;

	excl_ptr<rule>
	expand_complement(void);

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

	PRS_UNROLL_RULE_PROTO;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

private:
	// hide this from user
	using parent_type::push_back;

protected:
	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class rule_set

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
	unroll(const unroll_context&, const node_pool_type&, 		\
		PRS::footprint&) const

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

