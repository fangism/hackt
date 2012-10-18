/**
	\file "Object/ref/meta_value_reference.hh"
	$Id: meta_value_reference.hh,v 1.7 2010/01/03 01:34:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_VALUE_REFERENCE_H__
#define	__HAC_OBJECT_REF_META_VALUE_REFERENCE_H__

#include "util/macros.h"
#include "Object/ref/meta_value_reference_base.hh"
#include "Object/traits/class_traits_fwd.hh"
#include "util/memory/count_ptr.hh"
#include "util/boolean_types.hh"

namespace HAC {
namespace entity {
class const_param;
class unroll_context;
using util::good_bool;
using util::bad_bool;
using util::memory::count_ptr;

//=============================================================================
#define	META_VALUE_REFERENCE_TEMPLATE_SIGNATURE	template <class Tag>
#define	META_VALUE_REFERENCE_CLASS		meta_value_reference<Tag>

/**
	Base class for simple and aggregate (member?)
	parameter value references.  
 */
META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
class meta_value_reference :
		public meta_value_reference_base, 
		public class_traits<Tag>::expr_base_type {
	typedef	META_VALUE_REFERENCE_CLASS	this_type;
	typedef	class_traits<Tag>		traits_type;
public:
	typedef	typename traits_type::expr_base_type
						expr_base_type;
	typedef	expr_base_type			interface_type;
	typedef	typename traits_type::const_collection_type
						const_collection_type;
	typedef	typename traits_type::value_reference_collection_type
						value_reference_collection_type;
	typedef	count_ptr<const interface_type>	init_arg_type;
protected:
	meta_value_reference() :
		meta_value_reference_base(), expr_base_type() { }
public:
virtual	~meta_value_reference();

virtual	ostream&
	what(ostream&) const = 0;

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

virtual	bad_bool
	unroll_lvalue_references(const unroll_context&,
		value_reference_collection_type&) const = 0;

	/**
		Equivalent of bind/mem_fun/compose/dereference...
	 */
	struct unroll_resolver {
		const unroll_context&		_context;

		explicit
		unroll_resolver(const unroll_context& c) : _context(c) { }

		/**
			\param p pointer to this_type (non-NULL).  
		 */
		template <class P>
		count_ptr<const const_param>
		operator () (const P& p) const {
			NEVER_NULL(p);
			return p->unroll_resolve_rvalues(_context, p);
		}
	};	// end struct unroll_resolve_rvaluesr

virtual	void
	collect_transient_info(persistent_object_manager&) const = 0;

};	// end class meta_value_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_REF_META_VALUE_REFERENCE_H__
