#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>

#define MAX_CHARS 1000
#define MAX_TOPSCORES 5

typedef struct {
	char LINE[1000]; 	/* top scoring line */
	int LINE_NO;		/* line number */
	double LINE_SCORE;	/* score of current line */
}	topline_t;

int read_line(char *line, int maxlen);
int mygetchar();
double string_compare(int argc, char *argv[], char *line, int words, int line_no);
int process_line(char *line, int line_no);
double log2(double x);
double score_line(int words, int *match_f, int argc);
int check_input(int argc, char *argv[]);
void print_query(int argc, char *argv[]);
void zero_score(topline_t *A);
void compare_scores(double score, int line_no, char *line, topline_t *A);
void struct_swap(topline_t *from, topline_t *dest);
void print_topscores(topline_t *A);

int
main(int argc, char *argv[]) {
	char line[MAX_CHARS + 1];
	int line_no = 1, words;
	double score;
	topline_t top_scores[MAX_TOPSCORES];
	
	/* if no input or incorrect input is given, error message is shown and
	 * program exits
	 */
	if (!check_input(argc, argv)) {
		return 0;
	}
	
	/* initialise scores in top_scores to zero */
	zero_score(top_scores);
	
	print_topscores(top_scores);
	
	/* prints all queries */
	print_query(argc, argv);
	
	while(read_line(line, MAX_CHARS)) {
		if (strlen(line) > 0) {
			printf("---\n%s\n", line);
			words = process_line(line, line_no);
			score = string_compare(argc, argv, line, words, line_no);
			compare_scores(score, line_no, line, top_scores);
			line_no++;
		}
	}
	printf("------------------------------------------------");
	
	print_topscores(top_scores);
	return 0;
}

/* adapted from 2013 Assignment 1 sample solutions:  
 * read a line of input into the array passed as argument
 * returns false if there is no input available
 * all whitespace characters are removed
 */

int
read_line(char *line, int maxlen) {
	int i=0, c;
	while (((c=mygetchar())!=EOF) && (c!='\n')) {
		if (i<maxlen) {
			line[i++] = c;
		}
	}
	line[i] = '\0';
	return ((i>0) || (c!=EOF));
}

/* Taken from Assignment 1 FAQ page
 * getchar function required for assignment 1 */
int
mygetchar() {
	int c;
	while ((c=getchar())=='\r') {
	}
	return c;
}

/* processes lines (size of line, line number, number of words) */
int
process_line(char *line, int line_no) {
	int size = strlen(line), i, words = 0, space_var = 0;
	
	for (i = 0; i < size; i++) {
		if ((ispunct(line[i]) || isspace(line[i])) || (i == 0 && isalnum(line[i]))) {
			space_var = 1;
		}
		
		else if (isalnum(line[i]) && space_var == 1) {
			words++;
			space_var = 0;
		}
	}
	printf("S2: line = %d, bytes = %d, words = %d\n", line_no, size, words );
	return words;
}

/* log base 2 */
double
log2(double x) {
	return (log(x)/log(2.0));
}

/* scores the line */
double
score_line(int words,int *match_f, int argc) {
	int sum = 0, i;
	double score;
	
	for(i = 0; i < argc; i++) {
		sum += log2(1.0 + match_f[i]);
	}
	
	score = sum/log2(8.5 + words);
	
	return score;
}

/* compares argv to prefixes of words from a line */
double
string_compare(int argc, char *argv[], char *line, int words, int line_no) {
	char str1[25];
	int i, j, k, str_depth = 0, match_f[argc];
	
	for (k = 0; k < argc; k++) {
		match_f[k] = 0;
	}
	
	for (i = 0; i <= strlen(line); i++) {
		if (isspace(line[i]) || line[i] == '\0' || ispunct(line[i])) {
			str1[str_depth] = '\0'; 
			str_depth = 0;
			for (j = 1; j < argc; j++) {
				if ((strncmp(argv[j], str1, strlen(argv[j])))==0) {
					match_f[j]++;
				}
			}
		}
		else if (isalnum(line[i]) != 0){
			str1[str_depth] = tolower(line[i]);
			str_depth++;
		}
	}
	printf("S3: line = %d, score = %.3f\n",
		line_no ,score_line(words, match_f, argc));
	
	return score_line(words, match_f, argc);
	
}

/* checks the input arguments, returns 0 if any of the inputs fail, returns 1 
   if input arguments are valid */
int
check_input(int argc, char *argv[]) {
	int i, j;
	
	if (argc < 2) {
		printf("S1: No query specified, must provide at least one word");
		return 0;
	}
	
	for (i = 1; i < argc; i++) {
		for (j = 0; j < strlen(argv[i]); j++) {
			if (!islower(argv[i][j]) && !isdigit(argv[i][j])) {
				printf("S1: %s: invalid character(s) in query", argv[i]);
				return 0;
			}
		}
	}
	return 1;
}

/* prints query */
void
print_query(int argc, char *argv[]) {
	int i;
	printf("S1: query =");
	for (i = 1; i < argc; i++){
		printf(" %s", argv[i]);
	}
	printf("\n");
}

/* sets initial scores of all toplines to zero */
void
zero_score(topline_t *A) {
	int i;
	for (i = 0; i < MAX_TOPSCORES; i++) {
		A[i].LINE_SCORE = 0;
	}
	return;
}

/* compares score to existing top scores and changes them if necessary */
void
compare_scores(double score, int line_no, char *line, topline_t *A) {
	int i, j;
	
	for (i = 0; i < MAX_TOPSCORES; i++) {
		if (score > A[i].LINE_SCORE) {
			for(j = MAX_TOPSCORES - 1; j > i; j-- ) {
				struct_swap(&A[j-1], &A[j]);
			}
			A[i].LINE_SCORE = score;
			A[i].LINE_NO = line_no;
			strcpy(A[i].LINE, line);
			break;
		}
	}
}

/* swaps LINE_NO, LINE_SCORE, LINE from *from to *dest */
void
struct_swap(topline_t *from, topline_t *dest) {
	dest -> LINE_NO = from -> LINE_NO;
	dest -> LINE_SCORE = from -> LINE_SCORE;
	strcpy(dest -> LINE, from -> LINE);
	return;
}

/* prints all top scores */
void
print_topscores(topline_t *A) {
	int i;
	for(i = 0; i < MAX_TOPSCORES; i++) {
		if (A[i].LINE_SCORE == 0 || !strcmp(A[i].LINE, A[i-1].LINE)) {
			break;
		}
		printf("\nS4: line = %d, score = %.3f\n%s\n---",
			A[i].LINE_NO, A[i].LINE_SCORE, A[i].LINE);
	}
	return;
}


								/* Algorithms are fun!  */