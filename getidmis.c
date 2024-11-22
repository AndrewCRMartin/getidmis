#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bioplib/general.h"
#include "bioplib/macros.h"

#if !defined(__APPLE__) && !defined(MS_WINDOWS) && !defined(__USE_POSIX2)
extern FILE *popen(const char *, const char *);
#endif
#if !defined(__APPLE__) && !defined(__USE_POSIX2)
extern int pclose(FILE *);
#endif

char *strdup(const char *s);

#define MAXSTRING 160
#define HUGEBUFF 1024
char *gBaseURL = "https://extranets.who.int/inn";
/*
typedef short int BOOL;
#define TRUE 1
#define FALSE 0
*/

/*************************************************************************************/
STRINGLIST *SplitLine(char splitChar, char *page)
{
   int posInPage = 0,
       lastPosInPage = 0;

   STRINGLIST *strings = NULL;
   
   for(posInPage=0; page[posInPage] != '\0'; posInPage++)
   {
      if(page[posInPage] == splitChar)
      {
         page[posInPage] = '\0';
         if((strings = blStoreString(strings, page+lastPosInPage))==NULL)
            return(NULL);
         page[posInPage] = splitChar;
         lastPosInPage = posInPage;
      }
   }
   /* and the last one */
   if((strings = blStoreString(strings, page+lastPosInPage))==NULL)
      return(NULL);
   
   return(NULL);
}

/*************************************************************************************/
char *Substitute(char *string, char *old, char *new, BOOL global)
{
   char *newString = NULL,
      *offset = NULL,
      *chp;
   int newLen = strlen(string) - strlen(old) + strlen(new) + 1;
   int nChar;

   /* old string not found */
   if((offset = strstr(string, old))==NULL)
      return(string);
    
    if((newString = (char *)malloc(newLen * sizeof(char)))==NULL)
       return(NULL);

    nChar = 0;
    /* First part */
    for(chp=string; chp!=offset; chp++)
       newString[nChar++] = *chp;
    /* New part */
    for(chp=new; *chp!='\0'; chp++)
       newString[nChar++] = *chp;
    /* Last part */
    for(chp=string+strlen(old); *chp!='\0'; chp++)
       newString[nChar++] = *chp;
    /* Terminate */
    newString[nChar] = '\0';
    free(string);

    /* If it's global, recurse */
    if(global)
       newString = Substitute(newString, old, new, global);
    
    return(newString);
}

BOOL GetURLandFilename(char *line, char *url, char *filename)
{
   /* $line =~ m/href=\"(.*?)\"\>(.*?)\</;
      href="(.*?)">(.*?)<
    */
   int nURL = 0;
   int nFilename = 0;
   
   char *chp = NULL;
   if((chp = strstr(line, "href"))==NULL)
      return(FALSE);

   chp += 4; /* Skip the 'href' */
   
   /* Found href, eat until we have the = */
   while(*chp != '=')
   {
      if(*chp == '\0')
         return(FALSE);
      chp++;
   }
   chp++; /* Skip the = */

   /* Found =, eat until we have the " or ' */
   while((*chp != '"') && (*chp != '\''))
   {
      if(*chp == '\0')
         return(FALSE);
      chp++;
   }
   chp++; /* Skip the ' or " */

   /* Copy into URL until we reach a ' or " */
   while((*chp != '"') && (*chp != '\''))
   {
      if(*chp == '\0')
         return(FALSE);
      url[nURL++] = *chp;
      chp++;
   }
   url[nURL] = '\0';
   chp++; /* Skip the ' or " */
   
   /* Found ' or ", eat until we have the > */
   while(*chp != '>')
   {
      if(*chp == '\0')
         return(FALSE);
      chp++;
   }
   chp++; /* Skip the > */

   /* Copy into filename until we reach a < */
   while(*chp != '<')
   {
      if(*chp == '\0')
         return(FALSE);
      filename[nFilename++] = *chp;
      chp++;
   }
   url[nFilename] = '\0';

   return(TRUE);
}

/*************************************************************************************/
BOOL StringContains(char *string, char *contains)
{
   if(strstr(string, contains)!=NULL)
      return(TRUE);
   return(FALSE);
}

/*************************************************************************************/
BOOL ReadPasswd(char *passwdFile, char *passwd)
{
   FILE *fp;
   if((fp=fopen(passwdFile, "r"))!=NULL)
   {
      fscanf(fp, "%s", passwd);
      TERMINATE(passwd);
      fclose(fp);
      return(TRUE);
   }
   
   return(FALSE);
}

/*************************************************************************************/
#define CHUNK 128
char *Execute(char *exe)
{
   FILE *fp;
   char *data=NULL,
      ch;
   int  nData=0,
      dataSize=0;

   if((fp = (FILE *)popen(exe,"r"))!=NULL)
   {
      while(1)
      {
         ch=fgetc(fp);
         if((nData+1) > dataSize)
         {
            dataSize += CHUNK;
            data = realloc(data, dataSize);
         }
         data[dataSize++] = ch;
         if(ch==EOF)
         {
            data[dataSize-1] = '\0';
            break;
         }
      }
      pclose(fp);
   }
   
   return(data);
}

BOOL ProcessPage(char *reqID, char *page, char *cFile, char *passwd, BOOL verbose);


/*************************************************************************************/
int main(int argc, char **argv)
{
    char tplURL[MAXSTRING],
         cFile[MAXSTRING],
         passwdFile[MAXSTRING],
         passwd[MAXSTRING],
         url[MAXSTRING],
         exe[MAXSTRING],
         *cDir = "/home/amartin/work/inn/idmis";
    BOOL verbose = TRUE;

    sprintf(tplURL, "%s/Report.php?Request=%%s&Report=INN_Phase_6b_Admin_Report", gBaseURL);
    sprintf(cFile,  "%s/Martin-cert.p12",    cDir);
    sprintf(passwdFile,  "%s/Martin-cert.passwd", cDir);

    ReadPasswd(passwdFile, passwd);

    argc--; argv++;
    while(argc)
    {
        char *page;
        sprintf(url, tplURL, *argv);
        sprintf(exe, "curl -s --cert-type p12 --cert %s:%s %s", cFile, passwd, url);
        page = Execute(exe);
        ProcessPage(*argv, page, cFile, passwd, verbose);
        free(page);
        argc--; argv++;
    }
    return(0);
}

/*************************************************************************************/
BOOL ProcessPage(char *reqID, char *page, char *cFile, char *passwd, BOOL verbose)
{
   STRINGLIST *lines    = NULL,
      *theLine;
    char url[MAXSTRING];
    char exe[MAXSTRING];
    BOOL inData = FALSE;
    char *urlPart,
       *filename;
    urlPart  = (char *)malloc(MAXSTRING * sizeof(char));
    filename = (char *)malloc(MAXSTRING * sizeof(char));
    
    page = Substitute(page, "\r", "", TRUE);
    lines = SplitLine('\n', page);
    
    for(theLine=lines; theLine !=NULL; NEXT(theLine))
    {
        char *line;
        if((line = strdup(theLine->string))==NULL)
           return(FALSE);
                              
        if(inData)
        {
           GetURLandFilename(line, urlPart, filename);  /* $line =~ m/href=\"(.*?)\"\>(.*?)\</; */
           filename = Substitute(filename, " ", "_", TRUE);
           urlPart  = Substitute(urlPart, "./", "", FALSE);
           urlPart  = Substitute(urlPart, "(", "\\(", TRUE);
           urlPart  = Substitute(urlPart, ")", "\\)", TRUE);
           filename  = Substitute(filename, "(", "\\(", TRUE);
           filename = Substitute(filename, ")", "\\)", TRUE);

           sprintf(url, "%s/%s", gBaseURL, urlPart);
           sprintf(exe, "curl -s --cert-type p12 --cert %s:%s --output %s %s", cFile, passwd, filename, url);
           if(verbose) fprintf(stderr, "%s\n", exe);
           Execute(exe);

           inData = FALSE;
        }
        else if(StringContains(line, "Annex"))
        {
           inData = TRUE;            
        }
        else if(StringContains(line, ">Word Document"))
        {
           GetURLandFilename(line, urlPart, filename);  /* $line =~ m/href=\"(.*?)\"\>(.*?)\</; */
           sprintf(filename, "%s_1_6b_Admin_Report.doc", reqID);
           urlPart  = Substitute(urlPart, "./", "", FALSE);
           urlPart  = Substitute(urlPart, "&", "\\&", TRUE);

           sprintf(url, "%s/%s", gBaseURL, urlPart);
           sprintf(exe, "curl -s --cert-type p12 --cert %s:%s --output %s %s", cFile, passwd, filename, url);

           if(verbose) fprintf(stderr, "%s\n", exe);
           Execute(exe);
        }
    }
    free(urlPart);
    free(filename);
    return(TRUE);
}
