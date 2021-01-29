#include <stdlib.h>
#include <stdio.h>
#include "extras.h"
#include "types.h"

u8* readFile(char* fname, u64* size) {
   FILE *fp;
   u64 lSize;
   u8 *buffer;
   
   fp = fopen(fname, "rb");
   if(!fp) perror(fname), exit(1);
   
   fseek(fp, 0L, SEEK_END);
   lSize = ftell(fp);
   rewind(fp);
  
   /* allocate memory for entire content */
   buffer = calloc(1, lSize+1);
   if(!buffer) fclose(fp), fputs("memory alloc fails", stderr), exit(1);
   
   /* copy the file into the buffer */
   if(1 != fread(buffer, lSize, 1, fp))
      fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);

   *size = (lSize);
   return buffer;
}
