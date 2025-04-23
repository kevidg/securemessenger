/* logger.c is the code for logging the chat text to a local file
    Attr: Alice Deleon*/
#include "secmsg.h"
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
    
    printf("Would you like to save the chat log? [Y/n] > ");
    if(fgets(buffer, sizeof(buffer), stdin) == NULL){
        perror("Input Error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    
    // Save the log is the default option
    if(strcasecmp(buffer, "y") == 0 || strlen(buffer) == 0){
        printf("Chat saved as %s\n", filename);
    }

    else if(strcasecmp(buffer, "n") == 0){
        if (remove(filename) == 0)
        printf("File: %s removed\n", filename);
        else
            perror("Error deleting log file");
    }
    else{
        printf("Input unknown. Saving log file.\n");
    
    }    
}
