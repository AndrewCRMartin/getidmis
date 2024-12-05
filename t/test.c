#ifdef TEST_SPLITLINE
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

#ifdef TEST_GETURLANDFILENAME
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

#ifdef TEST_STRINGCONTAINS
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

#ifdef TEST_READPASSWD
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

#ifdef TEST_EXECUTE
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

#ifdef TEST_PROCESSPAGE
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
    
    data = ProcessPage("12079", data, cFile, passwd, TRUE);
    free(data);

    return(0);
}
#endif

#ifdef TEST_SUBSTITUTE
int main(int argc, char **argv)
{
   char *string;

   string = (char *)malloc(200*sizeof(char));
   strcpy(string,"Once upon a time\nThere was a piece of code\n\
That needed to be tested");
   string = Substitute(string, "\n", "|", TRUE);
   printf("%s\n", string);
   FREE(string);

   string = (char *)malloc(200*sizeof(char));
   strcpy(string,"This & is & a & string & with & lots & of & ampersands");
   string = Substitute(string, "&", "\\&", TRUE);
   printf("%s\n", string);
   FREE(string);

   return(0);
   
}
#endif


