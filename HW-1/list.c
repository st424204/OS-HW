
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libvirt/libvirt.h>

virConnectPtr conn;
char states[9][20]={"NOSTATE","RUNNING","BLOCKED","PAUSED","SHUTDOWN","SHUTOFF","CRASHED","PMSUSPENDED","LAST"};
void cpuload(virDomainPtr d);

int main(int argc, char **argv) {

	conn = virConnectOpen("qemu:///system");
	
	int i,ret;
	virDomainPtr *domains;
	unsigned int flags = VIR_CONNECT_LIST_DOMAINS_RUNNING |
                     VIR_CONNECT_LIST_DOMAINS_PERSISTENT;

	ret = virConnectListAllDomains(conn, &domains, flags);
	
	printf("<table><tr><th>ID</th><th>NAME</th><th>IP Address</th><th>State</th><th>Cpu Loading(%)</th></tr>");
	for (i = ret-1 ; i >=0 ; i--) cpuload(domains[i]);
	printf("</table>");
	virConnectClose(conn);
}
void cpuload(virDomainPtr d){
    virDomainInfo   di,de;

    virDomainGetInfo(d, &di);
    sleep(1);
    virDomainGetInfo(d, &de);
    FILE *fp;
    char address[1024];
    char cmd[1024]="./find_addr ";
    strcat(cmd,virDomainGetName(d));
    fp = popen(cmd, "r");
    fscanf(fp,"%s",address);
    printf("<tr><td>%d</td><td>%s</td>",virDomainGetID(d),virDomainGetName(d));
    printf("<td>%s</td>",address);
    printf("<td>%s</td><td>%ld%</td></tr>",states[di.state], (de.cpuTime-di.cpuTime)/10000000/di.nrVirtCpu);
    virDomainFree(d);
}
