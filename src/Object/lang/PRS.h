/**
	\file "Object/lang/PRS.h"
	Structures for production rules.
	$Id: PRS.h,v 1.3.2.2 2005/10/10 22:13:51 fang Exp $
	TODO: support loop expressions of AND and OR.  
 */

#ifndef	__OBJECT_LANG_PRS_H__
#define	__OBJECT_LANG_PRS_H__

#include "Object/art_object_fwd.h"
#include "Object/lang/PRS_base.h"
// #include "Object/expr/pint_range.h"
// #include "Object/inst/pint_value_collection.h"
#include <vector>
#include "util/memory/chunk_map_pool_fwd.h"

namespace ART {
namespace entity {
class meta_range_expr;
struct pint_tag;
template <class, size_t> class value_array;

namespace PRS {
using std::vector;
//=============================================================================
// forward declarations

typedef	count_ptr<simple_bool_meta_instance_reference>	literal_base_ptr_type;

enum {
	PRS_LITERAL_TYPE_ENUM = 0,
	PRS_NOT_EXPR_TYPE_ENUM = 1,
	PRS_AND_EXPR_TYPE_ENUM = 2,
	PRS_OR_EXPR_TYPE_ENUM = 3
};

//=============================================================================
/**
	Literal expression.  
 */
class literal : public prs_expr {
	typedef	literal				this_type;
private:
	literal_base_ptr_type			var;
private:
	enum { print_stamp = PRS_LITERAL_TYPE_ENUM };
public:
	literal();

	explicit
	literal(const literal_base_ptr_type&);

	// default copy constructor (is copy-constructible)

	~literal();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream& o) const { return dump(o, expr_dump_context()); }

	const literal_base_ptr_type&
	get_bool_var(void) const { return var; }

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;

	// fanout.. not until actually instantiated, unrolled, created...
	PERSISTENT_METHODS_DECLARATIONS

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
	dump(ostream&, const rule_dump_context&) const;

	ostream&
	dump(ostream& o) const {
		return dump(o, rule_dump_context());
	}

	void
	check(void) const;

	excl_ptr<rule>
	expand_complement(void);

	PRS_UNROLL_RULE_PROTO;

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
	dump(ostream&, const rule_dump_context&) const;

	ostream&
	dump(ostream& o) const {
		return dump(o, rule_dump_context());
	}

	void
	check(void) const;

	excl_ptr<rule>
	expand_complement(void);

	PRS_UNROLL_RULE_PROTO;

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
	typedef	rule			parent_type;
protected:
	prs_expr_ptr_type		guard;
	literal				output1;
	literal				output2;
public:
	pass();
	~pass();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context&) const;

	ostream&
	dump(ostream& o) const {
		return dump(o, rule_dump_context());
	}

	void
	check(void) const;

	excl_ptr<rule>
	expand_complement(void);

	PRS_UNROLL_RULE_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// and class pass

//=============================================================================
/**
	Common elements to expression loop.  
 */
class expr_loop_base {
public:
	typedef	value_array<pint_tag, 0>		pint_scalar;
	typedef	count_ptr<pint_scalar>			ind_var_ptr_type;
	typedef	count_ptr<const meta_range_expr>	range_ptr_type;

protected:
	ind_var_ptr_type			ind_var;
	range_ptr_type				range;
	prs_expr_ptr_type			body_expr;

	expr_loop_base();

	expr_loop_base(const ind_var_ptr_type&, const range_ptr_type&);

	expr_loop_base(const ind_var_ptr_type&, const range_ptr_type&,
		const prs_expr_ptr_type&);

	~expr_loop_base();

	ostream&
	dump(ostream&, const expr_dump_context&, const char) const;

	size_t
	unroll_base(const unroll_context&, const node_pool_type&, 
		PRS::footprint&, const char) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end struct expr_loop_base

//=============================================================================
/**
	Logical AND expression.  
 */
class and_expr : public prs_expr, public prs_expr::expr_sequence_type {
	typedef	and_expr			this_type;
	typedef	prs_expr::expr_sequence_type	sequence_type;
private:
	enum { print_stamp = PRS_AND_EXPR_TYPE_ENUM };
public:
	and_expr();
	~and_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class and_expr

//-----------------------------------------------------------------------------
/**
	Logical AND loop.  
 */
class and_expr_loop : public prs_expr, public expr_loop_base {
	typedef	and_expr_loop			this_type;
	typedef	prs_expr			parent_type;
private:
	enum { print_stamp = PRS_AND_EXPR_TYPE_ENUM };
public:
	and_expr_loop();

	and_expr_loop(const ind_var_ptr_type&, const range_ptr_type&);

	and_expr_loop(const ind_var_ptr_type&, const range_ptr_type&, 
		const prs_expr_ptr_type&);

	~and_expr_loop();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
	// CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class and_expr

//=============================================================================
/**
	Logical OR expression.  
 */
class or_expr : public prs_expr, public prs_expr::expr_sequence_type {
	typedef	or_expr				this_type;
	typedef	prs_expr::expr_sequence_type	sequence_type;
private:
	enum { print_stamp = PRS_OR_EXPR_TYPE_ENUM };
public:
	or_expr();
	~or_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class or_expr

//-----------------------------------------------------------------------------
/**
	Logical OR loop.  
 */
class or_expr_loop : public prs_expr, public expr_loop_base {
	typedef	or_expr_loop			this_type;
	typedef	prs_expr			parent_type;
private:
	enum { print_stamp = PRS_OR_EXPR_TYPE_ENUM };
public:
	or_expr_loop();

	or_expr_loop(const ind_var_ptr_type&, const range_ptr_type&);

	or_expr_loop(const ind_var_ptr_type&, const range_ptr_type&, 
		const prs_expr_ptr_type&);

	~or_expr_loop();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
	// CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
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
	enum { print_stamp = PRS_NOT_EXPR_TYPE_ENUM };
public:
	not_expr();

	explicit
	not_expr(const prs_expr_ptr_type&);
	~not_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class not_expr

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_LANG_PRS_H__

