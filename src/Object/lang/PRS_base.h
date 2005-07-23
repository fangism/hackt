/**
	\file "Object/lang/PRS_base.h"
	Structures for production rules.
	$Id: PRS_base.h,v 1.1.2.1 2005/07/23 01:05:59 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_PRS_BASE_H__
#define	__OBJECT_ART_OBJECT_PRS_BASE_H__

#include <list>
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"

namespace ART {
namespace entity {
/**
	Namespace for PRS objects.  
	There are classes that are stored in process definitions, 
		but not the final result of unroll-creation.  
 */
namespace PRS {
using std::list;
using std::istream;
using std::ostream;
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

//=============================================================================
/**
	A collection or production rules.  
 */
class rule_set : public list<sticky_ptr<rule> > {
	typedef	rule_set			this_type;
protected:
	typedef	list<sticky_ptr<rule> >		parent_type;
public:
	typedef	parent_type::value_type		value_type;
public:
	rule_set();
	~rule_set();

	ostream&
	dump(ostream&) const;

	void
	expand_complements(void);

	void
	compact_references(void);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class rule_set

//=============================================================================
/**
	Abstract base class for a production rule.  
 */
class rule : public persistent {
public:
	rule() { }
virtual	~rule() { }

virtual	ostream&
	dump(ostream&) const = 0;

virtual	excl_ptr<rule>
	expand_complement(void) = 0;

virtual	void
	check(void) const = 0;

	struct checker {
		template <class P>
		void
		operator () (const P&) const;
	};

	struct dumper {
		ostream& os;
		dumper(ostream& o) : os(o) { }

		template <class P>
		void
		operator () (const P&);
	};	// end struct dumper

};	// end class rule

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
	dump(ostream&, const int) const = 0;

	ostream&
	dump(ostream& o) const { return dump(o, 0); }

virtual	prs_expr_ptr_type
	negate(void) const = 0;

virtual	prs_expr_ptr_type
	negation_normalize(void) = 0;

virtual	void
	check(void) const = 0;

	struct checker {
		void
		operator () (const const_prs_expr_ptr_type&) const;
	};

	struct negater {
		prs_expr_ptr_type
		operator () (const const_prs_expr_ptr_type&) const;
	};	// end struct negation_normalizer

	struct negation_normalizer {
		const bool nb;
		negation_normalizer(const bool b) : nb(b) { }

		prs_expr_ptr_type
		operator () (const prs_expr_ptr_type&) const;
	};	// end struct negation_normalizer

};	// end class prs_expr

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_PRS_BASE_H__

