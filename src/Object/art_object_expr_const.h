/**
	\file "art_object_expr_const.h"
	Classes related to constant expressions, symbolic and parameters.  
	$Id: art_object_expr_const.h,v 1.1 2004/12/06 07:11:19 fang Exp $
 */

#ifndef __ART_OBJECT_EXPR_CONST_H__
#define __ART_OBJECT_EXPR_CONST_H__

#include "art_object_expr_base.h"
#include "discrete_interval_set_fwd.h"
#include "multikey_fwd.h"

//=============================================================================
// note: need some way of hashing expression? 
//	using string of fully-qualified name?

/***
	Instead of making two flavors of queries, use an integer return
	value instead of a bool to enumerate:
		definitely no, don't know (maybe), definitely yes.  
		do we need may be yes, may be no?
	May be bit fields?
***/

//=============================================================================
namespace ART {
namespace entity {
	using namespace MULTIKEY_NAMESPACE;
	using namespace	DISCRETE_INTERVAL_SET_NAMESPACE;
	USING_LIST
	using std::string;
	using std::ostream;

//=============================================================================
/**
	Silly as it may seem...
	Interface for pure constants.  
	Used for unroll time, when everything must be bound.  
	Need this to distinguish between dynamic and const param_expr_lists
 */
class const_param : virtual public param_expr {
protected:
	// no members
public:
	const_param();
virtual	~const_param();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual	string hash_string(void) const = 0;
virtual	size_t dimensions(void) const = 0;

// don't know if these are applicable... 
// depends on whether or not we use this for collective constants...
virtual	bool has_static_constant_dimensions(void) const = 0;
// only call this if dimensions are non-zero and sizes are static constant.  
virtual	const_range_list static_constant_dimensions(void) const = 0;

	bool may_be_initialized(void) const { return true; }
	bool must_be_initialized(void) const { return true; }
virtual bool may_be_equivalent(const param_expr& p) const = 0;
virtual bool must_be_equivalent(const param_expr& p) const = 0;

	bool is_static_constant(void) const { return true; }
virtual	count_ptr<const const_param>
		static_constant_param(void) const = 0;

	bool is_loop_independent(void) const { return true; }
	bool is_unconditional(void) const { return true; }
};	// end class const_param

//-----------------------------------------------------------------------------
/**
	List of strictly constant param expressions.  
	Only scalar expressions allowed, no array indirections or collections.  
 */
class const_param_expr_list : public param_expr_list, 
		public list<count_ptr<const const_param> > {
friend class dynamic_param_expr_list;
protected:
	typedef	list<count_ptr<const const_param> >	parent;
public:
	typedef parent::iterator		iterator;
	typedef parent::const_iterator		const_iterator;
	typedef parent::reverse_iterator	reverse_iterator;
	typedef parent::const_reverse_iterator	const_reverse_iterator;
public:
	const_param_expr_list();
// lazy: use default copy constructor
//	const_param_expr_list(const const_param_expr_list& pl);
	~const_param_expr_list();

	size_t size(void) const;

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;

	excl_ptr<param_expr_list>
		make_copy(void) const;

	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;

#if 0
	list<const param_expr&>	get_const_ref_list(void) const;
#else
	bool may_be_equivalent(const param_expr_list& p) const;
	bool must_be_equivalent(const param_expr_list& p) const;
#endif

	bool is_static_constant(void) const { return true; }
	bool is_loop_independent(void) const { return true; }
#if 0
private:
	bool may_be_equivalent_const(const const_param_expr_list& p) const;
	bool may_be_equivalent_dynamic(const dynamic_param_expr_list& p) const;
	bool must_be_equivalent_const(const const_param_expr_list& p) const;
	bool must_be_equivalent_dynamic(const dynamic_param_expr_list& p) const;
#endif
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class const_param_expr_list

//-----------------------------------------------------------------------------
#if 0
/**
	Generalized list of parameter expressions, can be dynamic.  
 */
class dynamic_param_expr_list : public param_expr_list, 
		public list<count_ptr<const param_expr> > {
friend class const_param_expr_list;
protected:
	typedef	list<count_ptr<const param_expr> >	parent;
public:
	typedef parent::iterator		iterator;
	typedef parent::const_iterator		const_iterator;
	typedef parent::reverse_iterator	reverse_iterator;
	typedef parent::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_param_expr_list();
// lazy: use default copy constructor
//	dynamic_param_expr_list(const dynamic_param_expr_list& pl);
	~dynamic_param_expr_list();

	size_t size(void) const;

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;

	excl_ptr<param_expr_list>
		make_copy(void) const;

	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;

#if 0
	list<const param_expr&>	get_const_ref_list(void) const;
#else
	bool may_be_equivalent(const param_expr_list& p) const;
	bool must_be_equivalent(const param_expr_list& p) const;
#endif

	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
#if 0
private:
	bool may_be_equivalent_const(const const_param_expr_list& p) const;
	bool may_be_equivalent_dynamic(const dynamic_param_expr_list& p) const;
	bool must_be_equivalent_const(const const_param_expr_list& p) const;
	bool must_be_equivalent_dynamic(const dynamic_param_expr_list& p) const;
#endif
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class dynamic_param_expr_list
#endif

//-----------------------------------------------------------------------------
/**
	Abstract interface for constant indices and index ranges.  
 */
class const_index : virtual public index_expr {
protected:
public:
	const_index();
virtual	~const_index();

// same pure virtual functions, and more...

virtual	count_ptr<const_index> resolve_index(void) const = 0;
virtual	int lower_bound(void) const = 0;
virtual	int upper_bound(void) const = 0;
virtual	bool operator == (const const_range& c) const = 0;
virtual	bool range_size_equivalent(const const_index& i) const = 0;
};	// end class const_index

//-----------------------------------------------------------------------------
/**
	Index list whose indices are all constant.
	This means we need a const_index interface to objects.  
	Because of arbitrary pointer copying,
	members must be reference counted.  
 */
class const_index_list : public index_list, 
		private list<count_ptr<const_index> > {
protected:
	/** need list of pointers b/c const_index is abstract */
	typedef	list<count_ptr<const_index> >	parent;
public:
	typedef parent::iterator		iterator;
	typedef parent::const_iterator		const_iterator;
	typedef parent::reverse_iterator	reverse_iterator;
	typedef parent::const_reverse_iterator	const_reverse_iterator;
public:
	const_index_list();
	const_index_list(const const_index_list& l, 
		const pair<list<int>, list<int> >& f);
	~const_index_list();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;

/** NOT THE SAME **/
	size_t size(void) const;
	size_t dimensions_collapsed(void) const;

	const_range_list collapsed_dimension_ranges(void) const;

	using parent::empty;
	using parent::clear;
	using parent::begin;
	using parent::end;
	using parent::rbegin;
	using parent::rend;
	void push_back(const count_ptr<const_index>& i);

	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;

	const_index_list resolve_index_list(void) const;
#if 0
	bool resolve_multikey(excl_ptr<multikey_base<int> >& k) const;
#endif
	excl_ptr<multikey_base<int> > upper_multikey(void) const;
	excl_ptr<multikey_base<int> > lower_multikey(void) const;

	bool equal_dimensions(const const_index_list& ) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class const_index_list

//-----------------------------------------------------------------------------
#if 0
/**
	Elements of this index list are no necessarily static constants.  
 */
class dynamic_index_list : public index_list, 
		private list<count_ptr<index_expr> > {
protected:
	typedef	list<count_ptr<index_expr> >	parent;
public:
	typedef parent::iterator		iterator;
	typedef parent::const_iterator		const_iterator;
	typedef parent::reverse_iterator	reverse_iterator;
	typedef parent::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_index_list();
	~dynamic_index_list();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;

	using parent::begin;
	using parent::end;
	using parent::rbegin;
	using parent::rend;
	void push_back(const count_ptr<index_expr>& i);

/** NOT THE SAME **/
	size_t size(void) const;
	size_t dimensions_collapsed(void) const;

	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;

	const_index_list resolve_index_list(void) const;
#if 0
	bool resolve_multikey(excl_ptr<multikey_base<int> >& k) const;
#endif
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class dynamic_index_list
#endif

//=============================================================================
/**
	List of constant range expressions.  
	Would a vector be more appropriate?   consider changing later...
 */
class const_range_list : public range_expr_list, public list<const_range> {
protected:
	// no need for pointers here
	typedef	list<const_range>	list_type;
public:
	typedef	list_type::iterator			iterator;
	typedef	list_type::const_iterator		const_iterator;
	typedef	list_type::reverse_iterator		reverse_iterator;
	typedef	list_type::const_reverse_iterator	const_reverse_iterator;
public:
#if 0
	// maybe don't need?
	/**
		Utility class for generating keys within a multidimensional
		range.  
	 */
	template <size_t D>
	class multikey_generator {
	protected:
		const_range_list	range_list;
	public:
		multikey_generator(const const_range_list& rl);
		// default destructor

	};	// end class multikey_generator
#endif
public:
	const_range_list();
	const_range_list(const list_type& l);
explicit const_range_list(const const_index_list& i);
	~const_range_list();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	size_t size(void) const;
	bool is_static_constant(void) const { return true; }
	const_range_list static_overlap(const range_expr_list& r) const;

	bool is_size_equivalent(const const_range_list& il) const;
	bool operator == (const const_range_list& c) const;

	bool resolve_ranges(const_range_list& r) const;
	excl_ptr<multikey_base<int> > upper_multikey(void) const;
	excl_ptr<multikey_base<int> > lower_multikey(void) const;

	template <size_t D>
	void make_multikey_generator(multikey_generator<D, int>& k) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class const_range_list

//-----------------------------------------------------------------------------
#if 0
/**
	Base class.  
	List of range expressions, not necessarily constant.  
	May contain constant ranges however.  
	Interface methods for unrolling.  

	Replace the dynamic subclasses of instance_collection_stack_item
	with this class by adding attributes...
		is_unconditional
		is_loop_independent
	also cache these results...?
 */
class dynamic_range_list : public range_expr_list,
		public list<count_ptr<pint_range> > {
protected:
	// list of pointers to pint_ranges?  or just copy construct?
	// can't copy construct, is abstract
	typedef	list<count_ptr<pint_range> >	list_type;
public:
	typedef	list_type::iterator			iterator;
	typedef	list_type::const_iterator		const_iterator;
	typedef	list_type::reverse_iterator		reverse_iterator;
	typedef	list_type::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_range_list();
virtual	~dynamic_range_list();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	size_t size(void) const;
	bool is_static_constant(void) const;
	const_range_list static_overlap(const range_expr_list& r) const;
		// false, will be empty
	bool resolve_ranges(const_range_list& r) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class dynamic_range_list
#endif

//=============================================================================
/**
	NOT SURE THIS CLASS IS USEFUL... eventually...
	For arrays of expressions.
	Statically type-checked.  
	Should sub-type...
	Consider multidimensional_qmap for unroll time.
class param_expr_collective : public param_expr {
protected:
	list<excl_ptr<param_expr> >	elist;
public:
	param_expr_collective();
	~param_expr_collective();

	ostream& what(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const;
	bool may_be_equivalent(const param_expr& p) const;
	bool must_be_equivalent(const param_expr& p) const;
virtual	bool has_static_constant_dimensions(void) const = 0;
virtual	const_range_list static_constant_dimensions(void) const = 0;
};	// end class param_expr_collective
**/

//=============================================================================
#if 0
/**
	A reference to a instance of built-in type pbool.  
	Consider multiply deriving from pbool_expr, 
	and replacing pbool_literal.  
 */
class pbool_instance_reference : public param_instance_reference, 
		public pbool_expr {
protected:
	never_ptr<pbool_instance_collection>		pbool_inst_ref;
private:
	pbool_instance_reference();
public:
	pbool_instance_reference(never_ptr<pbool_instance_collection> pi, 
		excl_ptr<index_list> i);
	~pbool_instance_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	using param_instance_reference::dump;
	never_ptr<const instance_collection_base> get_inst_base(void) const;
	never_ptr<const param_instance_collection>
		get_param_inst_base(void) const;

	size_t dimensions(void) const;
	bool has_static_constant_dimensions(void) const;
	const_range_list static_constant_dimensions(void) const;

	bool initialize(count_ptr<const pbool_expr> i);
	string hash_string(void) const;
	// try these
	// using param_instance_reference::may_be_initialized;
	// using param_instance_reference::must_be_initialized;
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_unconditional(void) const;
	bool is_loop_independent(void) const;
	bool static_constant_bool(void) const;

	bool resolve_value(bool& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<bool>& l) const;
public:
	/**
		Helper class for assigning values to instances.
	 */
	class assigner {
	protected:
		/** reference to the source of values */
		const pbool_expr&	src;
		/** resolved range list */
		const_index_list	ranges;
		/** flat list of unrolled values */
		list<bool>		vals;
	public:
		assigner(const pbool_expr& p);
		// default destructor

		bool
		operator () (const bool b,
			const pbool_instance_reference& p) const;

		template <template <class> class P>
		bool
		operator () (const bool b,
			const P<const pbool_instance_reference>& p) const {
			assert(p);
			return this->operator()(b, *p);
		}
	};	// end class assigner

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class pbool_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a instance of built-in type pint.  
	Consider multiply deriving from pint_expr, 
	and replacing pint_literal.  
 */
class pint_instance_reference : public param_instance_reference, 
		public pint_expr {
protected:
	/**
		Back-reference to integer collection.  
		Non-const because it may be modifiable via assignment.  
	 */
	never_ptr<pint_instance_collection>		pint_inst_ref;
private:
	pint_instance_reference();
public:
	pint_instance_reference(never_ptr<pint_instance_collection> pi, 
		excl_ptr<index_list> i);
	~pint_instance_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_ptr<const instance_collection_base> get_inst_base(void) const;
	never_ptr<const param_instance_collection>
		get_param_inst_base(void) const;

	size_t dimensions(void) const;
	bool has_static_constant_dimensions(void) const;
	const_range_list static_constant_dimensions(void) const;

	bool initialize(count_ptr<const pint_expr> i);
	string hash_string(void) const;
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_unconditional(void) const;
	bool is_loop_independent(void) const;
	int static_constant_int(void) const;

	bool resolve_value(int& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<int>& l) const;

protected:
//	bool assign(const list<int>& l) const;

public:
	/**
		Helper class for assigning values to instances.
	 */
	class assigner {
	protected:
		/** reference to the source of values */
		const pint_expr&	src;
		/** resolved range list */
		const_index_list	ranges;
		/** flat list of unrolled values */
		list<int>		vals;
	public:
		assigner(const pint_expr& p);
		// default destructor

		bool
		operator () (const bool b,
			const pint_instance_reference& p) const;

		template <template <class> class P>
		bool
		operator () (const bool b,
			const P<const pint_instance_reference>& p) const {
			assert(p);
			return this->operator()(b, *p);
		}
	};	// end class assigner

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class pint_instance_reference
#endif

//=============================================================================
/**
	Constant integer parameters.  
	Currently limited in width by the machine's long size.  
 */
class pint_const : public pint_expr, public const_index, public const_param {
public:
	typedef long			value_type;
protected:
	const value_type		val;
public:
	pint_const(const long v) :
		pint_expr(), const_index(), const_param(), val(v) { }
	~pint_const() { }
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }	// empty list

	bool may_be_initialized(void) const { return true; }
	bool must_be_initialized(void) const { return true; }
	bool may_be_equivalent(const param_expr& e) const
		{ return pint_expr::may_be_equivalent(e); }
	bool must_be_equivalent(const param_expr& e) const
		{ return pint_expr::must_be_equivalent(e); }
	bool is_static_constant(void) const { return true; }
	count_ptr<const const_param>
		static_constant_param(void) const;
	int static_constant_int(void) const { return val; }
	bool is_loop_independent(void) const { return true; }
	bool is_unconditional(void) const { return true; }
	bool operator == (const const_range& c) const;
	bool range_size_equivalent(const const_index& i) const;

	int lower_bound(void) const;
	int upper_bound(void) const;
	bool resolve_value(int& i) const;
	count_ptr<const_index> resolve_index(void) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<int>& l) const;

private:
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class pint_const

//-----------------------------------------------------------------------------
/**
	Constant boolean parameters, true or false.  
 */
class pbool_const : public pbool_expr, public const_param {
public:
	typedef	bool			value_type;
protected:
	const value_type		val;
public:
	pbool_const(const bool v) :
		pbool_expr(), const_param(), val(v) { }
	~pbool_const() { }
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }

	bool may_be_initialized(void) const { return true; }
	bool must_be_initialized(void) const { return true; }
	bool may_be_equivalent(const param_expr& e) const
		{ return pbool_expr::may_be_equivalent(e); }
	bool must_be_equivalent(const param_expr& e) const
		{ return pbool_expr::must_be_equivalent(e); }
	bool is_static_constant(void) const { return true; }
	count_ptr<const const_param>
		static_constant_param(void) const;
	bool static_constant_bool(void) const { return val; }
	bool is_loop_independent(void) const { return true; }
	bool is_unconditional(void) const { return true; }

	bool resolve_value(bool& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<bool>& l) const;

private:
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class pbool_const

//-----------------------------------------------------------------------------
#if 0
/**
	Only possibilities, unary negation, bit-wise negation.  
 */
class pint_unary_expr : public pint_expr {
public:
	typedef	char			op_type;
protected:
	const op_type			op;
	/** expression argument must be 0-dimensional */
	count_ptr<const pint_expr>	ex;
private:
	pint_unary_expr();
public:
	pint_unary_expr(const op_type o, count_ptr<const pint_expr> e);
	pint_unary_expr(count_ptr<const pint_expr> e, const op_type o);

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }
	bool may_be_initialized(void) const
		{ return ex->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return ex->must_be_initialized(); }
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	int static_constant_int(void) const;

	bool resolve_value(int& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<int>& l) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class pint_unary_expr

//-----------------------------------------------------------------------------
/**
	Only possibility, NOT-expr.  
	Character may be '~' or '!'.  
 */
class pbool_unary_expr : public pbool_expr {
public:
	typedef	char			op_type;
protected:
	const op_type			op;
	/** argument expression must be 0-dimensional */
	count_ptr<const pbool_expr>	ex;
private:
	pbool_unary_expr();
public:
	pbool_unary_expr(const op_type o, count_ptr<const pbool_expr> e);
	pbool_unary_expr(count_ptr<const pbool_expr> e, const op_type o);

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }
	bool may_be_initialized(void) const
		{ return ex->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return ex->must_be_initialized(); }
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	bool static_constant_bool(void) const;

	bool resolve_value(bool& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<bool>& l) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class pbool_unary_expr

//-----------------------------------------------------------------------------
/**
	Binary arithmetic expression accepts ints and returns an int.  
 */
class arith_expr : public pint_expr {
public:
	typedef	binary_arithmetic_operation<int,int>	op_type;
	static const plus<int,int>		adder;
	static const minus<int,int>		subtractor;
	static const multiplies<int,int>	multiplier;
	static const divides<int,int>		divider;
	static const modulus<int,int>		remainder;

private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	qmap<char, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, char>	reverse_op_map_type;
	static const op_map_type		op_map;
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const char, const op_type* );
	static size_t op_map_init(void);
protected:
	count_ptr<const pint_expr>	lx;
	count_ptr<const pint_expr>	rx;
#if 0
	const char			op;
#else
	/**
		Safe to use a naked pointer, b/c/ refers to a static object.  
	 */
	const op_type*			op;
#endif
private:
	arith_expr();
public:
	// change: const ptr& arguments
	arith_expr(count_ptr<const pint_expr> l, const char o, 
		count_ptr<const pint_expr> r);
	~arith_expr();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }
	bool may_be_initialized(void) const
		{ return lx->may_be_initialized() && 
			rx->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return lx->must_be_initialized() && 
			rx->must_be_initialized(); }
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	int static_constant_int(void) const;
	bool resolve_value(int& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<int>& l) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class arith_expr

//-----------------------------------------------------------------------------
/**
	Binary relational expression accepts ints and returns a bool.  
 */
class relational_expr : public pbool_expr {
public:
	typedef	binary_relational_operation<bool,int>	op_type;
	static const equal_to<bool,int>		op_equal_to;
	static const not_equal_to<bool,int>	op_not_equal_to;
	static const less<bool,int>		op_less;
	static const greater<bool,int>		op_greater;
	static const less_equal<bool,int>	op_less_equal;
	static const greater_equal<bool,int>	op_greater_equal;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	qmap<string, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, string>	reverse_op_map_type;
	static const op_map_type		op_map;
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const string&, const op_type* );
	static size_t op_map_init(void);

protected:
	count_ptr<const pint_expr>	lx;
	count_ptr<const pint_expr>	rx;
#if 0
	const string			op;
#else
	const op_type*			op;
#endif

private:
	relational_expr();
public:
	relational_expr(count_ptr<const pint_expr> l, const string& o, 
		count_ptr<const pint_expr> r);
	~relational_expr();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }
	bool may_be_initialized(void) const
		{ return lx->may_be_initialized() && 
			rx->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return lx->must_be_initialized() && 
			rx->must_be_initialized(); }
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	bool static_constant_bool(void) const;

	bool resolve_value(bool& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<bool>& l) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class relational_expr

//-----------------------------------------------------------------------------
/**
	Binary logical expression accepts bools and returns a bool.  
 */
class logical_expr : public pbool_expr {
public:
	typedef	binary_logical_operation<bool,bool>	op_type;
	static const util::logical_and<bool,bool>	op_and;
	static const util::logical_or<bool,bool>	op_or;
	static const util::logical_xor<bool,bool>	op_xor;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	qmap<string, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, string>	reverse_op_map_type;
	static const op_map_type		op_map;
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const string&, const op_type* );
	static size_t op_map_init(void);

protected:
	count_ptr<const pbool_expr>	lx;
	count_ptr<const pbool_expr>	rx;
#if 0
	const string			op;
#else
	const op_type*			op;
#endif

private:
	logical_expr();
public:
	logical_expr(count_ptr<const pbool_expr> l, const string& o, 
		count_ptr<const pbool_expr> r);
	~logical_expr();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }
	bool may_be_initialized(void) const
		{ return lx->may_be_initialized() && 
			rx->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return lx->must_be_initialized() && 
			rx->must_be_initialized(); }
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	bool static_constant_bool(void) const;

	bool resolve_value(bool& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<bool>& l) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class logical_expr

//=============================================================================
/**
	Range expression object, not necessarily constant.  
	Must contain pint_expr's.
	Derive from object or param_expr?
 */
class pint_range : public range_expr {
protected:
	// need to be const, or modifiable?
	count_ptr<const pint_expr>	lower;
	count_ptr<const pint_expr>	upper;
private:
	pint_range();
public:
	/** implicit conversion from x[N] to x[0..N-1] */
explicit pint_range(count_ptr<const pint_expr> n);
	pint_range(count_ptr<const pint_expr> l,
		count_ptr<const pint_expr> u);
	pint_range(const pint_range& pr);
	~pint_range();

	// first, second? pair interface?

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;		// unused?

	bool may_be_initialized(void) const {
		return lower->may_be_initialized() &&
			upper->may_be_initialized();
	}
	bool must_be_initialized(void) const {
		return lower->must_be_initialized() &&
			upper->must_be_initialized();
	}
	bool is_sane(void) const;
	bool is_static_constant(void) const;
	// for now just return false, don't bother checking recursively...
	bool is_loop_independent(void) const { return false; }
	bool is_unconditional(void) const { return false; }
	const_range static_constant_range(void) const;

	bool resolve_range(const_range& r) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class pint_range
#endif

//-----------------------------------------------------------------------------
/**
	Constant version of range expression.  
	Deriving from pair to inherit its interface with first and second.  
 */
class const_range : public range_expr, public const_index,
		public pair<int,int> {
friend class const_range_list;
protected:
	typedef	pair<int,int>			parent;
	/** implementation type for range-checking */
	typedef	discrete_interval_set<int>	interval_type;
public:
	// dispense with pint_const objects here
	const_range();
	/** explicit conversion from x[N] to x[0..N-1] */
explicit const_range(const int n);
explicit const_range(const pint_const& n);
explicit const_range(const parent p);
	const_range(const int l, const int u);
	const_range(const const_range& r);
protected:
	const_range(const interval_type& i);
public:
	~const_range() { }

	/** use this to query whether or not range is valid */
	bool empty(void) const { return first > second; }
	int lower(void) const {
		assert(!empty());
		return first;
	}
	int upper(void) const {
		assert(!empty());
		return second;
	}
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;

	const_range static_overlap(const const_range& r) const;

	bool operator == (const const_range& c) const;
	bool may_be_initialized(void) const { return !empty(); }
	bool must_be_initialized(void) const { return !empty(); }
	bool is_sane(void) const;
	bool is_static_constant(void) const { return !empty(); }
	bool is_loop_independent(void) const { return !empty(); }
	bool is_unconditional(void) const { return !empty(); }
	bool range_size_equivalent(const const_index& i) const;

	int lower_bound(void) const;
	int upper_bound(void) const;
	bool resolve_range(const_range& r) const;
	count_ptr<const_index> resolve_index(void) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class const_range

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __ART_OBJECT_EXPR_CONST_H__

