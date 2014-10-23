/**
	\file "Object/lang/PRS.hh"
	Structures for production rules.
	$Id: PRS.hh,v 1.33 2010/07/09 02:14:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_H__
#define	__HAC_OBJECT_LANG_PRS_H__

#include "Object/lang/PRS_base.hh"
#include "Object/lang/PRS_enum.hh"
#include "Object/lang/bool_literal.hh"
#include "Object/lang/generic_attribute.hh"
#include "Object/lang/directive_source.hh"
#include "Object/unroll/meta_loop_base.hh"
#include "Object/unroll/meta_conditional_base.hh"
#include "Object/devel_switches.hh"
#include <string>
#include <vector>
#include "util/memory/chunk_map_pool_fwd.hh"

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
class footprint_expr_node;
using std::string;
//=============================================================================
// forward declarations

/**
	Because PRS only ever deal with bools (nodes).  
 */
typedef	bool_literal_base_ptr_type		literal_base_ptr_type;
typedef	directive_source_params_type		literal_params_type;

//=============================================================================
typedef	generic_attribute_list_type	rule_attribute_list_type;

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
	generic_attribute_list_type		attr;
public:
	literal();

	explicit
	literal(const literal_base_ptr_type&);

	explicit
	literal(const node_literal_ptr_type&);

	literal(const bool_literal&, const params_type&, 
		const generic_attribute_list_type&);

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

	generic_attribute_list_type&
	get_attributes(void) { return attr; }

	const generic_attribute_list_type&
	get_attributes(void) const { return attr; }

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	flip_literals(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;
	PRS_UNROLL_COPY_PROTO;

protected:
	size_t
	unroll_node(const unroll_context&) const;
public:

	// fanout.. not until actually instantiated, unrolled, created...
	PERSISTENT_METHODS_DECLARATIONS

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class literal

//=============================================================================
class pull_base : public rule {
public:
	enum arrow_type {
		ARROW_NORMAL, 		// ->
		ARROW_COMPLEMENT, 	// =>
		ARROW_FLIP		// #>
	};
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
		Uses arrow_type member enumeration.  
	 */
	char				arrow_type;
	/**
		Attribute list.  
		Want to make this a pointer for efficient duplication?
	 */
	rule_attribute_list_type	attributes;

	pull_base();

	pull_base(const prs_expr_ptr_type&, const bool_literal&, const char);

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
	unroll_base(const unroll_context&, 
		const bool) const;

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

	pull_up(const prs_expr_ptr_type&, const bool_literal&, const char);

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

	pull_dn(const prs_expr_ptr_type&, const bool_literal&, const char);

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
class rule_set : public rule, public nested_rules {
	typedef	rule_set			this_type;
public:
	// for netlist generation purposes
	// is bool_literal_base_ptr_type from "Object/lang/bool_literal.h"
	typedef	bool_literal_base_ptr_type	supply_node_ref_type;
	// kind of wastes a little memory for derived classes that
	// don't need overrides... oh well.
	/// optional: GND override
	supply_node_ref_type			GND;	// supply
	/// optional: Vdd override
	supply_node_ref_type			Vdd;	// supply
	// substrate contact override for NFETs
	supply_node_ref_type			GND_substrate;
	// substrate contact override for PFETs
	supply_node_ref_type			Vdd_substrate;

public:
	rule_set();
	~rule_set();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context& = rule_dump_context()) const;

	PRS_UNROLL_RULE_PROTO;
	PRS_CHECK_RULE_PROTO;
	PRS_EXPAND_COMPLEMENT_PROTO;
	using nested_rules::append_rule;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class rule_set

//=============================================================================
/**
	Named, nested substructure for local production rules.
	Mostly needed for convenient netlist generation.
 */
class subcircuit : public rule, public nested_rules {
	typedef	subcircuit		this_type;
	string				name;
public:
	subcircuit();

	explicit
	subcircuit(const string&);
	~subcircuit();

	const string&
	get_name(void) const { return name; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context&) const;

	PRS_UNROLL_RULE_PROTO;
	PRS_CHECK_RULE_PROTO;
	PRS_EXPAND_COMPLEMENT_PROTO;
	using nested_rules::append_rule;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);
};	// end class subcircuit

//=============================================================================
/**
	A set of rules to be repeatedly unrolled in a loop.  
	Could derive privately from nested_rules...
 */
class rule_loop : public rule, public nested_rules, private meta_loop_base {
	typedef	rule_loop			this_type;
	typedef	nested_rules::value_type		value_type;
	typedef	nested_rules			implementation_type;
	friend struct meta_loop<this_type>;
	typedef	meta_loop<this_type>		meta_loop_type;
public:
	rule_loop();
	rule_loop(const ind_var_ptr_type&, const range_ptr_type&);
	~rule_loop();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context&) const;

	PRS_UNROLL_RULE_PROTO;
	PRS_CHECK_RULE_PROTO;
	PRS_EXPAND_COMPLEMENT_PROTO;
	using nested_rules::append_rule;

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
	typedef	nested_rules::value_type		value_type;
	friend struct meta_conditional<this_type>;
	typedef	meta_conditional<this_type>	meta_conditional_type;
private:
	/**
		DO NOT use vector unless size is pre-reserved, 
		because of underlying list of sticky_ptrs.
	 */
	typedef	std::list<nested_rules>	clause_list_type;
	clause_list_type			clauses;
public:
	rule_conditional();

	~rule_conditional();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const rule_dump_context&) const;

	bool
	empty(void) const;

	PRS_UNROLL_RULE_PROTO;

	void
	check(void) const;

	excl_ptr<rule>
	expand_complement(void);

	void
	append_guarded_clause(const guard_ptr_type&);

	nested_rules&
	get_last_clause(void) { return clauses.back(); }

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
	unroll_base(const unroll_context&, const char) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end struct expr_loop_base

//=============================================================================
/**
	Boolean expression used to precharge internal node.
	Pull is indicated by direction.
 */
class precharge_expr {
	count_ptr<const prs_expr>		expr;
	bool					dir;
public:
	// arbitrary default
	precharge_expr() : expr(), dir(false) { }

	precharge_expr(const count_ptr<const prs_expr>& e, const bool d) :
		expr(e), dir(d) { }
	// default copy-ctor
	~precharge_expr();

	operator bool () const { return expr; }

	good_bool
	unroll(const unroll_context&, const size_t, const size_t) const;


//	PRS_UNROLL_EXPR_PROTO;
//	PRS_UNROLL_COPY_PROTO;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class precharge_expr

//=============================================================================
typedef	precharge_expr				precharge_type;
typedef	std::vector<precharge_type>		precharge_array_type;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Logical AND expression.  
	Now requires precharge support...
	Precharging can be sparse, so we have to use a map.
	Q: should we allow optimization transformations in the presence
		of precharge expressions???
 */
class and_expr : public prs_expr, public prs_expr::expr_sequence_type {
	typedef	and_expr			this_type;
	typedef	prs_expr::expr_sequence_type	sequence_type;
private:
	enum { print_stamp = PRS_AND_EXPR_TYPE_ENUM };
	precharge_array_type			precharge_array;
#if 0
private:
	using expr_sequence_type::push_back;	// hide
	// clang is unable to find public definition with this
	using expr_sequence_type::push_front;	// hide
#endif
public:
	typedef	expr_sequence_type::const_reference	const_reference;
	/**
		This ctor *should* be reserved b/c of invariant
		|precharge_array| +1 = |sequence|.
	 */
	and_expr();

	explicit
	and_expr(const_reference);

	~and_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	push_front(const_reference, const precharge_type&);

	void
	push_front(const_reference);

	void
	push_back(const_reference, const precharge_type&);

	void
	push_back(const_reference);

	void
	check(void) const;

	prs_expr_ptr_type
	negate(void) const;

	prs_expr_ptr_type
	flip_literals(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;
	PRS_UNROLL_COPY_PROTO;

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
	flip_literals(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;
	PRS_UNROLL_COPY_PROTO;

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
	flip_literals(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;
	PRS_UNROLL_COPY_PROTO;

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
	flip_literals(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;
	PRS_UNROLL_COPY_PROTO;

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
	flip_literals(void) const;

	prs_expr_ptr_type
	negation_normalize(void);

	PRS_UNROLL_EXPR_PROTO;
	PRS_UNROLL_COPY_PROTO;

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
class macro : public rule, public bool_directive_source {
	typedef	macro				this_type;
	generic_attribute_list_type		attr;
public:
	macro();

	explicit
	macro(const string&);

	~macro();

	generic_attribute_list_type&
	get_attributes(void) { return attr; }

	const generic_attribute_list_type&
	get_attributes(void) const { return attr; }

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

