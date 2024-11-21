#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bioplib/macros.h"

#if !defined(__APPLE__) && !defined(MS_WINDOWS) && !defined(__USE_POSIX2)
extern FILE *popen(const char *, const char *);
#endif
#if !defined(__APPLE__) && !defined(__USE_POSIX2)
extern int pclose(FILE *);
#endif



#define MAXSTRING 160
char *gBaseURL = "https://extranets.who.int/inn";
typedef short int BOOL;
#define TRUE 1
#define FALSE 0

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

void ProcessPage(char *reqID, char *page, char *cFile, char *passwd, BOOL verbose);


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

    sprintf(tplURL, "%s/Report.php?Request=%%d&Report=INN_Phase_6b_Admin_Report", gBaseURL);
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

char *Substitute(char *string, char *old, char *new, BOOL global)
{
    char *newString = NULL;

    
    free(string);
    return(newString);
}

void GetURLandFilename(char *line, char *url, char *filename)
{  /* $line =~ m/href=\"(.*?)\"\>(.*?)\</; */
}

char **SplitLine(char splitChar, char *page, int *nLines)
{
   return(NULL);
}

BOOL StringContains(char *string, char *contains)
{
   return(TRUE);
}

void ProcessPage(char *reqID, char *page, char *cFile, char *passwd, BOOL verbose)
{
    char **lines    = NULL;
    char url[MAXSTRING];
    char exe[MAXSTRING];
    int nLines, i;
    BOOL inData = FALSE;
    char *urlPart,
       *filename;
    urlPart  = (char *)malloc(MAXSTRING * sizeof(char));
    filename = (char *)malloc(MAXSTRING * sizeof(char));
    
    page = Substitute(page, "\r", "", TRUE);
    lines = SplitLine('\n', page, &nLines);
    
    for(i=0; i<nLines; i++)
    {
        char *line;
        line = lines[i];  /* Malloc it */
                              
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
}
