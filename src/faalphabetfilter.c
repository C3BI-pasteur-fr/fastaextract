#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ctype.h>
#include <err.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
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


#define DNA_STRICT  "ATGC"
#define DNA_LAX     "ACGTN"
#define DNA_DEGEN   "ACGTURYWSMKHDBVN"
#define RNA         "AUGC"
#define PROT_DEGEN  "ARNDCQEGHILKMFPSTWYV"
#define PROT_STRICT "ARNDCQEGHILKMFPSTWYVBZX"

/* Number of elements in an array */
#define NELEM(a) (sizeof (a) / sizeof *(a))


static  char *alphabets[] = { DNA_STRICT, DNA_LAX, DNA_DEGEN ,\
                             RNA , PROT_STRICT, PROT_DEGEN  };

static  char *alph_names[] = {  "DNA (strict)", "DNA (lax)", 
                                "DNA (degenerated)", "RNA", 
                                "PROT (strict)", "PROT (degenerated)"  };



static int alphabet_check(sequence_t *seq, char *alphabet){
  int ret;
  char *p;
  ret = 0; // assume no errors

  p = seq->str;
  
  while (*p) {
	 if (strchr(alphabet, toupper((int)*p)) == NULL){
		ret = 1;
		break;
	 }
	 p++;
  }
  return (ret == 0);
}


static void  show_alphabets(void) {
  int i;
  (void)fprintf(stdout, "\nBuiltin codes are:\n");
  for (i = 0 ; i < NELEM(alphabets); i++) {
    (void)fprintf(stdout, "%i :  %-20s", i+1, alph_names[i]);
    (void)fprintf(stdout, "%s\n", alphabets[i]);
  }
}


static void usage(char *prog, int exval) {
  FILE *f = stderr;
  (void)fprintf(f, "usage: %s [options] file ...\n", prog);
  (void)fprintf(f, "  -A <string>  ... Use string as alphabet for sequence check.\n");
  (void)fprintf(f, "  -a <code>    ... Use code as alphabet for sequence check.\n");
  (void)fprintf(f, "                   see \"faalphabetsort -c\" for the details of known alphabets.\n");
  (void)fprintf(f, "                   If no code is provided, no filtering is performed\n"); 
  (void)fprintf(f, "  -c           ... display bultins alphabets..\n");
  (void)fprintf(f, "  -h           ... Print this message and exit.\n");
  (void)fprintf(f, "  -o <file>    ... Use <file> as output results. (default stdout)\n");
  (void)fprintf(f, "  -x           ... If set, display excluded sequences instead of matching ones.\n");
  exit(exval);
}				 


int main(int argc, char **argv){
  FILE *IN, *OUT;
  sequence_t *seq;
  int i, excl, keep;
  int code_num;
  char *code, *q;
  int alph_len;
  uint64_t n, total;
  char *fa, *out, *prog;
  float p;
  
  /*default values */
  code_num = -1;
  code  = DNA_STRICT;
  excl = 0;
  out = NULL;
  OUT = stdout; 
  
  prog = basename(argv[0]);

  while((i = getopt(argc, argv, "A:a:cho:x")) != -1) {      
    switch(i) {
	 case 'A':
		code = optarg ; alph_len = strlen(code); code_num=0;
                /* avoid case probems while checking the alphabet */
	        q = code;
	        while (*q) {*q = toupper((int)*q), q++; }
	        break;
	 case 'a':
		code_num = atoi(optarg)-1;
		if (code_num >= 0 && code_num < NELEM(alphabets)) {
		  code = alphabets[code_num];
        alph_len = strlen(code);
      }
		else 
		  errx(EXIT_FAILURE, 
				 "unknown alphabet %d see \"faalphabetsort\" -c for code description",
				 code_num+1);
      break;
	 case 'c':
      show_alphabets(); return EXIT_SUCCESS;
    case 'h':
      usage(prog, EXIT_SUCCESS); break;
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
		keep = 1;
		if (code_num != -1)  keep = alphabet_check(seq, code); 
		if (excl ==1) { keep = 1 - keep; }
		if (keep == 1) { fprintf(OUT, "%s\n", seq->nam); n++; }
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




