/**
	\file "libchpfn/io.cc"
	Link note: this depends on libhackt.la from the src/ dir and installed.
	This relies on the executable being built with -export-dynamic
	for proper dynamic linking.  
	TODO: binary I/O modes
	$Id: io.cc,v 1.4.12.1 2008/11/21 17:26:21 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <fstream>
#include <map>
#include "libchpfn/io.h"
#include "util/memory/count_ptr.h"	// .tcc
#include "util/format_IO.h"
#include "Object/expr/dlfunction.h"
// #include "util/string.tcc"		// for string_to_num
#include "util/stacktrace.h"

// from libhackt.la
#include "Object/expr/const_param_expr_list.h"

namespace HAC {
namespace CHP {
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::ios_base;
using std::ofstream;
using std::ifstream;
using entity::extract_chp_value;
using util::memory::count_ptr;
using std::ostringstream;

//=============================================================================
/**
	End-of-file exception, not necessarily fatal.  
 */
struct EOF_exception { };

static
inline
void
throw_EOF(void) { throw EOF_exception(); }

//=============================================================================
/***
@texinfo fn/echo.texi
@deffn Function echo args...
@deffnx Function cout args...
@deffnx Function print args...
@end deffn
Prints all arguments sequentially to @file{stdout}.
The @command{print} variant includes a terminating newline, 
while the others do not.  
@end texinfo
***/
chp_function_return_type
echo(const chp_function_argument_list_type& p) {
	p.dump_raw(cout);
	return chp_function_return_type(NULL);
}

chp_function_return_type
print(const chp_function_argument_list_type& p) {
	p.dump_raw(cout) << endl;
	return chp_function_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/printerr.texi
@deffn Function printerr args...
@deffnx Function cerr args...
Prints all arguments sequentially to @file{stderr}.
The @command{printerr} variant includes a terminating newline, 
while the others do not.
@end deffn
@end texinfo
***/
chp_function_return_type
printerr(const chp_function_argument_list_type& p) {
	p.dump_raw(cerr);
	return chp_function_return_type(NULL);
}

chp_function_return_type
printerr_nl(const chp_function_argument_list_type& p) {
	p.dump_raw(cerr) << endl;
	return chp_function_return_type(NULL);
}

//=============================================================================
static const char eof_err_msg[] = "Error trying to scan past EOF.";

//=============================================================================
template <typename T>
static
T
__scan(istream& i, const char* eof_msg) {
	STACKTRACE_VERBOSE;
	T v;
	i >> v;
	if (i.eof()) {
		if (eof_msg) { cerr << eof_msg << endl; }
		throw_EOF();
	} else if (i.fail()) {
		cerr << "Error reading value." << endl;
		THROW_EXIT;
	}
	return v;
}

template <typename T>
static
T
scan(istream& i) {
	return __scan<T>(i, eof_err_msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
int_value_type
__bzscan(istream& i, const char* eof_msg) {
	STACKTRACE_VERBOSE;
	size_t v = 0;
	string vs;
	i >> vs;
	if (i.eof()) {
		if (eof_msg) { cerr << eof_msg << endl; }
		throw_EOF();
	} else if (i.fail()) {
		cerr << "Error reading binary value." << endl;
		THROW_EXIT;
	} else if (!util::string_to_int_binary(vs.c_str(), v).good) {
		cerr << "Error converting string to binary: \"" << vs << "\"."
			<< endl;
		THROW_EXIT;
	}
	return v;	// convert unsigned to signed
}

static
int_value_type
bzscan(istream& i) {
	return __bzscan(i, eof_err_msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
int_value_type
__xzscan(istream& i, const char* eof_msg) {
	STACKTRACE_VERBOSE;
	int_value_type v;
	i >> std::hex >> v;	// doesn't seem to work on file streams!?
	// works on stdin/cin
	if (i.eof()) {
		if (eof_msg) { cerr << eof_msg << endl; }
		throw_EOF();
	} else if (i.fail()) {
		cerr << "Error reading hexadecimal value." << endl;
		THROW_EXIT;
	}
	return v;
}

static
int_value_type
xzscan(istream& i) {
	return __xzscan(i, eof_err_msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
// static
string_value_type
__scan<string_value_type>(istream& i, const char* eof_msg) {
	STACKTRACE_VERBOSE;
	char buf[1024];
	i.getline(buf, 1024);
	if (i.eof()) {
		if (eof_msg) { cerr << eof_msg << endl; }
		throw_EOF();
	} else if (i.fail()) {
		cerr << "Error reading string." << endl;
		THROW_EXIT;
	}
	return string_value_type(buf);
}

//-----------------------------------------------------------------------------
template <typename T>
static
T
__try_scan(istream& i, T (*f)(istream&, const char*)) {
	return (*f)(i, NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as scan, but suppresses error message on EOF failure.
 */
template <typename T>
static
T
try_scan(istream& i) {
	return __try_scan(i, &__scan<T>);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this could be replaced using std::bind...
static
int_value_type
try_bzscan(istream& i) {
	return __try_scan(i, &__bzscan);
}

static
int_value_type
try_xzscan(istream& i) {
	return __try_scan(i, &__xzscan);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never need custom prompt for non-stdin input streams, right?
 */
template <typename T>
static
T
scan_prompt(const string_value_type& ppt) {
	cout << ppt;
	return scan<T>(cin);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/zscan.texi
@deffn Function zscan
@deffnx Function dzscan
Read an integer from @file{stdin}.  
Use with caution, because events in the simulator are relatively asynchronous.
The `d' in the @command{dzscan} command alias is for decimal, base-10.  
See also @command{zscan_prompt}.
@end deffn
@end texinfo
***/
int_value_type
zscan(void) {
	return scan<int_value_type>(cin);
}

/***
@texinfo fn/bzscan.texi
@deffn Function bzscan
Reads an integer, expected in binary, containing only 0's and 1's.  
Input should exclude any ``0b'' prefix.  
@end deffn
@end texinfo
***/
int_value_type
bzscan(void) {
	return bzscan(cin);
}

/***
@texinfo fn/xzscan.texi
@deffn Function xzscan
Reads an integer, expected in hexadecimal.  
Input may include an optional ``0x'' prefix.
@end deffn
@end texinfo
***/
int_value_type
xzscan(void) {
	return xzscan(cin);
}

/***
@texinfo fn/zscan_prompt.texi
@deffn Function zscan_prompt str
@deffnx Function dzscan_prompt str
Same as @command{zscan}, but takes a prompt string @var{str} as an argument
and prints it to prompt the user.  
@end deffn
@end texinfo
***/
int_value_type
zscan_prompt(const string_value_type& str) {
	return scan_prompt<int_value_type>(str);
}

/***
@texinfo fn/bzscan_prompt.texi
@deffn Function bzscan_prompt str
Prompts use to enter an integer in binary.
@end deffn
@end texinfo
***/
int_value_type
bzscan_prompt(const string_value_type& str) {
	cout << str;
	return bzscan();
}

/***
@texinfo fn/xzscan_prompt.texi
@deffn Function xzscan_prompt str
Prompts use to enter an integer in hexadecimal.
@end deffn
@end texinfo
***/
int_value_type
xzscan_prompt(const string_value_type& str) {
	cout << str;
	return xzscan();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/bscan.texi
@deffn Function bscan
Read a boolean (0 or 1) from @file{stdin}.  
Use with caution, because events in the simulator are relatively asynchronous.
See also @command{bscan_prompt}.
@end deffn
@end texinfo
***/
bool_value_type
bscan(void) {
	return scan<bool_value_type>(cin);
}

/***
@texinfo fn/bscan_prompt.texi
@deffn Function bscan_prompt str
Same as @command{bscan}, but takes a prompt string @var{str} as an argument
and prints it to prompt the user.  
@end deffn
@end texinfo
***/
bool_value_type
bscan_prompt(const string_value_type& str) {
	return scan_prompt<bool_value_type>(str);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/sscan.texi
@deffn Function sscan
Reads a newline-terminated string from @file{stdin}.  
@end deffn
@end texinfo
***/
string_value_type
sscan(void) {
	return scan<string_value_type>(cin);
}

/***
@texinfo fn/sscan_prompt.texi
@deffn Function sscan_prompt str
Same as @command{sscan}, but takes a prompt string @var{str} as an argument
and prints it to prompt the user.  
@end deffn
@end texinfo
***/
string_value_type
sscan_prompt(const string_value_type& str) {
	return scan_prompt<string_value_type>(str);
}

//=============================================================================
typedef	std::map<string, count_ptr<ofstream> >	ofstream_map_type;
typedef	std::map<string, count_ptr<ifstream> >	ifstream_map_type;

/**
	CHP library open file registry.  
 */
static
ofstream_map_type
ofstream_map;

static
ifstream_map_type
ifstream_map;

/***
@texinfo fn/fprint.texi
@deffn Function fprint file args...
Print @var{args} to file @var{file} by appending.  
Throw run-time exception if opening file fails.  
File streams are automatically closed and flushed upon library closing.  
@end deffn
@end texinfo
***/
chp_function_return_type
fprint(const chp_function_argument_list_type& args) {
	STACKTRACE_VERBOSE;
	const string_value_type&
		fn(extract_chp_value<string_value_type>(args[0]));
	count_ptr<ofstream>& fp(ofstream_map[fn]);
	if (!fp) {
		fp = count_ptr<ofstream>(
			new ofstream(fn.c_str())
			// new ofstream(fn.c_str(), ios_base::app)
			// stream remains open between calls, already appends
		);
		if (!fp || !*fp) {
			cerr << "Error opening file `" << fn <<
				"\' in write mode." << endl;
			THROW_EXIT;
		}
	}
	INVARIANT(*fp);
	args.dump_raw_from(*fp, 1);
	return chp_function_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/fappend.texi
@deffn Function fappend file
Like @command{fopen}, except @var{file} is first opened in append mode, 
to not overwrite existing file.  
Call this before @command{fprint} to append to @var{file}.  
@end deffn
@end texinfo
***/
bool_value_type
fappend(const string_value_type& fn) {
	count_ptr<ofstream>& fp(ofstream_map[fn]);
	if (!fp) {
		fp = count_ptr<ofstream>(new ofstream(fn.c_str(), 
			ios_base::app));
		if (!fp || !*fp) {
			cerr << "Error opening file `" << fn <<
				"\' in write-append mode." << endl;
		}
	}
	return fp && *fp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/fopen.texi
@deffn Function fopen file
Open file @var{file} for writing, overwrite previous contents.
Subsequent calls to @command{fprint} will still continue to append
to the file.  
If the file stream is already open, do nothing.  
Return true if the stream is opened successfully (or was already open).  
@end deffn
@end texinfo
***/
bool_value_type
fopen(const string_value_type& fn) {
	count_ptr<ofstream>& fp(ofstream_map[fn]);
	if (!fp) {
		fp = count_ptr<ofstream>(new ofstream(fn.c_str()));
		if (!fp || !*fp) {
			cerr << "Error opening file `" << fn <<
				"\' in write mode." << endl;
		}
	}
	return fp && *fp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/fclose.texi
@deffn Function fclose file
Close and flush file input and output stream(s) @var{file}.  
@end deffn
@end texinfo
***/
void
fclose(const string_value_type& fn) {
	count_ptr<ofstream>& ofp(ofstream_map[fn]);
	if (ofp) {
		ofp->close();
		ofstream_map.erase(fn);
	}
	count_ptr<ifstream>& ifp(ifstream_map[fn]);
	if (ifp) {
		ifp->close();
		ifstream_map.erase(fn);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/fflush.texi
@deffn Function fflush file
Flush output file stream @var{file}.  
@end deffn
@end texinfo
***/
void
fflush(const string_value_type& fn) {
	count_ptr<ofstream>& ofp(ofstream_map[fn]);
	if (ofp) {
		ofp->flush();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
static
T
__fscan(const string_value_type& fn, T (*f)(istream&)) {
	STACKTRACE_VERBOSE;
	count_ptr<ifstream>& fp(ifstream_map[fn]);
	if (!fp) {
		fp = count_ptr<ifstream>(new ifstream(fn.c_str()));
		if (!fp || !*fp) {
			cerr << "Error opening file `" << fn <<
				"\' in read mode." << endl;
			THROW_EXIT;
		}
	}
	INVARIANT(*fp);
	return (*f)(*fp);
}

template <class T>
static
T
fscan(const string_value_type& fn) {
	return __fscan(fn, &scan<T>);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param f1 the function to try first, may throw.
	\param f2 the function to try second (retry).
 */
template <class T>
static
T
__fscan_loop(const string_value_type& fn,
	T (*f1)(istream&), 
	T (*f2)(istream&)) {
	STACKTRACE_VERBOSE;
	count_ptr<ifstream>& fp(ifstream_map[fn]);
	if (!fp) {
		fp = count_ptr<ifstream>(new ifstream(fn.c_str()));
		if (!fp || !*fp) {
			cerr << "Error opening file `" << fn <<
				"\' in read mode." << endl;
			THROW_EXIT;
		}
	}
	INVARIANT(*fp);
	// not the most robust handling...
try {
	// may EOF
	return (*f1)(*fp);
} catch (const EOF_exception& e) {
	// on EOF, re-open file
	fp = count_ptr<ifstream>(new ifstream(fn.c_str()));
	INVARIANT(fp && *fp);
	return (*f2)(*fp);
}
}

template <class T>
static
T
fscan_loop(const string_value_type& fn) {
	return __fscan_loop(fn, &try_scan<T>, &scan<T>);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/fzscan.texi
@deffn Function fzscan file
@deffnx Function fdzscan file
Read the next integer from input file @var{file}.  
Expects integer in decimal. 
Automatically opens new input file stream when referenced first time.  
@end deffn
@end texinfo
***/
int_value_type
fzscan(const string_value_type& fn) {
	return fscan<int_value_type>(fn);
}

/***
@texinfo fn/fbzscan.texi
@deffn Function fbzscan file
Same as @command{fzscan}, but expects integer in binary.  
@end deffn
@end texinfo
***/
int_value_type
fbzscan(const string_value_type& fn) {
	return __fscan(fn, &bzscan);
}

/***
@texinfo fn/fxzscan.texi
@deffn Function fxzscan file
Same as @command{fzscan}, but expects integer in binary.  
@end deffn
@end texinfo
***/
int_value_type
fxzscan(const string_value_type& fn) {
	return __fscan(fn, &xzscan);
}

/***
@texinfo fn/fbscan.texi
@deffn Function fbscan file
Read the next boolean from input file @var{file}.  
Automatically opens new input file stream when referenced first time.  
@end deffn
@end texinfo
***/
bool_value_type
fbscan(const string_value_type& fn) {
	return fscan<bool_value_type>(fn);
}

/***
@texinfo fn/fsscan.texi
@deffn Function fsscan file
Read the next boolean from input file @var{file}.  
Automatically opens new input file stream when referenced first time.  
@end deffn
@end texinfo
***/
string_value_type
fsscan(const string_value_type& fn) {
	return fscan<string_value_type>(fn);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/fzscan_loop.texi
@deffn Function fzscan_loop file
@deffnx Function fdzscan_loop file
Read the next integer from input file @var{file}.  
Re-opens file to beginning after EOF is reached.
@end deffn
@end texinfo
***/
int_value_type
fzscan_loop(const string_value_type& fn) {
	return fscan_loop<int_value_type>(fn);
}

/***
@texinfo fn/fbzscan_loop.texi
@deffn Function fbzscan_loop file
Like @command{fzscan_loop}, but expects integer in binary.  
@end deffn
@end texinfo
***/
int_value_type
fbzscan_loop(const string_value_type& fn) {
	return __fscan_loop(fn, &try_bzscan, &bzscan);
}

/***
@texinfo fn/fxzscan_loop.texi
@deffn Function fxzscan_loop file
Like @command{fzscan_loop}, but expects integer in hexadecimal.
@end deffn
@end texinfo
***/
int_value_type
fxzscan_loop(const string_value_type& fn) {
	return __fscan_loop(fn, &try_xzscan, &xzscan);
}

/***
@texinfo fn/fbscan_loop.texi
@deffn Function fbscan_loop file
Read the next boolean from input file @var{file}.  
Re-opens file to beginning after EOF is reached.
@end deffn
@end texinfo
***/
bool_value_type
fbscan_loop(const string_value_type& fn) {
	return fscan_loop<bool_value_type>(fn);
}

/***
@texinfo fn/fsscan_loop.texi
@deffn Function fsscan_loop file
Read the next boolean from input file @var{file}.  
Re-opens file to beginning after EOF is reached.
@end deffn
@end texinfo
***/
string_value_type
fsscan_loop(const string_value_type& fn) {
	return fscan_loop<string_value_type>(fn);
}

//=============================================================================
}	// end namespace CHP
}	// end namespace HAC
