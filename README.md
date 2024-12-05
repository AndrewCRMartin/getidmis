getidmis
========

(c) 2024, Prof. Andrew C.R. Martin
----------------------------------

`getidmis` is a program to grab a set of files from IDMIS associated
with one or more INN request numbers.

You will need an IDMIS P12 certificate file and password to use it!

`getidmis` must be run from the command line.

Usage
-----

```
getidmis [-c cert.p12] [-f passwordfile] [-p password] reqnum [reqnum ...]
         -c Specify the certificate file
         -f Specify a file containing the password
         -p Specify the password
```

`getidmis` will expect the certificate file to be called `cert.p12`
and present in the current directory; this can be overridden using
`-c` flag or the `IDMISCERTFILE` environment variable.

It will expect the password to be contained in a file called
`certpw.txt` present in the current directory; this can be overridden
using the `-f` flag or the `IDMISPWDFILE` environment variable.
Alternatively, it can be overridden using the `-p` flag to specify the
password itself on the command line or the `IDMISPASSWD` environment
variable. Values specified on the command line take precedence over
values in environment variables.

If both the password file and the actual password are specified, the
actual password takes precedence.

Installation
------------

You must have `curl` installed. This will be present on all modern
Linux, Windows and Mac systems.


