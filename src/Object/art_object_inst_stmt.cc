/**
	\file "art_object_inst_stmt.cc"
	Method definitions for instantiation statement classes.  
 	$Id: art_object_inst_stmt.cc,v 1.2 2004/12/12 23:32:06 fang Exp $
 */

#include <iostream>
#include <algorithm>

#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_instance_param.h"
#include "art_object_inst_stmt.h"
#include "art_object_inst_ref_base.h"
#include "art_object_expr_base.h"
#include "art_built_ins.h"
#include "art_object_type_hash.h"

#include "persistent_object_manager.tcc"

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// class instantiation_statement method definitions

#if 0
/**	Private empty constructor. */
instantiation_statement::instantiation_statement(void) :
//		inst_base(NULL), type_base(NULL),
		indices(NULL) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_statement::instantiation_statement(
//		never_ptr<instance_collection_base> b, 
//		count_ptr<const fundamental_type_reference> t, 
		const index_collection_item_ptr_type& i) :
//		inst_base(NULL), type_base(t), 
		indices(i) {
//	assert(inst_base);		// attach later...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
instantiation_statement::~instantiation_statement() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instantiation_statement::dump(ostream& o) const {
	count_ptr<const fundamental_type_reference>
		type_base(get_type_ref());
	assert(type_base);
	type_base->dump(o) << " ";
	never_ptr<const instance_collection_base>
		inst_base(get_inst_base());
	if(inst_base) {
		o << inst_base->get_name();
	} else {
		o << "<unknown>";
	}
	if (indices)
		indices->dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instantiation_statement::get_name(void) const {
	never_ptr<const instance_collection_base>
		inst_base(get_inst_base());
	assert(inst_base);
	return inst_base->get_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference-counted pointer to instantiating indices, 
		which contains expressions, and may be null.  
 */
index_collection_item_ptr_type
instantiation_statement::get_indices(void) const {
	return indices;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
instantiation_statement::dimensions(void) const {
	if (indices)
		return indices->dimensions();
	else return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary, should be pure virtual in the end.
 */
void
instantiation_statement::unroll(void) const {
	cerr << "instantiation_statement::unroll(): Fang, finish me!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (indices)
		indices->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement::load_object_base(
		persistent_object_manager& m, istream& i) {
	m.read_pointer(i, indices);
}

//=============================================================================
// class param_instantiation_statement method definitions

#if 0
/**
	Private empty constructor.
 */
param_instantiation_statement::param_instantiation_statement() :
		instantiation_statement() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instantiation_statement::param_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		instantiation_statement(i) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
param_instantiation_statement::~param_instantiation_statement() {
}
#endif

//=============================================================================
// class pbool_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pbool_instantiation_statement, 
	PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pbool_instantiation_statement::pbool_instantiation_statement() :
		object(), param_instantiation_statement(), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation_statement::pbool_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		object(), param_instantiation_statement(i), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation_statement::~pbool_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_instantiation_statement::what(ostream& o) const {
	return o << "pbool-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_instantiation_statement::dump(ostream& o) const {
	return instantiation_statement::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
pbool_instantiation_statement::attach_collection(
		never_ptr<instance_collection_base> i) {
	assert(!inst_base);
	inst_base = i.is_a<collection_type>();
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
pbool_instantiation_statement::get_inst_base(void) {
	assert(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
pbool_instantiation_statement::get_inst_base(void) const {
	assert(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
pbool_instantiation_statement::get_type_ref(void) const {
	return pbool_type_ptr;		// built-in type pointer
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::unroll(void) const {
	assert(inst_base);
	inst_base->instantiate_indices(indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	NEVER_NULL(inst_base);
	// let the scopespace take care of it
	// inst_base->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pbool_instantiation_statement::construct_empty(const int i) {
	return new pbool_instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);
	parent_type::write_object_base(m, f);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst_base);
	parent_type::load_object_base(m, f);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class pint_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_instantiation_statement, 
	PINT_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pint_instantiation_statement::pint_instantiation_statement() :
		object(), param_instantiation_statement(), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation_statement::pint_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		object(), param_instantiation_statement(i), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation_statement::~pint_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_instantiation_statement::what(ostream& o) const {
	return o << "pint-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_instantiation_statement::dump(ostream& o) const {
	return instantiation_statement::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
pint_instantiation_statement::attach_collection(
		never_ptr<instance_collection_base> i) {
	assert(!inst_base);
	inst_base = i.is_a<collection_type>();
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
pint_instantiation_statement::get_inst_base(void) {
	assert(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
pint_instantiation_statement::get_inst_base(void) const {
	assert(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
pint_instantiation_statement::get_type_ref(void) const {
	return pint_type_ptr;		// built-in type pointer
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::unroll(void) const {
	assert(inst_base);
	inst_base->instantiate_indices(indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PINT_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	assert(inst_base);
	// let the scopespace take care of it
	// inst_base->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pint_instantiation_statement::construct_empty(const int i) {
	return new pint_instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);
	parent_type::write_object_base(m, f);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst_base);
	parent_type::load_object_base(m, f);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class process_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(process_instantiation_statement, 
	PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
process_instantiation_statement::process_instantiation_statement() :
		object(), instantiation_statement(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instantiation_statement::process_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		object(), instantiation_statement(i),
		type(t), inst_base(NULL) {
	assert(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instantiation_statement::~process_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instantiation_statement::what(ostream& o) const {
	return o << "process-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instantiation_statement::dump(ostream& o) const {
	return instantiation_statement::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
process_instantiation_statement::attach_collection(
		never_ptr<instance_collection_base> i) {
	assert(!inst_base);
	inst_base = i.is_a<collection_type>();
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
process_instantiation_statement::get_inst_base(void) {
	assert(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
process_instantiation_statement::get_inst_base(void) const {
	assert(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
process_instantiation_statement::get_type_ref(void) const {
	return type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	NEVER_NULL(inst_base);
	NEVER_NULL(type);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
process_instantiation_statement::construct_empty(const int i) {
	return new process_instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.write_pointer(f, type);		NEVER_NULL(type);
	parent_type::write_object_base(m, f);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.read_pointer(f, type);		NEVER_NULL(type);
	parent_type::load_object_base(m, f);
#if 0
	type->load_object(m);
	inst_base->load_object(m);
	if (indices)
		indices->load_object(m);
#endif
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class channel_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(channel_instantiation_statement, 
	CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
channel_instantiation_statement::channel_instantiation_statement() :
		object(), instantiation_statement(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instantiation_statement::channel_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		object(), instantiation_statement(i),
		type(t), inst_base(NULL) {
	assert(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instantiation_statement::~channel_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instantiation_statement::what(ostream& o) const {
	return o << "channel-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instantiation_statement::dump(ostream& o) const {
	return instantiation_statement::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
channel_instantiation_statement::attach_collection(
		never_ptr<instance_collection_base> i) {
	assert(!inst_base);
	inst_base = i.is_a<collection_type>();
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
channel_instantiation_statement::get_inst_base(void) {
	assert(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
channel_instantiation_statement::get_inst_base(void) const {
	assert(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
channel_instantiation_statement::get_type_ref(void) const {
	return type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	assert(inst_base);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
channel_instantiation_statement::construct_empty(const int i) {
	return new channel_instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation_statement::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.write_pointer(f, type);		NEVER_NULL(type);
	parent_type::write_object_base(m, f);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation_statement::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.read_pointer(f, type);		NEVER_NULL(type);
	parent_type::load_object_base(m, f);
#if 0
	type->load_object(m);
	inst_base->load_object(m);
	if (indices)
		indices->load_object(m);
#endif
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
// class data_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(data_instantiation_statement, 
	DATA_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
data_instantiation_statement::data_instantiation_statement() :
		object(), instantiation_statement(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_instantiation_statement::data_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		object(), instantiation_statement(i),
		type(t), inst_base(NULL) {
	assert(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_instantiation_statement::~data_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
data_instantiation_statement::what(ostream& o) const {
	return o << "data-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
data_instantiation_statement::dump(ostream& o) const {
	return instantiation_statement::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
data_instantiation_statement::attach_collection(
		never_ptr<instance_collection_base> i) {
	assert(!inst_base);
	inst_base = i.is_a<collection_type>();
	assert(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
data_instantiation_statement::get_inst_base(void) {
	assert(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
data_instantiation_statement::get_inst_base(void) const {
	assert(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
data_instantiation_statement::get_type_ref(void) const {
	return type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, DATA_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	assert(inst_base);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
data_instantiation_statement::construct_empty(const int i) {
	return new data_instantiation_statement();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_instantiation_statement::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.write_pointer(f, type);		NEVER_NULL(type);
	parent_type::write_object_base(m, f);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_instantiation_statement::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.read_pointer(f, type);		NEVER_NULL(type);
	parent_type::load_object_base(m, f);
#if 0
	type->load_object(m);
	inst_base->load_object(m);
	if (indices)
		indices->load_object(m);
#endif
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

