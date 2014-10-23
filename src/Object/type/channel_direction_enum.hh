/**
	\file "Object/type/channel_direction_enum.hh"
 	$Id: channel_direction_enum.hh,v 1.3 2011/03/23 00:36:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_CHANNEL_DIRECTION_ENUM_HH__
#define	__HAC_OBJECT_TYPE_CHANNEL_DIRECTION_ENUM_HH__

#include "Object/devel_switches.hh"	// for ENABLE_SHARED_CHANNELS

namespace HAC {
namespace entity {

//-----------------------------------------------------------------------------
/**
	SEND and RECEIVE are not mutually exclusive, 
	but at least one of them should be set.
	When writing this data to object file, use char type.
	Enumeration and flags must fit in 1 byte, of course.  
 */
enum direction_type {
	CHANNEL_TYPE_NULL = 0,
//	CHANNEL_TYPE_BIDIRECTIONAL, 
	CHANNEL_TYPE_SEND = 0x01, 
	CHANNEL_TYPE_RECEIVE = 0x02,
	CHANNEL_TYPE_BIDIRECTIONAL = CHANNEL_TYPE_SEND | CHANNEL_TYPE_RECEIVE,
#if ENABLE_SHARED_CHANNELS
	CHANNEL_SEND_SHARED_FLAG = 0x04,
	CHANNEL_RECEIVE_SHARED_FLAG = 0x08,
	CHANNEL_TYPE_SEND_SHARED =
		CHANNEL_TYPE_SEND | CHANNEL_SEND_SHARED_FLAG,
	CHANNEL_TYPE_RECEIVE_SHARED =
		CHANNEL_TYPE_RECEIVE | CHANNEL_RECEIVE_SHARED_FLAG,
//	CHANNEL_TYPE_BIDIRECTIONAL_SHARED,
#endif
	CHANNEL_DIRECTION_DEFAULT = CHANNEL_TYPE_BIDIRECTIONAL,
	PROCESS_DIRECTION_DEFAULT = CHANNEL_TYPE_NULL
};

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_CHANNEL_DIRECTION_ENUM_HH__

