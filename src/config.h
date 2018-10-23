/**
	\file "config.h"
	Convenient wrapper for the one automatically generated
	by configure.
	$Id: config.h,v 1.5 2006/04/08 18:53:48 fang Exp $
 */

#ifndef	__CONFIG_H__
#define	__CONFIG_H__

/***
	NOTE: the generated "config.h" doesn't have include-once protection!
	One can predefine -DIGNORE_CONFIG_H to suppress it, but good luck 
	trying to compile everything without it!  (Decent chance of success, 
	since config.h wasn't depended upon for a long time.)
***/

#if	defined(HAVE_CONFIG_H) && !defined(IGNORE_CONFIG_H)
#include "__config__.h"
#endif

/**
	Define this if extra using declarations are needed for overloaded
	virtual functions, detected by configure.
	Works around differences between gcc and clang.
 */
#define	OVERLOAD_VIRTUAL_USING	defined(OVERLOAD_VIRTUAL_USING_ALLOWED) && defined(OVERLOAD_VIRTUAL_USING_REQUIRED)

/**
	Define this if friend function declarations in classes want
	the functions to be declared with their home namespaces.
	Works around differences between gcc (various versions) and clang.
 */
#if	defined(FRIEND_FUNCTION_HOME_NAMESPACE_ALLOWED) && defined(FRIEND_FUNCTION_HOME_NAMESPACE_REQUIRED)
#define	FRIEND_FUNCTION_HOME_NAMESPACE  1
#else
#define	FRIEND_FUNCTION_HOME_NAMESPACE  0
#endif

#ifdef	const
#error	Detected preprocessor definition of const.  \
	This means that a configure test found const non-ANSI-conforming.  \
	Sorry, but I refuse to let you compile this project with broken const.
#endif

#endif	/* __CONFIG_H__ */

