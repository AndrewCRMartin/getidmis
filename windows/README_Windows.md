Under Linux, you can compile by installing
`mingw64-gcc.x86_64`

```
sudo dnf -y install mingw64-gcc.x86_64
```

(and maybe `ucrt64-gcc.x86_64`)

```
sudo dnf -y install ucrt6464-gcc.x86_64
```

And then use:
```
mingw64-make -f Makefile.windows64
```



