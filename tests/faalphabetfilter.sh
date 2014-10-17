#! /bin/sh

### Set verbose mode
test "x$VERBOSE" = "xx" && set -x

#does it run ?
../src/faalphabetfilter -h &> /dev/null || exit 1

#run without any specification 
../src/faalphabetfilter -o tmp.out $srcdir/test.fa 2> tmp.err || exit 1
diff full_id.txt tmp.out || exit 1
diff full_len_exclude.err tmp.err || exit 1

exit 0

