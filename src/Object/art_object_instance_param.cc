/**
	\file "art_object_instance_param.cc"
	Method definitions for parameter instance collection classes.
 	$Id: art_object_instance_param.cc,v 1.9.2.2 2005/01/31 04:16:34 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_PARAM_CC__
#define	__ART_OBJECT_INSTANCE_PARAM_CC__

#define	ENABLE_STACKTRACE		0

#include <iostream>

#include "art_object_definition_base.h"
#include "art_object_namespace.h"
#include "art_object_type_ref_base.h"
#include "art_object_instance_param.h"
#include "art_object_inst_ref_base.h"
#include "art_object_inst_stmt_base.h"
#include "art_object_expr_base.h"

#include "indent.h"
#include "stacktrace.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

//=============================================================================
namespace ART {
namespace entity {
#include "using_ostream.h"
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
			indent indenter(o);
			o << auto_indent <<
				"unrolled index-value pairs: {" << endl;
			{
				indent indenter(o);
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
	This should only be applicable to instance_references...
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
count_ptr<member_instance_reference_base>
param_instance_collection::make_member_instance_reference(
		const count_ptr<const simple_instance_reference>& b) const {
	typedef	count_ptr<member_instance_reference_base>	return_type;
	NEVER_NULL(b);
	cerr << "Referencing parameter members is strictly forbidden!" << endl;
	DIE;
	return return_type(NULL);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_PARAM_CC__

