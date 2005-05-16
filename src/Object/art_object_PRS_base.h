/**
	\file "Object/art_object_PRS_base.h"
	Structures for production rules.
	$Id: art_object_PRS_base.h,v 1.1.2.3 2005/05/16 21:43:42 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_PRS_BASE_H__
#define	__OBJECT_ART_OBJECT_PRS_BASE_H__

#include <list>
#include "Object/art_object_fwd.h"
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
using util::persistent;
using util::persistent_object_manager;
//=============================================================================

class rule;
class prs_expr;
typedef	sticky_ptr<prs_expr>			guard_type;
typedef	excl_ptr<prs_expr>			guard_arg_type;

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

	struct dumper {
		ostream& os;
		dumper(ostream& o) : os(o) { }

		template <class P>
		void
		operator () (const P&);
	};	// end struct dumper

virtual	ostream&
	dump(ostream&) const = 0;

virtual	excl_ptr<rule>
	complement(void) const = 0;

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
	typedef	list<guard_type>	expr_sequence_type;
public:
	prs_expr() { }
virtual	~prs_expr() { }

virtual	ostream&
	dump(ostream&) const = 0;

virtual	excl_ptr<prs_expr>
	negation_normalize(void) const = 0;

#if 0
	struct negation_normalizer {
		excl_ptr<prs_expr>
		operator () (const sticky_ptr<prs_expr>&) const;
	};	// end struct negation_normalizer
#endif

};	// end class prs_expr

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_PRS_BASE_H__

