/*****************/
/* Start Here */

/* Progam entry, main() parses the cli arguments and uses getopt() to select which 
   process client or server to start. It aslso calls get_default_user() to retrieve a user name from the 
   system
   attr: Michael Guerette (MDG), Alice Deleon, Jay Patel*/*/
/*vvvvvvvvvvvvvvvv*/

#include "secmsg.h"


int main(int argc, char *argv[]){
    // Arg Parser
    int opt, is_client=false, is_server=false;
    char *username = NULL;

    // Adds libraries and functions necessary for OpenSSL encryption
    OpenSSL_add_all_algorithms();

    while((opt = getopt(argc, argv, "csn:")) != -1){
        switch (opt){
            case 'c':
                is_client = true;
                break;
            case 's':
                is_server= true;
                break;
            case 'n':
                // !!! Needs Validation
                username = optarg;
                break;
            default:
               fprintf(stderr, "Usage: %s -c | -s [-n username]\n", argv[0]);
               return -1;
        }
    } // End Arg Parser While loop
    if(!username){
        //Get's the users logon id from system if a username isn't supplied
        username = (char *)get_default_name();
    }

    if(is_client){
        run_client(username);
    }
    else if(is_server){
        run_server(username);
    }
    else{
        fprintf(stderr, "Please choose option -c or -s\n");
    }
    return 0;    

} 

/* ---- End Main ----*/
/********************************************************************/

