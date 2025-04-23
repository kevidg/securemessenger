/*****************/
/* Start Here */
/*vvvvvvvvvvvvvvvv*/

#include "secmsg.h"


int main(int argc, char *argv[]){
    // Arg Parser
    int opt, is_client=false, is_server=false;
    char *username = NULL;

   
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
    } 
    if(!username){
        
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

