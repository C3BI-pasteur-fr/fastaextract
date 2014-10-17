#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <err.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

//#include "squizz/sequence.h"
//#include "squizz/sequence/fasta.h"

#include "sequence.h"
#include "sequence/fasta.h"

/* how to print uint64_t depending on the platform we run on */
#ifdef _LP64
#define PRI_U64 "lu"
#else
#define PRI_U64 "llu"
#endif


static int lenseq_check(sequence_t *seq, size_t len){
  
  return seq->strlen > len; 
}




static void usage(char *prog, int exval) {
  FILE *f = stderr;
  (void)fprintf(f, "usage: %s [options] file ...\n", prog);
  (void)fprintf(f, "  -h          ... Print this message and exit.\n");
  (void)fprintf(f, "  -l <val>    ... Minimum size in base to consider.\n"); 
  (void)fprintf(f, "  -o <file>   ... Use <file> as output results. (default stdout)\n");
  (void)fprintf(f, "  -x           ... If set, display excluded sequences instead of matching ones.\n");
  exit(exval);
}				 


int main(int argc, char **argv){
  FILE *IN, *OUT;
  sequence_t *seq;
  int i, excl, keep;
  size_t len;
  uint64_t n, total;
  char *fa, *out, *prog;
  float p;
  
  /*default values */
  len = excl = 0;
  out = NULL;
  OUT = stdout; 

  prog = basename(argv[0]);

  while((i = getopt(argc, argv, "hl:o:x")) != -1) {      
    switch(i) {
    case 'h':
      usage(prog, EXIT_SUCCESS); break;
    case 'l':
      len = (size_t)atoi(optarg); break; 
    case 'o':
      out = optarg; break; 
	 case 'x':
		excl = 1; break;
    default :
      usage(prog, EXIT_FAILURE);
    }
  }
  
  /* check for mandatory parameters */
  if (argc - optind < 1) { usage(prog, EXIT_FAILURE); }
  fa = argv[optind];
    
  if((out != NULL) && ((OUT = fopen(out, "w"))) == NULL) 
	 err(1, "%s", out);
  
  /* deal with files */
  for (i = optind; i < argc; i++) {
	 fa = *(argv+i);
	 
	 if((IN = fopen(fa, "r")) == NULL) 
		err(1, "%s", fa);
	 
	 total = n = 0;
	 while ((seq = sequence_parse(IN, SEQFMT_FASTA)) != NULL) {
		total++;
		keep = lenseq_check(seq, len); 
		if (excl ==1) { keep = 1 - keep; }
		if (keep == 1) {
		  n++;
		  fprintf(OUT, "%s\n", seq->nam);
		}
		sequence_free(seq);
	 }
	 
	 /* check for parsing error vs EOF */
	 if ((feof(IN) == 0) || (total == 0)){
		errx(1, "%s: unable to read sequence %"PRI_U64, fa, total+1);
	 }
	 
	 p = (float)(n)/total*100.0;
	 fprintf(stderr, "file %s: Excluded %"PRI_U64" from %"PRI_U64" sequences (%.2f %%)\n" ,
				basename(fa), total-n, total, p);
	   
	 fclose(IN);
	 
  }
  if (out != NULL) fclose(OUT);

  return 0;
}




