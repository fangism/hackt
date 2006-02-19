/**
	\file "Object/ref/meta_reference_union.h"
	$Id: meta_reference_union.h,v 1.1.4.2 2006/02/19 03:53:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_REFERENCE_UNION_H__
#define	__HAC_OBJECT_REF_META_REFERENCE_UNION_H__

#include "Object/ref/meta_instance_reference_base.h"
#include "Object/ref/meta_value_reference_base.h"
#include "Object/expr/param_expr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using util::memory::count_ptr;

//=============================================================================
/**
	This class exists, solely to facilitate reference lookups 
	in the type-check phase.  
	Even though value- and instance- references are now 
	in distinct class hierarchies.  
	Not a true union, contains two pointers, should be mutually exclusive.  
 */
class meta_reference_union {
public:
 	typedef	count_ptr<meta_value_reference_base>	value_ref_ptr_type;
 	typedef	count_ptr<meta_instance_reference_base>	inst_ref_ptr_type;
	typedef	count_ptr<param_expr>			expr_ptr_type;
private:
	// these two pointers are mutually exclusive
 	value_ref_ptr_type				_value_ref;
 	inst_ref_ptr_type				_inst_ref;

public:
	meta_reference_union() : _value_ref(NULL), _inst_ref(NULL) { }

	explicit
	meta_reference_union(const void*) :
		_value_ref(NULL), _inst_ref(NULL) { }

	// implicit
	meta_reference_union(const value_ref_ptr_type& v) :
		_value_ref(v), _inst_ref(NULL) { }

	// implicit
	meta_reference_union(const inst_ref_ptr_type& i) :
		_value_ref(NULL), _inst_ref(i) { }

	// implicit
	meta_reference_union(const expr_ptr_type& e) :
		_value_ref(e.is_a<meta_value_reference_base>()),
		_inst_ref(NULL) { }

	
	operator bool () const {
		return _value_ref || _inst_ref;
	}

	value_ref_ptr_type&
	value_ref(void) { return _value_ref; }

	const value_ref_ptr_type&
	value_ref(void) const { return _value_ref; }

	inst_ref_ptr_type&
	inst_ref(void) { return _inst_ref; }

	const inst_ref_ptr_type&
	inst_ref(void) const { return _inst_ref; }

	struct inst_ref_selector {
		inst_ref_ptr_type
		operator () (const meta_reference_union& u) const {
			return u._inst_ref;
		}
	};	// end struct inst_ref_selector

	struct value_ref_selector {
		value_ref_ptr_type
		operator () (const meta_reference_union& u) const {
			return u._value_ref;
		}
	};	// end struct inst_ref_selector

};	// end class meta_reference_union

//=============================================================================
/**
	Read-only pointer version.  
 */
class const_meta_reference_union {
public:
 	typedef	count_ptr<const meta_value_reference_base>
							value_ref_ptr_type;
 	typedef	count_ptr<const meta_instance_reference_base>
							inst_ref_ptr_type;
	typedef	count_ptr<const param_expr>		expr_ptr_type;
private:
	// these two pointers are mutually exclusive
 	value_ref_ptr_type				_value_ref;
 	inst_ref_ptr_type				_inst_ref;

public:
	const_meta_reference_union() : _value_ref(), _inst_ref() { }

	explicit
	const_meta_reference_union(const void*) :
		_value_ref(NULL), _inst_ref(NULL) { }

	// implicit
	const_meta_reference_union(const value_ref_ptr_type& v) :
		_value_ref(v), _inst_ref(NULL) { }

	// implicit
	const_meta_reference_union(const inst_ref_ptr_type& i) :
		_value_ref(NULL), _inst_ref(i) { }

	// implicit
	const_meta_reference_union(const expr_ptr_type& e) :
		_value_ref(e.is_a<const meta_value_reference_base>()),
		_inst_ref(NULL) { }

	const_meta_reference_union(const meta_reference_union& u) :
		_value_ref(u.value_ref()), _inst_ref(u.inst_ref()) { }

	
	operator bool () const {
		return _value_ref || _inst_ref;
	}

	value_ref_ptr_type&
	value_ref(void) { return _value_ref; }

	const value_ref_ptr_type&
	value_ref(void) const { return _value_ref; }

	inst_ref_ptr_type&
	inst_ref(void) { return _inst_ref; }

	const inst_ref_ptr_type&
	inst_ref(void) const { return _inst_ref; }

	struct inst_ref_selector {
		inst_ref_ptr_type
		operator () (const const_meta_reference_union& u) const {
			return u._inst_ref;
		}
	};	// end struct inst_ref_selector

	struct value_ref_selector {
		value_ref_ptr_type
		operator () (const const_meta_reference_union& u) const {
			return u._value_ref;
		}
	};	// end struct inst_ref_selector

};	// end class const_meta_reference_union

//=============================================================================
}	// namespace entity
}	// namespace HAC

#endif	// __HAC_OBJECT_REF_META_REFERENCE_UNION_H__

