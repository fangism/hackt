/**
	\file "art_object_template_formals_manager.cc"
	Template formals manager implementation.
	$Id: art_object_template_formals_manager.cc,v 1.1.2.1 2005/04/30 21:27:26 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "hash_specializations.h"	// include as early as possible

#include "art_object_template_formals_manager.h"
#include "art_object_instance_param.h"
#include "art_object_expr.h"

#include "hash_qmap.tcc"
#include "persistent_object_manager.tcc"
#include "IO_utils.tcc"
#include "indent.h"
#include "stacktrace.h"

namespace ART {
namespace entity {
// using std::_Select2nd;
#include "using_ostream.h"
// using parser::scope;
using util::indent;
using util::auto_indent;
USING_STACKTRACE
// using namespace ADS;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;
// using util::persistent_traits;

//=============================================================================
// class template_formals_manager method definitions

template_formals_manager::template_formals_manager() :
		template_formals_map(), 
		strict_template_formals_list(), 
		relaxed_template_formals_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template_formals_manager::~template_formals_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
template_formals_manager::dump(ostream& o) const {
	STACKTRACE("template_formals_manager::dump_template_formals()");
	// sanity check
	INVARIANT(strict_template_formals_list.size()
		+relaxed_template_formals_list.size()
		== template_formals_map.size());
	if (!strict_template_formals_list.empty()) {
		indent tfl_ind(o);
		o << '<' << endl;	// continued from last print
		template_formals_list_type::const_iterator
			i = strict_template_formals_list.begin();
		const template_formals_list_type::const_iterator
			e = strict_template_formals_list.end();
		for ( ; i!=e; i++) {
			// sanity check
			NEVER_NULL(*i);
			INVARIANT((*i)->is_template_formal());
			(*i)->dump(o << auto_indent) << endl;
		}
		o << auto_indent << '>' << endl;
	}
	if (!relaxed_template_formals_list.empty()) {
		// need to be able to read that this is the second param list
		if (strict_template_formals_list.empty()) {
			o << "<>" << endl;	// continued from last print
		}
		indent tfl_ind(o);
		o << auto_indent << '<' << endl;
		template_formals_list_type::const_iterator
			i = relaxed_template_formals_list.begin();
		const template_formals_list_type::const_iterator
			e = relaxed_template_formals_list.end();
		for ( ; i!=e; i++) {
			// sanity check
			NEVER_NULL(*i);
			INVARIANT((*i)->is_template_formal());
			(*i)->dump(o << auto_indent) << endl;
		}
		o << auto_indent << '>' << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
UNVEIL LATER
/**
	Creates a map from template formals to actual values.  
	Precondition: list passed by reference must be initially empty.
	Considering making appropriate virtual call interface in 
		param_expr_list?
 */
void
definition_base::fill_template_actuals_map(
		template_actuals_map_type& am, 
		const param_expr_list& al) const {
	INVARIANT(am.empty());
	INVARIANT(template_formals_list.size() == al.size());
	// convert to virtual call interface to param_expr_list?
	const const_param_expr_list* cpl =
		IS_A(const const_param_expr_list*, &al);
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &al);
	template_formals_list_type::const_iterator f_iter =
		template_formals_list.begin();
if (cpl) {
	const_param_expr_list::const_iterator i = cpl->begin();
	for ( ; f_iter!=template_formals_list.end(); f_iter++, i++) {
		// const-reference saves unnecessary copying
		const template_formals_value_type& tf(*f_iter);
		// reminder: value type is pointer to param_instance_collection
		NEVER_NULL(tf);
		// reminder: actuals map is of count_ptr
		NEVER_NULL(*i);
		am[tf->get_name()] = *i;
	}
} else {
	NEVER_NULL(dpl);
	dynamic_param_expr_list::const_iterator i = dpl->begin();
	for ( ; f_iter!=template_formals_list.end(); f_iter++, i++) {
		// const-reference saves unnecessary copying
		const template_formals_value_type& tf(*f_iter);
		// reminder: value type is pointer to param_instance_collection
		NEVER_NULL(tf);
		// reminder: actuals map is of count_ptr
		NEVER_NULL(*i);
		am[tf->get_name()] = *i;
	}
}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only looks up the identifier in the set of template formals.  
 */
never_ptr<const param_instance_collection>
template_formals_manager::lookup_template_formal(const string& id) const {
	return static_cast<const template_formals_map_type&>
		(template_formals_map)[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	UPDATED: 2005-04-29
	\return the (1-indexed) position of the referenced parameter 
		in the list if found, else 0 if not found.
		IMPORTANT: The index is enumerated from the strict-formal 
		list and the continued into the relaxed-list, 
		i.e. as if they were one concatenated list.  
 */
size_t
template_formals_manager::lookup_template_formal_position(
		const string& id) const {
	const never_ptr<const param_instance_collection>
		pp(lookup_template_formal(id));
	// default, uses pointer comparison
	if (pp) {
		template_formals_list_type::const_iterator
			pb = strict_template_formals_list.begin();
		// find the position in the list/array
		size_t ret = 1 +distance(pb,
			std::find(pb, strict_template_formals_list.end(), pp));
		if (ret <= strict_template_formals_list.size()) {
			return ret;
		} else {
			// if end was reached without finding match, 
			// then match should be in the relaxed-param list.
			pb = relaxed_template_formals_list.begin();
			ret += distance(pb, std::find(pb,
				relaxed_template_formals_list.end(), pp));
			return ret;
		}
	} else	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Used for checking when a type should have null template arguments.  
	Really just a special case of general template argument checking.  
	\return true if this definition is not templated, 
		or the template formals signature is empty, 
		or default parameters are available for all formals.  
 */
good_bool
template_formals_manager::check_null_template_argument(void) const {
	STACKTRACE("template_formals_manager::check_null_template_argument()");
	if (!strict_template_formals_list.empty()) {
		// make sure each formal has a default parameter value
		// starting with strict formal parameters
		template_formals_list_type::const_iterator
			i = strict_template_formals_list.begin();
		for ( ; i!=strict_template_formals_list.end(); i++) {
			const never_ptr<const param_instance_collection> p(*i);
			NEVER_NULL(p);
			p.must_be_a<const param_instance_collection>();
			// if any formal is missing a default value, then this 
			// definition cannot have null template arguments
			if (!(*p).default_value()) {
				return good_bool(false);
			}
			// else continue;       // keep checking
		}
	}
	// else an empty parameter list is acceptable
	// same check with relaxed formals list
	if (!relaxed_template_formals_list.empty()) {
		template_formals_list_type::const_iterator
			i = relaxed_template_formals_list.begin();
		for ( ; i!=relaxed_template_formals_list.end(); i++) {
			const never_ptr<const param_instance_collection> p(*i);
			NEVER_NULL(p);
			p.must_be_a<const param_instance_collection>();
			if (!(*p).default_value()) {
				return good_bool(false);
			}
		}
	}
	// if we've reached end of both lists, we're good!
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compares the sequence of template formals for a generic definition.  
	Useful for comparing template signature equivalence for 
	definition redeclarations.  
	\return true if they are equivalent.  
 */
bool
template_formals_manager::equivalent_template_formals(
		const template_formals_manager& d) const {
	// local aliases
	const template_formals_list_type&
		dsl = d.strict_template_formals_list;
	const template_formals_list_type&
		drl = d.relaxed_template_formals_list;
	bool err = false;
	if (strict_template_formals_list.size() != dsl.size()) {
		// useful error message here
		cerr << "ERROR: number of strict template formal parameters "
			"doesn\'t match! (" <<
			strict_template_formals_list.size() << " vs. " << 
			dsl.size() << ')' << endl;
		err = true;
	}
	if (relaxed_template_formals_list.size() != drl.size()) {
		// useful error message here
		cerr << "ERROR: number of relaxed template formal parameters "
			"doesn\'t match! (" <<
			relaxed_template_formals_list.size() << " vs. " << 
			drl.size() << ')' << endl;
		err = true;
	}
	if (err)
		return false;
	template_formals_list_type::const_iterator
		i = strict_template_formals_list.begin();
	template_formals_list_type::const_iterator j = dsl.begin();
	for ( ; i!=strict_template_formals_list.end() && j!=dsl.end(); i++, j++) {
		const never_ptr<const param_instance_collection> itf(*i);
		const never_ptr<const param_instance_collection> jtf(*j);
		NEVER_NULL(itf);        // template formals not optional
		NEVER_NULL(jtf);        // template formals not optional
		// only type and size need to be equal, not name
		if (!itf->template_formal_equivalent(jtf)) {
			// useful error message goes here
			cerr << "ERROR: strict template formals do not match!"
				<< endl;
			// specifically, which one?
			return false;
		}
		// else continue checking
	}
	// sanity check, we made sure sizes match.
	INVARIANT(i == strict_template_formals_list.end() && j == dsl.end());
	i = relaxed_template_formals_list.begin();
	j = drl.begin();
	for ( ; i!=relaxed_template_formals_list.end() && j!=drl.end(); i++, j++) {
		const never_ptr<const param_instance_collection> itf(*i);
		const never_ptr<const param_instance_collection> jtf(*j);
		NEVER_NULL(itf);        // template formals not optional
		NEVER_NULL(jtf);        // template formals not optional
		// only type and size need to be equal, not name
		if (!itf->template_formal_equivalent(jtf)) {
			// useful error message goes here
			cerr << "ERROR: relaxed template formals do not match!"
				<< endl;
			// specifically, which one?
			return false;
		}
		// else continue checking
	}
	INVARIANT(i == relaxed_template_formals_list.end() && j == drl.end());
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: upgrade dynamic_param_expr_list to a template_argument_list
		that is subdivided into strict and relaxed parameters.  
	TEMPORARY (wrong):
		Just check strict formal parameter list only.  
	Certifies the template arguments against this definition's
	template signature.  
	This also replaces NULL arguments in the list with defaults
	where appropriate.  
	\param ta modifiable list of template argument expressions.
		If this list is empty, however, it will try to 
		construct the default arguments entirely.  
	\return true if arguments successfully type-checked 
		and default arguments supplied in missing places.  
 */
good_bool
template_formals_manager::certify_template_arguments(
		const never_ptr<dynamic_param_expr_list> ta) const {
if (ta) {
	// first, number of arguments must match
	const size_t a_size = ta->size();
	const size_t f_size = strict_template_formals_list.size();
	const template_formals_list_type::const_iterator f_end =
		strict_template_formals_list.end();
	template_formals_list_type::const_iterator
		f_iter = strict_template_formals_list.begin();
	if (a_size != f_size) {
		if (a_size)
			return good_bool(false);
		// else a_size == 0, passed actuals list is empty, 
		// try to fill in all default arguments
		for ( ; f_iter!=f_end; f_iter++) {
			const never_ptr<const param_instance_collection>
				pinst(*f_iter);
			NEVER_NULL(pinst);
			const count_ptr<const param_expr>
				default_expr(pinst->default_value());
			if (!default_expr) {
				// no default value to supply
				return good_bool(false);
			} else {
				ta->push_back(default_expr);
			}
		}
		// if it fails, then list will be incomplete.  
		// if this point is reached, then fill-in was successfull
		return good_bool(true);
	}
	dynamic_param_expr_list::iterator p_iter = ta->begin();
	for ( ; f_iter!=f_end; p_iter++, f_iter++) {
		// need method to check param_instance_collection against param_expr
		// eventually also work for complex aggregate types!
		// "I promise this pointer is only local."  
		const count_ptr<const param_expr> pex(*p_iter);
		const never_ptr<const param_instance_collection>
			pinst(*f_iter);
		NEVER_NULL(pinst);
		if (pex) {
			// type-check assignment, conservative w.r.t. arrays
			if (!pinst->type_check_actual_param_expr(*pex).good) {
				// error message?
				return good_bool(false);
			}
			// else continue checking successive arguments
		} else {
			// no parameter expression given, 
			// check for default -- if exists, use it, 
			// else is error
			const count_ptr<const param_expr>
				default_expr(pinst->default_value());
			if (!default_expr) {
				// error message?
				return good_bool(false);
			} else {
				// else, actually assign it a copy in the list
				*p_iter = default_expr;
			}
		}
	}
	// end of checking reached, everything passed
	return good_bool(true);
} else {
	// no arguments supplied, make sure template specification is
	// null, or every formal has default values.  
	return check_null_template_argument();
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: return a pair of parameter lists (strict/relaxed).
	Prerequisiste for calling this: must satisfy
		check_null_template_arguments.  
	\returns a list of default parameter expressions.  
 */
excl_ptr<dynamic_param_expr_list>
template_formals_manager::make_default_template_arguments(void) const {
	typedef excl_ptr<dynamic_param_expr_list>       return_type;
	INVARIANT(check_null_template_argument().good);
	if (strict_template_formals_list.empty())
		return return_type(NULL);
	// defaulting to dynamic_param_expr_list
	return_type ret(new dynamic_param_expr_list);
	template_formals_list_type::const_iterator
		i = strict_template_formals_list.begin();
	for ( ; i!=strict_template_formals_list.end(); i++) {
		const count_ptr<const param_expr> d((*i)->default_value());
		NEVER_NULL(d);  // everything must have default
		ret->push_back(d);
	}
	// should transfer ownership
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param pf the formal instance to add to the strict formal list.  
	\pre Already checked that formal's name is not already taken.  
 */
void
template_formals_manager::add_strict_template_formal(
		const never_ptr<const param_instance_collection> pf) {
	NEVER_NULL(pf);
	strict_template_formals_list.push_back(pf);
	template_formals_map[pf->get_name()] = pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param pf the formal instance to add to the relaxed formal list.  
	\pre Already checked that formal's name is not already taken.  
 */
void
template_formals_manager::add_relaxed_template_formal(
		const never_ptr<const param_instance_collection> pf) {
	NEVER_NULL(pf);
	relaxed_template_formals_list.push_back(pf);
	template_formals_map[pf->get_name()] = pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: calling this is unnecessary if one guarantees that
	the template formals are a strict subset of the used_id_map.  
	However, it can't hurt to revisit pointers.
 */
void
template_formals_manager::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE("template_formals_manager::collect_transients()");
	template_formals_list_type::const_iterator
		iter = strict_template_formals_list.begin();
	template_formals_list_type::const_iterator
		end = strict_template_formals_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
	iter = relaxed_template_formals_list.begin();
	end = relaxed_template_formals_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Template formals will need to be in list order.
	Just write out the list, the hash_qmap is redundant.  
 */
void
template_formals_manager::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE("template_formals_manager::write_object()");
	INVARIANT(strict_template_formals_list.size()
		+relaxed_template_formals_list.size()
		== template_formals_map.size());
	m.write_pointer_list(o, strict_template_formals_list);
	m.write_pointer_list(o, relaxed_template_formals_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Fake empty list.  
 */
void
template_formals_manager::write_object_base_fake(
		const persistent_object_manager& m, ostream& o) {
	static const template_formals_list_type dummy;
	m.write_pointer_list(o, dummy);
	m.write_pointer_list(o, dummy);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: eliminate code duplication, add some static methods.
	Template formals are loaded in list order.
	Remember that the redundant hash_map also needs to be reconstructed.  
	Another method will add the entries to the corresponding
	used_id_map where appropriate.  
 */
void
template_formals_manager::load_object_base(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE("template_formals_manager::load_object()");
	m.read_pointer_list(f, strict_template_formals_list);
	m.read_pointer_list(f, relaxed_template_formals_list);
	// then copy list into hash_map to synchronize
	template_formals_list_type::const_iterator
		iter = strict_template_formals_list.begin();
	template_formals_list_type::const_iterator
		end = strict_template_formals_list.end();
	for ( ; iter!=end; iter++) {
		STACKTRACE("for-loop: load a map entry");
		const template_formals_value_type inst_ptr = *iter;
		NEVER_NULL(inst_ptr);
		// we need to load the instantiation to use its key!
		m.load_object_once(const_cast<param_instance_collection*>(
			&*inst_ptr));
		template_formals_map[inst_ptr->get_name()] = inst_ptr;
	}
	// duplicate code...
	iter = relaxed_template_formals_list.begin();
	end = relaxed_template_formals_list.end();
	for ( ; iter!=end; iter++) {
		STACKTRACE("for-loop: load a map entry");
		const template_formals_value_type inst_ptr = *iter;
		NEVER_NULL(inst_ptr);
		// we need to load the instantiation to use its key!
		m.load_object_once(const_cast<param_instance_collection*>(
			&*inst_ptr));
		template_formals_map[inst_ptr->get_name()] = inst_ptr;
	}
	INVARIANT(strict_template_formals_list.size()
		+relaxed_template_formals_list.size()
		== template_formals_map.size());
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

