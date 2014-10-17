#! /bin/sh

srcdir=.
### Set verbose mode
test "x$VERBOSE" = "xx" && set -x

#extract without size specification
../src/faalphabetfilter -o tmp.out $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/full_id.txt tmp.out || exit 1
diff $srcdir/full_len_exclude.err tmp.err || exit 1

#extract with alphabet specification
../src/faalphabetfilter -o tmp.out -a 1 $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/lenfilter.out tmp.out || exit 1
diff $srcdir/lenfilter.err tmp.err || exit 1

#extract with alphabet specification with exclusion
../src/faalphabetfilter -o tmp.out -x -a 1 $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/xlenfilter.out tmp.out || exit 1
diff $srcdir/xlenfilter.err tmp.err || exit 1

#extract with custom alphabet
../src/faalphabetfilter -o tmp.out -A 'ATGC' $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/lenfilter.out tmp.out || exit 1
diff $srcdir/lenfilter.err tmp.err || exit 1

exit 0

