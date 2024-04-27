#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdbool.h>
#include <curl/curl.h>
#include <cJSON.h>

typedef struct MessageChunk MessageChunk;

// Basic structures
typedef struct {
    int h;
    char *mimetype;
    int size;
    int w;
} ChunkContentInfo;

typedef struct {
    char *event_id;
} RelatesToInReplyTo;

typedef struct {
    char *event_id;
    bool is_falling_back;
    RelatesToInReplyTo *in_reply_to;
    char *rel_type;
    char *key;
} ContentRelatesTo;

typedef struct {
    char **com_reddit_thread_heroes_user_ids;
    int count;
    bool current_user_participated;
    MessageChunk *latest_event;
} Thread;

typedef struct {
    Thread *thread;
} Relations;

// Complex structures
typedef struct {
    char *membership;
    char *avatar_url;
    char *displayname;
    char *msgtype;
    char *body;
    char *com_reddit_blurred_url;
    bool *com_reddit_nsfw_image;
    ChunkContentInfo *info;
    char *url;
    ContentRelatesTo *relates_to;
} ChunkContent;

typedef struct {
    double age;
    char *com_reddit_subreddit_id;
    Relations *relations;
    ChunkContent *prev_content;
    char *prev_sender;
    double prev_stream_pos;
    char *replaces_state;
    bool *com_reddit_is_moderator;
} ChunkUnsigned;

typedef struct MessageChunk {
    ChunkContent *content;
    char *event_id;
    double origin_server_ts;
    char *room_id;
    char *sender;
    char *state_key;
    char *type;
    ChunkUnsigned *unsigned_chunk;
} MessageChunk;


typedef struct {
    MessageChunk *chunk;
} MessageState;


typedef struct {
    struct MessageChunk** chunks;
    int size;
} MessageChunkArray;

typedef struct {
    char *start;
    char *start_stream;
    char *end;
    MessageChunkArray *chunk;
    char *updates;
    MessageState *state;
} MessageResponse;

// Function declarations
ChunkContentInfo *parseChunkContentInfo(cJSON *info);
ContentRelatesTo *parseContentRelatesTo(cJSON *relates_to);
ChunkContent *parseChunkContent(cJSON *content);
Thread *parseThread(cJSON *thread);
Relations *parseRelations(cJSON *relations);
ChunkUnsigned *parseChunkUnsigned(cJSON *unsigned_chunk);
MessageChunkArray *parseMessageChunks(cJSON *message);
MessageState *parseMessageState(cJSON *state);
MessageResponse *parseMessageResponse(char *response_body);
void printMessageResponse(MessageResponse *messageResponse);

#endif // MESSAGES_H