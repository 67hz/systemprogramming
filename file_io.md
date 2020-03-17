```c

fd = open(pathname, flags, mode);

numread = read(fd, buffer, count);

numwritten = write(fd, buffer, count);

status = close(fd);
```


The `open()` flags
------------------

* File access mode flags: retrieved with fcntl() `F_GETFL` op
* File creation flags: cannot be retrieved or changed
* Open file status flags: retrieved with fcntrl() `F_GETFL` `FSET_FL`
    * >= kernel 2.6.22 read `/proc/PID/fdinfo` to obtain fd info on any sys process

The `creat()` call
------------------

Early unix `open()` had 2 args only and could not be used to create a new file.
`creat()` used to create, open, and truncate to zero length.

* now obsolete, `open` can be used instead:
```c
fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, mode);
```

The `read()` call
-----------------

```c
ssize_t read(int fd, void *buffer, size_t count);
```
Returns # of bytes read, 0 on EOF, -1 on error

* count - max # bytes to read
* buffer must be >= count long

Syscalls do not allocate memory for buffers used to return info to caller.
We must pass a pointer to previously allocated mem buffer of correct size.

Reading from terminal reads only up to '\n'

* does not place a terminating null byte at end of string
    * can be used to read any sequence of bytes so \n is not implicit
        * add null terminator explicitly when required (increment buffer size for null)


```c
char buffer = [MAX_READ + 1];
numRead = read(STDIN_FILENO, buffer, MAX_READ);
...
buffer[numRead] = '\0';
printf("input data was: %s\n", buffer);
```

The `write()` call
------------------

Partial writes are possible if disk full or process limit on file sizes reached.


The `close()` call
------------------

* closes open fd freeing it for subsequent reuse by process.

* when process terminates, all open fd's are automatically closed
    * good practice to explicitly close unneeded fd's


Changing File Offset `lseek()` - `tell(fd)` in other Unix
------------------------------

* read-write offset (pointer)
* location in file for next `read()` or `write()`
* original byte position relative to start of file. first byte of file is at offset 0.
* read/writes progress sequentially through file - each call adjusts offset
* lseek used to adjust offset

```c
off_t lseek(int fd, off_t offset, int whence);
```
`off_t` is signed int specd by SUSv3
`whence` base point where offset is interpreted:
  * `SEEK_SET` bytes from start of file
  * `SEEK_CUR` adj by offset bytes rel to current offset
  * `SEEK_END` size of file plus offset. Interpreted with respect to next byte after last byte of file

Returns new file offset if successful, -1 on error

* cannot apply to all file types: (e.g., terminal not permitted and fails with errno set to ESPIPE but is possible to use on a disk or tape device)


File holes
----------

Seeking past end of file and writing - space between EOF and write pos is a file hole

* reading from a hole returns buffer of bytes containing 0 (null bytes)
* do not take up disk space

* sparsely populated files take up less space
  * disk block are not allocated in hole until data written to them

* core dump files contain large holes

* file's nominal size may be larger than amount of disk storage it uses
    * writing bytes to middle of file hole descreases amount of free disk space even though file size does not change
        * `posix_fallocate(fd, offset, len)` can be used to ensure space allocated for hole
            * from `fallocate` sys call
    * `stat` can tell current file size and # blocks allocated to file
        * `du -h filename`

File Descriptors
----------------

Open file descriptions: Open file table
  * entries are called file handles


```bash
$ ./myscript > results.log 2>&1
```
Send both stdin, stderr to results.log (stderr is fd 2, stdout 1)
2 is referring to same open file description as fd 1


pread and pwrite
----------------

* r/w and leaves offset unchanged - good for atomicity


readv and writev for vectorized ops
-----------------------------------

