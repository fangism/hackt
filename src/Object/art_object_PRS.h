/**
	\file "Object/art_object_PRS.h"
	Structures for production rules.
	$Id: art_object_PRS.h,v 1.1.2.1 2005/05/15 02:39:10 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_PRS_H__
#define	__OBJECT_ART_OBJECT_PRS_H__

#include <list>
#include <vector>
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
using std::istream;
using std::ostream;
using std::list;
using std::vector;
using util::memory::excl_ptr;
using util::memory::count_ptr;
using util::persistent;
using util::persistent_object_manager;
//=============================================================================

class rule;
class prs_expr;
typedef	excl_ptr<bool_instance_reference>	prs_literal_type;
typedef	sticky_ptr<prs_expr>			guard_type;

//=============================================================================
/**
	A collection or production rules.  
 */
class rule_set : public list<excl_ptr<rule> > {
	typedef	rule_set			this_type;
protected:
	typedef	list<excl_ptr<rule> >		parent_type;
public:
	rule_set();
	~rule_set();

};	// end class rule_set

//=============================================================================
/**
	Abstract base class for a production rule.  
 */
class rule : public persistent {
public:
	rule() { }
virtual	~rule() { }

// virtual	ostream& dump(ostream&) const;
};	// end class rule

//=============================================================================
/**
	Pull-down production rule.  
 */
class pull_up : public rule {
	typedef	pull_up			this_type;
protected:
	guard_type			guard;
	prs_literal_type		output;
public:
	pull_up();
	pull_up(guard_type&, prs_literal_type&);
	~pull_up();

	ostream&
	what(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class pull-up

//=============================================================================
/**
	Pull-up production rule.  
 */
class pull_dn : public rule {
	typedef	pull_dn			this_type;
protected:
	guard_type			guard;
	prs_literal_type		output;
public:
	pull_dn();
	pull_dn(guard_type&, prs_literal_type&);
	~pull_dn();

	ostream&
	what(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class pull_dn

//=============================================================================
/**
	Specialized construct for a pass-gate.
	PUNT!
 */
class pass : public rule {
protected:
	guard_type			guard;
	prs_literal_type		output1;
	prs_literal_type		output2;
public:

	ostream&
	what(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS;
};	// and class pass

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
};	// end class prs_expr

//=============================================================================
/**
	Logical AND expression.  
 */
class and_expr : public prs_expr {
	typedef	and_expr			this_type;
private:
	/**
		List of terms to AND.  
	 */
	expr_sequence_type			operands;
public:
	and_expr();
	~and_expr();

	ostream&
	what(ostream&) const;

#if 0
	void
	reserve(size_t n) { operands.reserve(n); }
#endif

	PERSISTENT_METHODS_DECLARATIONS;
	// POOL ALLOCATE
};	// end class and_expr

//=============================================================================
/**
	Logical OR expression.  
 */
class or_expr : public prs_expr {
	typedef	or_expr				this_type;
private:
	/**
		List of terms to AND.  
	 */
	expr_sequence_type			operands;
public:
	or_expr();
	~or_expr();

	ostream&
	what(ostream&) const;

#if 0
	void
	reserve(size_t n) { operands.reserve(n); }
#endif

	PERSISTENT_METHODS_DECLARATIONS;
	// POOL ALLOCATE
};	// end class or_expr

//=============================================================================
/**
	Logical NOT expression.  
 */
class not_expr : public prs_expr {
	typedef	not_expr			this_type;
private:
	guard_type		var;
public:
	not_expr();

	explicit
	not_expr(guard_type&);
	~not_expr();

	ostream&
	what(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS;
	// POOL ALLOCATE
};	// end class not_expr

//=============================================================================
/**
	Literal expression.  
 */
class literal : public prs_expr {
	typedef	literal				this_type;
private:
	prs_literal_type		var;
public:
	literal();

	explicit
	literal(prs_literal_type&);

	ostream&
	what(ostream&) const;

	~literal();
	// fanout.. not until actually instantiated, unrolled, created...
	PERSISTENT_METHODS_DECLARATIONS;
	// POOL ALLOCATE
};	// end class literal

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_PRS_H__

