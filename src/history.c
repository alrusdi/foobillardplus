/* history.c
**
**    code for game history
**
**    Version foobillard++ started at 12/2010
**    Copyright (C) 2011 - 2013 Holger Schaekel (foobillardplus@go4more.de)
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

// don't use functions of history unless the language is initialized!

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys_stuff.h>
#ifdef USE_WIN
  #include <sys/time.h>
  #include <windows.h>
  #include <shellapi.h>
#else
  #include <time.h>
#endif
#include "language.h"
#include "billmove.h"
#include "history.h"

int history = 0; //history for one game 0 = work, 1 = no work
static char file_name[512];

/***********************************************************************
 *                         clear history status                        *
 *                     is then free for xml-write                      *
 ***********************************************************************/
void history_clear(void) {
	 history = 0;
}
/***********************************************************************
 *                         set history status                          *
 *                 is then not free for xml-write                      *
 ***********************************************************************/
void history_set(void) {
	 history = 1;
}

/***********************************************************************
 *                         get history status                          *
 ***********************************************************************/

int history_free(void) {
	 return(history);
}

/***********************************************************************
 *                    get history profile directory                    *
 *                    string length minimum 256                        *
 ***********************************************************************/

void get_history(char *strpointer) {
	 strcpy(strpointer,file_name);
}

/***********************************************************************
 *                         check for xml-file                          *
 ***********************************************************************/

int check_xml(char *filecheck) {

	  char file[1024];

	  sprintf(file,"%s/html/%s",file_name,filecheck);
      return(file_exists(file));
}

/***********************************************************************
 *                  show normal history in browser                     *
 ***********************************************************************/

void show_history(char * historyfile) {
	  char workstring[1024];
	  // build the browser call string
#ifdef __APPLE__
	  sprintf(workstring,"%s/html/%s",file_name,historyfile);

	  void open_file_with_browser_mac(char *file);

	  open_file_with_browser_mac(workstring);
#else

#ifdef USE_WIN
	  sprintf(workstring,"%s/html/%s",file_name,historyfile);
#else
	  char callstring[1024];
	  get_browser(callstring);
	  sprintf(workstring," %s%s/html/%s",callstring,file_name,historyfile);
#endif
	  launch_command(workstring);
#endif
}

/***********************************************************************
 *               write data to tournament historyfile                  *
 ***********************************************************************/

void file_tournament_history( struct TournamentState_ * ts, char *winner, enum gameType gametype ) {

   /* gametype enum from billmove.h GAME_8BALL, GAME_9BALL, GAME_CARAMBOL, GAME_SNOOKER
    localeText[432] etc.*/
   //fprintf(stderr,"Start: %s, %s, %s, %i, %i, %i\n",player1,player2,winner,hits,rounds,gametype);
   FILE *fp, *wfp;
   char historyfile[1024];  // no file with directory should be longer
   char newfile[1024];      // no file with directory should be longer
   char checkstring[2048];  // no entry inside the history-file should be longer
   char datestring[25];
   char gameid[200];
   time_t rawtime;
   struct tm * timeinfo;
   int i,j;

   if(!ts->tournament_over) {
     fprintf(stderr,"Something is wrong with tournament write to history\n");
     return; // nothing to do
   }
   time(&rawtime);
   timeinfo = localtime(&rawtime);
   snprintf(gameid,sizeof(gameid),"%02d%02d%04d%02d%02d%02d-%i",timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_year+1900,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,rand());
   snprintf(datestring,sizeof(datestring),"%02d.%02d.%04d %02d:%02d:%02d",timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_year+1900,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
   //fprintf(stderr,"%s\n",datestring);
   if(check_xml("tournament.xml")) {
     // write new history to the file
     sprintf(historyfile,"%s/html/%s",file_name,"tournament.xml");
     sprintf(newfile,"%s/html/%s",file_name,"tournament.tmp");
     if((fp=fopen(historyfile,"r"))) {
        if((wfp=fopen(newfile,"w+"))) {
          while(fgets(checkstring, sizeof(checkstring), fp) != NULL) {
             if(strstr(checkstring,"</start>")) {
               break;
             } else {
               fprintf(wfp,"%s",checkstring);
             }
          }
        // write tournament to file
        fprintf(wfp,"<%s>\n<gameid>%s</gameid>\n<date>%s</date>\n<allwin>%s</allwin>\n\n",
             localeText[432+gametype],gameid,datestring,winner);
        for(i=0;i<=ts->round_ind;i++) {
             for(j=0;j<(1<<(ts->round_num-i-1));j++){
                 fprintf(wfp,"<g%i>\n<go>%i</go>\n<p1>%s</p1>\n<p2>%s</p2>\n",
                 		 i+1,j+1,ts->roster.player[ts->game[i][j].roster_player1].name,ts->roster.player[ts->game[i][j].roster_player2].name);
                 if(ts->game[i][j].winner == 0) {
                    fprintf(wfp,"<w>%s</w>\n",ts->roster.player[ts->game[i][j].roster_player1].name);
                 } else {
                    fprintf(wfp,"<w>%s</w>\n",ts->roster.player[ts->game[i][j].roster_player2].name);
                 }
                 fprintf(wfp,"<h>%i</h>\n<r>%i</r>\n</g%i>\n",ts->game[i][j].hits,ts->game[i][j].rounds,i+1);
             }
        }
        fprintf(wfp,"</%s>\n</start>\n",localeText[432+gametype]);
        fclose(fp);
        fclose(wfp);
        remove(historyfile);
        rename(newfile,historyfile);
        } else {
          fclose(fp);
        }
     }
   }
}

/***********************************************************************
 *                 write data to normal historyfile                    *
 ***********************************************************************/

void file_history(char *player1, char *player2, char *winner, int hits, int rounds,enum gameType gametype) {

	  /* gametype enum from billmove.h GAME_8BALL, GAME_9BALL, GAME_CARAMBOL, GAME_SNOOKER
    localeText[432] etc.*/
   //fprintf(stderr,"Start: %s, %s, %s, %i, %i, %i\n",player1,player2,winner,hits,rounds,gametype);
	  FILE *fp, *wfp;
   char historyfile[1024];  // no file with directory should be longer
   char newfile[1024];      // no file with directory should be longer
   char checkstring[2048];  // no entry inside the history-file should be longer
  	char datestring[25];
   time_t rawtime;
   struct tm * timeinfo;

   time(&rawtime);
   timeinfo = localtime(&rawtime);
   snprintf(datestring,sizeof(datestring),"%02d.%02d.%04d %02d:%02d:%02d",timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_year+1900,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
   //fprintf(stderr,"%s\n",datestring);
	  if(check_xml("history.xml")) {
     // write new history to the file
     snprintf(historyfile,sizeof(historyfile),"%s/html/%s",file_name,"history.xml");
     snprintf(newfile,sizeof(newfile),"%s/html/%s",file_name,"history.tmp");
		   if((fp=fopen(historyfile,"r"))) {
        if((wfp=fopen(newfile,"w+"))) {
		   	    while(fgets(checkstring, sizeof(checkstring), fp) != NULL) {
		   	  	    if(strstr(checkstring,"</start>")) {
		   	  	  	   break;
		   	  	    } else {
		   	  	      fprintf(wfp,"%s",checkstring);
		   	  	    }
		        }
        fprintf(wfp,"<%s>\n<date>%s</date>\n<player1>%s</player1>\n<player2>%s</player2>\n<winner>%s</winner>\n<hits>%i</hits>\n<rounds>%i</rounds>\n</%s>\n</start>\n",
        		  localeText[432+gametype],datestring,player1,player2,winner,hits,rounds,localeText[432+gametype]);
  	     fclose(fp);
  	     fclose(wfp);
  	     remove(historyfile);
  	     rename(newfile,historyfile);
		      } else {
		        fclose(fp);
		      }
		   }
	  }
}

/***********************************************************************
 *                           parse xml files                           *
 ***********************************************************************/

void parse_history(char *inputfile, char *outputfile) {

   FILE *fp,*wfp;
   int i;
   char *parseinput[] = {"Game History","8 ball","9 ball","Carom","Snooker","Date","Player 1","Player 2","Winner","Strokes",
   		"Rounds","Tournament History","Game No.","Tournament Date","Round"};
   char *parseoutput[] = {localeText[436],localeText[93],localeText[94],localeText[95],localeText[96],localeText[437],localeText[0],
   		localeText[2],localeText[438],localeText[439],localeText[440],localeText[444],localeText[445],localeText[446],localeText[447]};
#define ARRAYLEN 15
   char checkstring[2048];  // no entry inside the history-file should be longer
   char *buffer;

   if((fp=fopen(inputfile,"r"))) {
   	  if((wfp=fopen(outputfile,"w+"))) {
  	      while(fgets(checkstring, sizeof(checkstring), fp) != NULL) {
  	      	 for(i=0;i< ARRAYLEN ;i++) {
  	      	 	 buffer=replace(checkstring, parseinput[i], parseoutput[i]);
	            strcpy(checkstring,buffer);
  	      	 }
  	      	 fprintf(wfp,"%s",checkstring);
  	      }
  	   fclose(wfp);
   	  }
  	   fclose(fp);
   }
#undef ARRAYLEN
}

/***********************************************************************
 *                         init history system                         *
 ***********************************************************************/

void init_history(void) {

	  char *filenames[] = {"body-bg.png","content-bg.png","feature.jpg","footer-bg.png","gradient-shadow.png","header-bg.jpg","logo.jpg","sidebar-h3-bg.jpg"};
#define ARRAYLEN 8
   char directorycheck[1024];
   char directorycheck1[1024];
   char copy_file[1024];
   int i, checktournament, checkhistory;

#ifdef USE_WIN
   strcpy(file_name,getenv("USERPROFILE"));
   strcat(file_name,"/Desktop/foobillardplus-data");
   mkdir(file_name); //build directory every time is not a problem
#elif defined(__APPLE__)
   strcpy(file_name,getenv("HOME"));
   strcat(file_name,"/Library/Application Support/Foobillard++");
   mkdir(file_name,0777); //build directory every time is not a problem
#else
   strcpy(file_name,getenv("HOME"));
   strcat(file_name,"/foobillardplus-data");
   mkdir(file_name,0777); //build directory every time is not a problem
#endif
   checktournament = check_xml("tournament.xml");
   checkhistory = check_xml("history.xml");
   if(!checkhistory || !checktournament) {
   	  //only build, if no xml-file is there
   	  //fprintf(stderr,"Call only one time\n");
#ifdef USE_WIN
      sprintf(directorycheck,"%s/html",file_name);
      mkdir(directorycheck);
      sprintf(directorycheck1,"%s/html/images",file_name);
      mkdir(directorycheck1);
#else
      sprintf(directorycheck,"%s/html",file_name);
      mkdir(directorycheck,0777);
      sprintf(directorycheck1,"%s/html/images",file_name);
      mkdir(directorycheck1,0777);
#endif
      if(!chdir("html") ) {
      	if(!checkhistory) {
         sprintf(copy_file,"%s/%s",directorycheck,"history.xsl");
         filecopy("history.xsl",copy_file);
         sprintf(copy_file,"%s/%s",directorycheck,"history.xml");
         parse_history("history.xml",copy_file);
      	}
      	if(!checktournament) {
         sprintf(copy_file,"%s/%s",directorycheck,"tournament.xsl");
         filecopy("tournament.xsl",copy_file);
         sprintf(copy_file,"%s/%s",directorycheck,"tournament.xml");
         parse_history("tournament.xml",copy_file);
      	}
         sprintf(copy_file,"%s/%s",directorycheck,"styles.css");
         filecopy("styles.css",copy_file);
         if(!chdir("images")) {
         	for(i=0;i< ARRAYLEN ;i++) {
            sprintf(copy_file,"%s/%s",directorycheck1,filenames[i]);
            filecopy(filenames[i],copy_file);
         	}
            chdir("../..");
         } else {
         	chdir("..");
         }
      }
    }
#undef ARRAYLEN
}

