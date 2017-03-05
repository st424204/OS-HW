#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char** argv){
	char  *ip,*limit,*timeout;
	for(int i=0;i<argc;i++){
		if(strcmp(argv[i],"-ip")==0)ip=argv[i+1];
		else if(strcmp(argv[i],"-i")==0) limit=argv[i+1];
		else if(strcmp(argv[i],"-t")==0) timeout=argv[i+1];
	}
	char command[1024];
	sprintf(command,"ssh %s \'timeout %s cpulimit -l %s cpu100 &\'",ip,timeout,limit);
	system(command);
}
