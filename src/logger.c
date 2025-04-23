/* logger.c is the code for logging the chat text to a local file
    Attr: Alice Deleon*/

#include "logger.h"

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
    /* Using fixed format strings to write to the file prevents format string vulnerabilities*/
    fprintf(fp, "%s >> %s\n", sender, message);
    fclose(fp);
}
/* End log_msg()*/
/***********************************************************************************/

/*Begin quit_opt()*/
void quit_opt(const char *filename){
    char buffer[BUFFER_SIZE];
    int save_opt=0;
    printf("Would you like to save the chat log? [Y | n]\n");
    if(fgets(buffer, sizeof(buffer), stdin) == NULL){
        printf("File saved\n");
        perror("Input Error\n");
    }
        
    if(strcasecmp(buffer, "y") == 0)
        printf("Chat saved as %s\n", filename);

    if(strcasecmp(buffer, "n") == 0){
        remove(filename);
        printf("File: %s removed\n", filename);
    }    
}
