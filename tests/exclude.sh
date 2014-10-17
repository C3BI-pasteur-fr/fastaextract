#! /bin/sh

### Set verbose mode
test "x$VERBOSE" = "xx" && set -x

#exclude without list
../src/faextract -x -o tmp.out $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/fullexclude.out tmp.out || exit 1
diff $srcdir/fullexclude.err tmp.err || exit 1

#extract with list
../src/faextract -x -o tmp.out -l $srcdir/id_list.txt $srcdir/test.fa 2> tmp.err || exit 1
diff $srcdir/exclude.out tmp.out || exit 1
diff $srcdir/exclude.err tmp.err || exit 1


exit 0

