#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// include windows.h for sleep function
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include "utils/ini_utils.h"
#include "utils/test.h"

#include "matrix/matrix_client.h"
#include "reddit/reddit.h"

#define USER_TABLE_SIZE 5000


// handle sleep for windows and linux and mac
void universal_sleep(int seconds) {
    #ifdef _WIN32
        Sleep(seconds * 1000);
    #else
        sleep(seconds);
    #endif
}

int main(void) {

    char* r_croatia = "!GS8kU7qKTMydi5gBrShQtg:reddit.com";

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
    Reddit_WhoAmI(reddit);
 
    RedMatrix *matrix = RedMatrix_new(redditToken);
    RedMatrix_login(matrix);    

    char *from_token = NULL;
    MessageResponseList *response_list = malloc(sizeof(MessageResponseList));
    //MessageResponse *messages = RedMatrix_getRoomMessages(matrix, r_croatia);

    response_list->responses = malloc(sizeof(MessageResponse) * USER_TABLE_SIZE);
    response_list->size = 0;

    int depth = 1;
    for (int i=0; i < depth; i++) {
        MessageResponse *messages = RedMatrix_getRoomMessages(matrix, r_croatia, from_token);
        response_list->responses[response_list->size] = messages;
        response_list->size++;
        from_token = messages->end;
        // sleep for 1 second to avoid rate limiting
        printf("\r%d/%d", i, depth);
        fflush(stdout);

        universal_sleep(1);
    // print the update with return carriage
    }
    
    printDevider();

    User **users = createUserTable(USER_TABLE_SIZE);
    int user_count = 0;

    for (int i = 0; i < response_list->size; i++){
        MessageResponse *messages = response_list->responses[i];
        for (int j = 0; j < messages->state->size; j++) {
            MessageChunk *message_chunk = messages->state->chunks[j];
            ChunkContent *content = message_chunk->content;
            char *displayname = NULL;

            if (strcmp(message_chunk->type, "m.room.member") == 0 && getUserDisplayname(users, USER_TABLE_SIZE, message_chunk->sender) == NULL) {
                if(strcmp(content->membership, "join") == 0) {
                    displayname = content->displayname;
                } else if (strcmp(content->membership, "leave") == 0) {
                    // get the displayname from the previous content in the unsigned chunk
                    if (message_chunk->unsigned_chunk && message_chunk->unsigned_chunk->prev_content) {
                        displayname = message_chunk->unsigned_chunk->prev_content->displayname;
                    }
                }

                if (displayname != NULL) {
                    insertUser(users, USER_TABLE_SIZE, message_chunk->sender, displayname);
                    user_count++;
                }
            }
        }
    }

for (int i = response_list->size - 1; i >= 0; i--) {
    MessageResponse *messages = response_list->responses[i];
    for (int j = messages->chunk->size - 1; j >= 0; j--) {
        MessageChunk *message = messages->chunk->chunks[j];
        ChunkContent *content = message->content;
        char* displayname = getUserDisplayname(users, USER_TABLE_SIZE, message->sender);

        if (content->body != NULL) {

            if (content->msgtype != NULL && strcmp(content->msgtype, "m.image") == 0) {
                printf("Image: %s - URL: %s - [%s] %s\n", content->body ? content->body : " ", content->url, content->msgtype, displayname ? displayname : message->sender);
            } else {
                printf("Message: %s - [%s] %s\n",  content->body, content->msgtype, displayname ? displayname : message->sender);
            }
        } else if (strcmp(message->type, "m.room.member") == 0) {
            printf("Member: %s - [%s]\n", strcmp(content->membership, "join") == 0 ? content->displayname : displayname, content->membership);
        } else {
            printf("Message: [Attachment] - [%s] %s\n", message->type, displayname);
        }
    }
}
    // print the date range of the messages
    double start = response_list->responses[0]->chunk->chunks[0]->origin_server_ts / 1000.0;
    double end = response_list->responses[response_list->size - 1]->chunk->chunks[response_list->responses[response_list->size - 1]->chunk->size - 1]->origin_server_ts / 1000.0;

    printDevider();

    char *start_time_str = convertTimestampToLocaltime(start);
    printf("Start: %s\n", start_time_str);
    free(start_time_str);  // Don't forget to free the string when you're done with it

    char *end_time_str = convertTimestampToLocaltime(end);
    printf("End: %s\n", end_time_str);
    free(end_time_str);  // Don't forget to free the string when you're done with it
    //RedMatrix_getDisplayName(matrix, "@t2_hltcflqh8:reddit.com");
    //RedMatrix_getJoinedRooms(matrix);
    
    freeUserTable(users, USER_TABLE_SIZE);
    Reddit_free(reddit);
    RedMatrix_free(matrix);
    return 0;
}
