/**
	\file "art_object_expr_const.h"
	Classes related to constant expressions, symbolic and parameters.  
	$Id: art_object_expr_const.h,v 1.3 2004/12/11 06:22:42 fang Exp $
 */

#ifndef __ART_OBJECT_EXPR_CONST_H__
#define __ART_OBJECT_EXPR_CONST_H__

#include "STL/pair_fwd.h"
#include "art_object_expr_base.h"
#include "multikey_fwd.h"
#include "persistent.h"

//=============================================================================
namespace ART {
namespace entity {

using namespace MULTIKEY_NAMESPACE;
USING_LIST
using std::pair;
using std::string;
using std::ostream;
using util::persistent;
using util::persistent_object_manager;	// forward declared

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
	const_param() : param_expr() { }
virtual	~const_param() { }

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
	PERSISTENT_METHODS
};	// end class const_param_expr_list

//-----------------------------------------------------------------------------
/**
	Abstract interface for constant indices and index ranges.  
 */
class const_index : virtual public index_expr {
protected:
public:
	const_index() : index_expr() { }
virtual	~const_index() { }

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
	PERSISTENT_METHODS
};	// end class const_index_list

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
	PERSISTENT_METHODS
};	// end class const_range_list

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
	PERSISTENT_METHODS
};	// end class pbool_const

//-----------------------------------------------------------------------------
/**
	Constant version of range expression.  
	Deriving from pair to inherit its interface with first and second.  
 */
class const_range : public range_expr, public const_index,
		public pair<int,int> {
friend class const_range_list;
private:
	typedef	pair<int,int>			parent;
	// typedef for interval_type (needs discrete_interval_set)
	// relocated to source file
public:
	// dispense with pint_const objects here
	const_range();
	/** explicit conversion from x[N] to x[0..N-1] */
explicit const_range(const int n);
explicit const_range(const pint_const& n);
explicit const_range(const parent& p);
	const_range(const int l, const int u);
	const_range(const const_range& r);
private:
#if 0
	const_range(const interval_type& i);
#else
	const_range(const int l, const int u, const bool);
#endif
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
	PERSISTENT_METHODS
};	// end class const_range

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __ART_OBJECT_EXPR_CONST_H__

