/*--------------------------------------------------------------------------
  Simple scanf test program
  student copy
--------------------------------------------------------------------------*/
#include <stdio.h>                    /* ex2a_scan.c */

const double mi2km = 1.609;
double convert(double mi) {return (mi * mi2km);}
int main() {
   double miles; 
   printf("Miles to Km Conversion (enter negative number to end)\n");
   do {
      printf("Input distance in miles: ");
      // Your code here
      int rc = scanf("%lf, ",&miles);
	  if(rc<=0){
		  fprintf(stderr,"non positive input\n");
		  return 99;
	  }
	  convert(miles);
	  printf("miles in km ",convert(miles) );
      
      printf("\n%f miles = %f km\n", miles, convert(miles));
   } while (miles > 0.0);
   return 0;
}