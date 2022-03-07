/* Applied Programming Examples: sorting.c
 * student copy
 *
 * Uses qsort() to sort an array of random doubles
 *
 */
 
#include <stdio.h>  
#include <stdlib.h> /* for qsort()    */

/* Function prototypes */
int cmpdbl(const void *p1,const void *p2); /* for qsort() */
 
/* 
 Initialize an array of doubles of size N, with random numbers
 between -50 and 50, sort it and print it 
*/
int main(int argc, char *argv[]) {
   double *array;
   int num;
   
   // Check the command line entry
	if(argc !=2){
		fprintf(stderr,"wrong num param\n");
		return -1;
	}
   
   // Get the memory
	num =atoi(argv[1]);
	printf("the amount of nums %d\n",num);
	if(num<2){
		printf("min number you can give is is 2\n");
		return -1;
	}
   array = malloc(num*sizeof(double));
   if(array==NULL){
	   fprintf(stderr,"err");
	   return -99;
   }
   // Initialize the array with random data
   for(int i = 0; i < num; ++i) {
      array[i] = (rand() % 1001) /10.0 - 50.0;
   }
   
   // Sort the data
	qsort(array,num,sizeof(double),&cmpdbl);
   
   // Print the sorted dat
   for(int i = 0; i < num; ++i) {
      printf("% 10.1f\n", array[i]);
   }
   free(array);
   array=NULL;
   printf("\n");   
return(0);
}  
 
/*---------------------------------------------------------------------------
  The compare function to use.  Cast p1 and p2 to doubles in this case
---------------------------------------------------------------------------*/
int cmpdbl(const void *p1, const void *p2) {
	if(p1>p2)
		return -1;
	else if (p1<p2)
	{
		return 1;
	}
	else return 0;	
}


