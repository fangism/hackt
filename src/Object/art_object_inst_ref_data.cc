/**
	\file "art_object_inst_ref_data.cc"
	Method definitions for datatype instance reference classes.
	$Id: art_object_inst_ref_data.cc,v 1.3 2005/01/06 17:44:53 fang Exp $
 */

#include "art_object_inst_ref_data.h"
#include "art_object_instance_bool.h"
#include "art_object_instance_int.h"
#include "art_object_instance_enum.h"
#include "art_object_instance_struct.h"

#include "art_object_type_hash.h"
#include "persistent_object_manager.tcc"

namespace ART {
namespace entity {
#include "using_ostream.h"
using std::istream;
using util::write_value;
using util::read_value;

//=============================================================================
// class int_instance_reference method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(int_instance_reference, 
		SIMPLE_DINT_INSTANCE_REFERENCE_TYPE_KEY)

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
#if 0
/**
	Obsolete constructor...
 */
int_instance_reference::int_instance_reference(
		const never_ptr<const instance_collection_type> iic, 
		excl_ptr<index_list>& i) :
		parent_type(i, iic->current_collection_state()), 
		int_inst_ref(iic) {
	NEVER_NULL(int_inst_ref);
}
#endif

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
void
int_instance_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, SIMPLE_DINT_INSTANCE_REFERENCE_TYPE_KEY)) {
	parent_type::collect_transient_info_base(m);
	int_inst_ref->collect_transient_info(m);
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
int_instance_reference::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, int_inst_ref);
#if 0
	write_instance_collection_state(f);
	m.write_pointer(f, array_indices);
#else
	parent_type::write_object_base(m, f);
#endif
	WRITE_OBJECT_FOOTER(f);
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
int_instance_reference::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, int_inst_ref);
	NEVER_NULL(int_inst_ref);
	const_cast<instance_collection_type&>(*int_inst_ref).load_object(m);
#if 0
	load_instance_collection_state(f);
	m.read_pointer(f, array_indices);
	if (array_indices)
		array_indices->load_object(m);
#else
	parent_type::load_object_base(m, f);
#endif
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class bool_instance_reference method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(bool_instance_reference, 
		SIMPLE_DBOOL_INSTANCE_REFERENCE_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
bool_instance_reference::bool_instance_reference() :
		parent_type(), bool_inst_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool_instance_reference::bool_instance_reference(
		const never_ptr<const instance_collection_type> iic) :
		parent_type(iic->current_collection_state()), 
		bool_inst_ref(iic) {
	NEVER_NULL(bool_inst_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
bool_instance_reference::bool_instance_reference(
		const never_ptr<const instance_collection_type> iic, 
		excl_ptr<index_list>& i) :
		parent_type(i, iic->current_collection_state()), 
		bool_inst_ref(iic) {
	NEVER_NULL(bool_inst_ref);
}
#endif

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
void
bool_instance_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, SIMPLE_DBOOL_INSTANCE_REFERENCE_TYPE_KEY)) {
	parent_type::collect_transient_info_base(m);
	bool_inst_ref->collect_transient_info(m);
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
bool_instance_reference::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, bool_inst_ref);
#if 0
	write_instance_collection_state(f);
	m.write_pointer(f, array_indices);
#else
	parent_type::write_object_base(m, f);
#endif
	WRITE_OBJECT_FOOTER(f);
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
bool_instance_reference::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, bool_inst_ref);
	NEVER_NULL(bool_inst_ref);
	const_cast<instance_collection_type&>(*bool_inst_ref).load_object(m);
#if 0
	load_instance_collection_state(f);
	m.read_pointer(f, array_indices);
	if (array_indices)
		array_indices->load_object(m);
#else
	parent_type::load_object_base(m, f);
#endif
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class enum_instance_reference method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(enum_instance_reference, 
		SIMPLE_ENUM_INSTANCE_REFERENCE_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
enum_instance_reference::enum_instance_reference() :
		parent_type(), enum_inst_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

enum_instance_reference::enum_instance_reference(
		const never_ptr<const instance_collection_type> iic) :
		parent_type(iic->current_collection_state()), 
		enum_inst_ref(iic) {
	NEVER_NULL(enum_inst_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
enum_instance_reference::enum_instance_reference(
		const never_ptr<const instance_collection_type> iic, 
		excl_ptr<index_list> i) :
		parent_type(i, iic->current_collection_state()), 
		enum_inst_ref(iic) {
	NEVER_NULL(enum_inst_ref);
}
#endif

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
void
enum_instance_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, SIMPLE_ENUM_INSTANCE_REFERENCE_TYPE_KEY)) {
	parent_type::collect_transient_info_base(m);
	enum_inst_ref->collect_transient_info(m);
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
enum_instance_reference::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, enum_inst_ref);
#if 0
	write_instance_collection_state(f);
	m.write_pointer(f, array_indices);
#else
	parent_type::write_object_base(m, f);
#endif
	WRITE_OBJECT_FOOTER(f);
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
enum_instance_reference::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, enum_inst_ref);
	NEVER_NULL(enum_inst_ref);
	const_cast<instance_collection_type&>(*enum_inst_ref).load_object(m);
#if 0
	load_instance_collection_state(f);
	m.read_pointer(f, array_indices);
	if (array_indices)
		array_indices->load_object(m);
#else
	parent_type::load_object_base(m, f);
#endif
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
// class datastruct_instance_reference method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(datastruct_instance_reference, 
		SIMPLE_STRUCT_INSTANCE_REFERENCE_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
datastruct_instance_reference::datastruct_instance_reference() :
		parent_type(), struct_inst_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

datastruct_instance_reference::datastruct_instance_reference(
		const never_ptr<const instance_collection_type> iic) :
		parent_type(iic->current_collection_state()), 
		struct_inst_ref(iic) {
	NEVER_NULL(struct_inst_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
datastruct_instance_reference::datastruct_instance_reference(
		const never_ptr<const instance_collection_type> iic, 
		excl_ptr<index_list> i) :
		parent_type(i, iic->current_collection_state()), 
		struct_inst_ref(iic) {
	NEVER_NULL(struct_inst_ref);
}
#endif

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
void
datastruct_instance_reference::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, SIMPLE_STRUCT_INSTANCE_REFERENCE_TYPE_KEY)) {
	parent_type::collect_transient_info_base(m);
	struct_inst_ref->collect_transient_info(m);
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
datastruct_instance_reference::write_object(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	WRITE_POINTER_INDEX(f, m);
	m.write_pointer(f, struct_inst_ref);
#if 0
	write_instance_collection_state(f);
	m.write_pointer(f, array_indices);
#else
	parent_type::write_object_base(m, f);
#endif
	WRITE_OBJECT_FOOTER(f);
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
datastruct_instance_reference::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	STRIP_POINTER_INDEX(f, m);
	m.read_pointer(f, struct_inst_ref);
	NEVER_NULL(struct_inst_ref);
	const_cast<instance_collection_type&>(*struct_inst_ref).load_object(m);
#if 0
	load_instance_collection_state(f);
	m.read_pointer(f, array_indices);
	if (array_indices)
		array_indices->load_object(m);
#else
	parent_type::load_object_base(m, f);
#endif
	STRIP_OBJECT_FOOTER(f);
}
// else already visited
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

