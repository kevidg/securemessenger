/* util.c contains secure messenger app utility functions to retrieve
    a user name from the system and for input validation functions
    Attr: Alice Deleon, Michael Guerette (MDG)*/
    
#include "util.h"

/****************** */
/* Begin get_default_name()*/
/* Attr: (MDG)*/
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


// Commented out since the app does not need a user entered IP Address
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