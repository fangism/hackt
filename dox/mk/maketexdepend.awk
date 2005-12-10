#!/usr/bin/awk -f

# "maketexdepend.awk"
# by Fang <fangism@users.sourceforge.net>
#	$Id: maketexdepend.awk,v 1.4 2005/12/10 03:56:27 fang Exp $
#
# auto-generate LaTeX dependencies
# usage: awk -f <this script> [variables] <top-level tex file>
#
# Public Variables (pass using -v, all are optional):
#	script -- name of this awk script, including path
#	srcdir -- relative path to root of src files, default "" (same as ./)
#	readable -- human-readable formatting
#	sort -- alphabetic sorting
#	flat_mode -- {0=hierarchical, 1=flat} dependency style
#	extensions -- file extensions of targets (.dvi, .pdf...)
#	touch -- whether or not to touch timestamps of files (default off)
#
# Other files:
# "maketexdepend.tex" -- LaTeX additions to help this script
#	contains commands for ignoring or adding dependencies

# uses tail-recursive self-invocation of awk!
# library macro files will appear in dependence list, 
# but can't be found locally (doesn't search paths)
# so use phony command (you define) \maketexdependignore{...}
# e.g.: \newcommand{\maketexdependignore}[1]{\relax}	% doesn't do anything
# and invoke \maketexdependignore{somelib.tex}
# analogously, \maketexdependstop{...} will NOT
# recursively descend into said file, but will keep it as a dependence
#	this is useful for stopping recursion at automatically generated files
#	that will not exist in a clean distribution

# limitation: if file is included multiple times in different environments?
#	proposed solution: take conjunctive minimum of conditions?

BEGIN {
if (script == "")			# use -v script=... to override
	script = "maketexdepend.awk";	# can be overridden for path

if (srcdir == "")			# default to this directory
	srcdir = ".";

# default values overrideable during recursive invocation!
# fig_level = 0;
# table_level = 0;
# comment_level = 0;
# readable = 1;			# readable deps
# sort = 1;			# sorted dependencies
# flat_mode = 0;		# flatten mode
if (!length(extensions))
	extensions = "tex";	# default left-hand-side
if (!length(touch))
	touch = 1;		# default: touch targets

# keep worklist of files to process, breadth first file traversal
use_figures = 0;
num_deps = 0;
abort = 0;
}

# allows for user-defined environment wrappers that contain the 
#	key words "figure" and "table"
/\\begin\{comment\}/ {		comment_level++;	}
/\\end\{comment\}/ {		comment_level--;	}
/\\begin\{figure\*?\}/ {		figure_level++;		}
/\\end\{figure\*?\}/ {		figure_level--;		}
/\\begin\{.*table.*\*?\}/ {	table_level++;		}
/\\end\{.*table.*\*?\}/ {		table_level--;		}

# assumptions: exactly one \input command without line break
#	possibly preceeded and nullified by % comment
/\\input/ {
if (!is_commented($0)) {
	add_dependence(extract_input_file($0));
}}

# to add explicit dependence (such as auxiliary list files)
# for implicit file dependences
/\\maketexdependadd/ {
if (!is_commented($0)) {
	add_dependence(extract_ignore_file($0));
}}

/\\maketexdependignore/ {
if (!is_commented($0)) {
	ignore[extract_ignore_file($0)] = 1;
}}

/\\maketexdependstop/ {
if (!is_commented($0)) {
	stop[extract_ignore_file($0)] = 1;
}}

/\\maketexdependabort/ {
if (!is_commented($0)) {
	abort = 1;
}}

/\\bibliography\{/ {	# need { to ignore bibstyle command
if (!is_commented($0)) {
	dep_files[extract_bib_file($0)] = 1;
	num_deps++;
}}

function add_dependence(file) {
if (!dep_files[file]) {		# if already added, skip
	dep_files[file] = 1;
	fig[file] = figure_level;
	tab[file] = table_level;
	com[file] = comment_level;
	# problem: if same file is included multiple times in different places?
	# take minimum.... (else case)
	num_deps++;
} else {
	if (figure_level < fig[file]) fig_file = figure_level;
	if (table_level < tab[file]) tab[file] = table_level;
	if (comment_level < com[file]) com[file] = comment_level;
}
}

# assumes: comment is whole line, starts with %
function is_commented(str) {
	if (comment_level) return comment_level;
	# else
	split(str, strtok);
	return (match(strtok[1],"^%"));
}

# assumes is the first token after one with "input"
function extract_input_file(str,
	# local var
	ret_str) {
	split(substr(str, index(str, "\\input")), strtok);
	ret_str = strtok[2];
	gsub("}","",ret_str);	# remove trailing `}'
		# trailing garbage?
	return ret_str (match(ret_str,"\\.tex$") ? "" : ".tex");
		# automatic extension of file name completed
}

function extract_bib_file(str,
	# local vars
	ret_str) {
	# ret_str = extract_substr(str, "{", "}");
	ret_str = extract_inside_braces(str);
	return ret_str (match(ret_str,"\\.bib$") ? "" : ".bib");
}

function extract_ignore_file(str,
	# local vars
	ret_str) {
	# return extract_substr(str, "{", "}");
	return extract_inside_braces(str);
}

# NOTE: this only works if the head_str and tail_str contain
# no escape sequences or regular expressions, 
# because the length of the string and the 
# length of the characters would be mismatched.  
function extract_substr(str, head_str, tail_str,
	# local variables
	m_start, m_stop) {
	m_start = match(str, head_str) +length(head_str);
	m_stop = match(str, tail_str);
	return substr(str, m_start, m_stop-m_start);
}

# This is what we usually want.  
function extract_inside_braces(str, 
	# local variables
	m_start, m_stop) {
	m_start = match(str, "\\{") +1;
	m_stop = match(str, "\\}");
	return substr(str, m_start, m_stop-m_start);
}

# qsort - sort A[left..right] by quicksort
# stolen from http://www.netlib.org/research/awkbookcode/qsort.awk
function qsort(A,left,right,
	# local vars
	i,last) {
	if (left >= right)	# do nothing if array contains
		return;		# less than two elements
	swap(A, left, left + int((right-left+1)*rand()));
	last = left;		# A[left] is now partition element
	for (i = left+1; i <= right; i++)
		if (A[i] < A[left])
			swap(A, ++last, i);
	swap(A, left, last);
	qsort(A, left, last-1);
	qsort(A, last+1, right);
}

function swap(A,i,j,   t) {
	t = A[i]; A[i] = A[j]; A[j] = t
}
# end of stolen code

END {
if (!abort) {
# splash banner
if (!flat_mode) {
	print "# This Makefile was generated by the invocation:";
	print "# awk -f " script;
	print "#\t-v script=" script;
	print "#\t-v srcdir=" srcdir;
	print "#\t-v readable=" readable;
	print "#\t-v sort=" sort;
#	"#\t-v comment_level=" com[f];		# not for general use
#	"#\t-v figure_level=" fig[f];		# not for general use
#	"#\t-v table_level=" tab[f];		# not for general use
	print "#\t-v flat_mode=1";
	print "#\t-v extensions=\"" extensions "\"";
	print "#\t-v touch=" touch;
}

# filter out ignored files
for (f in dep_files) {
	if (ignore[f] || (!use_figures && fig[f])) {
		 	# ignore figures!
		delete dep_files[f];
		num_deps--;
	}
}
if (num_deps) {		# > 0

	i=1;
	for (f in dep_files)
		sorted_dep_files[i++] = f;
	if (sort) qsort(sorted_dep_files, 1, num_deps);

if (!flat_mode) {
	# print left-hand-side of the dependence rule
	# NOTE: delete <array> is a gawk extension and not POSIX!
	# delete extarray;
	for (e in extarray) delete extarray[e];
	num_ext = split(extensions, extarray);
	print "";
	for (i=1; i<=num_ext; i++) {
		fname = FILENAME;
		gsub("\\.tex$", "." extarray[i], fname);
		printf("%s ", fname);
	}
	printf(":");
#	printf("\n%s:", FILENAME);
}
	for (i=1; i<=num_deps; i++) {
		f = sorted_dep_files[i];
		if (f != "") {
			if (readable) {	# each dependent file on one line
				printf(" \\\n\t\t");
			} else {
				printf(" ");
			}
			if (srcdir != ".") {
				printf(srcdir "/");
			}
			printf(f);
		}
	}

# post-recursion
	for (f in dep_files) {	
		# ignored files removed already
	if (!stop[f]) {
		# force flat-mode for children, never the root
		system("awk -f " script \
			" -v script=" script \
			" -v srcdir=" srcdir \
			" -v readable=" readable \
			" -v sort=" sort \
			" -v comment_level=" com[f] \
			" -v figure_level=" fig[f] \
			" -v table_level=" tab[f] \
			" -v flat_mode=1" \
			" -v extensions=\"" extensions "\"" \
			" -v touch=\"" touch "\"" \
			" " srcdir "/" f);
	}}
if (!flat_mode) {
	print "";		# end line last dependence
	if (touch > 0)
		print "\ttouch $@";	# force timestamp update? needed...
	print "";
}
}}}



