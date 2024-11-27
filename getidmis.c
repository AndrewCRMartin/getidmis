/*
  There is still a problem in Substitute() - if we malloc an extra 1000 bytes it doesn't core dump
  when building the url, but it does in doing the & substituteion at line 129 as the recursion doesn't
  return!
 */


#undef TEST_GETURLANDFILENAME
#define TEST_PROCESSPAGE
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

/* Prototypes */
BOOL ProcessPage(char *reqID, char *page, char *cFile, char *passwd,
                 BOOL verbose);
char *strdup(const char *s);

#define MAXSTRING 512
#define HUGEBUFF 1024
char *gBaseURL = "https://extranets.who.int/inn";
/*
typedef short int BOOL;
#define TRUE 1
#define FALSE 0
*/

char *mystrdup(char *in)
{
   char *out;
   ULONG inlen;
   inlen = strlen(in) + 8;
#ifdef TEST
   printf("Input length: %ld\n", inlen);
#endif
   out = (char *)malloc(inlen*sizeof(char));
   strcpy(out, in);
   return(out);
}


/** WORKING ******************************************************************/
STRINGLIST *SplitLine(char splitChar, char *page)
{
   ULONG posInPage = 0,
      lastPosInPage = 0,
      pageSize = 0;

   STRINGLIST *strings = NULL;
   char *pageCopy = NULL;

   if((pageCopy=mystrdup(page))==NULL)
      return(NULL);

   pageSize = strlen(pageCopy);
   
/*
   printf(">>> lens: %d %ld\n", strlen(page), pageSize);
   printf(">>> '%c'\n", pageCopy[pageSize]);
*/
   
   for(posInPage=0; posInPage<pageSize; posInPage++)
   {
#ifdef TEST
      printf("%c", pageCopy[posInPage]);
#endif
      printf("%ld,", posInPage);
      fflush(stdout);
      if(pageCopy[posInPage] == splitChar)
      {
         pageCopy[posInPage] = '\0';
         if((strings = blStoreString(strings, pageCopy+lastPosInPage))==NULL)
         {
            free(pageCopy);
            return(NULL);
         }
         
         posInPage++;
         lastPosInPage = posInPage;
      }
   }
   
   /* and the last one */
   if(strlen(pageCopy+lastPosInPage) > 1)
   {
      if((strings = blStoreString(strings, pageCopy+lastPosInPage))==NULL)
      {
         free(pageCopy);
         return(NULL);
      }
   }

   free(pageCopy);
   return(strings);
}
#ifdef TEST_SPLITLINE
#   define TEST 1

int main(int argc, char **argv)
{
   
   STRINGLIST *strings = NULL, *s;

   strings = SplitLine('\n', "Once upon a time\nThere was a piece of code\n\
That needed to be tested\n");
   for(s=strings; s!=NULL; NEXT(s))
   {
      printf("%s\n", s->string);
   }
   return(0);
}
#endif


/** WORKING ******************************************************************/
/*************************************************************************************/
char *Substitute(char *string, char *old, char *new, BOOL global)
{
   char *newString = NULL,
      *offset = NULL,
      *chp;
   int newLen;
   int nChar;

   /* old string not found */
   if((offset = strstr(string, old))==NULL)
      return(string);
    
    newLen = strlen(string) - strlen(old) + strlen(new) + 1000;
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
    for(chp=offset+strlen(old); *chp!='\0'; chp++)
       newString[nChar++] = *chp;
    /* Terminate */
    newString[nChar] = '\0';
    free(string);

    /* If it's global, recurse */
    if(global)
       newString = Substitute(newString, old, new, global);
    
    return(newString);
}

#ifdef TEST_SUBSTITUTE
#   define TEST 1
int main(int argc, char **argv)
{
   char *string;
   string = (char *)malloc(200*sizeof(char));
   strcpy(string,"Once upon a time\nThere was a piece of code\n\
That needed to be tested");

   string = Substitute(string, "\n", "|", TRUE);
   printf("%s\n", string);
   return(0);
   
}
#endif


/** WORKING  ******************************************************************/

BOOL GetURLandFilename(char *line, char *url, char *filename)
{
   /* $line =~ m/href=\"(.*?)\"\>(.*?)\</;
      href="(.*?)">(.*?)<
    */
   int nURL = 0;
   int nFilename = 0;
   char ch;
   
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
/*      url[nURL++] = *chp; */
      ch = *chp;
      url[nURL] = ch;
      nURL++;
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
   filename[nFilename] = '\0';

   return(TRUE);
}

#ifdef TEST_GETURLANDFILENAME
#   define TEST 1
int main(int argc, char **argv)
{
   /*
   char *line = "Some text \
<a href='http://xyz.com/something'>MyFileName.txt</a> More text";
   */
   char *line = "\t\t\t\t\t\t\t\t</td><td><table><tbody><tr><td><a href=\"./INN_Requests/12079/Document_Info/12079_decobatamab_dezotansine.pdf\">12079_decobatamab_dezotansine.pdf</a></td></tr></tbody></table></td></tr><tr><td>";
   char url[MAXSTRING],
      filename[MAXSTRING];
   BOOL ret;
      
   ret = GetURLandFilename(line, url, filename);
   
   printf("ret: %s\n", ret?"TRUE":"FALSE");
   printf("url: %s\n", url);
   printf("fnm: %s\n", filename);
   return(0);
   
}
#endif


/** WORKING ******************************************************************/
BOOL StringContains(char *string, char *contains)
{
   if(strstr(string, contains)!=NULL)
      return(TRUE);
   return(FALSE);
}
#ifdef TEST_STRINGCONTAINS
#   define TEST 1
int main(int argc, char **argv)
{
   char *line = "Some text which I want to test";
   BOOL ret;
      
   ret = StringContains(line, "text");
   printf("'text' %s\n", ret?"TRUE":"FALSE");

   ret = StringContains(line, "elephant");
   printf("'elephant' %s\n", ret?"TRUE":"FALSE");

   return(0);
   
}
#endif

/** WORKING ******************************************************************/
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
#ifdef TEST_READPASSWD
#   define TEST 1
int main(int argc, char **argv)
{
    char cFile[MAXSTRING],
         passwdFile[MAXSTRING],
         passwd[MAXSTRING],
         *cDir = "/home/amartin/work/inn/idmis";

    sprintf(cFile,  "%s/Martin-cert.p12",    cDir);
    sprintf(passwdFile,  "%s/Martin-cert.passwd", cDir);

    ReadPasswd(passwdFile, passwd);

    printf("pwd: %s\n", passwd);

    return(0);
   
}
#endif

/** WORKING ******************************************************************/
#define CHUNK 128
char *Execute(char *exe)
{
   FILE *fp;
   char *data    = NULL,
        ch;
   int  nData    = 0,
        dataSize = 0;

#ifdef TEST_PROCESSPAGE
   printf("%s", exe);
#else
   if((fp = (FILE *)popen(exe,"r"))!=NULL)
   {
      while(1)
      {
         ch=fgetc(fp);
         if(nData >= dataSize)
         {
            dataSize += CHUNK;
            if((data = realloc(data, dataSize*sizeof(char)))==NULL)
               return(NULL);
         }
         data[nData++] = ch;
         if(ch==EOF)
         {
            data[nData-1] = '\0';
            break;
         }
      }
      pclose(fp);
   }
#endif
   
   return(data);
}
#ifdef TEST_EXECUTE
#   define TEST 1
int main(int argc, char **argv)
{
    char *exe = "ls -ltr /tmp";
    char *data;

    if((data = Execute(exe))!=NULL)
    {
       printf("%s", data);
       FREE(data);
    }

    return(0);
}
#endif



/**  ******************************************************************/
BOOL ProcessPage(char *reqID, char *page, char *cFile, char *passwd,
                 BOOL verbose)
{
   STRINGLIST *lines    = NULL,
      *theLine;
    char url[MAXSTRING];
    char exe[MAXSTRING];
    BOOL inData = FALSE;
    char *urlPart = NULL,
       *filename = NULL;
    urlPart  = (char *)malloc(2 * MAXSTRING * sizeof(char));
    filename = (char *)malloc(2 * MAXSTRING * sizeof(char));
    
    page = Substitute(page, "\r", "", TRUE);
    lines = SplitLine('\n', page);
    
    for(theLine=lines; theLine !=NULL; NEXT(theLine))
    {
        char *line;
        if((line = mystrdup(theLine->string))==NULL)
           return(FALSE);
                              
        if(inData)
        {
           GetURLandFilename(line, urlPart, filename);  /* $line =~ m/href=\"(.*?)\"\>(.*?)\</; */
           filename = Substitute(filename, " ", "_", TRUE);
           urlPart  = Substitute(urlPart, "./", "", FALSE);
           urlPart  = Substitute(urlPart, "(", "\\(", TRUE);
           urlPart  = Substitute(urlPart, ")", "\\)", TRUE);
           filename = Substitute(filename, "(", "\\(", TRUE);
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
        free(line);
    }
    free(urlPart);
    free(filename);
    return(TRUE);
}

#ifdef TEST_PROCESSPAGE
#   define TEST 1
int main(int argc, char **argv)
{
   char *data    = NULL,
        ch;
   int  nData    = 0,
        dataSize = 0;
    FILE *fp;

    char cFile[MAXSTRING],
         passwdFile[MAXSTRING],
         passwd[MAXSTRING],
         *cDir = "/home/amartin/work/inn/idmis";

    sprintf(cFile,  "%s/Martin-cert.p12",    cDir);
    sprintf(passwdFile,  "%s/Martin-cert.passwd", cDir);

    ReadPasswd(passwdFile, passwd);
    
    if((fp=fopen("t/test.html", "r"))!=NULL)
    {
      while(1)
      {
         ch=fgetc(fp);
         if(nData >= dataSize)
         {
            dataSize += CHUNK;
            if((data = realloc(data, dataSize*sizeof(char)))==NULL)
               return(1);
         }
         data[nData++] = ch;
         if(ch==EOF)
         {
            data[nData-1] = '\0';
            break;
         }
      }
      fclose(fp);
    }
    
    ProcessPage("12079", data, cFile, passwd, TRUE);

    return(0);
}
#endif


/*************************************************************************************/
#ifndef TEST
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
#endif /* TEST */

