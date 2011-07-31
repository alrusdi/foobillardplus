/* Test Output of UTF8 Strings
   Not for use inside foobillard++
   ONLY FOR TESTING PURPOSES
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*

  Char. number range  |        UTF-8 octet sequence
     (hexadecimal)    |              (binary)
  --------------------+---------------------------------------------
  0000 0000-0000 007F | 0xxxxxxx
  0000 0080-0000 07FF | 110xxxxx 10xxxxxx
  0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
  0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

*/


int main( int argc, char *argv[] )
{

char versuch[100] = "abcdefghijklmnopqrstuvwxyzäöüßÄÖÜ";
unsigned char lb;
int n;
int realcount = 0;

for ( n = 0; versuch[n]!=0; n++ ) {
  lb = versuch[n];
  realcount++;
  if (( lb & 0x80 ) == 0 ) {         // lead bit is zero, must be a single ascii
    printf( "1 octet\n" );
  } else if (( lb & 0xE0 ) == 0xC0 ) { // 110x xxxx
    printf( "2 octets\n" );
    if(versuch[n+1] == 0) {
      printf("Error in 1 Octet. End of String reached\n");
    } else {
      n++;
    }
  } else if (( lb & 0xF0 ) == 0xE0 ) { // 1110 xxxx
    printf( "3 octets\n" );
    if(versuch[n+1] == 0 || versuch[n+2] == 0) {
      printf("Error in 2 Octet. End of String reached\n");
    } else {
      n+=2;
    }
  } else if (( lb & 0xF8 ) == 0xF0 ) { // 1111 0xxx
    printf( "4 octets\n" );
    if(versuch[n+1] == 0 || versuch[n+2] == 0 || versuch[n+3] == 0) {
      printf("Error in 3 Octet. End of String reached\n");
    } else {
      n+=3;
    }
  } else {
    printf( "Unrecognized lead byte (%02x)\n", lb );
  }
}

printf("Output length with strlen: %i\n",strlen(versuch));
printf("The real count is: %i\n",realcount);
printf("String is: %s\n",versuch);
}
