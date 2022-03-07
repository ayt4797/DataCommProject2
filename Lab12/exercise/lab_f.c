/*---------------------------------------------------------------------------
  This program demonstrates the limitations of scanf() and the use of fseek()
  student copy
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE 256

int main(int argc, char *argv[]) {
   // File name to read and the handle
   FILE *DataFile = NULL;
   char *fileName = "../data.txt";
   
   // The data to parse
   int num, v1, v2, v3;
   char *buffer;
   // Get memory
   buffer= malloc(sizeof(char)*BUFF_SIZE);
   if (NULL == buffer) {
      fprintf(stderr,"Malloc error\n");
      exit(99);
   }
   
   // Open the data
  	DataFile= fopen( fileName, "r+");
   if (NULL == DataFile) {
      printf("Error could not open '%s'\n", fileName);
   }
      
   // Read all the EXACT data in the file using fgets(), in a while loop
   // and print the results out, removing any trailing LF \n character, if any.
   while (fgets(buffer,BUFF_SIZE,DataFile)!=0){
      // remove the lf at the end
	// strncpy(&buffer[sizeof(buffer)-2],&buffer[sizeof(buffer)-1],sizeof(buffer)-1);
	if(strlen(buffer)>=1)
	  buffer[strlen(buffer)-1]='\0';
      printf("Read: '%s'\n", buffer);
   } 
   
   // Use fseek() to start back at the beginning of the file
	fseek(DataFile, 0,SEEK_SET);
   
   // Read all the data in the file and print the results out
   while ((num=(fscanf(DataFile, "%d %d %d",&v1,&v2,&v3)))>(0)){
	  
      printf("%d parameters %d %d %d\n", num, v1, v2, v3);
   } 
   
   // Return memory, close files
	free(buffer);
	fclose(DataFile);
   return(0);
}  
