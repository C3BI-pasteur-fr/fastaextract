#! /bin/sh

### Set verbose mode
test "x$VERBOSE" = "xx" && set -x

#extract without list
../src/faextract -o tmp.out $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/test.fa tmp.out || exit 1
diff $srcdir/fullextract.err tmp.err || exit 1

#extract with list
../src/faextract -o tmp.out -l $srcdir/id_list.txt $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/extract.out tmp.out || exit 1
diff $srcdir/extract.err tmp.err || exit 1


exit 0

