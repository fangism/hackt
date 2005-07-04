/**
	\file "Object/art_object_expr_const.h"
	Classes related to constant expressions, symbolic and parameters.  
	$Id: art_object_expr_const.h,v 1.16.4.4 2005/07/04 19:13:26 fang Exp $
 */

#ifndef __OBJECT_ART_OBJECT_EXPR_CONST_H__
#define __OBJECT_ART_OBJECT_EXPR_CONST_H__

#include "util/STL/pair_fwd.h"
#include <vector>
#include "Object/art_object_expr_base.h"
#include "Object/art_object_index.h"
#include "util/multikey_fwd.h"
#include "util/packed_array.h"
#include "util/persistent.h"
#include "util/memory/list_vector_pool_fwd.h"

//=============================================================================
namespace ART {
namespace entity {

USING_LIST
USING_CONSTRUCT
using std::vector;
using std::pair;
using std::string;
using std::ostream;
using std::istream;
using util::multikey_generic;
using util::multikey_generator;
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

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	size_t
	dimensions(void) const = 0;

// don't know if these are applicable... 
// depends on whether or not we use this for collective constants...
virtual	bool
	has_static_constant_dimensions(void) const = 0;

// only call this if dimensions are non-zero and sizes are static constant.  
virtual	const_range_list
	static_constant_dimensions(void) const = 0;

	bool
	may_be_initialized(void) const { return true; }

	bool
	must_be_initialized(void) const { return true; }

#if 0
virtual bool
	may_be_equivalent(const param_expr& p) const = 0;

virtual bool
	must_be_equivalent(const param_expr& p) const = 0;
#endif

	bool
	is_static_constant(void) const { return true; }

virtual	count_ptr<const const_param>
	static_constant_param(void) const = 0;

	bool
	is_loop_independent(void) const { return true; }

	bool
	is_unconditional(void) const { return true; }

virtual	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const = 0;
};	// end class const_param

//-----------------------------------------------------------------------------
/**
	List of strictly constant param expressions.  
	Only scalar expressions allowed, no array indirections or collections.  
 */
class const_param_expr_list : public param_expr_list, 
		public vector<count_ptr<const const_param> > {
friend class dynamic_param_expr_list;
	typedef	const_param_expr_list			this_type;
	typedef	param_expr_list				interface_type;
protected:
	typedef	vector<count_ptr<const const_param> >	parent_type;
public:
	typedef parent_type::iterator			iterator;
	typedef parent_type::const_iterator		const_iterator;
	typedef parent_type::reverse_iterator		reverse_iterator;
	typedef parent_type::const_reverse_iterator	const_reverse_iterator;
public:
	const_param_expr_list();

	explicit
	const_param_expr_list(const parent_type::value_type&);
// lazy: use default copy constructor
//	const_param_expr_list(const const_param_expr_list& pl);
	~const_param_expr_list();

	size_t
	size(void) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	excl_ptr<param_expr_list>
	make_copy(void) const;

	count_ptr<const param_expr>
	operator [] (const size_t) const;

	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

	bool
	may_be_equivalent(const param_expr_list& p) const;

	bool
	must_be_equivalent(const param_expr_list& p) const;

	bool
	is_static_constant(void) const { return true; }

	bool
	is_loop_independent(void) const { return true; }

	unroll_resolve_return_type
	unroll_resolve(const unroll_context&) const;

	good_bool
	certify_template_arguments(const template_formals_list_type&);

	good_bool
	must_validate_template_arguments(
		const template_formals_list_type&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class const_param_expr_list

//-----------------------------------------------------------------------------
/**
	Abstract interface for constant indices and index ranges.  
 */
class const_index : virtual public meta_index_expr {
	typedef	meta_index_expr			parent_type;
protected:
	const_index() : parent_type() { }

public:
virtual	~const_index() { }

// same pure virtual functions, and more...

virtual	count_ptr<const_index>
	resolve_index(void) const = 0;

virtual	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const = 0;

virtual	pint_value_type
	lower_bound(void) const = 0;

virtual	pint_value_type
	upper_bound(void) const = 0;

virtual	bool
	operator == (const const_range& c) const = 0;

virtual	bool
	range_size_equivalent(const const_index& i) const = 0;

};	// end class const_index

//-----------------------------------------------------------------------------
/**
	Index list whose indices are all constant.
	This means we need a const_index interface to objects.  
	Because of arbitrary pointer copying,
	members must be reference counted.  
 */
class const_index_list : public meta_index_list, 
		private list<count_ptr<const_index> > {
	typedef	const_index_list		this_type;
public:
	typedef	count_ptr<const_index>		const_index_ptr_type;
	typedef	const_index_ptr_type		value_type;
protected:
	/** need list of pointers b/c const_index is abstract */
	typedef	list<const_index_ptr_type>		parent_type;
public:
	typedef parent_type::iterator			iterator;
	typedef parent_type::const_iterator		const_iterator;
	typedef parent_type::reverse_iterator		reverse_iterator;
	typedef parent_type::const_reverse_iterator	const_reverse_iterator;
public:
	const_index_list();

	const_index_list(const const_index_list& l, 
		const pair<list<pint_value_type>, list<pint_value_type> >& f);

	~const_index_list();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

/** NOT THE SAME **/
	size_t
	size(void) const;

	size_t
	dimensions_collapsed(void) const;

	const_range_list
	collapsed_dimension_ranges(void) const;

	using parent_type::empty;
	using parent_type::clear;
	using parent_type::begin;
	using parent_type::end;
	using parent_type::rbegin;
	using parent_type::rend;

	void
	push_back(const const_index_ptr_type& i);

	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

	bool
	is_static_constant(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;

	const_index_list
	resolve_index_list(void) const;

	const_index_list
	unroll_resolve(const unroll_context&) const;

#if 0
	bool
	resolve_multikey(excl_ptr<multikey_index_type>& k) const;
#endif

	multikey_index_type
	upper_multikey(void) const;

	multikey_index_type
	lower_multikey(void) const;

	bool
	equal_dimensions(const const_index_list& ) const;

	bool
	must_be_equivalent_indices(const meta_index_list& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class const_index_list

//=============================================================================
/**
	List of constant range expressions.  
	Would a vector be more appropriate?   consider changing later...
 */
class const_range_list : public meta_range_list, public list<const_range> {
	typedef	const_range_list			this_type;
protected:
	// no need for pointers here
	typedef	list<const_range>			list_type;
public:
	typedef	list_type::iterator			iterator;
	typedef	list_type::const_iterator		const_iterator;
	typedef	list_type::reverse_iterator		reverse_iterator;
	typedef	list_type::const_reverse_iterator	const_reverse_iterator;
public:
	const_range_list();

	explicit
	const_range_list(const list_type& l);

	explicit
	const_range_list(const const_index_list& i);

	~const_range_list();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	ostream&
	dump_force(ostream& o) const;

	size_t
	size(void) const;

	bool
	is_static_constant(void) const { return true; }

	const_range_list
	static_overlap(const meta_range_list& r) const;

	bool
	is_size_equivalent(const const_range_list& il) const;
	// see also must_be_formal_size_equivalent, declared below

	// may be obsolete by must_be_formal_size_equivalent...
	bool
	operator == (const const_range_list& c) const;

	good_bool
	resolve_ranges(const_range_list& r) const;

	multikey_index_type
	upper_multikey(void) const;

	multikey_index_type
	lower_multikey(void) const;

	template <size_t D>
	void
	make_multikey_generator(
		multikey_generator<D, pint_value_type>& k) const;

	// is a pint_const_collection::array_type::key_type
	multikey_index_type
	resolve_sizes(void) const;

	good_bool
	unroll_resolve(const_range_list&, const unroll_context&) const;

	bool
	must_be_formal_size_equivalent(const meta_range_list& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class const_range_list

//=============================================================================
/**
	Constant integer parameters.  
	Currently limited in width by the machine's long size.  
	Going to need a pool allocator...
 */
class pint_const : public pint_expr, public const_index, public const_param {
private:
	typedef	pint_const		this_type;
public:
	typedef pint_expr::value_type	value_type;
	static const value_type		default_value = 0;
protected:
	// removed constness for assignability
	value_type			val;
private:
	pint_const();
public:
	explicit
	pint_const(const value_type v) :
		pint_expr(), const_index(), const_param(), val(v) { }

	// change back to inline later
	~pint_const();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const
		{ return const_range_list(); }	// empty list

	bool
	may_be_initialized(void) const { return true; }

	bool
	must_be_initialized(void) const { return true; }

#if 0
	bool
	may_be_equivalent(const param_expr& e) const
		{ return pint_expr::may_be_equivalent_generic(e); }

	bool
	must_be_equivalent(const param_expr& e) const
		{ return pint_expr::must_be_equivalent_generic(e); }
#endif

	bool
	is_static_constant(void) const { return true; }

	count_ptr<const const_param>
	static_constant_param(void) const;

	// may chop '_int' off for templating
	value_type
	static_constant_value(void) const { return val; }

	bool
	must_be_equivalent(const pint_expr& ) const;

	bool
	is_loop_independent(void) const { return true; }

	bool
	is_unconditional(void) const { return true; }

	bool
	operator == (const const_range& c) const;

	bool
	range_size_equivalent(const const_index& i) const;

	value_type
	lower_bound(void) const;

	value_type
	upper_bound(void) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	count_ptr<const_index>
	resolve_index(void) const;

	const_index_list
	resolve_dimensions(void) const;

	good_bool
	resolve_values_into_flat_list(list<value_type>& l) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;

private:
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS
};	// end class pint_const

//-----------------------------------------------------------------------------
/**
	Constant boolean parameters, true or false.  
 */
class pbool_const : public pbool_expr, public const_param {
private:
	typedef	pbool_const		this_type;
public:
	typedef	pbool_value_type	value_type;
	static const value_type		default_value = false;
protected:
	// removed const-ness for assignability
	value_type		val;
private:
	pbool_const();

public:
	explicit
	pbool_const(const value_type v) :
		pbool_expr(), const_param(), val(v) { }

	~pbool_const() { }

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const
		{ return const_range_list(); }

	bool
	may_be_initialized(void) const { return true; }

	bool
	must_be_initialized(void) const { return true; }

#if 0
	bool
	may_be_equivalent(const param_expr& e) const
		{ return pbool_expr::may_be_equivalent_generic(e); }

	bool
	must_be_equivalent(const param_expr& e) const
		{ return pbool_expr::must_be_equivalent_generic(e); }
#endif

	bool
	is_static_constant(void) const { return true; }

	count_ptr<const const_param>
	static_constant_param(void) const;

	bool
	static_constant_value(void) const { return val; }

	bool
	must_be_equivalent(const pbool_expr& ) const;

	bool
	is_loop_independent(void) const { return true; }

	bool
	is_unconditional(void) const { return true; }

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	good_bool
	resolve_values_into_flat_list(list<value_type>& l) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

private:
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS
};	// end class pbool_const

//-----------------------------------------------------------------------------
/**
	Constant version of range expression.  
	Deriving from pair to inherit its interface with first and second.  
 */
class const_range : public meta_range_expr, public const_index,
		public pair<pint_value_type, pint_value_type> {
friend class const_range_list;
private:
	typedef	const_range				this_type;
	typedef	pair<pint_value_type,pint_value_type>	parent_type;
	// typedef for interval_type (needs discrete_interval_set)
	// relocated to source file
public:
	// dispense with pint_const objects here
	/**
		Default constructor initializes with invalid range.  
	 */
	const_range() :
		meta_range_expr(), const_index(), parent_type(0, -1) { }

	/** explicit conversion from x[N] to x[0..N-1] */
	explicit
	const_range(const pint_value_type n);

	explicit
	const_range(const pint_const& n);

	explicit
	const_range(const parent_type& p);

	const_range(const pint_value_type l, const pint_value_type u);

#if 0
	const_range(const const_range& r);
#endif

private:
	const_range(const pint_value_type l, const pint_value_type u,
		const bool);
public:
	~const_range() { }

	/** use this to query whether or not range is valid */
	bool
	empty(void) const { return first > second; }

	pint_value_type
	lower(void) const {
		INVARIANT(!empty());
		return first;
	}

	pint_value_type
	upper(void) const {
		INVARIANT(!empty());
		return second;
	}

	/**
		\return The size spanned by this range.
	 */
	size_t
	size(void) const {
		INVARIANT(!empty());
		return second -first +1;
	}

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	ostream&
	dump_force(ostream& o) const;

	const_range
	static_overlap(const const_range& r) const;

	bool
	operator == (const const_range& c) const;

	bool
	may_be_initialized(void) const { return !empty(); }

	bool
	must_be_initialized(void) const { return !empty(); }

	bool
	is_sane(void) const;

	bool
	is_static_constant(void) const { return !empty(); }

	bool
	is_loop_independent(void) const { return !empty(); }

	bool
	is_unconditional(void) const { return !empty(); }

	bool
	range_size_equivalent(const const_index& i) const;

	pint_value_type
	lower_bound(void) const;

	pint_value_type
	upper_bound(void) const;

	good_bool
	resolve_range(const_range& r) const;

	good_bool
	unroll_resolve_range(const unroll_context&, const_range& r) const;

	count_ptr<const_index>
	resolve_index(void) const;

	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;

	bool
	must_be_formal_size_equivalent(const meta_range_expr& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
	// don't need robust declarations, unless dynamically allocating
	// during global static initialization.
};	// end class const_range

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_ART_OBJECT_EXPR_CONST_H__

