/**
	\file "common/TODO.h"
	Macros for maintainer's notes in code.  
	$Id: TODO.h,v 1.1.2.3 2005/08/16 03:48:49 fang Exp $
 */

#ifndef	__COMMON_TODO_H__
#define	__COMMON_TODO_H__

#include "util/macros.h"

/**
	Macro for maintainer and developers to remind oneself of TODO.
 */
#define	FINISH_ME(who)							\
	__finish_me_where( #who, __PRETTY_FUNCTION__, __FILE__, __LINE__)

/**
	TODO reminder and also exits the program.  
 */
#define	FINISH_ME_EXIT(who)						\
	FINISH_ME(who);							\
	THROW_EXIT;

extern
void
__finish_me_where(const char* who, const char* fun,
	const char* file, const size_t line);

#endif	// __COMMON_TODO_H__

