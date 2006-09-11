/**
	\file "Object/def/template_formals_manager.cc"
	Template formals manager implementation.
	This file was "Object/def/template_formals_manager.cc"
		in a previous life.  
	$Id: template_formals_manager.cc,v 1.12.6.3 2006/09/11 22:30:19 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DUMPS		(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <iterator>
#include "util/hash_specializations.h"	// include as early as possible

#include "Object/def/template_formals_manager.h"
#if USE_INSTANCE_PLACEHOLDERS
#include "Object/inst/param_value_placeholder.h"
#else
#include "Object/inst/param_value_collection.h"
#endif
#include "Object/expr/const_param.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/type/template_actuals.h"
#include "Object/unroll/unroll_context.h"
#include "Object/common/dump_flags.h"

#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/IO_utils.tcc"
#include "util/indent.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::distance;
using util::indent;
using util::auto_indent;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;

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
/**
	Pretty-prints template formals specification for a
	list of formals.  
 */
ostream&
template_formals_manager::dump_formals_list(ostream& o, 
		const template_formals_list_type& l) {
#if STACKTRACE_DUMPS
	STACKTRACE("tfm::dump");
#endif
	o << '<' << endl;	// continued from last print
	template_formals_list_type::const_iterator i(l.begin());
	const template_formals_list_type::const_iterator e(l.end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		// sanity check
		// not true any more: may be loop induction variable
		// INVARIANT((*i)->is_template_formal());
#if USE_INSTANCE_PLACEHOLDERS
		// not using template formals managers anymore
		// to hack support for induction variables.  
		INVARIANT((*i)->is_template_formal());
#else
		INVARIANT((*i)->is_template_formal() ||
			(*i)->is_loop_variable());
#endif
		(*i)->dump_formal(o << auto_indent) << endl;
	}
	return o << auto_indent << '>';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pretty-prints strict and relaxed template formal parameters.  
 */
ostream&
template_formals_manager::dump(ostream& o) const {
	STACKTRACE("template_formals_manager::dump_template_formals()");
	// sanity check
	INVARIANT(strict_template_formals_list.size()
		+relaxed_template_formals_list.size()
		== template_formals_map.size());
	if (!strict_template_formals_list.empty()) {
		INDENT_SECTION(o);
		dump_formals_list(o, strict_template_formals_list);
	}
	if (!relaxed_template_formals_list.empty()) {
		INDENT_SECTION(o);
		// need to be able to read that this is the second param list
		if (strict_template_formals_list.empty()) {
			o << "<>" << endl;	// continued from last print
		}
		dump_formals_list(o, relaxed_template_formals_list);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only looks up the identifier in the set of template formals.  
	\param id the local name of the formal parameter.  
	\return pointer to the formal template parameter.  
 */
template_formals_manager::template_formals_value_type
template_formals_manager::lookup_template_formal(const string& id) const {
	typedef	template_formals_value_type	return_type;
	const template_formals_map_type::const_iterator
		f(template_formals_map.find(id));
	return (f != template_formals_map.end()) ? f->second
		: return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks whether referenced instance is a relaxed template formal.
 */
bool
template_formals_manager::probe_relaxed_template_formal(
		const string& id) const {
	const size_t ind = lookup_template_formal_position(id);
	// remember: index is 1-indexed
	return ind > strict_template_formals_list.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
template_formals_manager::has_relaxed_formals(void) const {
	return !relaxed_template_formals_list.empty();
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
	const template_formals_value_type
		pp(lookup_template_formal(id));
	// default, uses pointer comparison
	if (pp) {
		template_formals_list_type::const_iterator
			pb(strict_template_formals_list.begin());
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
	Partial check on one of the template parameter lists:
	Used for checking when a type should have null template arguments.  
	Really just a special case of general template argument checking.  
	\return true if this definition is not templated, 
		or the template formals signature is empty, 
		or default parameters are available for all formals.  
 */
good_bool
template_formals_manager::partial_check_null_template_argument(
		const template_formals_list_type& l) {
	if (!l.empty()) {
		// make sure each formal has a default parameter value
		// starting with strict formal parameters
		template_formals_list_type::const_iterator i(l.begin());
		for ( ; i!=l.end(); i++) {
			const template_formals_value_type p(*i);
			NEVER_NULL(p);
			p.must_be_a<const placeholder_type>();
			// if any formal is missing a default value, then this 
			// definition cannot have null template arguments
			if (!(*p).default_value()) {
				// gives relative position in the partial
				// list, not the combined lists.  
				cerr << "ERROR: missing template actual at position "
					<< distance(l.begin(), i)+1 <<
					" where no default value is given." << endl;

				return good_bool(false);
			}
			// else continue;       // keep checking
		}
	}
	// else an empty parameter list is acceptable
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Used for checking when a type should have null template arguments.  
	Really just a special case of general template argument checking.  
	\return true if this definition is not templated, 
		or the template formals signature is empty, 
		or default parameters are available for all formals.  
	NOTE: null argument checking doesn't apply to relaxed formals.  
 */
good_bool
template_formals_manager::check_null_template_argument(void) const {
	STACKTRACE("template_formals_manager::check_null_template_argument()");
#if 0
	if (!partial_check_null_template_argument(
			strict_template_formals_list).good) {
		return good_bool(false);
	} else if (!partial_check_null_template_argument(
			relaxed_template_formals_list).good) {
		return good_bool(false);
	} else	return good_bool(true);
#else
	return partial_check_null_template_argument(
		strict_template_formals_list);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks equivalence between two template formal lists, 
	as used by definitions and declarations.  
	This is intelligent in that it does a *structural* comparison
	between the parameter-dependent expressions in the parameter
	declarations.  
	\pre size of lists l and r are the same.  
 */
bool
template_formals_manager::equivalent_template_formals_lists(
		const template_formals_list_type& l, 
		const template_formals_list_type& r, 
		const string& err_msg) {
	template_formals_list_type::const_iterator i(l.begin());
	template_formals_list_type::const_iterator j(r.begin());
	for ( ; i!=l.end() && j!=r.end(); i++, j++) {
		const template_formals_value_type itf(*i);
		const template_formals_value_type jtf(*j);
		NEVER_NULL(itf);        // template formals not optional
		NEVER_NULL(jtf);        // template formals not optional
		// only type and size need to be equal, not name
		if (!itf->template_formal_equivalent(*jtf)) {
			// useful error message goes here
			cerr << err_msg << endl;
			// specifically, which one?
			return false;
		}
		// else continue checking
	}
	// sanity check, we made sure sizes match.
	INVARIANT(i == l.end() && j == r.end());
	return true;
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
	if (!equivalent_template_formals_lists(
			strict_template_formals_list, dsl, 
			"ERROR: strict template formals do not match!")) {
		return false;
	}
	else	return equivalent_template_formals_lists(
			relaxed_template_formals_list, drl, 
			"ERROR: relaxed template formals do not match!");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
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
		template_actuals& t) const {
	const count_ptr<const param_expr_list>& spl(t.get_strict_args());
	good_bool sg;
	if (spl) {
		const count_ptr<param_expr_list> rep(spl->copy());
		sg = rep->certify_template_arguments(
			strict_template_formals_list);
		if (sg.good) {
			t.replace_strict_args(rep);
		}
		// else already have error message
	} else {
		sg = partial_check_null_template_argument(
			strict_template_formals_list);
	}
	// NOTE: at this phase of certification, the relaxed actuals
	// are allowed to be ommitted, and thus should not be filled in.  
	// Also note: relaxed formals aren't allowed to have default values.
	// we use the const variant which doesn't modify the list
	const count_ptr<const param_expr_list> rpl(t.get_relaxed_args());
#if 0
	const good_bool rg(rpl ?
		rpl->certify_template_arguments(
			relaxed_template_formals_list) :
		partial_check_null_template_argument(
			relaxed_template_formals_list));
#else
	const good_bool rg(rpl ?
		rpl->certify_template_arguments_without_defaults(
			relaxed_template_formals_list) : good_bool(true));
		// if relaxed is NULL, then accept for now
#endif
	return sg && rg;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Validating finally resolved (constant) template actuals
	against template formal parameters.
	\param t the template actuals must contain only resolved constants.  
 */
good_bool
template_formals_manager::must_validate_actuals(
		const template_actuals& t) const {
	STACKTRACE_VERBOSE;
	/***
		Need to construct a temporary context for situations where
		the formal parameters themselves depend on earlier actuals.  
	***/
#if RESOLVE_VALUES_WITH_FOOTPRINT
	unroll_context c;
	unroll_formal_parameters(c, t);
#else
	const unroll_context c(t, *this);
#endif
	const count_ptr<const param_expr_list> spl(t.get_strict_args());
	const count_ptr<const const_param_expr_list>
		cspl(spl.is_a<const const_param_expr_list>());
	if (spl) NEVER_NULL(cspl);
	const good_bool sg(cspl ?
		cspl->must_validate_template_arguments(
			strict_template_formals_list, c) :
		partial_check_null_template_argument(
			strict_template_formals_list));
	const count_ptr<const param_expr_list> rpl(t.get_relaxed_args());
	const count_ptr<const const_param_expr_list>
		crpl(rpl.is_a<const const_param_expr_list>());
	if (rpl) NEVER_NULL(crpl);
	// if relaxed actuals are NULL, don't check them for defaults
	const good_bool rg(crpl ?
		crpl->must_validate_template_arguments(
			relaxed_template_formals_list, c) : good_bool(true));
#if 0
		partial_check_null_template_argument(
			relaxed_template_formals_list);
#endif
	const good_bool ret(sg && rg);
	if (!ret.good) {
		cerr << "ERROR: actual parameter types do not "
			"completely match up against formals." << endl;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a set of default arguments based on prototype declaration.  
	NOTE: relaxed template formals are not allowed to have
		default values.  
	\pre satisfies this->check_null_template_argument().
	\returns a list of default parameter expressions.  
 */
template_actuals
template_formals_manager::make_default_template_arguments(void) const {
	typedef count_ptr<dynamic_param_expr_list>       return_type;
	INVARIANT(check_null_template_argument().good);
	if (strict_template_formals_list.empty())
		return template_actuals();
	const return_type
		ret(new dynamic_param_expr_list(
			strict_template_formals_list.size()));
	template_formals_list_type::const_iterator
		i(strict_template_formals_list.begin());
	for ( ; i!=strict_template_formals_list.end(); i++) {
		const count_ptr<const param_expr> d((*i)->default_value());
		NEVER_NULL(d);  // everything must have default
		ret->push_back(d);
	}
	// should transfer ownership
	return template_actuals(ret, template_actuals::arg_list_ptr_type());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a strict template formal with this manager.  
	\param pf the formal instance to add to the strict formal list.  
	\pre Already checked that formal's name is not already taken.  
 */
void
template_formals_manager::add_strict_template_formal(
		const template_formals_value_type pf) {
	NEVER_NULL(pf);
	strict_template_formals_list.push_back(pf);
	template_formals_map[pf->get_name()] = pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a relaxed template formal with this manager.  
	\param pf the formal instance to add to the relaxed formal list.  
	\pre Already checked that formal's name is not already taken.  
 */
void
template_formals_manager::add_relaxed_template_formal(
		const template_formals_value_type pf) {
	NEVER_NULL(pf);
	relaxed_template_formals_list.push_back(pf);
	template_formals_map[pf->get_name()] = pf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if RESOLVE_VALUES_WITH_FOOTPRINT
/**
	Helper function for unrolling and assigning actual values to
	template formal parameters.  
	\pre formals and actuals already size/type checked.  
 */
good_bool
template_formals_manager::__unroll_formal_parameters(
		const unroll_context& c, 
		const template_formals_list_type& formals, 
		const template_actuals::const_arg_list_ptr_type& v) {
	typedef	template_actuals::const_arg_list_ptr_type
			actual_values_ptr_type;
	// typedef	actual_values_ptr_type::element_type::const_iterator
	//			const_actual_iterator;
	typedef	template_formals_list_type::const_iterator
				const_formal_iterator;
	STACKTRACE_VERBOSE;
	bool good = true;
	if (v) {
		return v->unroll_assign_formal_parameters(c, formals);
#if 0
		const_formal_iterator iter(formals.begin()), end(formals.end());
		// const_actual_iterator a_iter(v->begin()), a_end(v->end());
		for ( ; iter!=end; ++iter, ++a_iter) {
			good &= (*iter)->unroll_assign_formal_parameter(
				c, *a_iter).good;
		}
		INVARIANT(a_iter == a_end);
#endif
	}
	return good_bool(good);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This unrolls and assigns actual values to formal template parameters.  
	\pre Formals vs. actuals already type/size checked by caller.  
 */
good_bool
template_formals_manager::unroll_formal_parameters(
		const unroll_context& c, const template_actuals& a) const {
	STACKTRACE_VERBOSE;
	return good_bool(
		__unroll_formal_parameters(c, strict_template_formals_list, 
			a.get_strict_args()).good &&
		__unroll_formal_parameters(c, relaxed_template_formals_list, 
			a.get_relaxed_args()).good);
}
#endif	// RESOLVE_VALUES_WITH_FOOTPRINT

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
		iter(strict_template_formals_list.begin());
	template_formals_list_type::const_iterator
		end(strict_template_formals_list.end());
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
	Loads one of the lists.  
	\param l list of template formals, already loaded with pointers.  
 */
void
template_formals_manager::load_template_formals_list(
		const persistent_object_manager& m, 
		template_formals_map_type& the_map, 
		template_formals_list_type& l) {
	template_formals_list_type::const_iterator iter(l.begin());
	const template_formals_list_type::const_iterator end(l.end());
	for ( ; iter!=end; iter++) {
		STACKTRACE("for-loop: load a map entry");
		const template_formals_value_type inst_ptr = *iter;
		NEVER_NULL(inst_ptr);
		// we need to load the instantiation to use its key!
		m.load_object_once(const_cast<placeholder_type*>(
			&*inst_ptr));
		the_map[inst_ptr->get_name()] = inst_ptr;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
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
	load_template_formals_list(m, template_formals_map, 
		strict_template_formals_list);
	load_template_formals_list(m, template_formals_map, 
		relaxed_template_formals_list);
	INVARIANT(strict_template_formals_list.size()
		+relaxed_template_formals_list.size()
		== template_formals_map.size());
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#if 0
// doesn't actually save space
namespace std {
// explicit template class instantiation
template class
vector<HAC::entity::template_formals_manager::template_formals_value_type>;
}	// end namespace std
#endif

