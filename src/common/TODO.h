/**
	\file "common/TODO.h"
	Macros for maintainer's notes in code.  
	$Id: TODO.h,v 1.1.2.1 2005/08/11 03:40:55 fang Exp $
 */

#ifndef	__COMMON_TODO_H__
#define	__COMMON_TODO_H__

#include "util/macros.h"

#define	FINISH_ME_WHERE(who, fn, file, line)				\
	std::cerr << #who ", finish writing " 				\
		#fn " at " #file ":" #line << std::endl
		

/**
	Macro for maintainer and developers to remind oneself of TODO.
 */
#define	FINISH_ME(who)							\
	FINISH_ME_WHERE(who, __PRETTY_FUNCTION__, __FILE__, __LINE__)

/**
	TODO reminder and also exits the program.  
 */
#define	FINISH_ME_EXIT(who)						\
	FINISH_ME(who);							\
	THROW_EXIT;

#endif	// __COMMON_TODO_H__

