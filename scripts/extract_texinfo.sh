#!/bin/sh -e
# "extract_texinfo.sh"
#	Extracts texinfo documentation excerpts from source and 
# writes them out to texinfo files.  
#
# Texinfo documentation can be slipped into block comments of the 
# source language.
# Documentation blocks are enclosed by:
# @texinfo filename
# @end texinfo
# where filename is the name of the file to print to.  

this_script="$0"

source_file="$1"

current_output_file=""
while read line
do
  case "$line" in
    @texinfo*)
      current_output_file=`echo "$line" | cut -d\  -f2`
      mkdir -p `dirname "$current_output_file"`
      rm -f "$current_output_file"
      cat >"$current_output_file" <<EOF
@c "$current_output_file"
@c extracted from "$source_file"
@c by "$this_script".
EOF
      ;;

    @end*texinfo*) current_output_file="" ;;

    *) test -z "$current_output_file" || echo "$line" >> "$current_output_file" ;;
  esac
done < "$source_file"

