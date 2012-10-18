/**
	\file "libchpfn/io.hh"
	user-level I/O functions
	$Id: io.hh,v 1.3 2007/08/30 00:20:28 fang Exp $
 */

#ifndef	__HAC_LIBCHPFN_IO_H__
#define	__HAC_LIBCHPFN_IO_H__

#include <iosfwd>
#include "Object/expr/dlfunction_fwd.hh"
#include "Object/expr/types.hh"

namespace HAC {
namespace CHP {
using std::ostream;
using std::istream;
using entity::chp_function_return_type;
using entity::chp_function_argument_list_type;
using entity::string_value_type;
using entity::int_value_type;
using entity::bool_value_type;

//=============================================================================
// output

extern
chp_function_return_type
echo(const chp_function_argument_list_type&);

extern
chp_function_return_type
print(const chp_function_argument_list_type&);

extern
chp_function_return_type
printerr(const chp_function_argument_list_type&);

extern
chp_function_return_type
printerr_nl(const chp_function_argument_list_type&);

//=============================================================================
// input

extern
int_value_type
zscan(void);

extern
int_value_type
zscan_prompt(const string_value_type&);

extern
int_value_type
bzscan(void);

extern
int_value_type
bzscan_prompt(const string_value_type&);

extern
int_value_type
xzscan(void);

extern
int_value_type
xzscan_prompt(const string_value_type&);

extern
bool_value_type
bscan(void);

extern
bool_value_type
bscan_prompt(const string_value_type&);

#if 0
extern
int_value_type
rscan(void);
#endif

extern
string_value_type
sscan(void);

extern
string_value_type
sscan_prompt(const string_value_type&);


//=============================================================================
// file I/O

extern
chp_function_return_type
fprint(const chp_function_argument_list_type&);

extern
bool_value_type
fappend(const string_value_type&);

extern
bool_value_type
fopen(const string_value_type&);

extern
void
fclose(const string_value_type&);

extern
void
fflush(const string_value_type&);

extern
int_value_type
fzscan(const string_value_type&);

extern
int_value_type
fbzscan(const string_value_type&);

extern
int_value_type
fxzscan(const string_value_type&);

extern
bool_value_type
fbscan(const string_value_type&);

extern
string_value_type
fsscan(const string_value_type&);

// loop-over-file variants
extern
int_value_type
fzscan_loop(const string_value_type&);

extern
int_value_type
fbzscan_loop(const string_value_type&);

extern
int_value_type
fxzscan_loop(const string_value_type&);

extern
bool_value_type
fbscan_loop(const string_value_type&);

extern
string_value_type
fsscan_loop(const string_value_type&);

//=============================================================================
// string conversion

extern
string_value_type
sprint(const chp_function_argument_list_type&);

//=============================================================================
}	// end namespace CHP
}	// end namespace HAC

#endif	// __HAC_LIBCHPFN_IO_H__

