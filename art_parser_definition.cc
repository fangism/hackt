// "art_parser_definition.cc"
// class method definitions for ART::parser
// organized for definition-related branches of the parse-tree classes

// template instantiations are at the end of the file

// rule-of-thumb for inline directives:
// only inline constructors if you KNOW that they will not be be needed
// outside of this module, because we don't have a means to export
// inline methods other than defining in the header or using
// -fkeep-inline-functions

#include <iostream>
#include <vector>

#include "art_parser_debug.h"
#include "art_macros.h"
#include "art_switches.h"
#include "art_parser_template_methods.h"

#include "art_parser_expr.h"
#include "art_parser_definition.h"
#include "art_parser_formal.h"

#include "art_symbol_table.h"
#include "art_object_definition.h"
#include "art_object_expr.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// debug section

//=============================================================================
namespace ART {
namespace parser {
using namespace std;

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

inline
signature_base::~signature_base() {
}

//=============================================================================
// class user_data_type_prototype method definitions

CONSTRUCTOR_INLINE
user_data_type_prototype::user_data_type_prototype(
	const template_formal_decl_list* tf, const token_keyword* df, 
	const token_identifier* n, const token_string* dp, 
	const concrete_type_ref* b, 
	const data_param_decl_list* p, const token_char* s) :
		prototype(), 
		user_data_type_signature(tf, df, n, dp, b, p), 
		semi(s) {
	assert(semi);
}

DESTRUCTOR_INLINE
user_data_type_prototype::~user_data_type_prototype() {
}

ostream&
user_data_type_prototype::what(ostream& o) const {
	return o << "(user-data-type-proto)";
}

line_position
user_data_type_prototype::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return id->leftmost();
}

line_position
user_data_type_prototype::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else		return params->rightmost();
}

never_const_ptr<object>
user_data_type_prototype::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"user_data_type_prototype::check_build(...): " << endl;
	)
#if 0
	never_const_ptr<object> o;
	c->declare_datatype(*this);	// really only need name
	// visit template formals
	o = bdt->check_build(c);
	assert(o);
	o = params->check_build(c);
	assert(o);
	c->close_datatype_definition();
#else
	cerr << "TO DO: user_data_type_prototype::check_build();" << endl;
#endif
	return c->top_namespace();
}

//=============================================================================
// class user_data_type_def method definitions

CONSTRUCTOR_INLINE
user_data_type_def::user_data_type_def(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const concrete_type_ref* b, 
		const data_param_decl_list* p, const token_char* l, 
		const language_body* s, const language_body* g,
		const token_char* r) :
		definition(), 
		user_data_type_signature(tf, df, n, dp, b, p), 
		lb(l), setb(s), getb(g), rb(r) {
	assert(lb); assert(setb); assert(getb); assert(rb);
}

DESTRUCTOR_INLINE
user_data_type_def::~user_data_type_def() {
}

ostream&
user_data_type_def::what(ostream& o) const {
	return o << "(user-data-type-def)";
}

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

/*** unveil later...
never_const_ptr<object>
user_data_type_def::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o;
	TRACE_CHECK_BUILD(
		cerr << c->auto_indent() <<
			"user_data_type_def::check_build(...): " << endl;
	)
	c->open_datatype(*this);	// really only need name
	o = bdt->check_build(c);
	assert(o);
	o = params->check_build(c);
	assert(o);
//	setb->check_build(c);
//	getb->check_build(c);
	c->close_datatype_definition();
	return c->top_namespace();
}
***/

//=============================================================================
// class enum_signature method definitions

/**
	Basic constructor.  
	Enums cannot be templated, so we pass NULL to the parent constructor.  
 */
enum_signature::enum_signature(const token_keyword* e, 
		const token_identifier* i) :
		signature_base(NULL, i), en(e) {
}

enum_signature::~enum_signature() {
}

/**
	Just constructs and returns an enumeration definition with 
	the appropriate signature.  There's really no signature, just name.  
 */
never_const_ptr<object>
enum_signature::check_build(never_ptr<context> c) const {
	excl_ptr<definition_base> ed(
		new enum_datatype_def(c->get_current_namespace(), *id));
	// elsewhere would need to add template and port formals
	// no need for set_current_prototype
	never_const_ptr<definition_base> ret(c->add_declaration(ed));
	if (!ret) {
		// error handling?
		cerr << where() << endl;
		exit(1);
	}
	return ret;
//	return c->set_current_prototype(ed);
}

//=============================================================================
// class enum_prototype method definitions

enum_prototype::enum_prototype(const token_keyword* e, 
		const token_identifier* i, const token_char* s) :
		prototype(), enum_signature(e, i), semi(s) {
}

enum_prototype::~enum_prototype() {
}

ostream&
enum_prototype::what(ostream& o) const {
	return o << "(enum-declaration)";
}

line_position
enum_prototype::leftmost(void) const {
	if (en)		return en->leftmost();
	else		return id->leftmost();
}

line_position
enum_prototype::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else		return id->rightmost();
}

/**
	Register this identifier as a enum type.  
	Is acceptable if already declared or defined, 
	Signature has no additional information to match other than name.  
	\return NULL, useless.  
 */
never_const_ptr<object>
enum_prototype::check_build(never_ptr<context> c) const {
	return enum_signature::check_build(c);	// using.
}

//=============================================================================
// class enum_member_list method definitions

enum_member_list::enum_member_list(const token_identifier* i) : parent(i) {
}

enum_member_list::~enum_member_list() {
}

never_const_ptr<object>
enum_member_list::check_build(never_ptr<context> c) const {
	// use current_open_definition
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		bool b = c->add_enum_member(**i);
		if (!b) {
			// should've exited by now...
			cerr << "BAD fang!" << endl;
		}
	}
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class enum_def method definitions

enum_def::enum_def(const token_keyword* e, const token_identifier* i,
		const enum_member_list* m) :
		definition(), enum_signature(e, i), members(m) {
	assert(members);
}

enum_def::~enum_def() {
}

ostream&
enum_def::what(ostream& o) const {
	return o << "(enum-definition)";
}

line_position
enum_def::leftmost(void) const {
	if (en)		return en->leftmost();
	else		return id->leftmost();
}

line_position
enum_def::rightmost(void) const {
	return members->rightmost();
}

/**
	Reminder: don't forget to reset_current_prototype()
 */
never_const_ptr<object>
enum_def::check_build(never_ptr<context> c) const {
	never_const_ptr<object> o(enum_signature::check_build(c));
	if (!o)	return never_const_ptr<object>(NULL);
	// lookup and open definition
	c->open_enum_definition(*id);	// marks as defined
	o = members->check_build(c);	// use current_open_definition
		// always returns NULL
#if 0
	if (!o) {
		cerr << where() << endl;
		exit(1);
	}
#endif
	c->close_enum_definition();
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class user_chan_type_signature method definitions

CONSTRUCTOR_INLINE
user_chan_type_signature::user_chan_type_signature(
		const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n,
		const token_string* dp, 
		const chan_type* b, const data_param_decl_list* p) :
		signature_base(tf, n), 
		def(df), dop(dp), bct(b), params(p) {
	assert(def); assert(dop);
	assert(bct); assert(params);
}

DESTRUCTOR_INLINE
user_chan_type_signature::~user_chan_type_signature() {
}

/**
	Just constructs and returns an channel definition with 
	the appropriate signature.
 */
never_const_ptr<object>
user_chan_type_signature::check_build(never_ptr<context> c) const {
	cerr << "user_chan_type_signature::check_build() FINISH ME!" << endl;
	excl_ptr<definition_base> dd(
		new user_def_chan(c->get_current_namespace(), *id));
	return c->set_current_prototype(dd);
}


//=============================================================================
// class user_chan_type_prototype method definitions

CONSTRUCTOR_INLINE
user_chan_type_prototype::user_chan_type_prototype(
		const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const chan_type* b, 
		const data_param_decl_list* p, const token_char* s) :
		prototype(), 
		user_chan_type_signature(tf, df, n, dp, b, p), 
		semi(s) {
	assert(semi);
}

DESTRUCTOR_INLINE
user_chan_type_prototype::~user_chan_type_prototype() {
}

ostream&
user_chan_type_prototype::what(ostream& o) const {
	return o << "(user-chan-type-proto)";
}

line_position
user_chan_type_prototype::leftmost(void) const {
	if (def)	return def->leftmost();
	else		return id->leftmost();
}

line_position
user_chan_type_prototype::rightmost(void) const {
	if (semi)	return semi->rightmost();
	else		return params->rightmost();
}

//=============================================================================
// class user_chan_type_def method definitions

CONSTRUCTOR_INLINE
user_chan_type_def::user_chan_type_def(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const chan_type* b, 
		const data_param_decl_list* p, const token_char* l, 
		const language_body* s, const language_body* g, 
		const token_char* r) :
		definition(), 
		user_chan_type_signature(tf, df, n, dp, b, p), 
		lb(l), sendb(s), recvb(g), rb(r) {
	assert(lb); assert(sendb); assert(recvb); assert(rb);
}

DESTRUCTOR_INLINE
user_chan_type_def::~user_chan_type_def() {
}

ostream&
user_chan_type_def::what(ostream& o) const {
	return o << "(user-chan-type-def)";
}

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

//=============================================================================
// class process_signature method definitions

CONSTRUCTOR_INLINE
process_signature::process_signature(const template_formal_decl_list* tf, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p) :
		signature_base(tf,i), def(d), ports(p) {
	assert(def);
	assert(ports);		// not any more!
}

DESTRUCTOR_INLINE
process_signature::~process_signature() {
}

const token_identifier&
process_signature::get_name(void) const {
	return *id;
}

/**
	FINISH ME.
	Creates and returns a process definition objects with signature.
	Works in temporary space of new definition, then compares with
	previous signature if found.  
	\param c context is modifiable in case new concrete-types update
		the type-cache.  
 */
never_const_ptr<object>
process_signature::check_build(never_ptr<context> c) const {
//	cerr << "process_signature::check_build() FINISH ME!" << endl;
	excl_ptr<definition_base> ret(
		new process_definition(c->get_current_namespace(), *id));
	c->set_current_prototype(ret);
	if (temp_spec) {
		never_const_ptr<object> o(temp_spec->check_build(c));
		if (!o) {
			cerr << temp_spec->where() << endl;
			exit(1);
		}
	}
	if (ports && !ports->empty()) {
		never_const_ptr<object> o(ports->check_build(c));
		if (!o) {
			cerr << ports->where() << endl;
			exit(1);
		}
	}
	// this checks for conflicts in definitions.  
	return c->add_declaration(c->get_current_prototype());
//	return c->set_current_prototype(ret);
}

//=============================================================================
// class process_prototype method definitions

CONSTRUCTOR_INLINE
process_prototype::process_prototype(const template_formal_decl_list* tf, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p, const token_char* s) :
		prototype(),
		process_signature(tf, d, i, p), 
		semi(s) {
	assert(semi);
}

DESTRUCTOR_INLINE
process_prototype::~process_prototype() {
}

ostream&
process_prototype::what(ostream& o) const {
	return o << "(process-prototype)";
}

line_position
process_prototype::leftmost(void) const {
	// temp_spec->leftmost()?
	return def->leftmost();
}

line_position
process_prototype::rightmost(void) const {
	return semi->rightmost();
}

/**
	TO DO: complete me!
 */
never_const_ptr<object>
process_prototype::check_build(never_ptr<context> c) const {
	return process_signature::check_build(c);
}

//=============================================================================
// class process_def method definitions

CONSTRUCTOR_INLINE
process_def::process_def(const template_formal_decl_list* tf, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p, const definition_body* b) :
		definition(),
		process_signature(tf, d, i, p), 
		body(b) {
	assert(body);		// body may be empty, is is not NULL
}

DESTRUCTOR_INLINE
process_def::~process_def() {
}

ostream&
process_def::what(ostream& o) const {
	return o << "(process-definition)";
}

line_position
process_def::leftmost(void) const {
	return def->leftmost();
}

line_position
process_def::rightmost(void) const {
	return body->rightmost();
}

/**
	To do: port_formals in process_signature...
 */
never_const_ptr<object>
process_def::check_build(never_ptr<context> c) const {
	return node::check_build(c);		// temporary
#if 0
	never_const_ptr<object> o;
	TRACE_CHECK_BUILD(
		idt->what(cerr << c->auto_indent() << 
			"process_def::check_build(...): ");
	)
	o = idt->check_build(c);
	assert(o);
	o = ports->check_build(c);
	assert(o);

	c->open_process(get_name());		// will handle errors
	o = body->check_build(c);
	assert(o);
	c->close_process_definition();
	// nothing better to do
	return c->top_namespace();
#endif
}

//=============================================================================
// class user_data_type_signature method definitions

user_data_type_signature::user_data_type_signature(
		const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, 
		const concrete_type_ref* b, 
		const data_param_decl_list* p) :
		signature_base(tf,n), def(df), dop(dp), bdt(b), params(p) {
	assert(def); assert(dop); assert(bdt); assert(params);
}

user_data_type_signature::~user_data_type_signature() {
}

/**
	Just constructs and returns an datatype definition with 
	the appropriate signature.
 */
never_const_ptr<object>
user_data_type_signature::check_build(never_ptr<context> c) const {
	cerr << "user_data_type_signature::check_build() FINISH ME!" << endl;
	excl_ptr<definition_base> dd(
		new user_def_datatype(c->get_current_namespace(), *id));
	return c->set_current_prototype(dd);
}


//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE


