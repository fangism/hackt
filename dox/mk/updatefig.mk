# "updatefig.mk"
#	$Id: updatefig.mk,v 1.1.2.1 2007/06/06 05:02:21 fang Exp $
# Makefile macros for "updatefig.awk".

# The following macros must be predefined:
#	CAT
#	SED
#	AWK
#	GREP
#	ECHO

#	UPDATEFIG: should point to the source script, updatefig.awk

UPDATEFIG = $(AWK) -f $(top_srcdir)/dox/mk/updatefig.awk

FIG_RED = 4
FIG_GREEN = 2
FIG_BLUE = 1
FIG_YELLOW = 6
FIG_CYAN = 3
FIG_MAGENTA = 5
FIG_WHITE = 7
FIG_BLACK = 0

MATCH_ELLIPSE = -v match_object_code=1
MATCH_CIRCLE_RADIUS = $(MATCH_ELLIPSE) -v match_sub_type=3
MATCH_CIRCLE_DIAMETER = $(MATCH_ELLIPSE) -v match_sub_type=4
MATCH_POLYLINE = -v match_object_code=2
MATCH_SPLINE = -v match_object_code=3
MATCH_TEXT = -v match_object_code=4
MATCH_LINE = $(MATCH_POLYLINE) -v match_npoints=2
MATCH_RECT = $(MATCH_POLYLINE) -v match_npoints=5
MATCH_TRIANGLE = $(MATCH_POLYLINE) -v match_npoints=4
MATCH_ARC_BOX = $(MATCH_POLYLINE) -v match_sub_type=4
MATCH_ARC = -v match_object_code=5

MATCH_LATEX_MATH = $(MATCH_TEXT) -v match_regexp='[$$].*[$$]'
MATCH_LATEX_COMMAND = $(MATCH_TEXT) -v match_regexp='\\\\'
FLAG_LATEX_TEXT = -v update_font_flags=2

# strips .fig file of its header, assuming 1200 dpi
STRIPFIG = $(SED) -e "/\#FIG/,/1200 2/d"

