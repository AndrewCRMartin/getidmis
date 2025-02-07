/************************************************************************/
/**

   Program:    getidmis
   \file       getidmis.c
   
   \version    V1.0
   \date       05.12.24   
   \brief      Grab a set of files from IDMIS
   
   \copyright  Prof. Andrew C. R. Martin 2024
   \author     Prof. Andrew C. R. Martin
   \par
               abYinformatics, Ltd.
   \par
               andrew@bioinf.org.uk
               andrew@abyinformatics.com
               
**************************************************************************

   This program is not in the public domain, but it may be copied
   according to the conditions laid out in the accompanying file
   COPYING.DOC

   The code may be modified as required, but any modifications must be
   documented so that the person responsible can be identified.

   The code may not be sold commercially or included as part of a 
   commercial product except as described in the file COPYING.DOC.

**************************************************************************

   Description:
   ============

**************************************************************************

   Usage:
   ======

**************************************************************************

   Revision History:
   =================

*************************************************************************/
/* Includes
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bioplib/general.h"
#include "bioplib/macros.h"


/************************************************************************/
/* Defines and macros
*/
#if !defined(__APPLE__)    && !defined(MS_WINDOWS) && \
    !defined(__USE_POSIX2) && !defined(GOT_PIPES)
extern FILE *popen(const char *, const char *);
#endif

#if !defined(__APPLE__) && !defined(__USE_POSIX2) && \
    !defined(GOT_PIPES)
extern int pclose(FILE *);
#endif

#define MAXSTRING 1024
#define HUGEBUFF  1024
#define CHUNK     128

/************************************************************************/
/* Globals
*/
char *gBaseURL = "https://extranets.who.int/inn";

/************************************************************************/
/* Prototypes
*/
STRINGLIST *SplitLine(char splitChar, char *page);
BOOL GetURLandFilename(char *line, char *url, char *filename);
BOOL StringContains(char *string, char *contains);
BOOL ReadPasswd(char *passwdFile, char *passwd);
char *Execute(char *exe);
char *ProcessPage(char *reqID, char *page, char *cFile, char *passwd,
                  BOOL verbose, BOOL *ok);
char *Substitute(char *string, char *old, char *new, BOOL global);
static char *doSubstitute(char *string, char *old, char *new,
                          BOOL global, ULONG *offsetNum);
void Usage(void);
BOOL ParseCmdLine(int *pArgc, char ***pArgv, char *pwfile, char *passwd, 
                  char *certFile, BOOL *verbose);
void SetFromEnvVars(char *passwdFile, char *passwd, char *certFile);
BOOL ShowErrors(char *page, BOOL ok, BOOL verbose, char *entry);
BOOL EmptyPage(char *page);


/************************************************************************/
/* Tests
   -----
   Select one of these if you wish to run a test, by changing
   #undef to #define
*/
#undef TEST_SPLITLINE
#undef TEST_GETURLANDFILENAME
#undef TEST_STRINGCONTAINS
#undef TEST_READPASSWD
#undef TEST_EXECUTE
#undef TEST_PROCESSPAGE
#undef TEST_SUBSTITUTE

#if defined(TEST_SPLITLINE)      || defined(TEST_SUBSTITUTE)  || \
    defined(TEST_STRINGCONTAINS) || defined(TEST_READPASSWD)  || \
    defined(TEST_EXECUTE)        || defined(TEST_PROCESSPAGE) || \
    defined(TEST_GETURLANDFILENAME)
#   define TEST
#   include "t/test.c"
#endif

/************************************************************************/
#ifndef TEST
int main(int argc, char **argv)
{
   char tplURL[MAXSTRING],
        certFile[MAXSTRING],
        passwdFile[MAXSTRING],
        passwd[MAXSTRING],
        url[MAXSTRING],
        exe[MAXSTRING];
   BOOL verbose = FALSE;

   /* Blank the password                                               */
   passwd[0] = '\0';

#if defined(MS_WINDOWS) || defined(_WIN32)
/*
   sprintf(tplURL,
           "'%s/Report.php?Request=%%s%%%%26Report=INN_Phase_6b_Admin_Report'",
           gBaseURL);

   curl --cert-type p12 --cert xxxx.p12:passwd --data-urlencode --get url
*/
   sprintf(tplURL,
           "--data-urlencode Request=%%s --get %s/Report.php?Report=INN_Phase_6b_Admin_Report",
           gBaseURL);
#else
   sprintf(tplURL,
           "'%s/Report.php?Request=%%s&Report=INN_Phase_6b_Admin_Report'",
           gBaseURL);
#endif   

   strcpy(certFile,   "cert.p12");
   strcpy(passwdFile, "certpw.txt");
   
   SetFromEnvVars(passwdFile, passwd, certFile);
   
   if(ParseCmdLine(&argc, &argv, passwdFile, passwd, certFile, &verbose))
   {
      if(passwd[0] == '\0')
      {
         if(!ReadPasswd(passwdFile, passwd))
         {
            printf("\nError reading password file: %s\n\n", passwdFile);
            return(1);
         }
      }
      
#ifdef DEBUG
      printf("passwdFile: %s\n", passwdFile);
      printf("passwd:     %s\n", passwd);
      printf("certFile:   %s\n", certFile);
      printf("Args left:  %d\n", argc);
#endif
      
      while(argc)
      {
         char *page;
         BOOL ok    = FALSE,
              fatal = FALSE;
         
         if(verbose)
            printf("Processing: %s\n", *argv);
         
         sprintf(url, tplURL, *argv);
         sprintf(exe,
                 "curl -s --cert-type p12 --cert %s:%s %s",
                 certFile, passwd, url);
         if(verbose)
            printf("Running: %s\n", exe);
         page = Execute(exe);
         if(EmptyPage(page))
         {
            char *errorPage;
            sprintf(exe,
                    "curl -S --cert-type p12 --cert %s:%s %s",
                    certFile, passwd, url);
            errorPage = Execute(exe);
            fprintf(stderr, "%s", errorPage);
            FREE(errorPage);
         }
         else
         {
            page = ProcessPage(*argv, page, certFile, passwd, verbose, &ok);
            if(ShowErrors(page, ok, verbose, *argv))
               fatal = TRUE;
         }
         
         FREE(page);

         if(fatal)
            return(1);
         argc--; argv++;
      }
   }
   else
   {
      Usage();
      return(1);
   }
   return(0);
}
#endif /* TEST                                                          */


BOOL EmptyPage(char *page)
{
   if(strlen(page)<1)
      return(TRUE);
   
   return(FALSE);
}

/************************************************************************/
BOOL ShowErrors(char *page, BOOL ok, BOOL verbose, char *entry)
{
   if(!ok)
   {
      if((page == NULL) || (page[0] == '\0'))
      {
         printf("\nThe connection failed (certificate file or \
password invalid?)\n\n");
         return(TRUE);
      }
      
      printf("Failed to download any data for %s\n", entry);
      if(verbose)
         printf("\n\n%s", page);
      else
         printf("   Run with -v for more information\n");
   }
   return(FALSE);
}

/************************************************************************/
void SetFromEnvVars(char *passwdFile, char *passwd, char *certFile)
{
   char *val = NULL;
   
   if((val = getenv("IDMISCERTFILE"))!=NULL)
      strcpy(certFile, val);
   if((val = getenv("IDMISPWDFILE"))!=NULL)
      strcpy(passwdFile, val);
   if((val = getenv("IDMISPASSWD"))!=NULL)
      strcpy(passwd, val);
}

/************************************************************************/
/*>BOOL ParseCmdLine(int *pArgc, char ***pArgv, char *pwFile,
                     char *passwd, char *certFile)
  ----------------------------------------------------------
*//**
   \param[in]      argc         Argument count
   \param[in]      **argv       Argument array
   \param[out]     *infile      Input file (or blank string)
   \param[out]     *outfile     Output file (or blank string)
   \param[out]     *showDNA     Display the original DNA sequence
   \param[out]     *showRF      Display the reading frame DNA
   \param[out]     *showAll     Show all translations
   \return                      Success?

   Parse the command line
   
-  03.11.17 Original    By: ACRM
-  02.12.22 Added -a / showAll
*/
BOOL ParseCmdLine(int *pArgc, char ***pArgv, char *pwFile, char *passwd, 
                  char *certFile, BOOL *verbose)
{
   (*pArgc)--;
   (*pArgv)++;
   
   while(*pArgc)
   {
      if((*pArgv)[0][0] == '-')
      {
         switch((*pArgv)[0][1])
         {
         case 'p':
            (*pArgc)--;
            (*pArgv)++;
            strcpy(passwd, (*pArgv)[0]);
            break;
         case 'f':
            (*pArgc)--;
            (*pArgv)++;
            strcpy(pwFile, (*pArgv)[0]);
            break;
         case 'c':
            (*pArgc)--;
            (*pArgv)++;
            strcpy(certFile, (*pArgv)[0]);
            break;
         case 'v':
            *verbose = TRUE;
            break;
         case 'h':
            Usage();
            return(TRUE);
         default:
            return(FALSE);
            break;
         }
         
         (*pArgc)--;
         (*pArgv)++;
      }
      else
      {
         break;
      }
   }
   
   /* Check that there are arguments left                               */
   if(*pArgc < 1)
      return(FALSE);
   
   return(TRUE);
}


/************************************************************************/
void Usage(void)
{
   printf("\ngetidmis [-c cert.p12] [-f passwordfile] [-p password] \
reqnum [reqnum ...]\n");
   printf("         -c Specify the certificate file\n");
   printf("         -f Specify a file containing the password\n");
   printf("         -p Specify the password\n");
   
   printf("\n`getidmis` will expect the certificate file to be called \
`cert.p12`\n");
   printf("and present in the current directory; this can be overridden \
using\n");
   printf("`-c` flag or the `IDMISCERTFILE` environment variable.\n");
   
   printf("\nIt will expect the password to be contained in a file \
called\n");
   printf("`certpw.txt` present in the current directory; this can be \
overridden\n");
   printf("using the `-f` flag or the `IDMISPWDFILE` environment \
variable.\n");
   printf("Alternatively, it can be overridden using the `-p` flag to \
specify the\n");
   printf("password itself on the command line or the `IDMISPASSWD` \
environment\n");
   printf("variable. Values specified on the command line take \
precedence over\n");
   printf("values in environment variables.\n");
   
   printf("\nIf both the password file and the actual password are \
specified, the\n");
   printf("actual password takes precedence.\n");
   
   printf("i.e.:\n");
   printf("   a provided password takes precedence over the password \
file\n");
   printf("--and--\n");
   printf("   values on the command line take precedence over \
environment\n");
   printf("   variables, which take precedence over defaults.\n\n");
}

/************************************************************************/
char *mystrdup(char *in)
{
   char *out;
   ULONG inlen;
   inlen = strlen(in) + 8;
#ifdef DEBUG
   printf("Input length: %ld\n", inlen);
#endif
   out = (char *)malloc(inlen*sizeof(char));
   strcpy(out, in);
   return(out);
}


/************************************************************************/
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
   
#ifdef DEBUG
   printf(">>> lens: %d %ld\n", strlen(page), pageSize);
   printf(">>> '%c'\n", pageCopy[pageSize]);
#endif
   
   for(posInPage=0; posInPage<pageSize; posInPage++)
   {
#ifdef DEBUG
      printf("%c", pageCopy[posInPage]);
      printf("%ld,", posInPage);
      fflush(stdout);
#endif
      if(pageCopy[posInPage] == splitChar)
      {
         pageCopy[posInPage] = '\0';
         if((strings = blStoreString(strings, pageCopy+lastPosInPage))
            ==NULL)
         {
            free(pageCopy);
            return(NULL);
         }
         
         posInPage++;
         lastPosInPage = posInPage;
      }
   }
   
   /* and the last one                                                  */
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


/************************************************************************/
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
   
   /* Skip over the href and eat until we have the =                    */
   chp += 4;
   while(*chp != '=')
   {
      if(*chp == '\0')
         return(FALSE);
      chp++;
   }
   
   /* Skip over the = and eat until we have the " or '                  */
   chp++;
   while((*chp != '"') && (*chp != '\''))
   {
      if(*chp == '\0')
         return(FALSE);
      chp++;
   }
   
   /* Skip the ' or " and copy into URL until we reach a ' or "         */
   chp++; 
   while((*chp != '"') && (*chp != '\''))
   {
      if(*chp == '\0')
         return(FALSE);
      url[nURL++] = *chp;
      chp++;
   }
   url[nURL] = '\0';
   
   /* Skip the ' or " and eat until we have the >                       */
   chp++; 
   while(*chp != '>')
   {
      if(*chp == '\0')
         return(FALSE);
      chp++;
   }
   
   /* Skip the > and copy into filename until we reach a <              */
   chp++; 
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


/************************************************************************/
BOOL StringContains(char *string, char *contains)
{
   if(strstr(string, contains)!=NULL)
      return(TRUE);
   return(FALSE);
}


/************************************************************************/
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


/************************************************************************/
char *Execute(char *exe)
{
   char *data = NULL;
   
#ifdef TEST_PROCESSPAGE
   printf("%s\n", exe);
#else
   FILE *fp;
   char ch;
   int  nData    = 0,
        dataSize = 0;
   
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


/************************************************************************/
char *ProcessPage(char *reqID, char *page, char *certFile, char *passwd,
                  BOOL verbose, BOOL *ok)
{
   STRINGLIST *lines = NULL,
              *theLine;
   char url[MAXSTRING];
   char exe[MAXSTRING];
   BOOL inData = FALSE;
   
   *ok = FALSE;
   
   page = Substitute(page, "\r", "", TRUE);
   lines = SplitLine('\n', page);
   
   for(theLine=lines; theLine!=NULL; NEXT(theLine))
   {
      char *line;
      if((line = mystrdup(theLine->string))==NULL)
      {
         FREE(page);
         return(NULL);
      }
      
#ifdef DEBUG
      fprintf(stderr,"%s\n", line);
#endif
      
      if(inData)
      {
         char *urlPart  = NULL,
              *filename = NULL;
         
         urlPart  = (char *)malloc(MAXSTRING * sizeof(char));
         filename = (char *)malloc(MAXSTRING * sizeof(char));
         
         /* $line =~ m/href=\"(.*?)\"\>(.*?)\</;                      */
         GetURLandFilename(line, urlPart, filename);  
         filename = Substitute(filename, " ", "_", TRUE);
         urlPart  = Substitute(urlPart, "./", "", FALSE);
         urlPart  = Substitute(urlPart, "(", "\\(", TRUE);
         urlPart  = Substitute(urlPart, ")", "\\)", TRUE);
         filename = Substitute(filename, "(", "\\(", TRUE);
         filename = Substitute(filename, ")", "\\)", TRUE);
         
         *ok = TRUE;
         
         sprintf(url, "%s/%s", gBaseURL, urlPart);
         if(verbose)
            printf("   Getting file: %s [%s]\n", filename, url);
         
         sprintf(exe,
                 "curl -s --cert-type p12 --cert %s:%s --output %s %s",
                 certFile, passwd, filename, url);
         FREE(urlPart);
         FREE(filename);
         
         Execute(exe);
         
         inData = FALSE;
      }
      else if(StringContains(line, "Annex"))
      {
         inData = TRUE;            
      }
      else if(StringContains(line, ">Word Document"))
      {
         char *urlPart  = NULL,
              *filename = NULL;
         
         urlPart  = (char *)malloc(2 * MAXSTRING * sizeof(char));
         filename = (char *)malloc(2 * MAXSTRING * sizeof(char));
         
         /* $line =~ m/href=\"(.*?)\"\>(.*?)\</;                        */
         GetURLandFilename(line, urlPart, filename);  
         sprintf(filename, "%s_1_6b_Admin_Report.doc", reqID);
         urlPart  = Substitute(urlPart, "./", "", FALSE);
         urlPart  = Substitute(urlPart, "&", "\\&", TRUE);
         
         if(verbose)
            printf("   Getting file: %s\n", filename);
         
         *ok = TRUE;
         
         sprintf(url, "%s/%s", gBaseURL, urlPart);
         sprintf(exe,
                 "curl -s --cert-type p12 --cert %s:%s --output %s %s",
                 certFile, passwd, filename, url);
         FREE(urlPart);
         FREE(filename);
         
         Execute(exe);
      }
      free(line);
   }
   return(page);
}


/************************************************************************/
char *Substitute(char *string, char *old, char *new, BOOL global)
{
   ULONG offset = 0;
   char *newString = NULL;
   
   newString = doSubstitute(string, old, new, global, &offset);
   return(newString);
}


/************************************************************************/
static char *doSubstitute(char *string, char *old, char *new, BOOL global,
                          ULONG *offsetNum)
{   
   char *newString = NULL,
        *offset    = NULL,
        *chp,
        *chpNew;
   ULONG newLen,
         nChar;
   
   /* Old string was not found                                          */
   if((offset = strstr(string+(*offsetNum), old))==NULL)
      return(string);
   
   *offsetNum = (offset - string) + strlen(new);
   
   newLen = strlen(string) - strlen(old) + strlen(new) + 1;
#ifdef DEBUG
   printf("oldLen: %ld  newLen: %ld  ", strlen(string), newLen);
#endif
   if((newString = (char *)malloc(newLen * sizeof(char)))==NULL)
      return(NULL);
   
   nChar = 0;
   
   /* First part                                                        */
   for(chp=string; chp!=offset; chp++)
      newString[nChar++] = *chp;
   /* New part                                                          */
   for(chpNew=new; *chpNew!='\0'; chpNew++)
      newString[nChar++] = *chpNew;
   /* Last part                                                         */
   for(chp=chp+strlen(old); *chp!='\0'; chp++)
      newString[nChar++] = *chp;
#ifdef DEBUG
   printf("nChar %ld\n", nChar);
   fflush(stdout);
#endif
   /* Terminate and free the input string                               */
   newString[nChar] = '\0';
   free(string);
   
   /* If it's global, recurse                                           */
   if(global)
      newString = doSubstitute(newString, old, new, global, offsetNum);
   
   return(newString);
}
