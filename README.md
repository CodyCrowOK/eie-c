# eie

A revision control system written in C.

This is, currently, more or less a clone of the eie Perl script I wrote 3 years ago. This version is faster, better, and generally done the way it should have been done originally. It can be used in precisely the same way, however.

*TL;DR*: 

```
make
make install
```

## Commands

`eie init` to create a new repository in a working directory.

`eie add <file1> [file2]...` queues any number of files to be committed.

`eie clear` clears the list of files to be committed.

`eie commit` commits the loaded changes.

`eie list <file>` lists all of the commits containing _file_.

`eie revert <file> <commit>` reverts _file_ to its contents at _commit_.

`eie destroy` removes the eie information folder and destroys the repository. The files in the working directory are left untouched.
