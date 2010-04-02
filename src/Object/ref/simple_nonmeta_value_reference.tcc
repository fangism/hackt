/**
	\file "Object/ref/simple_nonmeta_value_reference.tcc"
	Class method definitions for semantic expression.  
	This file was reincarnated from 
		"Object/art_object_nonmeta_value_reference.cc"
 	$Id: simple_nonmeta_value_reference.tcc,v 1.29 2010/04/02 22:18:48 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_TCC__

// flags for controlling conditional compilation, mostly for debugging
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE				0
#endif

#ifndef	STACKTRACE_DESTRUCTORS
#define	STACKTRACE_DESTRUCTORS				(0 && ENABLE_STACKTRACE)
#endif

#include <iostream>
#include <exception>
#include <algorithm>

#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/traits/class_traits.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/type/data_type_reference.h"
#include "Object/inst/param_value_placeholder.h"
#include "Object/inst/instance_placeholder.h"
#include "Object/inst/value_placeholder.h"
#include "Object/unroll/unroll_context.h"
#include "Object/type/canonical_generic_datatype.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_scalar.h"
#include "Object/common/dump_flags.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/expr_visitor.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/dynamic_meta_index_list.h"
#include "Object/traits/classification_tags.h"
#include "Object/global_entry.h"
#include "Object/ref/nonmeta_ref_implementation.tcc"
#include "Object/nonmeta_context.tcc"
#include "Object/nonmeta_variable.h"
#include "Object/nonmeta_state.h"
#include "Object/nonmeta_channel_manipulator.h"

#include "common/ICE.h"

#include "util/what.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.h"
#include "util/memory/count_ptr.tcc"
#include "util/multikey.h"

//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
#include "util/using_ostream.h"
using util::persistent_traits;
using std::transform;

//=============================================================================
// defined by specializations only, 
// in "Object/ref/instance_reference_datatype.cc"
template <class>
struct nonmeta_reference_type_check_policy;

//=============================================================================
/**
	Specialization for nonmeta types that lack a corresponding
	meta type, like enum and struct.  
 */
template <class Tag>
struct __VISIBILITY_HIDDEN__ nonmeta_unroll_resolve_copy_policy<Tag, datatype_tag> {
	typedef	simple_nonmeta_value_reference<Tag>	reference_type;
	typedef	typename reference_type::index_list_type
							index_list_type;
	typedef	typename reference_type::const_expr_type
							const_expr_type;
	typedef	typename reference_type::data_expr_base_type
							data_expr_base_type;
	typedef	typename reference_type::assign_update_arg_type
							assign_update_arg_type;
	typedef	count_ptr<const typename reference_type::data_expr_base_type>
							return_type;
	typedef	count_ptr<const typename reference_type::const_expr_type>
							const_return_type;

static
return_type
unroll_resolve_copy(const reference_type& _this, const unroll_context& c,
		const return_type& p) {
	typedef	reference_type				this_type;
	STACKTRACE_VERBOSE;
	if (_this.array_indices) {
		// resolve the indices
		// if indices are all meta-valued, then resolve all the way
		// otherwise there is at least one nonmeta dependence, 
		// which prevents further compile-time resolution.  
		count_ptr<index_list_type>
			resolved_indices(_this.array_indices
				->unroll_resolve_copy(c));
		if (!resolved_indices) {
			cerr << "Error resolving nonmeta value reference\'s "
				"indices." << endl;
			return return_type(NULL);
		}
		if (std::equal(resolved_indices->begin(),
				resolved_indices->end(),
				_this.array_indices->begin())) {
			STACKTRACE_INDENT_PRINT("new indices match" << endl);
			// then resolution changed nothing, return this-copy
			return p;
		} else {
			STACKTRACE_INDENT_PRINT("new indices mismatch" << endl);
			excl_ptr<index_list_type>
				ri(resolved_indices.exclusive_release());
			count_ptr<this_type>
				ret(new this_type(_this.value_collection_ref));
			ret->attach_indices(ri);
			INVARIANT(!ri);		// transferred ownership
			return ret;
		}
	} else {
		STACKTRACE_INDENT_PRINT("scalar" << endl);
		// is scalar reference (cannot be implicit indices!)
		// just return self-copy
		return p;
	}
}	// end method unroll_resolve_copy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses the values held in the run-time instance/value pools to
	resolve values.  (bool, int, enum)
	TODO: factor out same code in __nonmeta_instance_global_lookup_impl
 */
static
const_return_type
nonmeta_resolve_rvalue(const reference_type& _this,
		const nonmeta_context_base& c, const return_type&) {
	typedef	reference_type				this_type;
	STACKTRACE_VERBOSE;
	const size_t global_index =
		c.lookup_nonmeta_reference_global_index(_this);
	if (!global_index) {
		cerr << "Run-time error resolving nonmeta reference." << endl;
		return const_return_type(NULL);
	}
	// the following statement split up to tolerate a g++-3.4.0 bug
	const typename nonmeta_state_base<Tag>::pool_type&
		vp(c.values.template get_pool<Tag>());
	return const_return_type(new const_expr_type(vp[global_index].value));
}	// end function nonmeta_resolve_value

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns rvalue to lvalue in the run-time, nonmeta context.  
	TODO: support aggregate expression assignments
 */
static
void
nonmeta_assign(const reference_type& lref, 
//		const count_ptr<const data_expr_base_type>& rval, 
		const count_ptr<const const_expr_type>& rv, 
		const nonmeta_context_base& c, 
		assign_update_arg_type& u) {
	STACKTRACE_VERBOSE;
#if 0
	NEVER_NULL(rval);	// assert dynamic_cast
	const count_ptr<const const_expr_type>
		rv(rval->__nonmeta_resolve_rvalue(c, rval));
	if (!rv) {
		// TODO: more verbose message
		cerr << "Run-time error resolving rvalue." << endl;
		THROW_EXIT;
	}
#else
	NEVER_NULL(rv);
#endif
	const size_t global_ind = lref.lookup_nonmeta_global_index(c);
	if (!global_ind) {
		cerr << "Run-time error resolving nonmeta lvalue reference."
			<< endl;
		THROW_EXIT;
	}
	// the following statement split up to tolerate a g++-3.4.0 bug
	typename nonmeta_state_base<Tag>::pool_type&
		vp(c.values.template get_pool<Tag>());
	vp[global_ind].value = rv->static_constant_value();
	u.push_back(std::make_pair(
		size_t(class_traits<Tag>::type_tag_enum_value), global_ind));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns to lvalue referenced, using the channel_data_reader.  
	TODO: support aggregate expression assignments
	TODO: support NULL lvalues (drop field)
 */
static
void
direct_assign(const reference_type& lref, 
		const nonmeta_context_base& c, 
		assign_update_arg_type& u, 
		channel_data_reader& r) {
	STACKTRACE_VERBOSE;
	const size_t global_ind = lref.lookup_nonmeta_global_index(c);
	if (!global_ind) {
		cerr << "Run-time error resolving nonmeta lvalue reference."
			<< endl;
		THROW_EXIT;
	}
	// post-increment read-iterator
	// the following statement split up to tolerate a g++-3.4.0 bug
	typename nonmeta_state_base<Tag>::pool_type&
		vp(c.values.template get_pool<Tag>());
	vp[global_ind].value = *r.template iter_ref<Tag>()++;
	u.push_back(std::make_pair(
		size_t(class_traits<Tag>::type_tag_enum_value), global_ind));
}

};	// end struct nonmeta_unroll_resolve_copy_policy

//-----------------------------------------------------------------------------
/**
	Specialization for nonmeta types that have a corresponding
	meta type, like bool and int.  
 */
template <class Tag>
struct __VISIBILITY_HIDDEN__ nonmeta_unroll_resolve_copy_policy<Tag, parameter_value_tag> {
	typedef	simple_nonmeta_value_reference<Tag>	reference_type;
	typedef	typename reference_type::index_list_type
							index_list_type;
	typedef	typename reference_type::data_expr_base_type
							data_expr_base_type;
	typedef	typename reference_type::assign_update_arg_type
							assign_update_arg_type;
	typedef	count_ptr<const typename reference_type::data_expr_base_type>
							return_type;
	typedef	count_ptr<const typename reference_type::const_expr_type>
							const_return_type;
	typedef typename reference_type::traits_type	traits_type;
	typedef	typename traits_type::template value_array<0>::type
							value_scalar_type;
	typedef	typename traits_type::data_value_type	data_value_type;
	typedef	typename traits_type::const_expr_type	const_expr_type;
	/**
		This is now a placeholder type.
	 */
	typedef	typename reference_type::value_collection_type
							value_collection_type;
	// for lack of better name...
	typedef	typename traits_type::value_collection_generic_type
						value_array_generic_type;

/**
	When indices are resolved to constants. 
 */
static
const_return_type
__lookup_const_resolved_value(const value_collection_type& vc, 
		const unroll_context& c, const multikey_index_type& k) {
	data_value_type _val;
	const never_ptr<const param_value_collection>
		pvc(c.lookup_rvalue_collection(vc));
	if (!pvc) {
		return const_return_type(NULL);
	}
	const never_ptr<const value_array_generic_type>
		va(pvc.template is_a<const value_array_generic_type>());
	if (!va) {
		return const_return_type(NULL);
	}
	if (va->lookup_value(_val, k).good) {
		return const_return_type(new const_expr_type(_val));
	} else {
		return const_return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	See comment of value_array::lookup_value for new usage.  
	\param i resolved indices.  
 */
static
return_type
__lookup_unroll_resolved_value(const value_collection_type& vc, 
		const unroll_context& c, const index_list_type& i, 
		const return_type& ret) {
	STACKTRACE_VERBOSE;
	multikey_index_type k(i.size());	// pre-size
	if (i.make_const_index_list(k).good) {
		return __lookup_const_resolved_value(vc, c, k);
	} else {
		// there is some nonmeta value in index expr
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: validate correctness.
 */
static
const_return_type
__lookup_unroll_resolved_value(const value_collection_type& vc, 
		const unroll_context& c, const const_index_list& i) {
	STACKTRACE_VERBOSE;
	const multikey_index_type u(i.upper_multikey());
	const multikey_index_type l(i.lower_multikey());
	INVARIANT(u == l);		// no nonmeta ranges!
	return __lookup_const_resolved_value(vc, c, u);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This may be obsolete with the use of placeholders.  
	Well, is it?
 */
static
return_type
unroll_resolve_copy(const reference_type& _this, const unroll_context& c,
		const return_type& p) {
	typedef	reference_type				this_type;
	STACKTRACE_VERBOSE;
	static const count_ptr<this_type> error(NULL);
	if (_this.array_indices) {
		// resolve the indices
		// if indices are all meta-valued, then resolve all the way
		// otherwise there is at least one nonmeta dependence, 
		// which prevents further compile-time resolution.  
		count_ptr<index_list_type>
			resolved_indices(_this.array_indices
				->unroll_resolve_copy(c));
		if (!resolved_indices) {
			cerr << "Error resolving nonmeta value reference\'s "
				"indices." << endl;
			return error;
		}
		if (std::equal(resolved_indices->begin(),
				resolved_indices->end(),
				_this.array_indices->begin())) {
			STACKTRACE_INDENT_PRINT("new indices match" << endl);
			// then resolution changed nothing, return this-copy
			// check for complete index resolution first
			return __lookup_unroll_resolved_value(
				*_this.value_collection_ref, c, 
				*resolved_indices, p);
		} else {
			STACKTRACE_INDENT_PRINT("new indices mismatch" << endl);
			excl_ptr<index_list_type>
				ri(resolved_indices.exclusive_release());
			count_ptr<this_type>
				ret(new this_type(_this.value_collection_ref));
			ret->attach_indices(ri);
			INVARIANT(!ri);		// transferred ownership
			return __lookup_unroll_resolved_value(
				*_this.value_collection_ref, c, 
				*ret->array_indices, ret);
		}
	} else {
		STACKTRACE_INDENT_PRINT("scalar" << endl);
		// is scalar reference (cannot be implicit indices!)
		// therefore, just look this up
		// code ripped from:
		// simple_meta_value_reference::unroll_resolve_rvalues
		const never_ptr<const param_value_collection>
			pvc(c.lookup_rvalue_collection(
				*_this.value_collection_ref));
		if (!pvc) {
			return error;
		}
		data_value_type _val;
		const never_ptr<const value_scalar_type>
			va(pvc.template is_a<const value_scalar_type>());
		if (va && va->lookup_value(_val).good) {
			return return_type(new const_expr_type(_val));
		} else {
                        cerr << "ERROR: in unroll_resolve-ing "
                                "simple_meta_value_reference, "
                                "uninitialized value." << endl;
                        return error;
		}
	}
}	// end method unroll_resolve_copy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: recycle as much code as possible, this is too much copying.
 */
static
const_return_type
nonmeta_resolve_rvalue(const reference_type& _this,
		const nonmeta_context_base& c, const return_type&) {
	typedef	reference_type				this_type;
	STACKTRACE_VERBOSE;
	const const_return_type error(NULL);
	const unroll_context
		uc((c.fpf ? c.fpf->_footprint : c.topfp), c.topfp);
	if (_this.array_indices) {
		// resolve the indices using run-time values
		const count_ptr<const const_index_list>
			resolved_indices(_this.array_indices
				->nonmeta_resolve_copy(c));
		if (!resolved_indices) {
			cerr << "Error resolving nonmeta value reference\'s "
				"indices." << endl;
			return error;
		}
		// then resolution changed nothing, return this-copy
		// check for complete index resolution first
		return __lookup_unroll_resolved_value(
			*_this.value_collection_ref, uc, 
			*resolved_indices);
	} else {
		STACKTRACE_INDENT_PRINT("scalar" << endl);
		// is scalar reference (cannot be implicit indices!)
		// therefore, just look this up
		// code ripped from:
		// simple_meta_value_reference::unroll_resolve_rvalues
		const never_ptr<const param_value_collection>
			pvc(uc.lookup_rvalue_collection(
				*_this.value_collection_ref));
		if (!pvc) {
			return error;
		}
		data_value_type _val;
		const never_ptr<const value_scalar_type>
			va(pvc.template is_a<const value_scalar_type>());
		if (va && va->lookup_value(_val).good) {
			return const_return_type(new const_expr_type(_val));
		} else {
                        cerr << "ERROR: in unroll_resolve-ing "
                                "simple_meta_value_reference, "
                                "uninitialized value." << endl;
                        return error;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Meta-parameter references can never be nonmeta-lvalues.  
 */
static
void
nonmeta_assign(const reference_type&, 
//		const count_ptr<const data_expr_base_type>&, 
		const count_ptr<const const_expr_type>&, 
		const nonmeta_context_base&, 
		const assign_update_arg_type&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Meta-parameter references can never be nonmeta-lvalues.  
 */
static
void
direct_assign(const reference_type&, 
		const nonmeta_context_base&, 
		assign_update_arg_type&, 
		channel_data_reader&) {
	ICE_NEVER_CALL(cerr);
}

};	// end struct nonmeta_unroll_resolve_copy_policy

//=============================================================================
// class simple_nonmeta_value_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::simple_nonmeta_value_reference() :
		parent_type(),
		common_base_type(), 
		interface_type(),
		value_collection_ref(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::simple_nonmeta_value_reference(
		const value_collection_ptr_type pi) :
		parent_type(), 
		common_base_type(), 
		interface_type(), 
		value_collection_ref(pi) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::~simple_nonmeta_value_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::value_collection_ptr_type
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::get_inst_base_subtype(void) const {
	return value_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::dump(ostream& o,
		const expr_dump_context& c) const {
	if (c.include_type_info) {
		this->what(o) << " ";
	}
	NEVER_NULL(this->value_collection_ref);
	if (c.parent_instance_name.size()) {
		o << c.parent_instance_name << ".";
		this->value_collection_ref->dump_qualified_name(o,
			dump_flags::no_definition_owner);
	} else if (c.enclosing_scope) {
		this->value_collection_ref->dump_qualified_name(o,
			dump_flags::no_definition_owner);
	} else {
		this->value_collection_ref->dump_qualified_name(o,
			dump_flags::default_value);
	}
	return simple_nonmeta_instance_reference_base::dump_indices(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::dimensions(void) const {
	size_t dim = this->value_collection_ref->get_dimensions();
	if (this->array_indices) {
		const size_t c = this->array_indices->dimensions_collapsed();
		INVARIANT(c <= dim);
		return dim -c;
	} else  return dim;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::is_lvalue(void) const {
	return traits_type::is_nonmeta_data_lvalue;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	2006-09-03: re-written to lookup the corresponding footprint
	instance, using the unrolli_context.  
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const data_type_reference>
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::get_unresolved_data_type_ref(void) const {
	return data_type_resolver<Tag>()(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
canonical_generic_datatype
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::get_resolved_data_type_ref(
		const unroll_context& c) const {
	return data_type_resolver<Tag>()(*this, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Tentative: no nonscalar references allowed in the nonmeta language.
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::attach_indices(
		excl_ptr<index_list_type>& i) {
	INVARIANT(!this->array_indices);
	NEVER_NULL(i);

	// dimension-check:
	// number of indices must be <= dimension of instance collection.  
	const size_t max_dim =
		this->value_collection_ref->get_dimensions();
	if (i->size() != max_dim) {
		cerr << "ERROR: instance collection " <<
			this->value_collection_ref->get_name()
			<< " is " << max_dim << "-dimensional, and thus, "
			"cannot be indexed " << i->size() <<
			"-dimensionally!  ";
			// caller will say where
		return good_bool(false);
	}
	// else proceed...

	// allow under-specified dimensions?  
	// NOT for nonmeta instance references, or ALL or NONE
	// TODO: enforce this, modifying the above check

	// TODO: limited static range checking?
	// only if indices are ALL meta values

	this->array_indices = i;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	TODO: reserve for later, in graph construction.  
	If both this and argument are instance references, 
	we consider them equivalent if they reference the same position
	parameter in the template formals list.  
	This allows us to correctly compare the equivalence of 
	template signatures whose member depend on template parameters.  
	\return true if boolean instance references are equivalent.  
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::must_be_equivalent(
		const data_expr_base_type& b) const {
	const this_type* const br = IS_A(const this_type*, &b);
	if (br) {
		// compare template formal parameter positions for equivalence!
		// INVARIANT (2005-01-30): if they are both template formals, 
		// then they refer to equivalent owners.  
		// This will not be true if the language allows nested 
		// templates, so beware in the distant future!

		// check owner pointer equivalence? not pointer equality!
		// same qualified name, namespace path...
		const size_t lpos =
			value_collection_ref->is_template_formal();
		const size_t rpos =
			br->value_collection_ref->is_template_formal();
		if (lpos && rpos && (lpos == rpos)) {
			if (this->array_indices && br->array_indices) {
				return this->array_indices->
					must_be_equivalent_indices(
						*br->array_indices);
			} else {
				return true;
			}
		} else {
			return false;
		}
	} else {
		// conservatively
		return false;
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	See if indicies were changed as a result of meta-parameter resolution.  
	If not, can just return this as a reference-counted copy!
	If alll dependent expressions are meta-values, then resolve them
	and return the resolved constant value, which must be scalar
	(because this is nonmeta).  
	Needs to be implemented using a traits_policy because
	enum and structs compile-time values are not to be resolved yet, 
	until support for these typed constants is added to the 
	language intrinsics.  
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const typename SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::data_expr_base_type>
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::unroll_resolve_copy(
		const unroll_context& c, 
		const count_ptr<const data_expr_base_type>& p) const {
	typedef	count_ptr<const data_expr_base_type>	return_type;
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	return nonmeta_unroll_resolve_copy_policy<Tag,
			typename Tag::parent_tag>::
				unroll_resolve_copy(*this, c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const typename SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::const_expr_type>
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::__nonmeta_resolve_rvalue(
		const nonmeta_context_base& c, 
		const count_ptr<const data_expr_base_type>& p) const {
	typedef	count_ptr<const const_expr_type>	return_type;
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	return nonmeta_unroll_resolve_copy_policy<Tag,
			typename Tag::parent_tag>::
				nonmeta_resolve_rvalue(*this, c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now only allow resolution of scalar values.  
	Later support array-aggregates.  
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const const_param>
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::nonmeta_resolve_copy(
		const nonmeta_context_base& c, 
		const count_ptr<const data_expr_base_type>& p) const {
	return __nonmeta_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	rvalue resolution moved here to avoid wrongly asserting type
	because dynamic function calls are late-type-bound.
	Currently only supports scalar rvalues.  
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::nonmeta_assign(
		const count_ptr<const data_expr>& rval, 
		const nonmeta_context_base& c,
		assign_update_arg_type& u) const {
	const count_ptr<const const_param>
		rv(rval->nonmeta_resolve_copy(c, rval));
	if (!rv) {
		// TODO: more verbose message
		cerr << "Run-time error resolving rvalue." << endl;
		THROW_EXIT;
	}
	const count_ptr<const const_expr_type>
		crv(rv.template is_a<const const_expr_type>());
	if (!crv) {
		cerr << "Run-time error: rvalue and lvalue types mismatch!"
			<< endl;
		// TODO: more verbose message
		THROW_EXIT;
	}
	nonmeta_unroll_resolve_copy_policy<Tag, typename Tag::parent_tag>::
		nonmeta_assign(*this, 
//			rval.template is_a<const data_expr_base_type>(),
			crv, 
			c, u);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::direct_assign(
		const nonmeta_context_base& c, 
		assign_update_arg_type& u, 
		channel_data_reader& r) const {
	STACKTRACE_VERBOSE;
	nonmeta_unroll_resolve_copy_policy<Tag, typename Tag::parent_tag>::
		direct_assign(*this, c, u, r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For physical types (int, bool), this collects sets of aliases, 
	for meta-parameter types, this does nothing.  
	NOTE: it is caller's reponsibility to visit indices.  
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::lookup_may_reference_global_indices(
		const global_entry_context& c, 
		vector<size_t>& indices) const {
	return __nonmeta_instance_lookup_may_reference_indices_impl(
		*this, c, indices, Tag());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up up a global run-time scalar reference. 
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::lookup_nonmeta_global_index(
		const nonmeta_context_base& c) const {
	return __nonmeta_instance_global_lookup_impl(*this, c, Tag());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.
	\param m the persistent object manager.
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {  
	collect_transient_info_base(m);
	value_collection_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}
// else already visited
}
		
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.
	\param m the persistent object manager.
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void    
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, value_collection_ref);
	write_object_base(m, f);
}
	
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** 
	Loads the instance reference from an input stream, translating
	indices to pointers.
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.
	\param m the persistent object manager.
 */
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, value_collection_ref);
	NEVER_NULL(value_collection_ref);
	m.load_object_once(
		const_cast<value_collection_type*>(&*value_collection_ref));
	load_object_base(m, f);
}

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_TCC__

