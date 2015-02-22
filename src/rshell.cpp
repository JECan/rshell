#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

void parse(const string &usercommand, const char specialchar[]);
void pipes(const string &usercommand);
void inputredirect(const string &usercommand);
void outputappend(const string &usercommand);
void outputtrunc(const string &usercommand);
int main()
{	
	//obtaining username and hostname
	char host[64] = {0};
	string displayuser;
	string userinput;

	string username = getlogin();
	if(getlogin() == NULL)
	{
		perror("getlogin() error.");
	}

	if(gethostname(host,sizeof(host)-1) == -1)
	{
		perror("gethostname() error.");
	}

	for(int i = 0; i < 64; i++)
	{
		if(host[i] == '.') host[i] = '\0';
	}
	
	displayuser = username + "@" + host + "$ ";
	char SEMICOLON[] = ";";
	char ANDING[] = "&&";
	char ORING[] = "||";
	while(1)
	{
		cout << displayuser;
		getline(cin, userinput);

		//checks for comment '#'
		string checkcomment = userinput;
		for(unsigned int i=0; i <= checkcomment.size()-1; i++)
		{
			if(checkcomment[i] == '#')
			{
				checkcomment[i] = '\0';
			}
		}
		userinput = checkcomment;

		//check for special characters ; || &&
		if(userinput.find("&&") != string::npos)
		{
			parse(userinput,ANDING);
		}
		else if(userinput.find("||")!= string::npos)
		{
			parse(userinput,ORING);
		}
		//cheeck for piping and io redirection
		else if(userinput.find("|") != string::npos)
		{
			pipes(userinput);
		}
/*		else if(userinput.find("<") != string::npos)
		{
			inputredirect(userinput);
		}
*/
		else if(userinput.find(">") != string::npos)
		{
			outputtrunc(userinput);
		}
		else if(userinput.find(">>") != string::npos)
		{
			outputappend(userinput);
		}
		else
		{
			parse(userinput,SEMICOLON);
		}

		//exit check
		typedef tokenizer<char_separator<char> > EXITTOKEN;
		char_separator<char> EXITSEPARATOR (" ");
		EXITTOKEN etok(userinput, EXITSEPARATOR);
		for(EXITTOKEN::iterator exit_iter = etok.begin();
			exit_iter != etok.end(); exit_iter++)
		{
			if(*exit_iter == "exit") exit(EXIT_SUCCESS);
		}

	}//while(1)
	return 0;
} // end of main
//-------------------------------------------------------------
void parse(const string &usercommand, const char specialchar[])
{
	typedef tokenizer<char_separator<char> > MYTOKENS;
	char_separator<char> MYSEPARATOR(specialchar);
	MYTOKENS tok(usercommand, MYSEPARATOR);
	string dothistoken;
	for(MYTOKENS::iterator tok_iter = tok.begin();
		tok_iter != tok.end(); tok_iter++)
	{
		int pid = fork();
		//if pid == -1 throw error
		if(pid == -1)
		{
			perror("error with fork()");
			exit(1);
		}
		//else if pid == 0 were in child
		else if(pid == 0)
		{
			dothistoken = *tok_iter;
			char *argv[512];
			int i = 0;
			typedef tokenizer<char_separator<char> > COMTOKEN;
			char_separator<char> COMSEPARATOR (" ");
			COMTOKEN etok(dothistoken, COMSEPARATOR);

			for(COMTOKEN::iterator com_iter = etok.begin();
				com_iter != etok.end(); i++, com_iter++)
			{
				argv[i] = new char[(*com_iter).size()];
				strcpy(argv[i],(*com_iter).c_str());
			}
			
			argv[i] = 0;
			if(execvp(argv[0],argv) == -1)
			{
				perror("error execvp()");
				exit(1);
			}
			//must delete argv
			int j = 0;
			for(COMTOKEN::iterator del_iter = etok.begin();
				del_iter != etok.end(); j++, del_iter++)
				{
					delete [] argv[j];
				}

		}
		//else we are in parent
		else
		{
			int parentstatus = 0;
			if(wait(&parentstatus) == -1)
			{
				perror("error with wait()");
				exit(1);
			}
		}
		
	}
}//void parse()
//-------------------------------------------------------------
void inputredirect(const string &usercommand)
{
		
}//void inputredirect()
//-------------------------------------------------------------
void outputappend(const string &usercommand)
{
	string carrot = ">>";
	int flags = O_CREAT | O_WRONLY | O_APPEND;
	int carrotloc = usercommand.find(carrot);
	string leftofout = usercommand.substr(0, carrotloc);
	string rightofout = usercommand.substr(usercommand.find(carrot) + 1);
	
	if(rightofout == "")
	{
		cout << "There is no file entered\n";
		exit(1);
	}

	//tokenize right side for spaces
	typedef tokenizer<char_separator<char> > MYTOKENS;
	char_separator<char> MYSEPARATOR(" ");
	MYTOKENS tok(rightofout, MYSEPARATOR);
	MYTOKENS::iterator tok_iter = tok.begin();

	//open the file
	int writeto = open((*tok_iter).c_str(), flags, 0666);
	if(writeto == -1)
	{
		perror("open() error");
		return;
	}
	
	//saving std out 
	int savestdout = dup(1);
	if(savestdout == -1)
	{
		perror("dup() error, output redirection");
		exit(1);
	}

	//setting writeto to the new stdout
	//must restore stdout later
	if(dup2(writeto, 1) == -1)
	{
		perror("dup2() error, output redirection");
		exit(1);
	}

	//closing writeto
	if (close(writeto) == -1)
	{
		perror("close() error, output redirection");
		exit(1);
	}

	//parsing left side for spaces, and calliing execvp on leftside
	char *argv[2];
	int i = 0;
	typedef tokenizer<char_separator<char> > COMTOKEN;
	char_separator<char> COMSEPARATOR (" ");
	COMTOKEN etok(leftofout, COMSEPARATOR);

	for(COMTOKEN::iterator com_iter = etok.begin();
		com_iter != etok.end(); i++, com_iter++)
	{
		argv[i] = new char[(*com_iter).size()];
		strcpy(argv[i],(*com_iter).c_str());
	}
	//only want first command, if argv i != NULL then error is thrown	
	argv[i] = NULL;
	if(execvp(argv[0],argv) == -1)
	{
		perror("error execvp()");
		exit(1);
	}

	//restoring stdout
	if(dup2(savestdout, 1) == -1)
	{
		perror("dup2 error restoring");
//		exit(1);
	}

	//closing savestdout
	if(close(savestdout) == -1)
	{
		perror("colse() error");
//		exit(1);
	}

	//must delete argv
	int j = 0;
	for(COMTOKEN::iterator del_iter = etok.begin();
		del_iter != etok.end(); j++, del_iter++)
		{
			delete [] argv[j];
		}


}//void outputappend()
//-------------------------------------------------------------
void outputtrunc(const string &usercommand)
{
	string carrot = ">";
	int flags = O_CREAT | O_WRONLY | O_TRUNC;
	int carrotloc = usercommand.find(carrot);
	string leftofout = usercommand.substr(0, carrotloc);
	string rightofout = usercommand.substr(usercommand.find(carrot) + 1);
	
	if(rightofout == "")
	{
		cout << "There is no file entered\n";
		exit(1);
	}

	//tokenize right side for spaces
	typedef tokenizer<char_separator<char> > MYTOKENS;
	char_separator<char> MYSEPARATOR(" ");
	MYTOKENS tok(rightofout, MYSEPARATOR);
	MYTOKENS::iterator tok_iter = tok.begin();

	//open the file
	int writeto = open((*tok_iter).c_str(), flags, 0666);
	if(writeto == -1)
	{
		perror("open() error");
		return;
	}
	
	//saving std out 
	int savestdout = dup(1);
	if(savestdout == -1)
	{
		perror("dup() error, output redirection");
		exit(1);
	}

	//setting writeto to the new stdout
	//must restore stdout later
	if(dup2(writeto, 1) == -1)
	{
		perror("dup2() error, output redirection");
		exit(1);
	}

	//closing writeto
	if (close(writeto) == -1)
	{
		perror("close() error, output redirection");
		exit(1);
	}

	//parsing left side for spaces, and calliing execvp on leftside
	char *argv[1];
	int i = 0;
	typedef tokenizer<char_separator<char> > COMTOKEN;
	char_separator<char> COMSEPARATOR (" ");
	COMTOKEN etok(leftofout, COMSEPARATOR);

	for(COMTOKEN::iterator com_iter = etok.begin();
		com_iter != etok.end(); i++, com_iter++)
	{
		argv[i] = new char[(*com_iter).size()];
		strcpy(argv[i],(*com_iter).c_str());
	}
	//only want first command, if argv i != NULL then error is thrown	
	argv[i] = NULL;
	if(execvp(argv[0],argv) == -1)
	{
		perror("error execvp()");
		exit(1);
	}

	//restoring stdout
	if(dup2(savestdout, 1) == -1)
	{
		perror("dup2 error restoring");
//		exit(1);
	}

	//closing savestdout
	if(close(savestdout) == -1)
	{
		perror("colse() error");
//		exit(1);
	}

	//must delete argv
	int j = 0;
	for(COMTOKEN::iterator del_iter = etok.begin();
		del_iter != etok.end(); j++, del_iter++)
		{
			delete [] argv[j];
		}

}//void outputtrunc()
//-------------------------------------------------------------
//-------------------------------------------------------------
void pipes(const string &usercommand)
{
	string leftofpipe = usercommand.substr(0,usercommand.find("|"));
	string rightofpipe = usercommand.substr(usercommand.find("|")+1);
//	char *pipeinput[512];
	
/*	typedef tokenizer <char_separator<char> > MYTOKENS;
	char_separator<char> PIPESEP("|");
	MYTOKENS tok(leftofpipe, PIPESEP);

	vector <string> tokenvector;

	for(MYTOKENS::iterator tok_iter = tok.begin();
		tok_iter != tok.end(); tok_iter++)
	{
		tokenvector.push_back(*tok_iter);
	}
	for(unsigned i = 0; i < tokenvector.size(); i++)
	{
*/
		int fd[2];
		if(pipe(fd) == -1)
		{
			perror("pipe() error");
			exit(1);
		}
		int pid = fork();

		if (pid == -1)
		{
			perror("piping fork() error");
			exit(1);
		}
//-----------------------------------------------------	
		else if(pid == 0)
		{
//			string dothistoken = tokenvector.at(i);
			char *argv[512];
			int i = 0;
			typedef tokenizer<char_separator<char> > COMTOKEN;
			char_separator<char> COMSEPARATOR (" ");
			COMTOKEN etok(leftofpipe, COMSEPARATOR);

			for(COMTOKEN::iterator com_iter = etok.begin();
				com_iter != etok.end(); i++, com_iter++)
			{
				argv[i] = new char[(*com_iter).size()];
				strcpy(argv[i],(*com_iter).c_str());
			}
	
			//in child, write to pipe
			if(close(fd[0]) == -1)
			{
				perror("close() error");
				exit(1);
			}
			if(dup2(fd[1], 1) == -1)
			{
				perror("dup2() error");
				exit(1);
			}
			if(execvp(argv[0], argv) == -1)
			{
				perror("piping execvp() error");
				exit(1);
			}
			//must delete argv
			int j = 0;
			for(COMTOKEN::iterator del_iter = etok.begin();
				del_iter != etok.end(); j++, del_iter++)
				{
					delete [] argv[j];
				}
		}
//-----------------------------------------------------	
		//now read in from pipe
		int holdstdin;
		int parentstatus;
		//changing stdin
		if((holdstdin = dup(0)) == -1)
		{
			perror("piping dup() error");
			exit(1);
		}
		if(dup2(fd[0],0) == -1)
		{
			perror("piping dup2() error");
			exit(1);
		}
		if(close(fd[1]) == -1)
		{
			perror("piping close() error");
			exit(1);
		}
		if(wait(&parentstatus) == -1)
		{
			perror("piping wait() error");
			exit(1);
		}
		//must restore stdin
		dup2 (holdstdin,0);
//	}
}//void pipes()
