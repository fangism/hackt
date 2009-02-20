/**
	\file "util/cppcat.h"
	Tricks for concatenating strings with the C-preprocessor.  
	I learned this trick from:
		http://www.slack.net/~ant/cpp/unqiue_name.html
	$Id: cppcat.h,v 1.4 2009/02/20 20:39:42 fang Exp $
 */

#ifndef	__UTIL_CPPCAT_H__
#define	__UTIL_CPPCAT_H__

/**
	Description also copied from the same web-site.

	Sometimes one needs to supply an identifier for something that will 
	never be referenced again. The most common occurrence is a local 
	object of some type whose construction and destruction side-effects 
	are wanted at the beginning and end of the scope it exists in, 
	respectively. Having to come up with a name is both tedious and 
	unclear, since the name will serve no purpose in the code. 
	If it were possible, providing no name would make the intent clearer.

	In the CONCAT macro, the seemingly-redundant helper macros 
	(CONCAT_2_ and CONCAT_3_) are needed due to particulars of the 
	preprocessor.

	Generally, this construct will only be needed in a source file 
	(and not a header). This makes it unlikely for it to clash with 
	anything else. If it's needed in a header file, care must be 
	taken that the name can't clash with any other identifier, 
	because a clash may not show up until the header is integrated 
	with another header file that happens to have an identical UNIQUE_NAME.
 */

#define	CONCAT_3_(x, y)		x##y
#define	CONCAT_2_(x, y)		CONCAT_3_(x,y)
#define	CONCAT(x, y)		CONCAT_2_(x,y)
#define	UNIQUIFY(str)		CONCAT(str,__LINE__)

/**
	Turn x into a string, typically used for debugging, asserts...
 */
#define	_STRINGIFY_(x)		#x
#define	STRINGIFY(x)		_STRINGIFY_(x)

/**
	To try to protect macros with commas inside, without
	requiring extra parentheses.  
 */
#define	CPPWRAP(x)		x


#endif	// __UTIL_CPPCAT_H__

