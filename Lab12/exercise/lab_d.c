/* Applied Programming Examples: sorting.c
 * student copy
 *
 * Uses qsort() to sort an array of random polar data
 *
 * Use compiler directive -DN=size to change the size of the array
 *
 * Reference: A. Kelley and I Pohl "A book on C" 4th Ed.
 * Revised: 3/31/2015 (JCCK)
 */
 
#include <stdio.h>  
#include <math.h>
#include <float.h>
#include <stdlib.h> /* for qsort()    */

/* The polar data structure */
typedef struct {double mag; double ang;} polar;

/* Add your polar compare function prototype here */
int cmpdbl(const void *p1,const void *p2); /* for qsort() */

/* 
 Initialize an array of doubles of size N, with random numbers
 between -50 and 50, sort it and print it 
*/
int main(int argc, char *argv[]) {

   polar *array;
   int num;
   int i;
   
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
   array = malloc(num*sizeof(polar));
   if(array==NULL){
	   fprintf(stderr,"err");
	   return -99;
   }
   // Initialize the array with random data, make one magnitude identical
   for(i = 0; i < num-1; ++i) {
      array[i].mag = (rand() % 1001) /10.0 - 50.0;
      array[i].ang = (rand() % 1001) /10.0 - 50.0;
   }
   // Identical 
   array[i].mag = array[i-1].mag;
   array[i].ang = (rand() % 1001) /10.0 - 50.0;


//    printf("\nnum : \n")
	qsort(array,num,sizeof(polar),&cmpdbl);
   
//    // Print the sorted dat
//    for(int i = 0; i < num; ++i) {
//       printf("% 10.1f\n", array[i]);
//    }
     // Print the sorted dat
   for(i = 0; i < num; ++i) {
      printf("%10.1f %10.1f\n", array[i].mag, array[i].ang);
   }
   printf("\n");   
   free(array);
   array=NULL;
   printf("\n");   

   
 
return(0);
}  
 
int cmpdbl(const void *p1, const void *p2) {
	const polar* q1=p1;
	const polar* q2=p2;

	if(q1->mag>q2->mag)
		return -1;
	else if (q1->mag<q2->mag)	{
		return 1;
	}
	else if( q1->ang>q2->ang){
		return -1;
	}
	else if( q1->ang<q2->ang){
		return 1;
	}
	else {
		return 0;
	}
}
