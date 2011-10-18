/* language.c
**
**    code for Language / Localisation
**    Copyright (C) 2010/2011  Holger Schaekel
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License Version 2 as
**    published by the Free Software Foundation;
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "options.h"

unsigned int manualthere = 0; //later quicker check for manual
char foomanual[512]; //for the way to the manual
char localeText[450][200];

#ifdef __MINGW32__ //HS

/***************************************************
 *    replace a string (max. 100 Bytes long)       *
 ***************************************************/

char *replace(char *st, char *orig, char *repl) {
  static char buffer[100];
  char *ch;
  if (!(ch = strstr(st, orig)))
   return st;
  strncpy(buffer, st, ch-st);
  buffer[ch-st] = 0;
  sprintf(buffer+(ch-st), "%s%s", repl, ch+strlen(orig));
  return buffer;
  }

#endif

/***************************************************
 *               Initialize language               *
 *               doing = 1 = all                   *
 *             doing = 0 = only manual             *
 ***************************************************/
void initLanguage(int doing) {

  FILE *fp;
  int counter = 0;
  static char langfile[300] = "locale/";
  static char program[512] = { "\0" };
  static char szTmp[32];
  static char charlang[5];
  char *cp;
  char langfiledefault[300] = "locale/en/";
  char buffer[50];
  char foomanualdefault[512];
#ifdef WETAB
  char foomanual1[512] = "tiitoo-browser-bin -t file://";
#else
  char foomanual1[512];
  if(!strcmp(options_browser,"browser")) {
    strcpy(options_browser,"./browser.sh");
  }
#ifdef __MINGW32__ //HS
  strcpy(foomanual1,"start ");
#else
  sprintf(foomanual1,"%s file://",options_browser);
#endif
#endif

  /* ### TODO ### Get the working directory of the program
   * Mac OS X: _NSGetExecutablePath() (man 3 dyld)
   * Linux: readlink /proc/self/exe
   * Solaris: getexecname()
   * FreeBSD: sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
   * BSD with procfs: readlink /proc/curproc/file
   * Windows: GetModuleFileName() with hModule = NULL
   */

if(doing) {
#ifndef __MINGW32__ //RB
  sprintf(szTmp, "/proc/%d/exe", getpid()); //for Linux at this time only
  readlink(szTmp, program, 512);
  if((cp = strrchr(program,'/'))) { //extract the program name from path
    cp[0] = 0;
  }
  chdir(program); //we need this for data = program-directory
  //fprintf(stderr,"%d %s\n",strlen(program),program);
  if((cp = strrchr(program,'/'))) { //extract the bin from path
    cp[0] = 0;
  }
  strcat(program,"/data");
#else
  strcpy(program,"data");
#endif
  //fprintf(stderr,"%d %s\n",strlen(program),program);
  if(chdir(program) ){
      fprintf(stderr,"Foobillard++ seems not to be correctly installed\n");
      fprintf(stderr,"cannot find valid data directory\n");
      fprintf(stderr,"(assuming the current directory contains the data)\n");
  }
  setlocale(LC_ALL,"");
  //fprintf(stderr,"Lokale: %s\n",setlocale(LC_MESSAGES,NULL));
  sprintf(buffer,"%s",setlocale(LC_MESSAGES,NULL));
  buffer[2] = 0;
  strncpy(charlang,buffer,3);
  if(strlen(charlang) != 2) strcpy(charlang,"en");
  charlang[0] = tolower(charlang[0]);
  charlang[1] = tolower(charlang[1]);
#ifdef __MINGW32__ //HS
  strcpy(charlang,replace(charlang, "ge", "de")); //windows have other locales than Linux/Unix
#endif
  strcat(langfile,charlang);
}
#define OTHER_OS
#ifdef  __MINGW32__ //HS
#undef OTHER_OS
  sprintf(foomanual,"locale/%s/index_a.html",charlang);
  strcpy(foomanualdefault,"locale/en/index_a.html");
  strcat(langfile,"/foobillard.txt");
  strcat(langfiledefault,"/foobillard.txt");
#endif
#ifdef WETAB
#undef OTHER_OS
  sprintf(foomanual,"%s/locale/%s/index.html",program,charlang);
  sprintf(foomanualdefault,"%s/locale/en/index.html",program);
  strcat(langfile,"/wetab-foobillard.txt");
  strcat(langfiledefault,"/wetab-foobillard.txt");
#endif
#ifdef OTHER_OS
#undef OTHER_OS
  sprintf(foomanual,"%s/locale/%s/index_a.html",program,charlang);
  sprintf(foomanualdefault,"%s/locale/en/index_a.html",program);
  strcat(langfile,"/foobillard.txt");
  strcat(langfiledefault,"/foobillard.txt");
#endif
  //fprintf(stderr,"%s\n%s\n",langfile,foomanual);

  if((fp=fopen(foomanual,"r")) != NULL ) {
     fclose(fp);
     manualthere = 1;
     strcat(foomanual1,foomanual);
     strcpy(foomanual,foomanual1);
  } else {
     if((fp=fopen(foomanualdefault,"r")) != NULL ) {
       fclose(fp);
       manualthere = 1;
       strcat(foomanual1,foomanualdefault);
       strcpy(foomanual,foomanual1);
     } else {
       fprintf(stderr,"No manual found.\n");
     }
}
 //fprintf(stderr,"Manual: %s\n",foomanual);
 if(doing) {
  if((fp = fopen(langfile, "r")) == NULL) {
    if((fp = fopen(langfiledefault, "r")) == NULL) {
      fprintf(stderr,"Cannot open language file - terminating.\n");
      exit(1);
      }
  }

  while(fgets(localeText[counter], 200, fp) != NULL) {
      cp = strrchr(localeText[counter],'\r');
      if(cp) *cp = '\0';
      cp = strrchr(localeText[counter],'\n');
      if(cp) *cp = '\0';
      counter++;
  }

  if( fclose( fp )) {
      fprintf(stderr,"Language file close error.\n");
  }
 }
 return;
  }

