
# /^\(/       { next }
  /^ *$/      { next }
# /^:/        { next }
# /^\)/       { next }
# /^Chapter/  { next }
# /^\\OT/     { next }
# /^ *\[/     { next }
# /^ *</      { next }
# /^\.$/      { next }
# /^Append/   { next }

/^LaTeX Font Warning:/   { next }

/Warning/   { print "\033[1;34m" $0 "\033[0m" ; next}
/Error/     { print "\033[1;31m" $0 "\033[0m" ; next}
/Overfull/  { print "\033[1;31m" $0 "\033[0m" ; next}
/Underfull/ { print "\033[1;35m" $0 "\033[0m" ; next}

{ print }

