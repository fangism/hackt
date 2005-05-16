/**
	\file "Object/art_object_PRS.h"
	Structures for production rules.
	$Id: art_object_PRS.h,v 1.1.2.4 2005/05/16 21:43:42 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_PRS_H__
#define	__OBJECT_ART_OBJECT_PRS_H__

#include "Object/art_object_PRS_base.h"
#include <vector>
#include "util/memory/chunk_map_pool_fwd.h"

namespace ART {
namespace entity {
namespace PRS {
using std::vector;
//=============================================================================
// forward declarations

typedef	count_ptr<bool_instance_reference>	literal_base_ptr_type;

//=============================================================================
/**
	Literal expression.  
 */
class literal : public prs_expr {
	typedef	literal				this_type;
private:
	literal_base_ptr_type			var;
public:
	literal();

	explicit
	literal(const literal_base_ptr_type&);

	// default copy constructor (is copy-constructible)

	~literal();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	excl_ptr<prs_expr>
	negation_normalize(void) const;

	// fanout.. not until actually instantiated, unrolled, created...
	PERSISTENT_METHODS_DECLARATIONS;
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class literal

//=============================================================================
/**
	Pull-up production rule.  
 */
class pull_up : public rule {
	typedef	pull_up			this_type;
protected:
	/**
		Guard expression.  
	 */
	guard_type			guard;
	/**
		Output node.  
	 */
	literal				output;
	/**
		Whether or not complement is implicit.
	 */
	bool				cmpl;
public:
	pull_up();
	pull_up(guard_arg_type&, const literal&, const bool);
	~pull_up();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	excl_ptr<rule>
	complement(void) const;

	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class pull-up

//=============================================================================
/**
	Pull-down production rule.  
 */
class pull_dn : public rule {
	typedef	pull_dn			this_type;
protected:
	/**
		Guard expression.  
	 */
	guard_type			guard;
	/**
		Output node.  
	 */
	literal				output;
	/**
		Whether or not complement is implicit.
	 */
	bool				cmpl;
public:
	pull_dn();
	pull_dn(guard_arg_type&, const literal&, const bool);
	~pull_dn();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	excl_ptr<rule>
	complement(void) const;

	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class pull_dn

//=============================================================================
/**
	Specialized construct for a pass-gate.
	PUNT!
 */
class pass : public rule {
	typedef	pass			this_type;
protected:
	guard_type			guard;
	literal				output1;
	literal				output2;
public:

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	excl_ptr<rule>
	complement(void) const;

	PERSISTENT_METHODS_DECLARATIONS;
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
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

	ostream&
	dump(ostream&) const;

	excl_ptr<prs_expr>
	negation_normalize(void) const;

	PERSISTENT_METHODS_DECLARATIONS;
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
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

	ostream&
	dump(ostream&) const;

	excl_ptr<prs_expr>
	negation_normalize(void) const;

	PERSISTENT_METHODS_DECLARATIONS;
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
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

	ostream&
	dump(ostream&) const;

	excl_ptr<prs_expr>
	negation_normalize(void) const;

	PERSISTENT_METHODS_DECLARATIONS;
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class not_expr

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_PRS_H__

