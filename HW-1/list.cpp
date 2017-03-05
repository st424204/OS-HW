
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libvirt/libvirt.h>
#include <string>
#include <vector>

using namespace std;

virConnectPtr conn;
virDomainPtr *domains;
char states[9][20]={"NOSTATE","RUNNING","BLOCKED","PAUSED","SHUTDOWN","SHUTOFF","CRASHED","PMSUSPENDED","LAST"};

vector< vector<string> > data;
FILE *fp;
char address[1024];


void list(int res);

int main(int argc, char **argv) {



	conn = virConnectOpen("qemu:///system");
	int ret = virConnectListAllDomains(conn, &domains, 1<<14-1);
	data.resize(ret);

	list(ret);

	for (int i = ret-1 ; i >=0 ; i--) virDomainFree(domains[i]);
	virConnectClose(conn);
}
void list(int ret){
	vector<virDomainInfo>   di,de;
	vector<long> loading;
	di.resize(ret);
	de.resize(ret);
	loading.resize(ret);


	for (int i = ret-1 ; i >=0 ; i--){
		data[i].resize(4);
		data[i][1]=virDomainGetName(domains[i]);

		string cmd = "./find_addr " + data[i][1];
		fp = popen(cmd.c_str(), "r");
		fscanf(fp,"%s",address);
		fclose(fp);

		data[i][2] = address;

	}
	for (int i = ret-1 ; i >=0 ; i--) virDomainGetInfo(domains[i], &di[i]);
	sleep(1);
	for (int i = ret-1 ; i >=0 ; i--){
		virDomainGetInfo(domains[i], &de[i]);
		data[i][3] = states[de[i].state];
		loading[i]=(de[i].cpuTime-di[i].cpuTime)/10000000/di[i].nrVirtCpu;
	}

	printf("<table><tr><th>ID</th><th>NAME</th><th>IP Address</th><th>State</th><th>Cpu Loading(%%)</th></tr>");
	for (int i = ret-1 ; i >=0 ; i--)
		printf("<tr><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%ld%%</td></tr>",virDomainGetID(domains[i]), \
			data[i][1].c_str(),data[i][2].c_str(),data[i][3].c_str(),loading[i]);


	printf("</table>");
}

