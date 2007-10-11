/**
	\file "Object/type/channel_direction_enum.h"
 	$Id: channel_direction_enum.h,v 1.1.2.1 2007/10/11 02:52:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_CHANNEL_DIRECTION_ENUM_H__
#define	__HAC_OBJECT_TYPE_CHANNEL_DIRECTION_ENUM_H__

#include "Object/devel_switches.h"	// for ENABLE_SHARED_CHANNELS

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
	CHANNEL_TYPE_BIDIRECTIONAL = CHANNEL_TYPE_SEND | CHANNEL_TYPE_RECEIVE
#if ENABLE_SHARED_CHANNELS
	, CHANNEL_SEND_SHARED_FLAG = 0x04
	, CHANNEL_RECEIVE_SHARED_FLAG = 0x08
	, CHANNEL_TYPE_SEND_SHARED =
		CHANNEL_TYPE_SEND | CHANNEL_SEND_SHARED_FLAG
	, CHANNEL_TYPE_RECEIVE_SHARED =
		CHANNEL_TYPE_RECEIVE | CHANNEL_RECEIVE_SHARED_FLAG
//	, CHANNEL_TYPE_BIDIRECTIONAL_SHARED
#endif
};

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_CHANNEL_DIRECTION_ENUM_H__

