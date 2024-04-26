#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix/matrix_client.h"
#include "reddit/reddit.h"
#include "ini_utils.h"

int main() {

    char* home = getenv("HOME");
    char path[256];
    snprintf(path, sizeof(path), "%s/dev/ingest/creds.ini", home);
    
    Creds* creds = parseIniFile(path);    
    Reddit *reddit = Reddit_new(creds->client_id, 
                                creds->client_secret, 
                                creds->user, 
                                creds->password, 
                                creds->user_agent);

    Reddit_login(reddit);
    char* redditToken = Reddit_getToken(reddit);
    printf("Reddit Token: %s\n", redditToken);

    RedMatrix *matrix = RedMatrix_new(redditToken);

    LoginResponse *response = RedMatrix_login(matrix);

    RedMatrix_getRoomMessages(matrix, "!GS8kU7qKTMydi5gBrShQtg:reddit.com");
    //RedMatrix_getJoinedRooms(matrix);

    Reddit_free(reddit);
    RedMatrix_free(matrix);
    return 0;
}
