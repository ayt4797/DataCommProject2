/*********************************************************************
* Example: Using the GSL (Gnu Scientific Library) to solve a system 
* of algebraic equations Ax=b via permuted LU factorization.
*
* student version
*
* Compile: gcc -o lu lu_fact.c -lgsl -lgslcblas
*
* 
* 12/03/2016   R. Repka - Fixed ansi warning for DECLARE_TIMER()
* 11/29/2019   R. Repka - Added true dynamic support 
* Reference: Golub and Vanloan Algorithms 3.4.1
*********************************************************************/
#include <stdio.h>
#include <gsl/gsl_linalg.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "ClassErrors.h"


#ifndef INITIAL_READ_SIZE 
   #define INITIAL_READ_SIZE  (20)
#endif 
#ifndef READ_GROWTH_SIZE
  #define READ_GROWTH_SIZE   (20)
#endif

// Used to return the size of GSL matrix rows and columns 
#define GSL_MAT_ROWS(x)    (x->size1)
#define GSL_MAT_COLS(x)    (x->size2)

// Function prototypes
gsl_matrix* initMatrix(FILE *file);
int readMatrix(FILE *file, gsl_vector *V, gsl_matrix *M);

/*---------------------------------------------------------------------------
      
---------------------------------------------------------------------------*/
int main (int argc, char* argv[]) {
    int  i, j, errorCode = 0;
    FILE *inHandle;
    
    // Allocate the GSL variables
    gsl_matrix *mat;
    gsl_vector *b, *x;
    gsl_permutation *p;
    int s;
    
   /*------------------------------------------------------------------------
     These variables are used to control the getopt_long_only command line 
     parsing utility.  
   ------------------------------------------------------------------------*/
   int rc;
   char *inFile = NULL;
   int verb     = 0;
   int showData = 0;
   
   // getopt_long stores the option index here. 
   int option_index = 0;
  
   // This contains the short command line parameters list 
   char *getoptOptions = "i:vd";
  
   /* This contains the long command line parameter list, it should mostly 
     match the short list.  Note: get_opt_long is not really needed       */
   struct option long_options[] = {
      /* These options don’t set a flag.
         We distinguish them by their indices. */
      {"data",    no_argument, 0, 'd'},
      {"in",      required_argument, 0, 'i'},
      {"input",   required_argument, 0, 'i'},
      {"verb",    no_argument, 0, 'v'},
      {"verbose", no_argument, 0, 'v'},
      {0, 0, 0, 0}
   };
         
   opterr = 1;           // Enable automatic error reporting 
   while ((rc = getopt_long_only(argc, argv, getoptOptions, long_options, 
                                                    &option_index)) != -1) {
   //  printf("getopt_long_only() returned ='%c' index = '%d'\n",  rc, option_index);   
      /* Detect the end of the options. */
      switch (rc)
        {
        case 'i':  
          inFile = optarg;
          break;

        case 'v':   
          verb = 1;
           break;

        case 'd':   
          showData = 1;
           break;
           
        case '?':  // Handled by the default error handler 
          break;

       default:
          printf ("Internal error: undefined option %0xX\n", rc);
          exit(PGM_INTERNAL_ERROR);
       } // End switch 
   } // end while 

   /*------------------------------------------------------------------------
     Check for command line syntax errors like not all parameters specified
     or the first parameter is zero.
   ------------------------------------------------------------------------*/
   if ((optind < argc) || (NULL == inFile)) {
      fprintf(stderr, "This program using GSL PLU factorization to solve a system \n");
      fprintf(stderr, "of algebraic equations via Ax=b\n");
      fprintf(stderr, "usage: hw12  -i[n[put]] file [-v[erb[ose] [-d[ata]\n");
      fprintf(stderr, "Where: -input file - the matrix file to process.\n");
      fprintf(stderr, "                     first line contains the number\n");
      fprintf(stderr, "                     rows and columns in the subsequent\n");
      fprintf(stderr, "                     data\n");
      fprintf(stderr,"        -data       - Print the input A and b data\n");
      fprintf(stderr, "       -verbose    - Enable optional debugging information\n\n");
      fprintf(stderr, "e.g.   hw12 -i rand.txt \n");
      return(PGM_SYNTAX_ERROR);
   } /* End if error */
    
   // Try to open the matrix 
   inHandle = fopen(inFile, "r");
   if (NULL == inHandle){ 
      fprintf(stdout, "Error: input file '%s' not found\n", inFile);
      return(FILE_NOT_FOUND);
   }
	
   // The file is there, initialize the matrix from the file 
   mat = initMatrix(inHandle);

   // We must have a good matrix file, allocate the data arrays 
   b = gsl_vector_calloc(GSL_MAT_ROWS(mat));
   x = gsl_vector_calloc(GSL_MAT_ROWS(mat));
   p = gsl_permutation_alloc(GSL_MAT_ROWS(mat));
   
   // Read the data into the GSL matrix.
   errorCode = readMatrix(inHandle, b, mat);
   if (errorCode) {
      // There is something wrong with the matrix file 
      return(errorCode);
   } /* End else */
   
   if (showData) {
      printf("Matrix A:\n");
      for (i = 0; i < GSL_MAT_ROWS(mat); i++) {
          for (j = 0; j < GSL_MAT_COLS(mat); j++)
              printf ("%8.4f ", gsl_matrix_get(mat, i, j));
      putchar('\n');
      } 
   
   // Print entries of vector b 
   printf("Vector b:\n");
   gsl_vector_fprintf (stdout, b,"%8.4f") ;
   }
   
   // Do the LU decomposition 
   errorCode = gsl_linalg_LU_decomp (mat, p, &s);
   
   if (verb) {               
      fprintf(stdout, "\nP = [");
      gsl_permutation_fprintf (stdout, p, " %u");
      fprintf(stdout, " ] \n");
      
      fprintf(stdout, "LU matrix = \n");
      printf("Matrix LU:\n");
      for (i = 0; i < GSL_MAT_ROWS(mat); i++) {
         for (j = 0; j < GSL_MAT_COLS(mat); j++) {
            printf ("%7.2g ", gsl_matrix_get (mat, i, j));
         } 
      putchar('\n');
      } // End for i              
   } // End if verbose 
   
   // Was the LU good? 
   if(0 != errorCode){
      fprintf(stdout, "Error: Matrix does not have a solution. \n");
      return(NO_SOLUTION);
   } /* End if */
   
   // We have a good LU, solve it 
   gsl_linalg_LU_solve (mat, p, b, x);
   
   printf("Solution x:\n");
   gsl_vector_fprintf (stdout, x, "%8.4f");
   
   // Free things 
   gsl_matrix_free(mat);
   gsl_vector_free(b);
   gsl_permutation_free(p);
   gsl_vector_free(x);
 
   fclose(inHandle);
     
   return(PGM_SUCCESS);
} /* main */


/*---------------------------------------------------------------------------
  This function allocates a GSL Matrix array from the first row of data supplied.
  The first line will have two numbers (separated by spaces), specifying the 
  dimensions of the augmented matrix G = [A b].  
  
  This routine supports input data of arbitrary length.
  
  The following lines will have the rows of G, each entry separated by spaces. 
  The example below shows the two first lines of the data file for a system of 
  5 equations with 5 unknowns

  # indicates a comment, must be at the top of the file 
  5            6
  1.1       3.4       5.6       7.8       7.8       1.1 
   
  Where:   FILE *file - Open file handle to a matrix data text file
  Returns: gsl_matrix * - an empty GSL matrix file of the correct size 
  
  Error policy: Prints an error message and exit with a unique error code
---------------------------------------------------------------------------*/
gsl_matrix* initMatrix(FILE *file){

  char string[256], *pch;
	int nc=0,nr=0;
   gsl_matrix *M;
	int counter; //line Counter
   // You will need this code
	while (fgets (string, 256, file))     {
		counter++;
		if(string[0]=='#') 
			continue;
		if(0==nc||0==nr){
			pch = strtok (string, " \t"); //makes it (nc, nr)
			if(pch == NULL){
				fprintf(stderr,"didn't find nc & nr");
				exit(99);
			}
			nr = atoi(pch); //will just get nc

			pch = strtok (NULL, " \t"); //makes it (nc, nr)
			if(pch == NULL){
				fprintf(stderr,"didn't find nc");
				exit(99);
			}

			nc = atoi(pch);

			if(nc<=0||nr<=0){
				fprintf(stderr,"can't work w/ negatives || zero dimensons");
				exit (99);
			}
			M = gsl_matrix_calloc(nr, nc-1);//nc-1 b/c the last collum is for the vector
			return M;
		}
    }
	return M=NULL;
} // initMatrix


/*---------------------------------------------------------------------------
  This function populates a GSL matrix from a supplied text file of the form
 
  
  The first line will have two numbers (separated by spaces), specifying the 
  dimensions of the augmented matrix G = [A b].  
  
  The following lines will have the rows of G, each entry separated by spaces. 
  The example below shows the two first lines of the data file for a system of 
  5 equations with 5 unknowns
  
  # indicates a comment, must be at the top of the file 
  5            6
  1.1       3.4       5.6       7.8       7.8       1.1 
  
  This routine supports input data of arbitrary length
  
  Where: FILE *file - Open file handle to a matrix text file, with the first line
                      already processed
         gsl_vector *V  - Pointer to a vector to hold the "b" vector 
         gsl_matrix *M  - Pointer to a matrix to hold the "A" matrix
  Errors: none
  Returns: int readMatrix - 0 = success, otherwise
                            4 = too many columns
                            5 = too many rows
---------------------------------------------------------------------------*/

//TODO
int readMatrix(FILE *file, gsl_vector *V, gsl_matrix *M){
   // You will need to use these functions
   // gsl_vector_set(V, i, num);
   // gsl_matrix_set(M, i, j, num);
   long int lineLocation= ftell(file);
   int bufferLength = INITIAL_READ_SIZE;
  char *string= malloc(bufferLength*sizeof(char));
  char* data;
  int row;
  if(string==NULL){
	  fprintf(stderr,"didn't assign data");
	  exit(MALLOC_ERROR);
  }
//   for (int j = 0; j < GSL_MAG_COL(M); j++){
	for(row =0; row<GSL_MAT_ROWS(M);row++){
		// printf("\nrow : %d\n",row);
		fgets (string, bufferLength, file);  //if the buffer length is too small make more buffer
		while ( (strlen(string)+1)>=bufferLength)  {//if the buffer length is too small make more buffer
			bufferLength+=READ_GROWTH_SIZE;

			string =realloc(string,bufferLength);

			if(string==NULL){
				fprintf(stderr,"didn't assign data");
				exit(99);
			}
			fseek(file, lineLocation, SEEK_SET); //moves to file pointer back to the pointer after the comment and the collum set.

			fgets (string, bufferLength, file);
		}
			lineLocation=ftell(file);
				// exit(99);
			if(feof(file)&&row <GSL_MAT_ROWS(M)-1){


				fprintf(stderr,"END OF FILE ERR not enough rows in data\n");
				exit(-99);
			}
			//TODO copy this above for the init function
			int length = strlen(string)-1;
			while (length>=0&&(string[length]=='\r'||string[length]=='\t'||string[length]=='\n'||string[length]==' ')){
				string[length]=0;
				length--;
			}
			
			data = strtok (string, "    \t"); 

			if(data == NULL){
				fprintf(stderr,"init didn't split data");
				exit(99);
			}
			gsl_matrix_set(M,row,0,atof(data)); //sets the zeroth collum
			for (int col = 1; col < GSL_MAT_COLS(M); col++){

				data= strtok(NULL," \t");
					if(data == NULL){

						fprintf(stderr,"didn't split data at row %d, col %d\n",row,col);
						exit(99);
				}
				gsl_matrix_set(M,row,col,atof(data));

			}
			//set vector b
			data= strtok(NULL," ");

			if(data == NULL){ 
				fprintf(stderr,"NOT ENOUGH COLLUMS\n");
				return 4;
			}
			gsl_vector_set(V,row,atof(data));
			data= strtok(NULL," "); //the below commented liens of code creates an error b/c the last digit of the data is 0
			// if(data == NULL){
			// 	exit(99);
			// }
			
		}
	if(row!=GSL_MAT_ROWS(M)){
		fprintf(stderr,"TOO MANY ROWS");
		return 5;
	}
	free(string);
	string =NULL;

	return 0;
} // end readMatrix
