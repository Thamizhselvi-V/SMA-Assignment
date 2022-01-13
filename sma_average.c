#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 256
#define SUCCESS 0
#define FAIL -1

struct attribute
{
   double open;
   double high;
   double low;
   double close;
   double adj_close;
   long int volume;
};

int calculate_sma(FILE *, FILE *,int);

int main ( int argc, char *argv[] )
{
   if (argc < 3 || argc > 3)
   {
      printf("Usage : ./sma_average <input_file> <output_file>\n");
      return EXIT_FAILURE;
   }
   FILE *fp = fopen(argv[1],"r");
   FILE *fp_w = fopen(argv[2],"w");
   if ( NULL == fp  || NULL == fp_w) {
      perror("fopen");
      return EXIT_FAILURE;
   }
   if(calculate_sma(fp, fp_w, 10) != SUCCESS) {
      printf("SMA calculation failed\n");
      return EXIT_FAILURE;
   }
   fclose(fp_w);
   fclose(fp);
   return EXIT_SUCCESS;
}

int calculate_sma(FILE *fp, FILE *fp_w, int sma_value)
{
   char buffer[BUFFER_SIZE] = {0};
   struct attribute values[sma_value];
   double open_total= 0 , high_total = 0, low_total = 0, close_total = 0, adj_close_total = 0;
   long long int volume_total = 0;
   int count = 0;
   int day, month, year;
   
   /* Read first row and write to output file */
   fgets(buffer, BUFFER_SIZE, fp);
   fprintf(fp_w,"%s",buffer);

   /* Read the first 9 row data */ 
   while((count < sma_value - 1) && fgets(buffer, BUFFER_SIZE, fp) != NULL ) {
      sscanf(buffer, "%d-%d-%d,%lf,%lf,%lf,%lf,%lf,%ld", &year, &month, &day, &values[count].open, &values[count].high, &values[count].low, &values[count].close, &values[count].adj_close, &values[count].volume);
      fprintf(fp_w,"%4d-%02d-%02d,%lf,%lf,%lf,%lf,%lf,%ld,,,,,,\n", year, month, day,values[count].open, values[count].high, values[count].low, values[count].close, values[count].adj_close, values[count].volume);
      open_total += values[count].open;
      high_total += values[count].high;
      low_total += values[count].low;
      close_total += values[count].close;
      adj_close_total += values[count].adj_close;
      volume_total += values[count].volume;
      count++;
   }

   if (count < 9) {
      printf("Not much data to calculate SMA 10\n");
      return FAIL;
   }

   /* Calculate the SMA based on last 10 entries */
   while(fgets(buffer, BUFFER_SIZE, fp) != NULL) {
      sscanf(buffer, "%d-%d-%d,%lf,%lf,%lf,%lf,%lf,%ld", &year, &month, &day, &values[count].open, &values[count].high, &values[count].low, &values[count].close, &values[count].adj_close, &values[count].volume);
      fprintf(fp_w,"%4d-%02d-%02d,%lf,%lf,%lf,%lf,%lf,%ld,", year, month, day, values[count].open, values[count].high, values[count].low, values[count].close, values[count].adj_close, values[count].volume);
      open_total += values[count].open;
      high_total += values[count].high;
      low_total += values[count].low;
      close_total += values[count].close;
      adj_close_total += values[count].adj_close;
      volume_total += values[count].volume;
      fprintf(fp_w,"%.7lf,%.7lf,%.7lf,%.7lf,%.7lf,%.7lf\n", open_total/sma_value, high_total/sma_value, low_total/sma_value, close_total/sma_value, adj_close_total/sma_value, (double)volume_total/sma_value);
      count++;
      if (count == 10) {
         count = 0;
      }
      open_total -= values[count].open;
      high_total -= values[count].high;
      low_total -= values[count].low;
      close_total -= values[count].close;
      adj_close_total -= values[count].adj_close;
      volume_total -= values[count].volume;
   }
   return SUCCESS;
}
