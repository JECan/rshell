#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>					// for perror()
#include <errno.h>					// for perror()
#include <sys/types.h>				// for wait()
#include <unistd.h>					// for fork() and execvp()
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

int main(int argc, char **argv)
{
	//obtaining username and hostname
	char user[50] = {0};
	char host[50] = {0};
	if(getlogin_r(user,sizeof(user)-1))
	{
		perror("User login error.");
	}
	if(gethostname(host,sizeof(host)-1))
	{
		perror("gethostname() error.");
	}

	for(int i = 0; i < 50; i++)
	{
		//loop so only user@hammer is displayed 
		//instead of user@hammer.cs.ucr.edu
		if(host[i] == '.') host[i] = '\0';
	}

	while(1)
	{
		//output user and host
		cout << user << "@" << host << "$ "; 

		char userinput[999];

		//gets user input, and if user input is "exit" program exits
		cin.getline(userinput,999);
		if(strcmp(userinput,"exit") == 0) exit(1);	
	}

	return 0;
} // end of main
