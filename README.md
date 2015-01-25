rshell
====
rshell is a basic command shell. It displays the currently logged in user as well as host name followed by a '$' command prompt. It is able to take in commands and execute them. There are however, several bugs and limitations in the program listed below.

Installation guide
====
```
   $ git clone  http://github.com/SpaceCowboy100/rshell.git
   $ cd rshell
   $ git checkout hw0
   $ make
   $ bin/rshell
```

Bugs/Limitations
====
1. Accepts invalid connectors such as
```
ls &&& ls -a
ls |||| false
```
2. echo of a spaced out phrase is compressed `` echo "hello               world "``` outputs as  ```echo hello world```

3. The ```cd``` command does not work with rshell

4. rshell accepts instances of the separators when it should not 
```
   jcand003@hammer$ ||||||
```
does not throw and error, the same applies for the & and ;

5. A semicolon followed by a valid command executes ` ;pwd`

6. The or connector does function not  correctly if the first arguement and second arguement are true

7. Valid arguements but connected by a ;; passes such as```ls;; ls```

8. If an invalid argument such exceeds 100 characters, and error is not thrown
```
   ddddddddddddddddd.....ddddddddddddd
```
9. ```exit``` command does exit program but throws error to execvp()
