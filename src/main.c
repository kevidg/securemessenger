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








/**************** */
/*Begin log_msg() */
/**************** */
void log_msg(const char *filename, const char *sender, const char *message){
    FILE *fp = fopen(filename, "a");
    if(!fp){
        perror("Cannot open file");
        return;
    }

    //Write to the log
    fprintf(fp, "%s >> %s\n", sender, message);
    fclose(fp);
}
/* End log_msg()*/
/***********************************************************************************/

/****************** */
/* Begin get_default_name()*/
/********************* */
const char *get_default_name(){
    struct passwd *pw = getpwuid(getuid());
    if(pw){
        return pw -> pw_name;
    }
    else
        return "NoNameLoser";
}
/* End get_default_name()*/
/***********************************************************************************/

/***************** */
/*Input Validation*/
/**************** */
//IP check
    /*
    if(user_in_addr == NULL){
        fprintf(stderr, "!! Error: Please provide an IP Address.\n");
        fprintf(stderr, "Usage: %s -i <ip_address>\n", argv[0]);
        return 1;
    }

    if (validate_ip(user_in_addr)){
        printf("## Connecting to IP Address: %s\n", user_in_addr);
        run_client();
    }
    else{
        fprintf(stderr, "!! Error: Invalid IP address");
    }

    */
/*
int validate_ip(const char *in_addr){
    int segments = 0;
    int num;
    char *token;
    char *cp_in_addr = strdup(in_addr);

    if (!cp_in_addr){
        return 0;
    }

    token =strtok(cp_in_addr, ".");
    while(token){
        if(isdigit(token[0]) == 0)
        {
            free(cp_in_addr);
            return 0;
        }
        
        num = atoi(token);
        if(num < 0 || num > 255){
            free(cp_in_addr);
            return 0;
        }

            segments++;
            token = strtok(NULL, ".");
    }
    free(cp_in_addr);

    return (segments == 4);
} 
/* End validate_ip() ***/
/***************************************************************/