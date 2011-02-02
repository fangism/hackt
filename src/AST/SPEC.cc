/**
	\file "AST/SPEC.cc"
	$Id: SPEC.cc,v 1.20 2011/02/02 23:54:20 fang Exp $
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
#include "Object/def/user_def_chan.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/traits/bool_traits.h"
#include "Object/expr/param_expr.h"
#include "Object/module.h"
#include "Object/lang/SPEC.h"
#include "Object/lang/SPEC_registry.h"
#include "Object/lang/PRS.h"	// for PRS::literal
#include "common/TODO.h"
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"

namespace util {
namespace memory {
// explicit template instantiations
using HAC::parser::SPEC::directive_base;
template class count_ptr<const directive_base>;
}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace parser {
namespace SPEC {
#include "util/using_ostream.h"
using std::find;
using std::copy;
using std::back_inserter;
using entity::definition_base;
using entity::process_definition;
using entity::user_def_chan;
using entity::SPEC::bool_directive;
using entity::SPEC::proc_directive;
using entity::META_TYPE_BOOL;
using entity::META_TYPE_PROCESS;
using std::mem_fun_ref;
using std::find_if;

//=============================================================================
// class directive_base method definitions

directive_base::~directive_base() { }

//=============================================================================
// class directive method definitions

directive::directive(const token_identifier* n, const expr_list* l, 
		const inst_ref_expr_list* a) :
		directive_base(), 
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
#define	SPEC_THROW_ERROR		THROW_EXIT
/**
	Mostly ripped off of PRS::macro::check_rule.
	Consider factoring out into common code for maintainability.  
 */
directive::return_type
directive::check_spec(context& c) const {
	const entity::SPEC::cflat_spec_registry_type::const_iterator
		f(entity::SPEC::cflat_spec_registry.find(*name));
	if (f == entity::SPEC::cflat_spec_registry.end()) {
	if (c.parse_opts.unknown_spec_policy != OPTION_IGNORE) {
		const bool err =
			(c.parse_opts.unknown_spec_policy == OPTION_ERROR);
		cerr << (err ? "Error: " : "Warning: ignoring ") <<
			"unrecognized spec directive \"" << *name <<
			"\" at " << where(*name) << endl;
		if (err) SPEC_THROW_ERROR;
	}
		return;		// skip the rest of this
	}
	const entity::SPEC::cflat_spec_definition_entry& sde(f->second);
	count_ptr<bool_directive> bret;
	count_ptr<proc_directive> pret;
	count_ptr<entity::directive_source_common> ret;
switch (sde.type_enum) {
	case META_TYPE_BOOL:
		bret = count_ptr<bool_directive>(new bool_directive(*name));
		ret = bret;
		break;
	case META_TYPE_PROCESS:
		pret = count_ptr<proc_directive>(new proc_directive(*name));
		ret = pret;
		break;
	default:
	cerr << "Error: unhandled spec meta-type at " << where(*args) << endl;
	SPEC_THROW_ERROR;
}
if (params) {
	if (!sde.check_num_params(params->size()).good) {
		// already have error message
		cerr << "\tat " << where(*params) << endl;
		SPEC_THROW_ERROR;
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
		SPEC_THROW_ERROR;
	}
	INVARIANT(temp.size());
	NEVER_NULL(ret);
	copy(i, e, back_inserter(ret->get_params()));
} else if (!sde.check_num_params(0).good) {
	// no params given where required, already have error message
	cerr << "\tat " << where(*this) << endl;
	SPEC_THROW_ERROR;
}
// check instance reference arguments
// now can be nodes or other types
	NEVER_NULL(args);
	if (!sde.check_num_nodes(args->size()).good) {
		// already have error message
		cerr << "\tat " << where(*args) << endl;
		SPEC_THROW_ERROR;
	}
switch (sde.type_enum) {
case META_TYPE_BOOL: {
	typedef	inst_ref_expr_list::checked_bool_groups_type
						checked_bools_type;
	typedef	checked_bools_type::const_iterator	const_iterator;
	typedef	checked_bools_type::value_type		value_type;
	checked_bools_type temp;
	args->postorder_check_grouped_bool_refs(temp, c);
	const const_iterator i(temp.begin()), e(temp.end());
	if (find_if(i, e, mem_fun_ref(&value_type::empty)) != e) {
		cerr << "Error checking spec arguments in " << where(*args)
			<< endl;
		SPEC_THROW_ERROR;
	}
//	INVARIANT(temp.size());	// can be empty for some directives
	NEVER_NULL(bret);
	copy(i, e, back_inserter(bret->get_nodes()));
	c.get_current_spec_body().push_back(bret);
	break;
}
case META_TYPE_PROCESS: {
	typedef	inst_ref_expr_list::checked_proc_groups_type
						checked_procs_type;
	typedef	checked_procs_type::const_iterator	const_iterator;
	typedef	checked_procs_type::value_type		value_type;
	checked_procs_type temp;
	args->postorder_check_grouped_proc_refs(temp, c);
	const const_iterator i(temp.begin()), e(temp.end());
	if (find_if(i, e, mem_fun_ref(&value_type::empty)) != e) {
		cerr << "Error checking spec arguments in " << where(*args)
			<< endl;
		SPEC_THROW_ERROR;
	}
	INVARIANT(temp.size());
	NEVER_NULL(pret);
	copy(i, e, back_inserter(pret->get_nodes()));
	c.get_current_spec_body().push_back(pret);
	break;
}
default:
	break;
}
}	// end method directive::check_spec

//=============================================================================
// class invariant method definitions

invariant::invariant(const expr* const e, const token_string* const m) :
		directive_base(), _expr(e), _msg(m) {
	NEVER_NULL(_expr);
	// _msg is optional
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
invariant::~invariant() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PARSER_WHAT_DEFAULT_IMPLEMENTATION(invariant)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
invariant::leftmost(void) const { return _expr->leftmost(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
line_position
invariant::rightmost(void) const {
	if (_msg)	return _msg->rightmost();
	else		return _expr->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
invariant::return_type
invariant::check_spec(context& c) const {
	const prs_expr_return_type p(_expr->check_prs_expr(c));
	if (!p) {
		cerr << "Error checking invariant expression at "
			<< where(*_expr) << endl;
		SPEC_THROW_ERROR;
	}
	string m;
	if (_msg) {
		m = *_msg;	// optional message string
	}
	const count_ptr<entity::SPEC::invariant>
		ret(new entity::SPEC::invariant(p, m));
	c.get_current_spec_body().push_back(ret);
}

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
bool
body::__check_specs(context& c) const {
if (directives) {
	try {
		directives->check_list_void(&directive_base::check_spec, c);
	} catch (...) {
		return false;
	}
}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return NULL always, a useless value.
	\throw std::exception on error, bare-bones error handling.  
 */
never_ptr<const object>
body::check_build(context& c) const {
	STACKTRACE_VERBOSE;
	const never_ptr<definition_base> d(c.get_current_open_definition());
	entity::SPEC::directives_set* dss;
	const never_ptr<process_definition> pd(d.is_a<process_definition>());
	const never_ptr<user_def_chan> cd(d.is_a<user_def_chan>());
	if (pd) {
		// top-level module is OK too, but not subnamespace
		if (c.reject_namespace_lang_body()) {
			cerr << "Error: top-level SPEC is only supported "
				"in the global namespace." << endl;
			cerr << "\tgot: spec { ... } " << where(*this)
				<< endl;
			THROW_EXIT;
		}
		dss = &pd->get_spec_directives_set();
	} else if (cd) {
		dss = &cd->get_spec_directives_set();
	} else {
		cerr << "ERROR: spec body can only be used in "
"process definitions or user-defined channels.  ";
		cerr << where(*this) << endl;
		THROW_EXIT;
	}
if (directives) {
	// TODO: access to private members should be granted per directive
	const context::private_member_accessor _priv_(c, true);
	// this line should be relocated eventually

	if (!__check_specs(c)) {
		cerr << "ERROR: at least one error in spec body." << endl;
		THROW_EXIT;
	}
}
	return never_ptr<const object>(NULL);
}

//=============================================================================
}	// end namespace SPEC

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS

// template class node_list<const directive>;
template node_list<const SPEC::directive_base>::node_list();
template node_list<const SPEC::directive_base>::node_list(const SPEC::directive_base*);
template node_list<const SPEC::directive_base>::~node_list();
template ostream& node_list<const SPEC::directive_base>::what(ostream&) const;
template line_position node_list<const SPEC::directive_base>::leftmost(void) const;
template line_position node_list<const SPEC::directive_base>::rightmost(void) const;

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

