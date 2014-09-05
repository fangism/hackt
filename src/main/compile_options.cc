/**
	\file "main/compile_options.cc"
	$Id: compile_options.cc,v 1.1 2010/08/05 18:25:33 fang Exp $
 */

#include <iostream>
#include "main/compile_options.hh"
#include "lexer/file_manager.hh"
#include "util/stacktrace.hh"
#include "util/dirent.hh"		// configured wrapper around <dirent.h>
#include "util/IO_utils.tcc"

namespace HAC {
#include "util/using_ostream.hh"
using util::write_value;
using util::read_value;
using lexer::file_manager;

//=============================================================================
// class compile_options method definitions

void    
compile_options::export_include_paths(file_manager& fm) const {
	STACKTRACE_VERBOSE;
	typedef include_paths_type::const_iterator      const_iterator;
	const_iterator i(include_paths.begin());
	const const_iterator e(include_paths.end());
	for ( ; i!=e; i++) {
		const string& s(*i);
		// check if path exists, otherwise, don't bother adding...
		if (util::dir_exists(s.c_str())) {
			fm.add_path(s);
			if (dump_include_paths) {
				cerr << "Added to search path: " << s << endl;
			}
		} else {
			if (dump_include_paths) {
				cerr << "Couldn\'t open dir: " << s << endl;
			}
		}
	}
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
compile_options::dump(ostream& o) const {
	// TODO: finish me
	create_opts.dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
compile_options::write_object(ostream& o) const {
	write_value(o, make_depend);
	write_value(o, use_stdin);
	write_value(o, make_depend_target);
	write_value(o, source_file);	// .hac
	write_value(o, target_object);	// .haco
	util::write_sequence(o, include_paths);
	util::write_sequence(o, prepend_files);
	parse_opts.write_object(o);
	create_opts.write_object(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
compile_options::load_object(istream& i) {
	read_value(i, make_depend);
	read_value(i, use_stdin);
	read_value(i, make_depend_target);
	read_value(i, source_file);	// .hac
	read_value(i, target_object);	// .haco
	util::read_sequence_back_insert(i, include_paths);
	util::read_sequence_back_insert(i, prepend_files);
	parse_opts.load_object(i);
	create_opts.load_object(i);
}

//=============================================================================
}	// end namespace HAC

