/**
	\file "Object/art_object_PRS.h"
	Structures for production rules.
	$Id: art_object_PRS.h,v 1.2 2005/05/19 18:43:30 fang Exp $
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
private:
	enum { print_stamp = 0 };
public:
	literal();

	explicit
	literal(const literal_base_ptr_type&);

	// default copy constructor (is copy-constructible)

	~literal();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const int) const;

	ostream&
	dump(ostream& o) const { return dump(o, 0); }

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

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
	prs_expr_ptr_type		guard;
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
	pull_up(const prs_expr_ptr_type&, const literal&, const bool);
	~pull_up();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	void
	check(void) const;

	excl_ptr<rule>
	expand_complement(void);

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
	prs_expr_ptr_type		guard;
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
	pull_dn(const prs_expr_ptr_type&, const literal&, const bool);
	~pull_dn();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	void
	check(void) const;

	excl_ptr<rule>
	expand_complement(void);

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
	prs_expr_ptr_type		guard;
	literal				output1;
	literal				output2;
public:

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	void
	check(void) const;

	excl_ptr<rule>
	expand_complement(void);

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
private:
	enum { print_stamp = 2 };
public:
	and_expr();
	~and_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const int) const;

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

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
private:
	enum { print_stamp = 3 };
public:
	or_expr();
	~or_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const int) const;

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

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
	prs_expr_ptr_type			var;
private:
	enum { print_stamp = 1 };
public:
	not_expr();

	explicit
	not_expr(const prs_expr_ptr_type&);
	~not_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const int) const;

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PERSISTENT_METHODS_DECLARATIONS;
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class not_expr

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_PRS_H__

