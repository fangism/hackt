/**
	\file "Object/nonmeta_channel_manipulator.hh"
	Helper classes for manipulating channel fields at run-time.  
	$Id: nonmeta_channel_manipulator.hh,v 1.5 2007/08/28 04:53:59 fang Exp $
 */

#ifndef	__HAC_OBJECT_NONMETA_CHANNEL_MANIPULATOR_H__
#define	__HAC_OBJECT_NONMETA_CHANNEL_MANIPULATOR_H__

#include <iosfwd>
#include "Object/type/canonical_type_fwd.hh"
#include "Object/nonmeta_variable.hh"
#include "Object/ref/reference_set.hh"
#include "util/memory/pointer_classes_fwd.hh"

namespace HAC {
namespace entity {
struct bool_tag;
struct int_tag;
struct enum_tag;
class data_expr;
class data_nonmeta_instance_reference;
class nonmeta_context_base;
class ChannelData;
using util::memory::count_ptr;
using std::ostream;

//=============================================================================
/**
	ChannelData-writing iterator, liks std::back_insert_iterator.
 */
template <class Tag>
struct channel_data_writer_base {
protected:
	typedef	typename channel_data_base<Tag>::member_variable_type
					member_variable_type;
	/**
		Reference to the array to 'push_back' into.  
	 */
	channel_data_base<Tag>&		data;
	/**
		post-increment value over-writing iterator.  
		Initialized to beginning of valarray.  
	 */
	member_variable_type*		iter;

	explicit
	channel_data_writer_base(channel_data_base<Tag>& d) :
		data(d), iter(&data.member_fields[0]) { }

	/**
		optional: assert iterator position checks upon destruction
		\post never written out of bounds.  
	 */
	~channel_data_writer_base();

};	// end struct channel_data_writer_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	ChannelData-reading iterator.
	This reads fields of a built-in channel-type and
	writes their values to the referenced data instances.
	Need to also track global indices of modified references.
 */
template <class Tag>
struct channel_data_reader_base {
protected:
	typedef	typename channel_data_base<Tag>::member_variable_type
					member_variable_type;
	/**
		Read-only reference to channel data.  
	 */
	const channel_data_base<Tag>&		data;
	/**
		Read-only iterator over valarray fields.  
	 */
	const member_variable_type*		iter;

	explicit
	channel_data_reader_base(const channel_data_base<Tag>& d) :
		data(d), iter(&data.member_fields[0]) { }

	/**
		optional: assert iterator position checks upon destruction
		\post never read out of bounds.  
	 */
	~channel_data_reader_base();

};	// end struct channel_data_reader_base

//-----------------------------------------------------------------------------
/**
	Pack of writing (output) iterators over channel field members.  
 */
struct channel_data_writer :
		public channel_data_writer_base<bool_tag>, 
		public channel_data_writer_base<int_tag>, 
		public channel_data_writer_base<enum_tag> {
	explicit
	channel_data_writer(ChannelData& d) :
		channel_data_writer_base<bool_tag>(d), 
		channel_data_writer_base<int_tag>(d), 
		channel_data_writer_base<enum_tag>(d) {
	}

	// default destructor (of base classes)
	~channel_data_writer();

	/**
		\return iterator by reference.
	 */
	template <class Tag>
	typename channel_data_writer_base<Tag>::member_variable_type*&
	iter_ref(void) { return channel_data_writer_base<Tag>::iter; }

};	// end struct channel_data_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pack of reading (input) iterators over channel field members.
 */
struct channel_data_reader :
		public channel_data_reader_base<bool_tag>, 
		public channel_data_reader_base<int_tag>, 
		public channel_data_reader_base<enum_tag> {
	explicit
	channel_data_reader(const ChannelData& d) :
		channel_data_reader_base<bool_tag>(d), 
		channel_data_reader_base<int_tag>(d), 
		channel_data_reader_base<enum_tag>(d) {
	}

	// default destructor (of base classes)
	~channel_data_reader();

	/**
		\return const_iterator by reference.
	 */
	template <class Tag>
	const typename channel_data_reader_base<Tag>::member_variable_type*&
	iter_ref(void) { return channel_data_reader_base<Tag>::iter; }

};	// end struct channel_data_reader

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor that prints out data, using canonical data-type list.  
 */
struct channel_data_dumper : public channel_data_reader {
	ostream&			out;
	bool				first;

	channel_data_dumper(const ChannelData& d, ostream& o) :
		channel_data_reader(d), out(o), first(true) { }

	void
	operator () (const canonical_generic_datatype&);

};	// end struct channel_data_dumper.

//=============================================================================
/**
	Helper functor that:
	1) evaluates nonmeta expressions
	2) packs values into the packed-fields of channel-data.  
	Used by channel_send::execute().
 */
class nonmeta_expr_evaluator_channel_writer {
	const nonmeta_context_base&		context;
	channel_data_writer			writer;
public:
	nonmeta_expr_evaluator_channel_writer(const nonmeta_context_base& c, 
		ChannelData& d) : context(c), writer(d) { }

	void
	operator () (const count_ptr<const data_expr>&);

};	// end class nonmeta_expr_evaluator_channel_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper functor that:
	1) resolves nonmeta data references
	2) extracts packed values from channel-data.
	Used by channel_receive::execute()
 */
class nonmeta_reference_lookup_channel_reader {
	const nonmeta_context_base&		context;
	typedef	global_references_set		updates_type;
	updates_type&				updates;
	channel_data_reader			reader;
public:
	nonmeta_reference_lookup_channel_reader(const nonmeta_context_base& c, 
		const ChannelData& d, updates_type& u) :
		context(c), updates(u), reader(d) { }

	void
	operator () (const count_ptr<const data_nonmeta_instance_reference>&);

};	// end class nonmeta_reference_lookup_channel_reader

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_NONMETA_CHANNEL_MANIPULATOR_H__
