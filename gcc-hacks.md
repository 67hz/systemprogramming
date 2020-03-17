TLPI - use default GNU C compiler options or the following for all examples:

    cc -std=c99 -D_XOPEN_SOURCE=600




* View value of any defined constant

```bash
echo __GNUC__ | gcc -E -
```

add include file

```bash
echo O_APPEND | gcc -include fcntl.h -E -
```

* assembly output

```bash
gcc -E file.c
```


    # run glibc shared library as executable to display version info
    $ /lib/libc.so.6 
    # find above path by inspecting lib dep
    $ ldd myprog | grep libc
