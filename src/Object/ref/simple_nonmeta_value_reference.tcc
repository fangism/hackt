/**
	\file "Object/ref/simple_nonmeta_value_reference.cc"
	Class method definitions for semantic expression.  
	This file was reincarnated from 
		"Object/art_object_nonmeta_value_reference.cc"
 	$Id: simple_nonmeta_value_reference.tcc,v 1.9.16.6 2006/06/23 21:08:31 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_SIMPLE_NONMETA_VALUE_REFERENCE_TCC__

// flags for controlling conditional compilation, mostly for debugging
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE				0
#endif

#ifndef	STACKTRACE_DESTRUCTORS
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#endif

#include <iostream>
#include <exception>
#include <algorithm>

#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/traits/class_traits.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/type/data_type_reference.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_collection.h"
#include "Object/common/dump_flags.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/nonmeta_index_list.h"

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
struct nonmeta_unroll_resolve_copy_policy<Tag, datatype_tag> {
	typedef	simple_nonmeta_value_reference<Tag>	reference_type;
	typedef	typename reference_type::index_list_type
							index_list_type;
	typedef	count_ptr<const typename reference_type::data_expr_base_type>
							return_type;

static
return_type
unroll_resolve_copy(const reference_type& _this, const unroll_context& c,
		const return_type& p) {
	typedef	reference_type				this_type;
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
			return count_ptr<this_type>(NULL);
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

} __VISIBILITY_HIDDEN__ ;	// end struct nonmeta_unroll_resolve_copy_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/**
	Specialization for nonmeta types that have a corresponding
	meta type, like bool and int.  
 */
template <class Tag>
struct nonmeta_unroll_resolve_copy_policy<Tag, parameter_value_tag> {
	typedef	simple_nonmeta_value_reference<Tag>	reference_type;
	typedef	typename reference_type::index_list_type
							index_list_type;
	typedef	count_ptr<const typename reference_type::data_expr_base_type>
							return_type;
	typedef typename reference_type::traits_type	traits_type;
	typedef	typename traits_type::template value_array<0>::type
							value_scalar_type;
	typedef	typename traits_type::data_value_type	data_value_type;
	typedef	typename traits_type::const_expr_type	const_expr_type;
	typedef	typename reference_type::value_collection_type
							value_collection_type;

/**
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
		data_value_type _val;
		if (vc.lookup_value(_val, k, c).good) {
			return return_type(new const_expr_type(_val));
		} else {
			// already have error message
			return return_type(NULL);
		}
	} else {
		// there is some nonmeta value in index expr
		return ret;
	}
}

static
return_type
unroll_resolve_copy(const reference_type& _this, const unroll_context& c,
		const return_type& p) {
	typedef	reference_type				this_type;
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
		const never_ptr<const value_scalar_type>
			ps(_this.value_collection_ref.
				template is_a<const value_scalar_type>());
		NEVER_NULL(ps);
		data_value_type _val;
		const bad_bool valid(ps->lookup_value(_val, c));
		if (valid.bad) {
                        cerr << "ERROR: in unroll_resolve-ing "
                                "simple_meta_value_reference, "
                                "uninitialized value." << endl;
                        return error;
		} else {
			return return_type(new const_expr_type(_val));
		}
	}
}	// end method unroll_resolve_copy
} __VISIBILITY_HIDDEN__ ;	// end struct nonmeta_unroll_resolve_copy_policy

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
	if (c.enclosing_scope) {
		this->value_collection_ref->dump_hierarchical_name(o,
			dump_flags::no_definition_owner);
	} else {
		this->value_collection_ref->dump_hierarchical_name(o,
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
SIMPLE_NONMETA_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const data_type_reference>
SIMPLE_NONMETA_VALUE_REFERENCE_CLASS::get_data_type_ref(void) const {
	return data_type_resolver<Tag>()(*this);
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

