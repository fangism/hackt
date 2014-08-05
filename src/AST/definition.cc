/**
	\file "AST/definition.cc"
	Class method definitions for HAC::parser definition-related classes.
	Organized for definition-related branches of the parse-tree classes.
	$Id: definition.cc,v 1.15 2011/01/20 00:09:44 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_definition.cc,v 1.29.10.1 2005/12/11 00:45:04 fang Exp
 */

#ifndef	__HAC_AST_DEFINITION_CC__
#define	__HAC_AST_DEFINITION_CC__

#define	ENABLE_STACKTRACE			0

// rule-of-thumb for inline directives:
// only inline constructors if you KNOW that they will not be be needed
// outside of this module, because we don't have a means to export
// inline methods other than defining in the header or using
// -fkeep-inline-functions

#include <exception>
#include <iostream>

#define	UTIL_WHAT_PARTIAL_SPECIALIZATIONS	0	// 1: DEATH

#include "AST/expr_base.hh"
#include "AST/definition.hh"
#include "AST/formal.hh"
#include "AST/token_char.hh"
#include "AST/token_string.hh"
#include "AST/node_list.tcc"
#include "AST/CHP.hh"
#include "AST/parse_context.hh"

#include "Object/type/data_type_reference.hh"
#include "Object/type/builtin_channel_type_reference.hh"
#include "Object/def/typedef_base.hh"
#include "Object/def/user_def_datatype.hh"
#include "Object/def/enum_datatype_def.hh"
#include "Object/def/user_def_chan.hh"
#include "Object/def/process_definition.hh"
#include "Object/common/namespace.hh"
#include "Object/unroll/port_scope.hh"

#include "util/what.hh"
#include "util/stacktrace.hh"
#include "util/memory/count_ptr.tcc"
#include "util/value_saver.hh"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what

namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::user_data_type_prototype,
	"(user-data-type-proto)")
SPECIALIZE_UTIL_WHAT(HAC::parser::user_data_type_def,
	"(user-data-type-def)")
SPECIALIZE_UTIL_WHAT(HAC::parser::enum_prototype,
	"(enum-declaration)")
SPECIALIZE_UTIL_WHAT(HAC::parser::enum_def,
	"(enum-definition)")
SPECIALIZE_UTIL_WHAT(HAC::parser::user_chan_type_prototype,
	"(user-chan-type-proto)")
SPECIALIZE_UTIL_WHAT(HAC::parser::user_chan_type_def,
	"(user-chan-type-def)")
SPECIALIZE_UTIL_WHAT(HAC::parser::process_prototype,
	"(process-prototype)")
SPECIALIZE_UTIL_WHAT(HAC::parser::process_def,
	"(process-definition)")
SPECIALIZE_UTIL_WHAT(HAC::parser::typedef_alias,
	"(typedef-alias)")
}

//=============================================================================
namespace HAC {
namespace parser {
#include "util/using_ostream.hh"
using entity::enum_datatype_def;
using entity::user_def_chan;
using entity::user_def_datatype;
using entity::process_definition;
using entity::typedef_base;
using entity::builtin_channel_type_reference;
using entity::data_type_reference;

//=============================================================================
// abstract class prototype method definitions

inline
prototype::prototype() : root_item() { }

inline
prototype::~prototype() { }

//=============================================================================
// class definition method definitions

inline
definition::definition() : root_item() {
}

inline
definition::~definition() {
}

//=============================================================================
// class signature_base method definitions

// safe to inline if this is the only translation unit that uses them
inline
signature_base::signature_base(const template_formal_decl_list_pair* tf,
		const token_identifier* i) :
		temp_spec(tf), id(i) {
}

signature_base::signature_base(signature_base& s) :
		temp_spec(s.temp_spec), id(s.id) {
}

inline
signature_base::~signature_base() {
}


//=============================================================================
// class user_data_type_signature method definitions

user_data_type_signature::user_data_type_signature(
		const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, 
		const concrete_type_ref* b, 
		const data_param_decl_list* p) :
		signature_base(tf,n), def(df), dop(dp), bdt(b), params(p) {
	NEVER_NULL(def); NEVER_NULL(dop); NEVER_NULL(bdt); NEVER_NULL(params);
}

user_data_type_signature::~user_data_type_signature() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just constructs and returns an datatype definition with 
	the appropriate signature.
 */
user_data_type_signature::return_type
user_data_type_signature::check_signature(context& c) const {
	STACKTRACE("user_data_type_signature::check_build()");
	excl_ptr<definition_base>
		dd(new user_def_datatype(c.get_current_namespace().is_a<const name_space>(), *id));
	const never_ptr<user_def_datatype>
		ndd(c.set_current_prototype(dd).is_a<user_def_datatype>());
	NEVER_NULL(ndd);
	if (temp_spec) {
		const never_ptr<const object> o(temp_spec->check_build(c));
		if (!o) {
			cerr << where(*temp_spec) << endl;
			THROW_EXIT;	// until better error reponting
		}
	}
	const count_ptr<const fundamental_type_reference>
		ftr(bdt->check_type(c));
	if (!ftr) {
		cerr << "ERROR in base type at " << where(*bdt) << endl;
		THROW_EXIT;
	}
	const count_ptr<const data_type_reference>
		bdr(ftr.is_a<const data_type_reference>());
	// does it have to be a built-in data type reference?
	if (!bdr) {
		cerr << "ERROR: base type of data type definition at " <<
			where(*bdt) << " is not a data type." << endl;
		THROW_EXIT;
	} else {
		ndd->attach_base_data_type(bdr);
	}
{
	STACKTRACE_INDENT_PRINT("processing ports..." << endl);
	const context::sequence_frame seqf(c, ndd);
	// temporarily open the definition's sequential scope
	const count_ptr<port_scope> ps(new port_scope);
	const context::port_scope_frame _psf(c, ps);
	if (!check_data_ports(*params, c).good) {
		cerr << "ERROR: in data ports list at " <<
			where(*params) << endl;
		THROW_EXIT;
	}
}
	const never_ptr<definition_base>
		o(c.add_declaration(c.get_current_prototype()));
	INVARIANT(!c.get_current_prototype());
	if (!o) {
		cerr << where(*this) << endl;
		THROW_EXIT;
	}
#if 0 && REQUIRE_DEFINITION_EXPORT
	if (exp) {
		o->mark_export();
	}
#endif
	return o;
}

//=============================================================================
// class user_data_type_prototype method definitions

CONSTRUCTOR_INLINE
user_data_type_prototype::user_data_type_prototype(
		const template_formal_decl_list_pair* tf,
		const generic_keyword_type* df, 
		const token_identifier* n,
		const string_punctuation_type* dp, 
		const concrete_type_ref* b, 
		const data_param_decl_list* p) :
		prototype(), 
		user_data_type_signature(tf, df, n, dp, b, p) {
}

DESTRUCTOR_INLINE
user_data_type_prototype::~user_data_type_prototype() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(user_data_type_prototype)

line_position
user_data_type_prototype::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return id->leftmost();
}

line_position
user_data_type_prototype::rightmost(void) const {
	return params->rightmost();
}

never_ptr<const object>
user_data_type_prototype::check_build(context& c) const {
	STACKTRACE("user_data_type_prototype::check_build()");
	if (!check_signature(c)) {
		THROW_EXIT;
	}
	// else would've exited already
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class user_data_type_def method definitions

CONSTRUCTOR_INLINE
user_data_type_def::user_data_type_def(
		const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, const concrete_type_ref* b, 
		const data_param_decl_list* p, const brace_type* l, 
		const definition_body* _body, 
		const CHP::body* s, const CHP::body* g,
		const brace_type* r) :
		definition(), 
		user_data_type_signature(tf, df, n, dp, b, p), 
		lb(l), body(_body), setb(s), getb(g), rb(r) {
	NEVER_NULL(lb); NEVER_NULL(setb); NEVER_NULL(getb); NEVER_NULL(rb);
}

DESTRUCTOR_INLINE
user_data_type_def::~user_data_type_def() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(user_data_type_def)

line_position
user_data_type_def::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return id->leftmost();
}

line_position
user_data_type_def::rightmost(void) const {
	if (rb)		return rb->rightmost();
	else		return getb->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
user_data_type_def::check_build(context& c) const {
	STACKTRACE("user_data_type_def::check_build()");
	size_t local_warnings = 0;
{
	const util::value_saver<size_t> _wc(c.warning_count, 0);
	const user_chan_type_signature::return_type
		o(check_signature(c));
	if (!o) {
		cerr << "ERROR checking signature for data-type " << *id <<
			" doesn\'nt match that of previous declaration!  " <<
			where(*this) << endl;
		THROW_EXIT;
	}
	const context::definition_frame<user_def_datatype> _cdf(c, *id);
	// check if already defined?
	if (body) {
		body->check_build(c);
		// useless return value, would've exited upon error already
	}
	if (!setb->check_datatype_CHP(c, true).good) {
		THROW_EXIT;
	}
	if (!getb->check_datatype_CHP(c, false).good) {
		THROW_EXIT;
	}
	const error_policy p(c.parse_opts.case_collision_policy);
	// check for case collisions at the end instead of on-the-fly
if (p != OPTION_IGNORE) {
	const bool e =
		c.get_current_named_scope()->check_case_collisions(cerr);
	if (e) {
		cerr << "Warnings found in datatype definition `" <<
			*id << "\' in " << where(*body) << endl;
	if (p == OPTION_ERROR) {
		cerr << "Promoting warning to error." << endl;
		THROW_EXIT;
	}
	}
}
	if (c.warning_count) {
		local_warnings = c.warning_count;
		cerr << "Warning: found " << c.warning_count <<
			" warning(s) in definition \"" << *id << "\"" << endl;
	}
}
	c.warning_count += local_warnings;	// accumulate
	return c.top_namespace().is_a<const object>();
}

//=============================================================================
// class enum_signature method definitions

/**
	Basic constructor.  
	Enums cannot be templated, so we pass NULL to the parent constructor.  
 */
enum_signature::enum_signature(const generic_keyword_type* e, 
		const token_identifier* i) :
		signature_base(NULL, i), en(e) {
}

enum_signature::~enum_signature() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just constructs and returns an enumeration definition with 
	the appropriate signature.  There's really no signature, just name.  
 */
never_ptr<const object>
enum_signature::check_build(context& c) const {
	STACKTRACE("enum_signature::check_build()");
	excl_ptr<definition_base>
		ed(new enum_datatype_def(c.get_current_namespace().is_a<const name_space>(), *id));
	// elsewhere would need to add template and port formals
	// no need for set_current_prototype
	const never_ptr<const definition_base>
		ret(c.add_declaration(ed));
	if (!ret) {
		// error handling?
		cerr << where(*this) << endl;
		THROW_EXIT;
	}
	return ret;
//	return c.set_current_prototype(ed);
}

//=============================================================================
// class enum_prototype method definitions

enum_prototype::enum_prototype(const generic_keyword_type* e, 
		const token_identifier* i) :
		prototype(), enum_signature(e, i) {
}

enum_prototype::~enum_prototype() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(enum_prototype)

line_position
enum_prototype::leftmost(void) const {
	if (en)		return en->leftmost();
	else		return id->leftmost();
}

line_position
enum_prototype::rightmost(void) const {
	return id->rightmost();
}

/**
	Register this identifier as a enum type.  
	Is acceptable if already declared or defined, 
	Signature has no additional information to match other than name.  
	\return NULL, useless.  
 */
never_ptr<const object>
enum_prototype::check_build(context& c) const {
	STACKTRACE("enum_prototype::check_build()");
	return enum_signature::check_build(c);	// using.
}

//=============================================================================
// class enum_member_list method definitions

enum_member_list::enum_member_list(const token_identifier* i) : parent(i) {
}

enum_member_list::~enum_member_list() {
}

never_ptr<const object>
enum_member_list::check_build(context& c) const {
	STACKTRACE("enum_member_list::check_build()");
	// use current_open_definition
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		NEVER_NULL(*i);
		const good_bool b(c.add_enum_member(**i));
		if (!b.good) {
			// should've exited by now...
			cerr << "BAD fang!" << endl;
		}
	}
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class enum_def method definitions

enum_def::enum_def(const generic_keyword_type* e, const token_identifier* i,
		const enum_member_list* m) :
		definition(), enum_signature(e, i), members(m) {
	NEVER_NULL(members);
}

enum_def::~enum_def() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(enum_def)

line_position
enum_def::leftmost(void) const {
	if (en)		return en->leftmost();
	else		return id->leftmost();
}

line_position
enum_def::rightmost(void) const {
	return members->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reminder: don't forget to reset_current_prototype()
 */
never_ptr<const object>
enum_def::check_build(context& c) const {
	typedef	never_ptr<const object>		return_type;
	STACKTRACE("enum_def::check_build()");
	never_ptr<const object> o(enum_signature::check_build(c));
	if (!o)	return return_type(NULL);
	// lookup and open definition
	const context::enum_definition_frame _edf(c, *id);
	o = members->check_build(c);	// use current_open_definition
		// always returns NULL, will exit upon error
#if 0
	if (!o) {
		cerr << where() << endl;
		THROW_EXIT;
	}
#endif
	return return_type(NULL);
}

//=============================================================================
// class user_chan_type_signature method definitions

CONSTRUCTOR_INLINE
user_chan_type_signature::user_chan_type_signature(
		const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n,
		const string_punctuation_type* dp, 
		const chan_type* b, const data_param_decl_list* p) :
		signature_base(tf, n), 
		def(df), dop(dp), bct(b), params(p) {
	NEVER_NULL(def); NEVER_NULL(dop);
	NEVER_NULL(bct); NEVER_NULL(params);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESTRUCTOR_INLINE
user_chan_type_signature::~user_chan_type_signature() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just constructs and returns an channel definition with 
	the appropriate signature.
 */
user_chan_type_signature::return_type
user_chan_type_signature::check_signature(context& c) const {
	STACKTRACE("user_chan_type_signature::check_signature()");
	excl_ptr<definition_base>
		cd(new user_def_chan(c.get_current_namespace().is_a<const name_space>(), *id));
	const never_ptr<user_def_chan>
		ncd(c.set_current_prototype(cd).is_a<user_def_chan>());
	NEVER_NULL(ncd);
	if (temp_spec) {
		const never_ptr<const object> o(temp_spec->check_build(c));
		if (!o) {
			cerr << where(*temp_spec) << endl;
			THROW_EXIT;
		}
	}
	const count_ptr<const builtin_channel_type_reference>
		bcr(bct->check_type(c)
			.is_a<const builtin_channel_type_reference>());
	if (!bcr) {
		cerr << "ERROR in base channel type at " << where(*bct) << endl;
		THROW_EXIT;
	} else {
		ncd->attach_base_channel_type(bcr);
	}
{
	STACKTRACE_INDENT_PRINT("processing ports..." << endl);
	const context::sequence_frame seqf(c, ncd);
	// temporarily open the definition's sequential scope
	const count_ptr<port_scope> ps(new port_scope);
	const context::port_scope_frame _psf(c, ps);
	if (!check_chan_ports(*params, c).good) {
		cerr << "ERROR: in channel ports list at " <<
			where(*params) << endl;
		THROW_EXIT;
	}
}
	const never_ptr<definition_base>
		o(c.add_declaration(c.get_current_prototype()));
	INVARIANT(!c.get_current_prototype());
	if (!o) {
		cerr << where(*this) << endl;
		THROW_EXIT;
	}
#if 0 && REQUIRE_DEFINITION_EXPORT
	if (exp) {
		o->mark_export();
	}
#endif
	return o;
}

//=============================================================================
// class user_chan_type_prototype method definitions

CONSTRUCTOR_INLINE
user_chan_type_prototype::user_chan_type_prototype(
		const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, const chan_type* b, 
		const data_param_decl_list* p) :
		prototype(), 
		user_chan_type_signature(tf, df, n, dp, b, p) {
}

DESTRUCTOR_INLINE
user_chan_type_prototype::~user_chan_type_prototype() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(user_chan_type_prototype)

line_position
user_chan_type_prototype::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return id->leftmost();
}

line_position
user_chan_type_prototype::rightmost(void) const {
	return params->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
user_chan_type_prototype::check_build(context& c) const {
	if (!check_signature(c)) {
		THROW_EXIT;
	}
	// would've exited if there was error
	return never_ptr<const object>(NULL);
}

//=============================================================================
// class user_chan_type_def method definitions

CONSTRUCTOR_INLINE
user_chan_type_def::user_chan_type_def(
		const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, const chan_type* b, 
		const data_param_decl_list* p, const char_punctuation_type* l, 
		const definition_body* _body, 
		const CHP::body* s, const CHP::body* g, 
		const char_punctuation_type* r) :
		definition(), 
		user_chan_type_signature(tf, df, n, dp, b, p), 
		lb(l), body(_body), sendb(s), recvb(g), rb(r) {
	NEVER_NULL(lb); NEVER_NULL(sendb); NEVER_NULL(recvb); NEVER_NULL(rb);
}

DESTRUCTOR_INLINE
user_chan_type_def::~user_chan_type_def() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(user_chan_type_def)

line_position
user_chan_type_def::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return id->leftmost();
}

line_position
user_chan_type_def::rightmost(void) const {
	if (rb)         return rb->rightmost();
	else            return recvb->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const object>
user_chan_type_def::check_build(context& c) const {
	STACKTRACE("user_chan_type_def::check_build()");
	size_t local_warnings = 0;
{
	const util::value_saver<size_t> _wc(c.warning_count, 0);
	user_chan_type_signature::return_type
		o(check_signature(c));
	if (!o) {
		cerr << "ERROR checking signature for channel "
			<< *id << " doesn\'t match that of "
			"previous declaration!  " << where(*this) << endl;
		THROW_EXIT;
	}
	const context::definition_frame<user_def_chan> _cdf(c, *id);
	// only problem from here is if channel type was already defined.  
	// in which case, open_channel_definition will THROW_EXIT;
	if (body) {
		body->check_build(c);
		// useless return value, would've exited upon error
	}
	if (!sendb->check_channel_CHP(c, true).good) {
		THROW_EXIT;
	}
	if (!recvb->check_channel_CHP(c, false).good) {
		THROW_EXIT;
	}
	const error_policy p(c.parse_opts.case_collision_policy);
	// check for case collisions at the end instead of on-the-fly
if (p != OPTION_IGNORE) {
	const bool e =
		c.get_current_named_scope()->check_case_collisions(cerr);
	if (e) {
		cerr << "Warnings found in channel definition `" <<
			*id << "\' in " << where(*body) << endl;
	if (p == OPTION_ERROR) {
		cerr << "Promoting warning to error." << endl;
		THROW_EXIT;
	}
	}
}
	if (c.warning_count) {
		local_warnings = c.warning_count;
		cerr << "Warning: found " << c.warning_count <<
			" warning(s) in definition \"" << *id << "\"" << endl;
	}
}
	c.warning_count += local_warnings;	// accumulate
	// nothing better to do
	return c.top_namespace().is_a<const object>();
}

//=============================================================================
// class process_signature method definitions

CONSTRUCTOR_INLINE
process_signature::process_signature(
		const generic_keyword_type* e, 
		const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* d, const token_identifier* i, 
		const port_formal_decl_list* p, 
		const unsigned char m) :
		signature_base(tf,i), exp(e), def(d), ports(p), 
		meta_type(m) {
	NEVER_NULL(def);
	NEVER_NULL(ports);		// not any more!
}

process_signature::process_signature(process_signature& s) :
		signature_base(s), exp(s.exp), def(s.def), ports(s.ports), 
		meta_type(s.meta_type) {
	NEVER_NULL(def);
	NEVER_NULL(ports);		// not any more!
}

DESTRUCTOR_INLINE
process_signature::~process_signature() { }

#if 0
const token_identifier&
process_signature::get_name(void) const {
	return *id;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: rewrite calls to subchecks (2005-05-25)
	Creates and returns a process definition objects with signature.
	Works in temporary space of new definition, then compares with
	previous signature if found.  
	\param c context is modifiable in case new concrete-types update
		the type-cache.  
	\return pointer to definition if either newly created, 
		or previous declaration if it is exact match, 
		else returns NULL upon failure.  
 */
process_signature::return_type
process_signature::check_signature(context& c) const {
	STACKTRACE("process_signature::check_build()");
	excl_ptr<definition_base>
		ret(new process_definition(c.get_current_namespace().is_a<const name_space>(), *id, 
			entity::meta_type_tag_enum(meta_type)));
	const never_ptr<process_definition> seq(ret.is_a<process_definition>());
	NEVER_NULL(seq);
	c.set_current_prototype(ret);
	// transfered ownership
	if (temp_spec) {
		const never_ptr<const object> o(temp_spec->check_build(c));
		if (!o) {
			cerr << where(*temp_spec) << endl;
			THROW_EXIT;
		}
	}
	if (ports && !ports->empty()) {
		STACKTRACE_INDENT_PRINT("processing ports..." << endl);
		const context::sequence_frame seqf(c, seq);
		// temporarily open the definition's sequential scope
		const count_ptr<port_scope> ps(new port_scope);
		const context::port_scope_frame _psf(c, ps);
		const never_ptr<const object> o(ports->check_build(c));
		// return value NULL, useless
		// would've exited already if there was error
	}
	// this checks for conflicts in definitions.  
	// transfers ownership between context members
	const never_ptr<entity::definition_base>
		o(c.add_declaration(c.get_current_prototype()));
	INVARIANT(!c.get_current_prototype());
	if (!o) {
		cerr << where(*this) << endl;
		THROW_EXIT;
	}
#if REQUIRE_DEFINITION_EXPORT
	if (exp) {
		o->mark_export();
	}
#endif
	return o;
//	return c.set_current_prototype(ret);
}

//=============================================================================
// class process_prototype method definitions

CONSTRUCTOR_INLINE
process_prototype::process_prototype(
		const generic_keyword_type* e, 
		const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* d, const token_identifier* i, 
		const port_formal_decl_list* p, 
		const unsigned char m) :
		prototype(),
		process_signature(e, tf, d, i, p, m) {
}

DESTRUCTOR_INLINE
process_prototype::~process_prototype() {
}

PARSER_WHAT_DEFAULT_IMPLEMENTATION(process_prototype)

line_position
process_prototype::leftmost(void) const {
	// if (exp)		return exp->leftmost();
	// if (temp_spec)	return temp_spec->leftmost();
	return def->leftmost();
}

line_position
process_prototype::rightmost(void) const {
	return ports->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
/**
	TO DO: complete me!
 */
never_ptr<const object>
process_prototype::check_build(context& c) const {
	STACKTRACE("process_prototype::check_build()");
	return check_signature(c);
}
#endif

//=============================================================================
// class process_def method definitions

CONSTRUCTOR_INLINE
process_def::process_def(
		process_signature& s, const definition_body* b, 
		const unsigned char m) :
		definition(),
		process_signature(s), 
		body(b) {
	NEVER_NULL(body);		// body may be empty, is is not NULL
	// consistency check
	if (m != meta_type) {
cerr << "Error: redefining previous declared type as a different meta-type!"
		<< endl;
		THROW_EXIT;
	}
}

DESTRUCTOR_INLINE
process_def::~process_def() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(process_def)

line_position
process_def::leftmost(void) const {
	// if (exp)		return exp->leftmost();
	// if (temp_spec)	return temp_spec->leftmost();
	return def->leftmost();
}

line_position
process_def::rightmost(void) const {
	return body->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks a whole process definition.  
 */
never_ptr<const object>
process_def::check_build(context& c) const {
	STACKTRACE("process_def::check_build()");
	size_t local_warnings = 0;
{
	const util::value_saver<size_t> _wc(c.warning_count, 0);
	process_signature::return_type
		o(check_signature(c));
	if (!o) {
		cerr << "ERROR checking signature for process "
			<< *id << " doesn\'t match that of "
			"previous declaration!  " << where(*this) << endl;
		THROW_EXIT;
	}

	// only problem from here is if process was already defined.  
	// in which case, open_process_definition will THROW_EXIT;
	const context::definition_frame<process_definition> _cdf(c, *id);
	// set up the current PRS body
	const context::prs_body_frame
		_pb(c, never_ptr<entity::PRS::rule_set_base>(
			&c.get_current_definition<process_definition>()
			->get_prs()));
	const context::rte_body_frame
		_rb(c, never_ptr<entity::RTE::assignment_set_base>(
			&c.get_current_definition<process_definition>()
			->get_rte()));
	const context::spec_body_frame
		_sb(c, never_ptr<entity::SPEC::directives_set>(
			&c.get_current_definition<process_definition>()
			->get_spec_directives_set()));
	// doesn't CHP have its own frame??
	body->check_build(c);
	// useless return value, would've exited upon error already
	const error_policy p(c.parse_opts.case_collision_policy);
	// check for case collisions at the end instead of on-the-fly
if (p != OPTION_IGNORE) {
	const bool e =
		c.get_current_named_scope()->check_case_collisions(cerr);
	if (e) {
		cerr << "Warnings found in process definition `" <<
			*id << "\' in " << where(*body) << endl;
	if (p == OPTION_ERROR) {
		cerr << "Promoting warning to error." << endl;
		THROW_EXIT;
	}
	}
}
	if (c.warning_count) {
		local_warnings = c.warning_count;
		cerr << "Warning: found " << c.warning_count <<
			" warning(s) in definition \"" << *id << "\"" << endl;
	}
}
	c.warning_count += local_warnings;	// accumulate
	// nothing better to do
	return c.top_namespace().is_a<const object>();
}

//=============================================================================
// class typedef_alias method definitions

typedef_alias::typedef_alias(const template_formal_decl_list_pair* t,
		const generic_keyword_type* k, const concrete_type_ref* b,
		const token_identifier* i) :
		def_body_item(), 
		temp_spec(t), td(k), base(b), id(i) {
	NEVER_NULL(base); NEVER_NULL(id);
}

typedef_alias::~typedef_alias() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(typedef_alias)

line_position
typedef_alias::leftmost(void) const {
	if (temp_spec)	return temp_spec->leftmost();
	else if (td)	return td->leftmost();
	else		return base->leftmost();
}

line_position
typedef_alias::rightmost(void) const {
	return id->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Three syntaces for typedef:
	1) typedef old new;
	2) typedef old<...> new;
	3) template <...> typedef old<...> new;
	is the LHS always supposed to be a *type* or in case 1, 
	can it be interpreted as the definition?
	A definition alias is just a hash_map duplicate entry.  
	A type alias also acts like a definition on behalf of the 
	(partially) complete type referenced.  
	Amibiguous semantics for case 1, default to interpreting "old" as 
	its definition.  
	Then we need to peek down the `base' member.
	Use keyword to check class?
	Chicken-before-egg between referenced type and template params.  
 */
never_ptr<const object>
typedef_alias::check_build(context& c) const {
	STACKTRACE("typedef_alias::check_build()");
	// NOTE: (2005-05-27)
	// built-in chan types have no base definition!
	// invalid to expect to call get_base_def or not?
	// temporary work-around: dynamic_cast check if is generic_type_ref, 
	// because chan_type cannot be templated.  
	// This all has to do the with typedef semantics described in the
	// commends for this method (also in lang. spec. documentation).  
	const never_ptr<const generic_type_ref>
		base_not_chan(base.is_a<const generic_type_ref>());

if (base_not_chan) {
	STACKTRACE_INDENT_PRINT("base type not channel" << endl);
	// Need to redo this!
	// first we read the user's mind by peeking down base.  
	// does base have any template params, even an empty list?
	const never_ptr<const type_base>
		basedef(base_not_chan->get_base_def());
	const never_ptr<const definition_base>
		d(basedef->check_definition(c));
	if (!d) {
		cerr << "typedef_alias: bad definition reference!  "
			"ERROR!  " << where(*basedef) << endl;
		THROW_EXIT;
	}
	// need to worry about resetting definition reference?

if (base_not_chan->get_temp_spec()) {
	STACKTRACE_INDENT_PRINT("type has template arguments" << endl);
	// then base certainly refers to a concrete type
	// so now we make a new definition to wrap around it
	// what kind of alias? we need to peek at the definition

	excl_ptr<definition_base>
		td_ex = d->make_typedef(c.get_current_named_scope(), *id);
	const never_ptr<definition_base> tdf(td_ex);
	const never_ptr<typedef_base> tdb(tdf.is_a<typedef_base>());
	NEVER_NULL(tdb);
	c.set_current_prototype(td_ex);
	if (temp_spec) {
		const never_ptr<const object> o(temp_spec->check_build(c));
		// will add template_formals to the alias
		if (!o) {
			cerr << "ERROR in template formals of typedef!  "
				<< where(*temp_spec) << endl;
			THROW_EXIT;
		}
	}
	STACKTRACE_INDENT_PRINT("template arguments OK" << endl);
	// prepare to transfer ownership
	count_ptr<const fundamental_type_reference>
		ftr(base_not_chan->check_type(c));
	// kludge: changing prototype context?
	excl_ptr<definition_base> td_ex2(c.get_current_prototype());

	// transfers ownership between context members
	STACKTRACE_INDENT_PRINT("adding new type reference" << endl);
	const never_ptr<const object>
		obj(c.add_declaration(td_ex2));
		// also resets current_prototype, *after* checking type ref
	if (!obj) {
		cerr << "(or is duplicate) " << where(*this) << endl;
		THROW_EXIT;
	}
	INVARIANT(!c.get_current_prototype());
	// useless return value NULL, check current_fundamental_type
	if (!ftr) {
		cerr << "ERROR resolving concrete type reference in typedef!  "
			<< where(*base_not_chan) << endl;
		THROW_EXIT;
	}
	// must reset because not making instances
	INVARIANT(ftr.refs() == 1);
	excl_ptr<const fundamental_type_reference>
		ftr_ex(ftr.exclusive_release());
	INVARIANT(!ftr);
	INVARIANT(ftr_ex);
	// else safe to alias type
	STACKTRACE_INDENT_PRINT("assigning new type definition" << endl);
	bool b = tdb->assign_typedef(ftr_ex);
	if (!b) {
		cerr << "ERROR assigning typedef!  " << where(*this) << endl;
		THROW_EXIT;
	}
	// let context add the complete alias to the scope
	// check for collision error
	return tdf;
} else {	// base may actually refer to the definition, not the type
	// in this case we create a definition alias
	// restriction: temp_spec for this definition must be empty or null.  
	if (temp_spec) {
		cerr << "ERROR: pure definition alias cannot have "
			"a template signature.  " << where(*this) << endl;
		THROW_EXIT;
	}
	// issue warning about this interpretation?
	STACKTRACE_INDENT_PRINT("registering new typedef" << endl);
	const good_bool b(c.alias_definition(d, *id));
	if (!b.good) {
		cerr << where(*id) << endl;
		THROW_EXIT;
		return never_ptr<const object>(NULL);
	}
	// else was successful
	return d;
}
} else {
	// base is a chan, which cannot have a template signature
	cerr << "Fang, finish typedef_alias for built-in chan types!" << endl;
	THROW_EXIT;
	return never_ptr<const object>(NULL);
}
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

template class node_list<const def_body_item>;

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_DEFINITION_CC__

