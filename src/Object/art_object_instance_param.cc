/**
	\file "Object/art_object_instance_param.cc"
	Method definitions for parameter instance collection classes.
 	$Id: art_object_instance_param.cc,v 1.15.2.3 2005/07/05 07:59:46 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_PARAM_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_PARAM_CC__

#define	ENABLE_STACKTRACE		0

#include <iostream>

#include "Object/art_object_definition_base.h"
#include "Object/art_object_namespace.h"
#include "Object/art_object_type_ref_base.h"
#include "Object/art_object_instance_param.h"
#include "Object/art_object_inst_ref_base.h"
#include "Object/art_object_inst_stmt_base.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"

#include "util/indent.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

//=============================================================================
namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::indent;
using util::auto_indent;
using util::disable_indent;
USING_STACKTRACE

//=============================================================================
// class param_instance_collection method definitions

/**
	Private empty constructor.  
 */
param_instance_collection::param_instance_collection(const size_t d) :
		parent_type(d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instance_collection::param_instance_collection(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instance_collection::~param_instance_collection() {
	STACKTRACE("~param_instance_collection()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_instance_collection::dump(ostream& o) const {
#if 0
	STACKTRACE("param_instance_collection::dump()");
#endif
	parent_type::dump(o);
	const count_ptr<const param_expr>
		init_def(default_value());
	if (init_def) {
		if (is_template_formal())
			init_def->dump(o << " (default = ") << ")";
		else	init_def->dump(o << " (init = ") << ")";
	}
	// print out the values of instances that have been unrolled
	if (is_partially_unrolled()) {
		if (dimensions) {
			INDENT_SECTION(o);
			o << auto_indent <<
				"unrolled index-value pairs: {" << endl;
			{
				INDENT_SECTION(o);
				dump_unrolled_values(o);
			}
			o << auto_indent << "}";	// << endl;
		} else {
			disable_indent no_indent(o);
			o << " value: ";
			// suppress indent
			dump_unrolled_values(o);	// already endl
		}
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// OBSOLETE, using parent_type's definition
/**
	To determine whether or not this is a formal parameter, 
	look itself up in the owning namespace.  
 */
bool
param_instance_collection::is_template_formal(void) const {
	STACKTRACE("param_instance_collection::is_template_formal()");
	// look itself up in owner namespace
	const never_ptr<const definition_base>
		def(owner.is_a<const definition_base>());
	if (def) {
		return def->lookup_template_formal(key);
	} else {
		INVARIANT(owner.is_a<const name_space>());
		// is owned by a namespace, i.e. actually instantiated
		return false;
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For multidimensional instances, we don't keep track of initialization
	of individual elements at compile-time, just conservatively 
	return true, that the instance MAY be initialized.  
	Template formals are considered initialized because concrete
	types will always have supplied parameters.  
	The counterpart must_be_initialized will check at unroll time
	whether or not an instance is definitely initialized.  
	\return true if the instance may be initialized.  
	\sa must_be_initialized
 */
bool
param_instance_collection::may_be_initialized(void) const {
	if (dimensions || is_template_formal())
		return true;
	else {
		// is not a template formal, thus we interpret
		// the "default_value" field as a one-time initialization
		// value.  
		const count_ptr<const param_expr>
			ret(default_value());
		if (ret)
			return ret->may_be_initialized();
		// if there's no initial value, then it is definitely
		// NOT already initialized.  
		else return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	At compile time, we don't keep track of arrays, thus
	one cannot conclude that a member of an array is definitely 
	initialized.  
	\sa may_be_initialized
 */
bool
param_instance_collection::must_be_initialized(void) const {
	if (dimensions)
		return false;
	else if (is_template_formal())
		return true;
	else {
		// is not a template formal, thus we interpret
		// the "default_value" field as a one-time initialization
		// value.  
		const count_ptr<const param_expr>
			ret(default_value());
		if (ret)
			return ret->must_be_initialized();
		// if there's no initial value, then it is definitely
		// NOT already initialized.  
		else return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks for dimension and size equality between expression and 
	instantiation.  
	So far, only used by param_instance_collection derivatives, 
		in the context of checking template formals.  
	May be useful else where for connections.  
	\return true if dimensions *may* match.  
 */
good_bool
param_instance_collection::may_check_expression_dimensions(
		const param_expr& pe) const {
	// MUST_BE_A(const param_instance_collection*, this);
	// else is not an expression class!

	// dimensions() used to be a pure virtual method
	// problem when dimensions() is called during construction:
	// error: pure virtual method called (during construction)
	// this occurs during static construction of the global 
	// built in definition object: ind_def, which is templated
	// with int width.  
	// Solutions: 
	// 1) make an unsafe/unchecked constructor for this special case.
	// 2) add the template parameter after contruction is complete, 
	//      which is safe as long as no other global (outside of
	//      art_built_ins.cc) depends on it.
	// we choose 2 because it is a general solution.  

	if (dimensions != pe.dimensions()) {
		// number of dimensions doesn't even match!
		// useful error message?
		return good_bool(false);
	}
	// dimensions match
	if (dimensions != 0) {
		INVARIANT(index_collection.size() == 1);	// huh? true?
		// this is true only if parameters that check this
		// are template formals.  
		// not sure if this will be called by non-formals, will see...

		// make sure sizes in each dimension
		index_collection_type::const_iterator
			i(index_collection.begin());
		const count_ptr<const const_range_list>
			crl((*i)->get_indices().is_a<const const_range_list>());
		if (crl) {
			if (pe.has_static_constant_dimensions()) {
				const const_range_list
					d(pe.static_constant_dimensions());
				return good_bool(*crl == d);
			} else {
				// is dynamic, conservatively return true
				return good_bool(true);
			}
		} else {
			// is dynamic, conservatively return true
			return good_bool(true);
		}
	} else {
		// dimensions == 0 means instantiation is a single instance.  
		// size may be zero b/c first statement hasn't been added yet
		INVARIANT(index_collection.size() <= 1);
		return good_bool(pe.dimensions() == 0);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks for dimension and size equality between expression and 
	instantiation.  
	So far, only used by param_instance_collection derivatives, 
		in the context of checking template formals.  
	May be useful else where for connections.  
	\return true if dimensions *may* match.  
 */
good_bool
param_instance_collection::must_check_expression_dimensions(
		const const_param& pe) const {
	// MUST_BE_A(const param_instance_collection*, this);
	// else is not an expression class!

	// dimensions() used to be a pure virtual method
	// problem when dimensions() is called during construction:
	// error: pure virtual method called (during construction)
	// this occurs during static construction of the global 
	// built in definition object: ind_def, which is templated
	// with int width.  
	// Solutions: 
	// 1) make an unsafe/unchecked constructor for this special case.
	// 2) add the template parameter after contruction is complete, 
	//      which is safe as long as no other global (outside of
	//      art_built_ins.cc) depends on it.
	// we choose 2 because it is a general solution.  

	if (dimensions != pe.dimensions()) {
		// number of dimensions doesn't even match!
		// useful error message?
		return good_bool(false);
	}
	// dimensions match
	if (dimensions != 0) {
		INVARIANT(index_collection.size() == 1);	// huh? true?
		// this is true only if parameters that check this
		// are template formals.  
		// not sure if this will be called by non-formals, will see...

		// make sure sizes in each dimension
		index_collection_type::const_iterator
			i(index_collection.begin());
		const count_ptr<const const_range_list>
			crl((*i)->get_indices().is_a<const const_range_list>());
		if (crl) {
			INVARIANT(pe.has_static_constant_dimensions());
			const const_range_list
				d(pe.static_constant_dimensions());
			return good_bool(*crl == d);
		} else {
			// is dynamic, conservatively return false
			return good_bool(false);
		}
	} else {
		// dimensions == 0 means instantiation is a single instance.  
		// size may be zero b/c first statement hasn't been added yet
		INVARIANT(index_collection.size() <= 1);
		return good_bool(pe.dimensions() == 0);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_instance_collection::is_static_constant(void) const {
	if (dimensions) {
		// conservatively return... depends on may or must...
		return false;
	} else if (is_template_formal()) {
		return false;
	} else {
		const count_ptr<const param_expr>
			ret(default_value());
		if (ret)
			return ret->is_static_constant();
		else return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Note: only one flavor needed (hopefully).  
	One should be able to statically determine whether or not
	something is loop-dependent.  
	Wait, does it even make sense for an "instantiation"?
	This should only be applicable to meta_instance_references...
	put this on hold...
 */
bool
param_instance_collection::is_loop_independent(void) const {
	
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	1) Parameters cannot be in public ports.  
	2) Thus they cannot even be referenced.  
	3) This is just a placeholder that should never be called.  
 */
param_instance_collection::member_inst_ref_ptr_type
param_instance_collection::make_member_meta_instance_reference(
		const inst_ref_ptr_type& b) const {
	typedef	member_inst_ref_ptr_type	return_type;
	NEVER_NULL(b);
	cerr << "Referencing parameter members is strictly forbidden!" << endl;
	DIE;
	return return_type(NULL);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_PARAM_CC__

