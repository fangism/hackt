/**
	\file "art_object_inst_ref_data.cc"
	Method definitions for datatype instance reference classes.
	$Id: art_object_inst_ref_data.cc,v 1.4.16.2.2.1.2.1 2005/02/20 20:35:52 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_REF_DATA_CC__
#define	__ART_OBJECT_INST_REF_DATA_CC__

#include "art_object_inst_ref_data.h"
#include "art_object_instance_bool.h"
#include "art_object_instance_int.h"
#include "art_object_instance_enum.h"
#include "art_object_instance_struct.h"
#include "art_object_connect.h"
#include "art_object_inst_ref.tcc"
#include "art_object_member_inst_ref.tcc"

#include "art_object_type_hash.h"
#include "persistent_object_manager.tcc"

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_instance_reference, 
		SIMPLE_DINT_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_instance_reference, 
		SIMPLE_DBOOL_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_instance_reference, 
		SIMPLE_ENUM_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::datastruct_instance_reference, 
		SIMPLE_STRUCT_INSTANCE_REFERENCE_TYPE_KEY)

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_member_instance_reference, 
		MEMBER_DINT_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_member_instance_reference, 
		MEMBER_DBOOL_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_member_instance_reference, 
		MEMBER_ENUM_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::datastruct_member_instance_reference, 
		MEMBER_STRUCT_INSTANCE_REFERENCE_TYPE_KEY)

SPECIALIZE_UTIL_WHAT(
	ART::entity::int_instance_reference, "int-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::bool_instance_reference, "bool-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::enum_instance_reference, "enum-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::datastruct_instance_reference, "struct-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::int_member_instance_reference, "int-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::bool_member_instance_reference, "bool-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::enum_member_instance_reference, "enum-member-inst-ref")
SPECIALIZE_UTIL_WHAT(
	ART::entity::datastruct_member_instance_reference,
		"struct-member-inst-ref")

}	// end namespace util

namespace ART {
namespace entity {
#include "using_ostream.h"
using std::istream;
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class int_instance_reference method definitions

#if !USE_INSTANCE_REFERENCE_TEMPLATE
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
int_instance_reference::int_instance_reference() :
		parent_type(), int_inst_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_instance_reference::int_instance_reference(
		const never_ptr<const instance_collection_type> iic) :
		parent_type(iic->current_collection_state()), 
		int_inst_ref(iic) {
	NEVER_NULL(int_inst_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_instance_reference::~int_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_instance_reference::what(ostream& o) const {
	return o << "int-inst-ref";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
int_instance_reference::get_inst_base(void) const {
	return int_inst_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<aliases_connection_base>
int_instance_reference::make_aliases_connection_private(void) const {
	return excl_ptr<aliases_connection_base>(new alias_connection_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_reference::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
	int_inst_ref->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	this->collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
int_instance_reference::construct_empty(const int) {
	return new int_instance_reference();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_reference::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, int_inst_ref);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_reference::write_object(
		const persistent_object_manager& m, ostream& f) const {
	this->write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_instance_reference::load_object_base(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, int_inst_ref);
	NEVER_NULL(int_inst_ref);
	m.load_object_once(
		const_cast<instance_collection_type*>(&*int_inst_ref));
	parent_type::load_object_base(m, f);
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
void
int_instance_reference::load_object(const persistent_object_manager& m, 
		istream& f) {
	this->load_object_base(m, f);
}

//=============================================================================
// class bool_instance_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
bool_instance_reference::bool_instance_reference() :
		parent_type(), bool_inst_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool_instance_reference::bool_instance_reference(
		const never_ptr<const instance_collection_type> bic) :
		parent_type(bic->current_collection_state()), 
		bool_inst_ref(bic) {
	NEVER_NULL(bool_inst_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance_reference::~bool_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_instance_reference::what(ostream& o) const {
	return o << "bool-inst-ref";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
bool_instance_reference::get_inst_base(void) const {
	return bool_inst_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<aliases_connection_base>
bool_instance_reference::make_aliases_connection_private(void) const {
	return excl_ptr<aliases_connection_base>(new alias_connection_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_reference::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
	bool_inst_ref->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	this->collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
bool_instance_reference::construct_empty(const int) {
	return new bool_instance_reference();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_reference::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, bool_inst_ref);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_reference::write_object(
		const persistent_object_manager& m, ostream& f) const {
	this->write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance_reference::load_object_base(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, bool_inst_ref);
	NEVER_NULL(bool_inst_ref);
	m.load_object_once(
		const_cast<instance_collection_type*>(&*bool_inst_ref));
	parent_type::load_object_base(m, f);
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
void
bool_instance_reference::load_object(const persistent_object_manager& m, 
		istream& f) {
	this->load_object_base(m, f);
}

//=============================================================================
// class enum_instance_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
enum_instance_reference::enum_instance_reference() :
		parent_type(), enum_inst_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

enum_instance_reference::enum_instance_reference(
		const never_ptr<const instance_collection_type> eic) :
		parent_type(eic->current_collection_state()), 
		enum_inst_ref(eic) {
	NEVER_NULL(enum_inst_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum_instance_reference::~enum_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
enum_instance_reference::what(ostream& o) const {
	return o << "enum-inst-ref";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
enum_instance_reference::get_inst_base(void) const {
	return enum_inst_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<aliases_connection_base>
enum_instance_reference::make_aliases_connection_private(void) const {
	return excl_ptr<aliases_connection_base>(new alias_connection_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance_reference::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
	enum_inst_ref->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	this->collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
enum_instance_reference::construct_empty(const int) {
	return new enum_instance_reference();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance_reference::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, enum_inst_ref);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance_reference::write_object(
		const persistent_object_manager& m, ostream& f) const {
	this->write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance_reference::load_object_base(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, enum_inst_ref);
	NEVER_NULL(enum_inst_ref);
	m.load_object_once(const_cast<instance_collection_type*>(
		&*enum_inst_ref));
	parent_type::load_object_base(m, f);
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
void
enum_instance_reference::load_object(const persistent_object_manager& m, 
		istream& f) {
	this->load_object_base(m, f);
}

//=============================================================================
// class datastruct_instance_reference method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
datastruct_instance_reference::datastruct_instance_reference() :
		parent_type(), struct_inst_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

datastruct_instance_reference::datastruct_instance_reference(
		const never_ptr<const instance_collection_type> sic) :
		parent_type(sic->current_collection_state()), 
		struct_inst_ref(sic) {
	NEVER_NULL(struct_inst_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datastruct_instance_reference::~datastruct_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
datastruct_instance_reference::what(ostream& o) const {
	return o << "struct-inst-ref";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
datastruct_instance_reference::get_inst_base(void) const {
	return struct_inst_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
excl_ptr<aliases_connection_base>
datastruct_instance_reference::make_aliases_connection_private(void) const {
	return excl_ptr<aliases_connection_base>(new alias_connection_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
datastruct_instance_reference::collect_transient_info_base(
		persistent_object_manager& m) const {
	parent_type::collect_transient_info_base(m);
	struct_inst_ref->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
datastruct_instance_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	this->collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
datastruct_instance_reference::construct_empty(const int) {
	return new datastruct_instance_reference();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
datastruct_instance_reference::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, struct_inst_ref);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
datastruct_instance_reference::write_object(
		const persistent_object_manager& m, ostream& f) const {
	this->write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
datastruct_instance_reference::load_object_base(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, struct_inst_ref);
	NEVER_NULL(struct_inst_ref);
	m.load_object_once(
		const_cast<instance_collection_type*>(&*struct_inst_ref));
	parent_type::load_object_base(m, f);
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
void
datastruct_instance_reference::load_object(const persistent_object_manager& m, 
		istream& f) {
	this->load_object_base(m, f);
}
#endif	// USE_INSTANCE_REFERENCE_TEMPLATE

//=============================================================================
// explicit template instantiations

template class
instance_reference<bool_instance_collection, datatype_instance_reference>;
template class
instance_reference<int_instance_collection, datatype_instance_reference>;
template class
instance_reference<enum_instance_collection, datatype_instance_reference>;
template class
instance_reference<struct_instance_collection, datatype_instance_reference>;

template class member_instance_reference<bool_instance_reference>;
template class member_instance_reference<int_instance_reference>;
template class member_instance_reference<enum_instance_reference>;
template class member_instance_reference<datastruct_instance_reference>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_DATA_CC__

