rshell
====
rshell is a basic command shell. It displays the currently logged in user as well as host name followed by a `$` command prompt. It is able to take in commands and execute them. There are however, several bugs and limitations in the program listed below.

Installation guide
====
```
   $ git clone  http://github.com/SpaceCowboy100/rshell.git
   $ cd rshell
   $ git checkout hw3
   $ make
   $ bin/rshell
```

Bugs/Limitations
====
* Accepts invalid connectors such as
```
   ls &&& ls -a
   ls |||| false
```
* `echo` of a spaced out phrase in quotes is compressed.
```
   echo "hello               world "
```
outputs  ```hello world``` 


* rshell accepts instances of the separators when it should not, the same applies for the `;` and `&` characters.
```
   jcand003@hammer$ ||||||
```

* A semicolon followed by a valid command executes `ie. jcand003@hammer$ ;pwd`

* Valid arguements but connected by a ;; passes such as```ls;; ls```.

* If an invalid argument such exceeds 100 characters, and error is not thrown.

* error is thrown if host name exceeds 50 characters

* piping only functions with one pipe, it cannot be chained.

* piping throws several perrors, with correct output in only some cases.

* `make` must be called after every IO redirection command ```<``` ```>``` ```>>``` ```|```.

* `cd` changes to home directory only, it cannot take in arguements.

ls
===
rshell also includes an implementation of the ```ls``` command to download this implementation of ls do the following.

```
	$ git clone  http://github.com/SpaceCowboy100/rshell.git
	$ cd rshell
	$ git checkout hw1
	$ make
	$ bin/ls
```
ls - Bugs/Limitations
===
* ```bin/ls -``` does not throw an error

* There is no support for the ```-R``` flag

* ```bin/ls -l``` of a directory path will output the entire path, instead of just the file or directory.
