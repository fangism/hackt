/**
	\file "AST/SPEC.cc"
	$Id: SPEC.cc,v 1.4.12.4 2006/04/12 06:34:59 fang Exp $
 */

#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>
#include <iterator>

#include "AST/SPEC.h"
#include "AST/node_list.tcc"
#include "AST/token_string.h"
#include "AST/expr_list.h"
#include "AST/parse_context.h"

#include "Object/def/process_definition.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/traits/bool_traits.h"
#include "Object/expr/param_expr.h"
#include "Object/lang/SPEC.h"
#include "Object/lang/SPEC_registry.h"
#include "Object/lang/PRS.h"	// for PRS::literal
#include "common/TODO.h"
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"


namespace HAC {
namespace parser {
namespace SPEC {
#include "util/using_ostream.h"
using std::find;
using std::copy;
using std::back_inserter;
using entity::definition_base;
using entity::process_definition;
using std::mem_fun_ref;
using std::find_if;

//=============================================================================
// class directive method definitions

directive::directive(const token_identifier* n, const expr_list* l, 
		const inst_ref_expr_list* a) :
		name(n), params(l), args(a) {
	NEVER_NULL(name); NEVER_NULL(args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive::~directive() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(directive)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
directive::leftmost(void) const { return name->leftmost(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
directive::rightmost(void) const { return args->rightmost(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Mostly ripped off of PRS::macro::check_rule.
	Consider factoring out into common code for maintainability.  
 */
directive::return_type
directive::check_spec(context& c) const {
	const entity::SPEC::spec_definition_entry
		sde(entity::SPEC::spec_registry[*name]);
	if (!sde) {
		cerr << "Error: unrecognized spec directive \"" << *name <<
			"\" at " << where(*name) << endl;
		return return_type(NULL);
	}
	const count_ptr<entity::SPEC::directive>
		ret(new entity::SPEC::directive(*name));
if (params) {
	if (!sde.check_num_params(params->size()).good) {
		// already have error message
		cerr << "\tat " << where(*params) << endl;
		return return_type(NULL);
	}
	typedef	expr_list::checked_meta_exprs_type	checked_exprs_type;
	typedef	checked_exprs_type::const_iterator	const_iterator;
	typedef	checked_exprs_type::value_type		value_type;
	checked_exprs_type temp;
	params->postorder_check_meta_exprs(temp, c);
	const const_iterator i(temp.begin()), e(temp.end());
	if (find(i, e, value_type(NULL)) != e) {
		cerr << "Error checking spec parameters in "
			<< where(*params) << endl;
		return return_type(NULL);
	}
	INVARIANT(temp.size());
	NEVER_NULL(ret);
	copy(i, e, back_inserter(ret->get_params()));
} else if (!sde.check_num_params(0).good) {
	// no params given where required, already have error message
	cerr << "\tat " << where(*this) << endl;
	return return_type(NULL);
}
{
	typedef	inst_ref_expr_list::checked_bool_groups_type
						checked_bools_type;
	typedef	checked_bools_type::const_iterator	const_iterator;
	typedef	checked_bools_type::value_type		value_type;
	NEVER_NULL(args);
	if (!sde.check_num_nodes(args->size()).good) {
		// already have error message
		cerr << "\tat " << where(*args) << endl;
		return return_type(NULL);
	}
	checked_bools_type temp;
	args->postorder_check_grouped_bool_refs(temp, c);
	const const_iterator i(temp.begin()), e(temp.end());
	if (find_if(i, e, mem_fun_ref(&value_type::empty)) != e) {
		cerr << "Error checking spec arguments in " << where(*args)
			<< endl;
		return return_type(NULL);
	}
	INVARIANT(temp.size());
	NEVER_NULL(ret);
	copy(i, e, back_inserter(ret->get_nodes()));
}
	return ret;
}	// end method directive::check_spec

//=============================================================================
// class body method definitions

body::body(const generic_keyword_type* t, const directive_list* d) :
		language_body(t), directives(d) {
	NEVER_NULL(directives);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
body::~body() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(body)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
body::leftmost(void) const { return language_body::leftmost(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
body::rightmost(void) const { return directives->rightmost(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return NULL always, a useless value.
	\throw std::exception on error, bare-bones error handling.  
 */
never_ptr<const object>
body::check_build(context& c) const {
	STACKTRACE_VERBOSE;
	const never_ptr<definition_base> d(c.get_current_open_definition());
	const never_ptr<process_definition> pd(d.is_a<process_definition>());
	NEVER_NULL(pd);
	typedef	std::vector<directive::return_type>	checked_directives_type;
	checked_directives_type checked_directives;
	directives->check_list(checked_directives, &directive::check_spec, c);
	checked_directives_type::const_iterator i(checked_directives.begin());
	checked_directives_type::const_iterator e(checked_directives.end());
	const checked_directives_type::const_iterator
		null_iter(find(i, e, directive::return_type()));
	if (null_iter == e) {
		// transfer over to process_definition's spec set
		copy(i, e, back_inserter(pd->get_spec_directives_set()));
	} else {
		cerr << "ERROR: at least one error in spec body." << endl;
		THROW_EXIT;
	}
	return never_ptr<const object>(NULL);
}

//=============================================================================
}	// end namespace SPEC

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS

// template class node_list<const directive>;
template node_list<const SPEC::directive>::node_list(const SPEC::directive*);
template ostream& node_list<const SPEC::directive>::what(ostream&) const;
template line_position node_list<const SPEC::directive>::leftmost(void) const;
template line_position node_list<const SPEC::directive>::rightmost(void) const;

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

