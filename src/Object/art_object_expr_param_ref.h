/**
	\file "art_object_expr_param_ref.h"
	Classes related to parameter instance reference expressions. 
	$Id: art_object_expr_param_ref.h,v 1.2 2004/12/10 22:02:16 fang Exp $
 */

#ifndef __ART_OBJECT_EXPR_PARAM_REF_H__
#define __ART_OBJECT_EXPR_PARAM_REF_H__

#include "art_object_expr_const.h"	// for const_index_list
#include "art_object_inst_ref_base.h"	// includes "art_object_base.h"

//=============================================================================
namespace ART {
namespace entity {
USING_LIST
using std::string;
using std::ostream;

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
/**
	A reference to a instance of built-in type pbool.  
	Consider multiply deriving from pbool_expr, 
	and replacing pbool_literal.  
 */
class pbool_instance_reference : public param_instance_reference, 
		public pbool_expr {
private:
	typedef param_instance_reference		parent_type;
	typedef	pbool_expr				interface_type;
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

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

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
private:
	typedef	param_instance_reference		parent_type;
	typedef	pint_expr				interface_type;
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

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class pint_instance_reference

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __ART_OBJECT_EXPR_PARAM_REF_H__

