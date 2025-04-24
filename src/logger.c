/* logger.c is the code for logging the chat text to a local file
    Attr: Alice DeLeon*/
#include "secmsg.h"
#include "logger.h"

/**************** */
/*Begin log_msg() 
  Alice DeLeon */
/**************** */
void log_msg(const char *filename, const char *sender, const char *message){
    FILE *fp = fopen(filename, "a");
    if(!fp){
        perror("Cannot open file");
        return;
    }

    /* ~~~~~~ Mitigate Format String Problems */
    // Format string vulnerabilities are mitigated in the function by using fixed format strings,
    // ensuring that user-controlled input is not directly used as a format specifier. This
    // prevents attackers from exploiting the format string to read or write arbitrary memory.
    /* ~~~~~~ Mitigate Failure to Protect Stored Data */
    // The mitigation ensures that chat logs are written to files using fixed format strings
    // in the log_msg function. This prevents tampering by avoiding the use of user-controlled
    // input as format specifiers, ensuring the integrity and structure of the log entries.
    // Write to the log
    fprintf(fp, "%s >> %s\n", sender, message);
    fclose(fp);
}
/* End log_msg()*/
/***********************************************************************************/

/*Begin quit_opt()*/
/* Alice DeLeon */
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
