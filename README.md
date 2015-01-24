rshell
====
rshell is a basic command shell. It displays the currently logged in user as well as host name followed by a '$' command prompt. It is able to take in commands and execute them. There are however, several bugs and limitations in the program listed below.

Installation guide
====
```
$ git clone  http://github.com/yourusername/rshell.git
$ cd rshell
$ git checkout hw0
$ make
$ bin/rshell
```

Bugs/Limitations
====
Accepts invalid connectors such as
```
ls &&& ls -a
ls |||| false
```
echo of a spaced out phrase is compressed
```
   echo "hello               world "
```
outputs 
```
   echo hello world
```
The cd command does not work with rshell

rshell accepts instances of the separators when it should not 
```
   jcand003@hammer$ ||||||||||||||||||||||||||||||
```
does not throw and error, the same applies for the & and ;

A semicolon followed by a valid command executes 
`
   ;pwd
`
