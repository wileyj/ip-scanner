//global.h

#ifndef _GLOBAL_H_ 
#define _GLOBAL_H_
#define KEY_LENGTH  2048
#define RESPONSE_SIZE 69
#define PUB_EXP     3
#define FAIL       -1
#define PRINT_KEYS
#define WRITE_TO_FILE
#define MSGBUFSIZ 1024
#define THREADS 20
/*
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_S STRINGIZE(__LINE__)
#define FILE_S (__FILE__)
#define DATE_S (__DATE__)
#define TIME_S (__TIME__)

#define DEFAULT ""
#define INFO_LINE   "("__TIME__") - "
#define WARN_LINE   "("__TIME__") ("__DATE__") - "
#define ERROR_LINE  "("__TIME__") ("__DATE__") ("__FILE__") - "
#define DEBUG_LINE  "("__TIME__") ("__DATE__") ("__FILE__" @"LINE_S") - "
*/
/* Log Function Prototype for all cpp files */
void logit(const char *fmt,...);
void error(const char *msg, int r_code);
#include <string>

extern char*           LOG;
extern char*           LOGINFO;
extern bool            email; 
extern bool            silent;
extern bool            help;
extern bool            d_mode;
extern int             vlevel;
extern char            output_log[25];
extern char*           active;
extern int             is_eth0;
extern int             is_eth1;
extern int             is_bond0;
extern std::string     networkFile;
extern std::string     iface_file;
extern std::string     iface_file_short;
extern std::string     iface_root;
extern char            hostname[128];
extern char*           fname;
extern int             numWarnings;
extern int             numErrors;
extern std::ofstream  logfile;


extern bool            found_iface;
extern std::string     iface_name_short;
extern std::string     iface_name;
extern std::string     iface_ip;
extern std::string     iface_hwaddr;
extern std::string     iface_netmask;
extern std::string     all_ip;

extern unsigned char  * buff;
extern size_t          pri_len;
extern size_t          pub_len;
extern char          * pri_key;
extern char          * pub_key;
extern char            msg[KEY_LENGTH/8]; 
extern char          * rsa_e;
extern char          * rsa_d;
extern char          * err;
extern int             encrypt_len;

extern int             server;
extern char          * portnum;
extern char            buf[1024];
extern char            reply[1024];
extern char          * line;
extern unsigned char * buff;
extern ssize_t         len;
extern char          * priv_out;
extern char          * pub_out;
extern char          * keyfile;
extern char            *response[];

// specific to cipc

extern char* euid;
#endif
