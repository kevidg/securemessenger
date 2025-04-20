#include "secmsg.h"

/*****************/
/* Start Here */
/*vvvvvvvvvvvvvvvv*/
int main(int argc, char *argv[]){
    // Arg Parser
    int opt, is_client=0, is_server=0;
    char *username = NULL;

    OpenSSL_add_all_algorithms();
    //generate_aes_key_iv();

    while((opt = getopt(argc, argv, "csn:")) != -1){
        switch (opt){
            case 'c':
                is_client = 1;
                break;
            case 's':
                is_server=1;
                break;
            case 'n':
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

