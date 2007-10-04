/**
	\file "Object/lang/PRS.h"
	Structures for production rules.
	$Id: PRS.h,v 1.19.6.4 2007/10/04 19:44:54 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_H__
#define	__HAC_OBJECT_LANG_PRS_H__

#include "Object/lang/PRS_base.h"
#include "Object/lang/PRS_enum.h"
#include "Object/lang/bool_literal.h"
#include "Object/lang/directive_source.h"
#include "Object/unroll/meta_loop_base.h"
#include "Object/unroll/meta_conditional_base.h"
#include <string>
#include <vector>
#include "util/memory/chunk_map_pool_fwd.h"

namespace HAC {
namespace entity {
class meta_range_expr;
class param_expr;
class param_expr_list;
class dynamic_param_expr_list;
class const_param_expr_list;
struct pint_tag;
template <class, size_t> class value_array;

namespace PRS {
using std::string;
//=============================================================================
// forward declarations

/**
	Because PRS only ever deal with bools (nodes).  
 */
typedef	bool_literal_base_ptr_type		literal_base_ptr_type;
typedef	directive_source_params_type		literal_params_type;

//=============================================================================
/**
	Literal expression, which can appear on LHS or RHS of any rule.  
	Re: internal nodes: we've decided to add support here instead of
	in bool_literal, because only production rules should ever touch
	internal nodes.  
 */
class literal : public prs_expr, public bool_literal {
	typedef	literal				this_type;
	typedef	bool_literal			base_type;
public:
	struct	unroller;
	typedef	literal_params_type		params_type;
private:
	enum { print_stamp = PRS_LITERAL_TYPE_ENUM };
	/**
		Parameters are only applicable to expression literals, 
		not the RHS of a rule.  
	 */
	params_type				params;
public:
	literal();

	explicit
	literal(const literal_base_ptr_type&);

#if PRS_INTERNAL_NODES
	explicit
	literal(const node_literal_ptr_type&);
#endif

	// default copy constructor (is copy-constructible)

	~literal();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream& o) const { return dump(o, expr_dump_context()); }

	params_type&
	get_params(void) { return params; }

	const params_type&
	get_params(void) const { return params; }

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;

protected:
	size_t
	unroll_node(const unroll_context&) const;
public:

	// fanout.. not until actually instantiated, unrolled, created...
	PERSISTENT_METHODS_DECLARATIONS

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class literal

//=============================================================================
/**
	Consideration: for efficient copy-constructing, 
	use a count_ptr<vector<...> > instead of a vector.  
	String are already efficiently copied internally.  
 */
class attribute {
public:
	typedef	dynamic_param_expr_list		values_type;
	typedef	count_ptr<const param_expr>	value_type;
	typedef	const value_type&		const_reference;
	typedef	value_type&			reference;
private:
	string					key;
	count_ptr<values_type>			values;
public:
	attribute();

	explicit
	attribute(const string&);

	~attribute();

	operator bool () const;

	// arg is equiv to const_reference
	void
	push_back(const value_type&);

	const string&
	get_key(void) const { return key; }

	ostream&
	dump(ostream&, const rule_dump_context& c) const;

	count_ptr<const const_param_expr_list>
	unroll_values(const unroll_context&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class attribute

//-----------------------------------------------------------------------------
typedef	std::vector<attribute>		rule_attribute_list_type;

//=============================================================================
class pull_base : public rule {
protected:
	/**
		Guard expression.  
	 */
	prs_expr_ptr_type		guard;
	/**
		Output node.  
		Only used if RHS is not an internal node, mutually exclusive.  
	 */
	bool_literal			output;
	/**
		Whether or not complement is implicit.
	 */
	bool				cmpl;
	/**
		Attribute list.  
		Want to make this a pointer for efficient duplication?
	 */
	rule_attribute_list_type	attributes;

	pull_base();

	pull_base(const prs_expr_ptr_type&, const bool_literal&, const bool);

	pull_base(const prs_expr_ptr_type&, const bool_literal&, 
		const rule_attribute_list_type&);

public:
	// because we go through an intermediate count_ptr, dtor needs to 
	// be public, thus need to be virtual to be safe, ah well...
virtual	~pull_base();

protected:
	ostream&
	dump_base(ostream&, const rule_dump_context&, const char) const;

	good_bool
	unroll_base(const unroll_context&, const node_pool_type&, 
		PRS::footprint&, const bool) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:

	// for convenience
	rule_attribute_list_type&
	get_attribute_list(void) { return attributes; }

	const rule_attribute_list_type&
	get_attribute_list(void) const { return attributes; }

	void
	check(void) const;

};	// end class pull_base

//=============================================================================
/**
	Pull-up production rule.  
 */
class pull_up : public pull_base {
	typedef	pull_up			this_type;
public:
	pull_up();

	pull_up(const prs_expr_ptr_type&, const bool_literal&, const bool);

	pull_up(const prs_expr_ptr_type&, const bool_literal&, 
		const rule_attribute_list_type&);
 
	~pull_up();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context&) const;

	ostream&
	dump(ostream& o) const {
		return dump(o, rule_dump_context());
	}

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
class pull_dn : public pull_base {
	typedef	pull_dn			this_type;
public:
	pull_dn();

	pull_dn(const prs_expr_ptr_type&, const bool_literal&, const bool);

	pull_dn(const prs_expr_ptr_type&, const bool_literal&, 
		const rule_attribute_list_type&);

	~pull_dn();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context&) const;

	ostream&
	dump(ostream& o) const {
		return dump(o, rule_dump_context());
	}

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
	Just a set of rules that were wrapped in a nested scope.
	For all practical purposes, these should just be treated as a 
	continuation list of rules, nothing special about them.  
 */
class nested_rules : public rule {
	typedef	nested_rules			this_type;
	typedef	rule_set::value_type		value_type;
private:
	rule_set				rules;
public:
	nested_rules();
	~nested_rules();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context&) const;

	PRS_UNROLL_RULE_PROTO;

	void
	check(void) const;

	excl_ptr<rule>
	expand_complement(void);

	void
	push_back(excl_ptr<rule>&);

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

private:
	void
	collect_transient_info(persistent_object_manager&) const;

protected:
	// these are also used by rule_loop
	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);
};	// end class nested_rules

//=============================================================================
/**
	A set of rules to be repeatedly unrolled in a loop.  
	Could derive privately from nested_rules...
 */
class rule_loop : public nested_rules, private meta_loop_base {
	typedef	rule_loop			this_type;
	typedef	rule_set::value_type		value_type;
private:
	rule_set				rules;
public:
	rule_loop();
	rule_loop(const ind_var_ptr_type&, const range_ptr_type&);
	~rule_loop();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context&) const;

	PRS_UNROLL_RULE_PROTO;

	using nested_rules::check;
	using nested_rules::expand_complement;
	using nested_rules::push_back;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);
};	// end class rule_loop

//=============================================================================
/**
	Class for conditional rule bodies.  
 */
class rule_conditional : public rule, private meta_conditional_base {
	typedef	rule_conditional		this_type;
	typedef	rule_set::value_type		value_type;
private:
	rule_set				if_rules;
	// meta_conditional_base		// no else condition needed
	rule_set				else_rules;
public:
	rule_conditional();

	explicit
	rule_conditional(const guard_ptr_type&);

	~rule_conditional();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context&) const;

	PRS_UNROLL_RULE_PROTO;

	void
	check(void) const;

	excl_ptr<rule>
	expand_complement(void);

	void
	push_back_if_clause(excl_ptr<rule>&);

	void
	import_else_clause(this_type&);

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);
};	// end class rule_conditional

//=============================================================================
/**
	Common elements to expression loop.  
 */
class expr_loop_base : protected meta_loop_base {
	typedef	meta_loop_base			parent_type;
public:
	typedef	parent_type::pint_placeholder_type
						pint_placeholder_type;
	typedef	parent_type::ind_var_ptr_type	ind_var_ptr_type;
	typedef	parent_type::range_ptr_type	range_ptr_type;
protected:
	prs_expr_ptr_type			body_expr;

protected:
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
/**
	A user-defined expansion which could result in a rule
	or some other construct, annotation, attribute, ...
	(not to be confused with AST::parser::PRS::macro)
	TODO: support parameter values.  
 */
class macro : public rule, public directive_source {
	typedef	macro				this_type;
public:
	macro();

	explicit
	macro(const string&);

	~macro();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context&) const;

	excl_ptr<rule>
	expand_complement(void);

	PRS_UNROLL_RULE_PROTO;

	void
	check(void) const;

	PERSISTENT_METHODS_DECLARATIONS
	// CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class macro

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_H__

