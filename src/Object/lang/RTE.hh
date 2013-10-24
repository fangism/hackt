/**
	\file "Object/lang/RTE.hh"
	Structures for production rules.
	$Id: RTE.hh,v 1.33 2010/07/09 02:14:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_RTE_HH__
#define	__HAC_OBJECT_LANG_RTE_HH__

#include "Object/lang/RTE_base.hh"
#include "Object/lang/bool_literal.hh"
#include "Object/lang/PRS_enum.hh"
#if 0
#include "Object/lang/RTE_enum.hh"
#include "Object/lang/generic_attribute.hh"
#include "Object/lang/directive_source.hh"
#include "Object/unroll/meta_loop_base.hh"
#include "Object/unroll/meta_conditional_base.hh"
#endif
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

namespace RTE {
class footprint_expr_node;
using std::string;
//=============================================================================
// forward declarations

/**
	Because RTE only ever deal with bools (nodes).  
 */
typedef	bool_literal_base_ptr_type		literal_base_ptr_type;

//=============================================================================
/**
	Literal expression, which can appear on LHS or RHS of any rule.  
	Re: internal nodes: we've decided to add support here instead of
	in bool_literal, because only production rules should ever touch
	internal nodes.  
 */
class literal : public rte_expr, public bool_literal {
	typedef	literal				this_type;
	typedef	bool_literal			base_type;
public:
	struct	unroller {
		const unroll_context& _context;

		explicit
		unroller(const unroll_context& c) : _context(c) { }

		// make sure argument pointer type matches
		// macro::const_reference
		size_t
		operator () (const count_ptr<const literal>& l) const {
			NEVER_NULL(l);
			return l->unroll_node(_context);
		}
	};
private:
	enum { print_stamp = PRS::PRS_LITERAL_TYPE_ENUM };
public:
	literal();

	explicit
	literal(const literal_base_ptr_type&);

#if 0
	explicit
	literal(const node_literal_ptr_type&);
#endif

	explicit
	literal(const bool_literal&);

	// default copy constructor (is copy-constructible)

	~literal();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump(ostream& o) const { return dump(o, expr_dump_context()); }

	void
	check(void) const;

	RTE_UNROLL_EXPR_PROTO;
	RTE_UNROLL_COPY_PROTO;

protected:
	size_t
	unroll_node(const unroll_context&) const;
public:

	// fanout.. not until actually instantiated, unrolled, created...
	PERSISTENT_METHODS_DECLARATIONS

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class literal

//=============================================================================
class assignment : public atomic_assignment {
	typedef	assignment		this_type;
protected:
	/**
		Guard expression.  
		Terrible inherited name, this doesn't guard anything...
	 */
	rte_expr_ptr_type		guard;
	/**
		Output node, expression root.  
	 */
	bool_literal			output;

public:
	assignment();

	assignment(const rte_expr_ptr_type&, const bool_literal&);

	// because we go through an intermediate count_ptr, dtor needs to 
	// be public, thus need to be virtual to be safe, ah well...
	~assignment();

#if 0
	ostream&
	dump_base(ostream&, const assignment_dump_context&, const char) const;

	good_bool
	unroll_base(const unroll_context&, 
		const bool) const;
#else
	ostream&
	dump(ostream&, const assignment_dump_context&) const;

	good_bool
	unroll(const unroll_context&) const;
#endif

	ostream&
	what(ostream&) const;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

public:
	void
	check(void) const;

};	// end class assignment

//=============================================================================
class assignment_set : public atomic_assignment, public nested_assignments {
	typedef	assignment_set			this_type;
public:
	assignment_set();

	~assignment_set();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const assignment_dump_context& = assignment_dump_context()) const;

	RTE_UNROLL_ASSIGN_PROTO;
	RTE_CHECK_ASSIGN_PROTO;
	using nested_assignments::append_assignment;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class assignment_set

//=============================================================================
#if 0
/**
	A set of rules to be repeatedly unrolled in a loop.  
	Could derive privately from nested_assignments...
 */
class assignment_loop : public atomic_assignment, public nested_assignments, private meta_loop_base {
	typedef	assignment_loop			this_type;
	typedef	nested_assignments::value_type		value_type;
	typedef	nested_assignments			implementation_type;
	friend struct meta_loop<this_type>;
	typedef	meta_loop<this_type>		meta_loop_type;
public:
	assignment_loop();
	assignment_loop(const ind_var_ptr_type&, const range_ptr_type&);
	~assignment_loop();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const assignment_dump_context&) const;

	RTE_UNROLL_ASSIGN_PROTO;
	RTE_CHECK_ASSIGN_PROTO;
	using nested_assignments::append_rule;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);
};	// end class assignment_loop

//=============================================================================
/**
	Class for conditional atomic_assignment bodies.  
 */
class assignment_conditional : public atomic_assignment, private meta_conditional_base {
	typedef	assignment_conditional		this_type;
	typedef	nested_assignments::value_type		value_type;
	friend struct meta_conditional<this_type>;
	typedef	meta_conditional<this_type>	meta_conditional_type;
private:
	/**
		DO NOT use vector unless size is pre-reserved, 
		because of underlying list of sticky_ptrs.
	 */
	typedef	std::list<nested_assignments>	clause_list_type;
	clause_list_type			clauses;
public:
	assignment_conditional();

	~assignment_conditional();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const assignment_dump_context&) const;

	bool
	empty(void) const;

	RTE_UNROLL_ASSIGN_PROTO;

	void
	check(void) const;

	excl_ptr<atomic_assignment>
	expand_complement(void);

	void
	append_guarded_clause(const guard_ptr_type&);

	nested_assignments&
	get_last_clause(void) { return clauses.back(); }

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);
};	// end class assignment_conditional
#endif

//=============================================================================
#if 0
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
	rte_expr_ptr_type			body_expr;

protected:
	expr_loop_base();

	expr_loop_base(const ind_var_ptr_type&, const range_ptr_type&);

	expr_loop_base(const ind_var_ptr_type&, const range_ptr_type&,
		const rte_expr_ptr_type&);

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
#endif

//=============================================================================
/**
	Boolean logical AND expression, or OR, or XOR.  
 */
class binop_expr : public rte_expr, public rte_expr::expr_sequence_type {
	typedef	binop_expr			this_type;
	typedef	rte_expr::expr_sequence_type	sequence_type;
private:
	enum { print_stamp = PRS::PRS_AND_EXPR_TYPE_ENUM };
private:
	using expr_sequence_type::push_back;	// hide
	using expr_sequence_type::push_front;	// hide
	char					op;
public:
	typedef	expr_sequence_type::const_reference	const_reference;
	binop_expr();

	binop_expr(const_reference, const char);

	~binop_expr();

	char
	get_op(void) const { return op; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	push_front(const_reference);

	void
	push_back(const_reference);

	void
	check(void) const;

	RTE_UNROLL_EXPR_PROTO;
	RTE_UNROLL_COPY_PROTO;

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class binop_expr

//-----------------------------------------------------------------------------
#if 0
/**
	Logical AND loop.  
 */
class binop_expr_loop : public rte_expr, public expr_loop_base {
	typedef	binop_expr_loop			this_type;
	typedef	rte_expr			parent_type;
private:
	enum { print_stamp = PRS::PRS_AND_EXPR_TYPE_ENUM };
public:
	binop_expr_loop();

	binop_expr_loop(const ind_var_ptr_type&, const range_ptr_type&);

	binop_expr_loop(const ind_var_ptr_type&, const range_ptr_type&, 
		const rte_expr_ptr_type&);

	~binop_expr_loop();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	check(void) const;

	RTE_UNROLL_EXPR_PROTO;
	RTE_UNROLL_COPY_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
	// CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class binop_expr_loop
#endif

//=============================================================================
#if 0
/**
	Logical OR expression.  
 */
class or_expr : public rte_expr, public rte_expr::expr_sequence_type {
	typedef	or_expr				this_type;
	typedef	rte_expr::expr_sequence_type	sequence_type;
private:
	enum { print_stamp = PRS::PRS_OR_EXPR_TYPE_ENUM };
public:
	or_expr();
	~or_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	check(void) const;

	RTE_UNROLL_EXPR_PROTO;
	RTE_UNROLL_COPY_PROTO;

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class or_expr_loop
#endif

//-----------------------------------------------------------------------------
#if 0
/**
	Logical OR loop.  
 */
class or_expr_loop : public rte_expr, public expr_loop_base {
	typedef	or_expr_loop			this_type;
	typedef	rte_expr			parent_type;
private:
	enum { print_stamp = PRS::PRS_OR_EXPR_TYPE_ENUM };
public:
	or_expr_loop();

	or_expr_loop(const ind_var_ptr_type&, const range_ptr_type&);

	or_expr_loop(const ind_var_ptr_type&, const range_ptr_type&, 
		const rte_expr_ptr_type&);

	~or_expr_loop();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	check(void) const;

	RTE_UNROLL_EXPR_PROTO;
	RTE_UNROLL_COPY_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
	// CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class or_expr_loop
#endif

//=============================================================================
/**
	Logical NOT expression.  
	The only unary expression.
 */
class not_expr : public rte_expr {
	typedef	not_expr			this_type;
private:
	rte_expr_ptr_type			var;
private:
	enum { print_stamp = PRS::PRS_NOT_EXPR_TYPE_ENUM };
public:
	not_expr();

	explicit
	not_expr(const rte_expr_ptr_type&);
	~not_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	check(void) const;

	RTE_UNROLL_EXPR_PROTO;
	RTE_UNROLL_COPY_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class not_expr

//=============================================================================
}	// end namespace RTE
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_RTE_HH__

