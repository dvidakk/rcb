#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "matrix/matrix_client.h"
#include "reddit/reddit.h"
#include "ini_utils.h"

#include "test.h"

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

    MessageResponse *messages = RedMatrix_getRoomMessages(matrix, "!GS8kU7qKTMydi5gBrShQtg:reddit.com");

    User **users = createUserTable(100);
    int user_count = 0;

    for (int i = 0; i < messages->state->size; i++) {
        MessageChunk *message_chunk = messages->state->chunks[i];
        ChunkContent *content = message_chunk->content;
        if (strcmp(message_chunk->type, "m.room.member") == 0) {
            if(strcmp(content->membership, "join") == 0) {
                if (getUserDisplayname(users, 100, message_chunk->sender) == NULL) {
                    insertUser(users, 100, message_chunk->sender, content->displayname);
                    user_count++;
                }
            } else if (strcmp(content->membership, "leave") == 0) {
                if (getUserDisplayname(users, 100, message_chunk->sender) == NULL) {
                    // get the displayname from the previous content in the unsigned chunk
                    char* inster_name = message_chunk->unsigned_chunk->prev_content->displayname;
                    insertUser(users, 100, message_chunk->sender, inster_name);
                    user_count++;
                }
            }
        } 
    }


    for (int i = messages->chunk->size - 1; i >= 0; i--) {
        MessageChunk *message = messages->chunk->chunks[i];
        ChunkContent *content = message->content;
        if (content->body != NULL) {
            char* displayname = getUserDisplayname(users, 100, message->sender);
            if(displayname != NULL) {
                printf("Message: %s - [%s] %s\n",  content->body, content->msgtype, displayname);
            } else {
                printf("Message: %s - [%s] %s\n",  content->body, content->msgtype, message->sender);
            }
        } else if (strcmp(message->type, "m.room.member") == 0) {
            if(strcmp(content->membership, "join") == 0) {
                printf("Member: %s - [%s]\n", content->displayname, content->membership);
            } else {
                char* displayname = getUserDisplayname(users, 100, message->sender);

                printf("Member: %s - [%s]\n", displayname, content->membership);
            }
        } else {
            char* displayname = getUserDisplayname(users, 100, message->sender);
            printf("Message: [Attachment] - [%s] %s\n", message->type, displayname);
        }
    }
    //RedMatrix_getDisplayName(matrix, "@t2_hltcflqh8:reddit.com");
    //RedMatrix_getJoinedRooms(matrix);
    
    freeUserTable(users, 100);
    Reddit_free(reddit);
    RedMatrix_free(matrix);
    return 0;
}
