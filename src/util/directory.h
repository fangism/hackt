/**
	\file "util/directory.h"
	$Id: directory.h,v 1.1 2009/11/14 03:12:14 fang Exp $
	Directory-like command-interface extensions for simulator
	command interpreters.  
	This could be useful enough to go into the util library.
 */

#ifndef	__HAC_UTIL_DIRECTORY_H__
#define	__HAC_UTIL_DIRECTORY_H__

#include <iosfwd>
#include <string>
#include <vector>

/**
	Define to 1 to implement directory as a sequence of substrings, 
	instead of a single string.
 */
#define	DIRECTORY_AS_SEQUENCE			0

#if DIRECTORY_AS_SEQUENCE
#include "util/tokenize_fwd.h"
#endif

namespace util {
using std::ostream;
using std::string;

/**
	Class for maintaining directory stack and other directory-based
	manipulations.  
	Working directories are string-based.  
	Alternatively, keep directory as a sequence of substrings
	instead of one long concatenated string.
 */
class directory_stack {
#if DIRECTORY_AS_SEQUENCE
	typedef	string_list			dir_type;
#else
	typedef	string				dir_type;
#endif
	typedef	std::vector<dir_type>		directory_stack_impl_type;
	directory_stack_impl_type		dir_stack;
public:
	/**
		"::" prefix to denote absolute path.
	 */
	string					absolute_prefix;
protected:
	/**
		usually "." for descending hierarchy.
		This must be a single character!
	 */
	string					separator;
public:
	/**
		usually something like "."
		Can be just ignored.
	 */
	string					this_dir_string;
	/**
		usually something like ".."
		Each time this string matches, strip away tail.
	 */
	string					parent_dir_string;
	/**
		Sometimes, need a different separator for parent dirs.
		default: '/'
	 */
	string					parent_separator;
#if DIRECTORY_AS_SEQUENCE
private:
	/// cache current working directory as flat string
	string					cwd_cache;
#endif
public:
	directory_stack();
	~directory_stack();

	void
	set_separator(const char);

	const string&
	get_separator(void) const { return separator; }

	bool
	change_directory(const string&);

	bool
	push_directory(const string&);

	bool
	pop_directory(void);

	const string&
	current_working_directory(void) const;

	const directory_stack_impl_type&
	get_stack(void) const { return dir_stack; }

	string
	transform(string) const;

	size_t
	common_prefix_length(void) const;

	ostream&
	dump_working_directory(ostream&) const;

	/// prints space-separated list
	ostream&
	dump_stack(ostream&) const;

	/**
		Binding mem_fun_ref.
	 */
	class transformer {
		const directory_stack&	ds;
		explicit
		transformer(const directory_stack& d) : ds(d) { }

		// return transformed absolute path of string
		string
		operator () (const string& s) const {
			return ds.transform(s);
		}
	};

private:
	bool
	__change_directory(string&, string) const;

};	// end class directory_stack

}	// end namespace util

#endif	// __HAC_UTIL_DIRECTORY_H__

