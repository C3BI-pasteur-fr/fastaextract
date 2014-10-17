#! /bin/sh


### Set verbose mode
test "x$VERBOSE" = "xx" && set -x

#extract without size specification
../src/fasizefilter -o tmp.out $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/full_id.txt tmp.out || exit 1
diff $srcdir/full_len_exclude.err tmp.err || exit 1

#extract with size specification
../src/fasizefilter -o tmp.out -l 150 $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/lenfilter.out tmp.out || exit 1
diff $srcdir/lenfilter.err tmp.err || exit 1

#extract with size specification with exclusion
../src/fasizefilter -o tmp.out -l 150 -x $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/xlenfilter.out tmp.out || exit 1
diff $srcdir/xlenfilter.err tmp.err || exit 1

exit 0

