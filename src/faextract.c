#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <err.h>
#include <libgen.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <sequence.h>
#include <sequence/fasta.h>

#include "seqlist.h"

#ifndef HAVE_STRNDUP
/* Implement the strndup function.
   Copyright (C) 2005 Free Software Foundation, Inc.
   Written by Kaveh R. Ghazi <ghazi@caip.rutgers.edu>.

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 51 Franklin
Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

char *strndup (const char *s, size_t n);

char *strndup (const char *s, size_t n)
{
  char *result;
  size_t len = strlen (s);

  if (n < len) len = n;

  result = (char *) malloc (len + 1);
  if ( result == NULL )
    return 0;

  result[len] = '\0';
  return (char *) memcpy (result, s, len);
}
#endif



#define BUFFLEN 1024


/* how to print uint64_t depending on the platform we run on */
#ifdef _LP64
#define PRI_U64 "lu"
#else
#define PRI_U64 "llu"
#endif


static void usage(char *prog, int exitval){
  FILE *f = stderr;
  (void)fprintf(f, "usage: %s [options] file ...\n", prog);
  (void)fprintf(f, "\toptions:\n");
  (void)fprintf(f, "  -h          ... Print this message and exit.\n");
  (void)fprintf(f, "  -l <file>   ... Filter results according to entry list.\n");
  (void)fprintf(f, "  -x          ... If set, all entries from list will be excluded.\n");
  (void)fprintf(f, "  -o <file>   ... Use <file> as output results. (default stdout)\n");
  exit(exitval);
}                          
      



static size_t namlen(char *nam) {
  char *p;
  size_t l;
  /* check that name was completly read */
  if ((p = strpbrk(nam, " \n")) == NULL)
    errx(EXIT_FAILURE, "sequence: %s name too long", nam);
  l = p - nam;
  return  l;
}


static int seqlist_populate(seqlist_t *lst, FILE *f){
  
  char *buf, *tmp;
  int i;
  
  /* allocate buffer for reading */
  if ((buf = malloc(BUFFLEN)) == NULL) 
	 err(EXIT_FAILURE, "memory: malloc failed"); 
  i = 0;
  while (fgets(buf, BUFFLEN, f) != NULL) {
	 if (*buf == '#' || *buf == '\n') continue;
	 i++;
	 tmp = strndup(buf, namlen(buf));
	 seqlist_add(lst, tmp); 
  }
  free(buf);
  return i;
}


int main(int argc, char **argv){
 
  FILE *IN, *OUT;
  seqlist_t IDs;
  sequence_t *seq;

  int i, keep, excl;
  char *prog, *list, *f, *out, *id;
  /* todo convert to uint_64 */
  uint64_t n, total;

  /* default values */
  prog = basename(*argv);
  list = NULL;
  out = NULL;
  OUT = stdout; 
  excl = keep = 0;

  /* Check command line */
  while ((i = getopt(argc, argv, "hl:o:x")) != -1) {
    switch (i) {
    case 'h':
      usage(prog, EXIT_SUCCESS); break;
	 case 'l':
		list = optarg; break;
	 case 'o':
		out = optarg; break;
	 case 'x':
		excl = 1; break;
	 default:
		usage(prog, EXIT_FAILURE); 
	 }
  }
  /* some files to deal with ? */
  if (argc - optind < 1) { usage(prog,  EXIT_FAILURE); }

  /* get ids from list */
  if (list != NULL) {
	 if ((IN = fopen(list, "r")) == NULL) 
		err(EXIT_FAILURE, "%s", list);

	 seqlist_init(&IDs);
	 i = seqlist_populate(&IDs, IN);
	 if (i == 0) errx(EXIT_FAILURE, "file: %s no identifier found", list); 
	 
	 if (fclose(IN) == EOF) err(EXIT_FAILURE, "%s: close failed", list);

    seqlist_sort(&IDs); 
  }

  /* open output file */
  if ((out != NULL) && ((OUT = fopen(out, "w")) == NULL))
	 err(EXIT_FAILURE, "%s", out);

  total = n = 0;

  /* start filtering all argument file*/
  for (i = optind; i < argc; i++) {
    f = *(argv+i);
	 if ((IN = fopen(f, "r")) == NULL) err(EXIT_FAILURE, "%s", f);
	 
	 while ((seq = sequence_parse(IN, SEQFMT_FASTA)) != NULL) {
		total++;
		id = seq->nam;
		keep = 1;
		if (list != NULL) { keep = 1 - seqlist_chk(&IDs, id); }
		if (excl == 1)   { keep = 1 - keep; }
		if (keep == 1) { n++; sequence_print(OUT, seq, SEQFMT_FASTA); }
		sequence_free(seq);
	 }//end while
	 
	  /* check for parsing error vs EOF */
	 if ((feof(IN) == 0) || (total == 0)){
		errx(1, "%s: unable to read sequence %d", f, total+1);
	 }
	 
	 if (fclose(IN) == EOF) err(EXIT_FAILURE, "%s: close failed", f);
  }//end for files
  
  if ((out != NULL) && (fclose(OUT) == EOF)) err(EXIT_FAILURE, "%s", out);
  if (list != NULL) seqlist_fini(&IDs);
  
  /*display resume on stderr */
  fprintf(stderr, "file %s: Extracted %"PRI_U64" from %"PRI_U64" sequences (%.2f %%)\n" ,
          basename(f), n , total, (float)(n)/total*100.0);

  return EXIT_SUCCESS; 
}
