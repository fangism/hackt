/**
	\file "Object/art_object_PRS.h"
	Structures for production rules.
	$Id: art_object_PRS.h,v 1.1.2.2 2005/05/16 03:52:20 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_PRS_H__
#define	__OBJECT_ART_OBJECT_PRS_H__

#include "Object/art_object_PRS_base.h"
#include <vector>

namespace ART {
namespace entity {
namespace PRS {
using std::istream;
using std::ostream;
using std::vector;
using util::persistent_object_manager;
//=============================================================================
// forward declarations

class literal;
typedef	excl_ptr<bool_instance_reference>	literal_base_ptr_type;

//=============================================================================
/**
	Pull-down production rule.  
 */
class pull_up : public rule {
	typedef	pull_up			this_type;
protected:
	guard_type			guard;
	excl_ptr<literal>		output;
public:
	pull_up();
	pull_up(guard_arg_type&, excl_ptr<literal>&);
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
	excl_ptr<literal>		output;
public:
	pull_dn();
	pull_dn(guard_arg_type&, excl_ptr<literal>&);
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
	excl_ptr<literal>		output1;
	excl_ptr<literal>		output2;
public:

	ostream&
	what(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS;
};	// and class pass

//=============================================================================
/**
	Logical AND expression.  
 */
class and_expr : public prs_expr, public prs_expr::expr_sequence_type {
	typedef	and_expr			this_type;
	typedef	prs_expr::expr_sequence_type	sequence_type;
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
class or_expr : public prs_expr, public prs_expr::expr_sequence_type {
	typedef	or_expr				this_type;
	typedef	prs_expr::expr_sequence_type	sequence_type;
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
	not_expr(guard_arg_type&);
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
//	typedef	bool_instance_reference		reference_parent_type;
private:
	literal_base_ptr_type		var;
public:
	literal();

	explicit
	literal(literal_base_ptr_type&);

	~literal();

	ostream&
	what(ostream&) const;

	// fanout.. not until actually instantiated, unrolled, created...
	PERSISTENT_METHODS_DECLARATIONS;
	// POOL ALLOCATE
};	// end class literal

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_PRS_H__

