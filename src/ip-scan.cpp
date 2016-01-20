#include <map>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "split.h"
#include <sstream>
#include <arpa/nameser.h>
#include <resolv.h>

using namespace::std;

//forward declarations
int pinger(char *ip, int external);
int string_regex(std::string str, std::string pattern);
int lookup(char *ip);
int scan(char *ip, int netmask);
int break_ip(char *ip);
bool isChar(char *x);
char * itoc(int x);

char  *ipaddress;
char  *hostname;
char  t_mask[2];
int   mask;
int   size;
int   external;
int   up_count;
int   down_count;
int   total_count;
int   need_ip = 0;
int   verbose = 1;
int   verify_ipnum;
int   reserved;
int   networks;
int   is_lookup;
char* arg_1;
int   octet_1; 
int   octet_2;
int   octet_3;
int   octet_4;
int   check_subnet;
int   is_char;
std::map<int, int> network;
const char* first_octet;
const char* second_octet; 
const char* third_octet;
const char* fourth_octet;
struct hostent *host;
struct addrinfo hints, *res, *p;

bool isChar(char *x){
      struct sockaddr_in sa;
      int result = inet_pton(AF_INET, x, &(sa.sin_addr));
      return result != 0;
}

char * itoc(int x){
      int y = x;
      int end = 0;
      while(x >= 1) {
            x = x/10;
                  end++;
      }
      char* cstr = NULL;
      cstr = new char [end+1];
      sprintf(cstr, "%i", y);
      return cstr;
}

int break_ip(char *ip){
      size = strlen(ip);
      char base_ip[size];
      for (int x=0; x<size;x++){
            base_ip[x] = ip[x];
      }
      base_ip[size] = '\0';
      verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<"Splitting on '.':"<<base_ip<<std::endl;
      Splitter split ( base_ip, ".");
      first_octet  = split[0].c_str();
      second_octet = split[1].c_str();
      third_octet  = split[2].c_str();
      fourth_octet = split[3].c_str();
      octet_1 = atoi(first_octet);
      octet_2 = atoi(second_octet);
      octet_3 = atoi(third_octet);
      octet_4 = atoi(fourth_octet);
      if ( (octet_1 < 1) && (octet_1 > 254) ){
          return -11;
          if ( (octet_2 < 1) && (octet_2 > 254) ){
                return -12;
                if ( (octet_3 < 1) && (octet_3 > 254) ){
                      return -13;
                      if ( (octet_4 < 1) || (octet_4 > 254) ){
                            return -14;
                      }
                }
          }
      }
      return 0;
}

int showHelp( char binary[] ){
      cout << "usage:\n  " <<  binary <<" <ip> <optional: number of ips to retrieve>    " << std::endl;
      cout << "\t-v - Enable verbose logging                                            " << std::endl;
      cout<<std::endl;
      cout<<"\t"<<binary<<" 166.77.175.0/24    # finds all available ips in vlan 175    "<<std::endl;
      cout<<"\t"<<binary<<" 166.77.175.0/24 10 # finds 10 available ips in vlan 175     "<<std::endl;
      cout<<"\t"<<binary<<" 166.77.175.100     # finds status of ip 166.77.175.100      "<<std::endl;
      cout<<"\t"<<binary<<" mtv.com            # finds ip(s) and status of ips for fqdn "<<std::endl;
      cout << std::endl<<std::endl;
      return 0;
}



int main(int argc, char *argv[]){
      up_count =0;
      down_count=0;
      /* value of number of ips in each mask we'll allow */
      network[20] = 4096;
      network[21] = 2048;
      network[22] = 1024;
      network[23] = 512;
      network[24] = 256;
      network[25] = 128;
      network[26] = 64;
      network[27] = 32;
      network[28] = 16;
      network[29] = 8;
      network[30] = 4;

      /* test for second arg of either "-v"  verbose, or an int for */
      /* or number of ips we're looking for  in arg2 or arg3 */
      if (argv[2]){
            char* arg_2 = argv[2];
            if (string_regex(arg_2, "-v") != 0 ){
                  /* we've found the verbose option in arg 2 */
                  verbose = 0;
            }else{
                  /* we found arg 2, but it's not -v. check to see if a valid int now */
                  /* if invalid, revert to default */
                  std::istringstream iss(arg_2);
                  iss >> need_ip;
                  if (iss.eof() == false){
                        need_ip = 0;
                  }
            }
      }
      /* repeat for arg3 */
      if (argv[3]){
            char* arg_3 = argv[3];
            if (string_regex(arg_3, "-v") != 0 ){
                  verbose = 0;
            }else{
                  std::istringstream iss(arg_3);
                  iss >> need_ip;
                  if (iss.eof() == false){
                        need_ip = 0;
                  }       
            }
      }
      verbose ? cout<<"" : cout<<"VERBOSE:"<<verbose<<std::endl;

      if (argv[1] == NULL) {
            cout<<__FILE__<<" @"<<__LINE__<<" Missing Arg"<<std::endl; 
            showHelp(argv[0]);
            return 1;
      }else{
            arg_1 = argv[1];
            /* test for chars, if true then we've got a dns name(fqdn) */
            is_char = isChar(arg_1);
            verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" is_char:"<<is_char<<":"<<std::endl;
            /* test for a subnet value, "/" ex: 166.77.175.0/24 */
            check_subnet = string_regex(arg_1, "/");
      }
      if (check_subnet == 1){
            /* this is a subnet */
            verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" check_subnet"<<std::endl;
            size = strlen(arg_1);
            t_mask[0] = arg_1[size-2];
            t_mask[1] = arg_1[size-1];
            mask = atoi(t_mask);
            verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" mask:"<<mask<<std::endl;
            if (mask > 19  && mask < 31){
                  int scan_return = scan(arg_1, mask);
                  /* test scan_return results for any invalid octets */
                  if (scan_return == -11){
                        verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<" Invalid ip (first octet):"<<arg_1<<std::endl;
                        return -4;
                  }else if (scan_return == -12){
                        verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<" Invalid ip (second octet):"<<arg_1<<std::endl;
                        return -5;
                  }else if (scan_return == -13){
                        verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<" Invalid ip (third octet):"<<arg_1<<std::endl;
                        return -6;
                  }else if (scan_return == -7){
                        verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<" Invalid ip (fourth octet):"<<arg_1<<std::endl;
                        return -7;
                  }else{
                        verbose ? cout<<"" : cout<<std::endl<<__FILE__<<" @"<<__LINE__<<" Scan function returned: "<<scan_return<<std::endl;
                  }
            } else {
                  /* test for any netmasks we're not going to look through */
                  if (mask < 20){
                        verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" no masks less than 20 allowed"<<std::endl;
                  }else{
                        verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" no masks greater than 30 allowed"<<std::endl;
                  }
            }
      }else{
            if (is_char == 0){
                  /* fail if localhost is arg 1 */
                  int check_localhost = string_regex(arg_1, "localhost");
                  if (check_localhost == 1){
                        verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<" arg_1 of:"<<arg_1<<" is not allowed("<<check_localhost<<")"<<std::endl;
                        cout<<"False. Arg: "<<arg_1<<" is invalid"<<std::endl;
                        return 7;
                  }
                  /* this is an fqdn */
                  is_lookup = 1;
                  int status;
                  memset(&hints, 0, sizeof hints);
                  hints.ai_family = AF_UNSPEC;
                  hints.ai_socktype = SOCK_STREAM;
                  /* initialize resolve.h options */
                  /* for now, hardcode dns to openDNS: 208.67.222.222 */
                  res_init();
                  _res.nscount=1;
                  _res.nsaddr_list[0].sin_family=AF_INET;
                  _res.nsaddr_list[0].sin_port=htons(53);
                  _res.nsaddr_list[0].sin_addr.s_addr=inet_addr("208.67.222.222");
                  if ((status = getaddrinfo(arg_1, NULL, &hints, &res)) != 0) {
                        /* we've got an error, print nd return */
                        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
                        return 2;
                  }
                  for(p = res;p != NULL; p = p->ai_next) {
                        char ipstr[INET6_ADDRSTRLEN];
                        void *addr;
                        char *ipver;
                        if (p->ai_family == AF_INET) {
                              struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
                              addr = &(ipv4->sin_addr);
                              ipver = "IPv4";
                        }
                        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
                        /* do a lookup of the value of "ipstr" and print out [status] fqdn ipaddress */
			verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<"IPSTR-"<<ipstr<<"-"<<std::endl;
			verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<"String_regex: "<<string_regex(ipstr, ":")<<std::endl;
      			if (string_regex(ipstr, ":") == 0 ){
                              break_ip(ipstr);
 			      verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<""<<std::endl;
                              lookup(ipstr);
                        }
                  }
                  freeaddrinfo(res);
                  cout<<std::endl;
                  return 0;
            }else{
                  /* this is a single IP */
                  int test_ip = break_ip(arg_1);
                  /* test for any invalid octets */
                  if (test_ip == -11){
                        cout<<__FILE__<<" @"<<__LINE__<<" Invalid ip (first octet):"<<arg_1<<std::endl;
                        return -4;
                  }else if (test_ip == -12){
                        cout<<__FILE__<<" @"<<__LINE__<<" Invalid ip (second octet):"<<arg_1<<std::endl;
                        return -5;
                  }else if (test_ip == -13){
                        cout<<__FILE__<<" @"<<__LINE__<<" Invalid ip (third octet):"<<arg_1<<std::endl;
                        return -6;
                  }else if(test_ip == -14){
                        cout<<__FILE__<<" @"<<__LINE__<<" Invalid ip (fourth octet):"<<arg_1<<std::endl;
                        return -7;
                  }else{
                        int lookup_single = lookup(arg_1);
                        /* if return is 50 (hardcoded), then the single ip is UP */
                        if(lookup_single == 50){
                              verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" IP ("<<arg_1<<") is not available"<<std::endl;
                              verbose ?  cout<<"[  UP  ] -->  "<<arg_1: cout<<"";
                              cout<<std::endl;
                        }else{
                              cout<<std::endl;
                              return 10;
                        }
                  }
            }
      }
      if ((check_subnet == 1) && (need_ip == 0)){
            if (mask > 24){
                  verify_ipnum = network[mask];
                  reserved = 0;
            }else{
                  verify_ipnum = networks * 254;
                  reserved = 2 * networks;
            }
            verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" mask:"<<mask<<std::endl;
            /* display stats after execution */
            cout<<"**********************************************"<<std::endl;
            cout<<std::endl<<"Scanned "<<total_count<<"/"<< verify_ipnum <<" ips"<<std::endl;
            cout<<"\tIgnoring "<<reserved<<" ips"<<std::endl;
            cout<<"\t"<<up_count<<" ips are up"<<std::endl;
            cout<<"\t"<<down_count<<" ips are down"<<std::endl;
      }
      /* line feed for clean reading */
      cout<<std::endl;
      return 0;
}

int scan(char *ip, int netmask){
      verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" Received IP:"<<ip<<std::endl;
      verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" Received Mask:"<<netmask<<std::endl;
      verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" Number of ips in ("<<netmask<<"): "<<network[netmask]<<std::endl;
      /* do math required in for loops */
      networks = network[netmask] / 256;
      if (networks < 1){
            networks = 1;
      }
      verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" Number of Networks: "<<networks<<std::endl;
      /* set size of new char, and copy contents of ip to it */
      char base_ip[size-5];
      for (int x=0; x<size-5;x++){
            base_ip[x] = ip[x];
      }
      base_ip[size-5] = '\0';
      verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" ip:"<<ip<<std::endl;
      verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" base_ip:"<<base_ip<<std::endl;
      int test_ip = break_ip(ip);
      if (test_ip == 0){
            int edge;
            int start_loop;
            if (network[netmask] > 253){
                  edge = 255;
                  start_loop = 1;
            }else{
                  if (octet_4 == 0){
                       start_loop = octet_4 + 1;
                        edge = network[netmask] + 1;
                  }else{
                      start_loop = octet_4;
                        edge = octet_4 + network[netmask];
                  }
            }
            verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" EDGE:"<<edge<<std::endl;
            verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" START_LOOP:"<<start_loop<<std::endl;
            int last_octet = 0;
            total_count = 0;
            /* start looping through the network */
            for (int i = 0; i<networks; i++){
                  last_octet = octet_3 + i;
                  for(int j=start_loop; j<edge; j++){
                        /* we've got a value, now we put it together as a char  */
                        total_count++;
                        int thesize = (strlen(itoc(octet_1))+1) + (strlen(itoc(octet_2))+1) + (strlen(itoc(last_octet))+1) + (strlen(itoc(j)) );
                        verbose ? cout<<"" : cout<<std::endl<<"\t"<<"("<< total_count<<") pinging: "<<octet_1<<"."<<octet_2<<"."<<last_octet<<"."<<j<<std::endl;
                        char ping_ip[thesize];
                        strcpy(ping_ip, itoc(octet_1));
                        strcat(ping_ip, ".");
                        strcat(ping_ip, itoc(octet_2));
                        strcat(ping_ip, ".");
                        strcat(ping_ip, itoc(last_octet));
                        strcat(ping_ip, ".");
                        strcat(ping_ip, itoc(j));
                        int retval = lookup(ping_ip);
                        /* if we're looking for n ips, and this is the nth....set j to end. */
                        /* also, set i to also end */
                        if ((need_ip > 0) && (retval == 10)){
                             j=edge;
                             i=networks;
                        }
                  }
            }
            return 0;
      }else{
            /* various errors */
            verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" BREAK_IP FAILURE:"<<test_ip<<std::endl;
            if (test_ip == -11){
                  verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" Invalid ip (first octet):"<<ip<<std::endl;
                  return -4;
            }else if (test_ip == -12){
                  verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" Invalid ip (second octet):"<<ip<<std::endl;
                  return -5;
            }else if (test_ip == -13){
                  verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" Invalid ip (third octet):"<<ip<<std::endl;
                  return -6;
            }else{
                  verbose ? cout<<"" : cout<<__FILE__<<" @"<<__LINE__<<" Invalid ip (fourth octet):"<<ip<<std::endl;
                  return -7;
            }
      }
}

int lookup(char *ip){
      /* test for an internal network, pinger function is timed based on this */
      if (strcmp(first_octet, "172") == 0){
            verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<" found internal 172"<<std::endl;
            external = 1;
      }
      if (strcmp(first_octet, "10") == 0){
            verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<" found internal 10"<<std::endl;
            external = 1;
      }
      if (strcmp(first_octet, "166") == 0){
            verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<" found internal 166"<<std::endl;
            external = 1;
      }
      verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<" looking up:"<<ip<<","<<external<<std::endl;
      int pingable = pinger(ip, external);
      verbose ? cout<<"" : cout<<"\t"<<__FILE__<<" @"<<__LINE__<<" pingable:"<<pingable<<std::endl;
      if (pingable == 0){
            /* ip is not pingable in configured time interval */
            down_count++;
            if (is_lookup == 1){
                  /* fqdn output */
                  cout<<"[ DOWN ] -->  "<<arg_1<<"\t"<<ip<<std::endl;
            }else{
                  /* default output */
                  cout<<"[ DOWN ] -->  "<<ip<<std::endl;
            }
            if ((need_ip > 0) && (need_ip == down_count)){
                  /* if we're looking for n ips, and this is the nth....return */
                  return 10;
            }
      }else{
            /* ip is pingable in configured time interval */
            pingable = 50;
            up_count++;
            if (is_lookup == 1){
                  /* fqdn output */
                  cout<<"[  UP  ] -->  "<<arg_1<<"\t"<<ip<<std::endl;
            }
            if (verbose == 0){
                  /* default output*/
                  cout<<"[  UP  ] -->  "<<ip<<std::endl;
            }
      }
      /* return what pinger function gave back */
      return pingable;
}
