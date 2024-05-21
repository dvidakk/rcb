#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdbool.h>
#include <curl/curl.h>
#include <cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MessageChunk MessageChunk;

// Basic structures
typedef struct {
    int h;
    char *mimetype;
    int size;
    int w;
} ChunkContentInfo;

typedef struct RelationsDisplaySettings {
    double display_origin_server_ts;
    bool distinguish_host;
} RelationsDisplaySettings;

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
    bool hide;

} RelationsHide;

typedef struct {
    int count;
    char *key;
    double origin_server_ts;
} RelationsAnotations;

typedef struct {
    bool nsfw;
} mAttributes;

typedef struct { 
    mAttributes *attributes;
    bool collapse;
    double origin_server_ts;
    char *reason;
} RelationsRedditPotentiallyToxic;

typedef struct {
    Thread *thread;
    RelationsHide *hide_user_content;
    RelationsDisplaySettings *display_settings;
    RelationsAnotations *annotations;
    RelationsRedditPotentiallyToxic *com_reddit_potentially_toxic;
} Relations;

// Complex structures
typedef struct {
    char *membership;
    char *avatar_url;
    char *displayname;
    char *msgtype;
    char *body;
    char *com_reddit_blurred_url;
    bool com_reddit_nsfw_image;
    ChunkContentInfo *info;
    char *url;
    ContentRelatesTo *relates_to;
    char *reason;
    mAttributes *attributes;
    bool hide;
    char *target_user_id;
    bool collapse;
} ChunkContent;

typedef struct {
    double age;
    char *com_reddit_subreddit_id;
    Relations *relations;
    ChunkContent *prev_content;
    char *prev_sender;
    double prev_stream_pos;
    char *replaces_state;
    bool com_reddit_is_moderator;
    char *redacted_by;
    struct MessageChunk *redacted_because;
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
    char *redacts;
} MessageChunk;


typedef struct {
    MessageChunk *chunk;
} MessageState;

typedef struct {
    struct MessageChunk** chunks;
    int size;
} MessageStateArray;

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
    MessageStateArray *state;
} MessageResponse;

// Function declarations
ChunkContentInfo                *msg_parseChunkContentInfo(cJSON *info);
ContentRelatesTo                *msg_parseContentRelatesTo(cJSON *relates_to);
ChunkContent                    *msg_parseChunkContent(cJSON *content);
Thread                          *msg_parseThread(cJSON *thread);
struct MessageChunk             *msg_parseSingleChunk(cJSON *message);
Relations                       *msg_parseRelations(cJSON *relations);
RelationsAnotations             *msg_parseRelationsAnotations(cJSON *annotations);
RelationsRedditPotentiallyToxic *msg_parseRelationsRedditPotentiallyToxic(cJSON *com_reddit_potentially_toxic);
mAttributes                     *msg_parseMAttributes(cJSON *attributes);
ChunkUnsigned                   *msg_parseChunkUnsigned(cJSON *unsigned_chunk);
MessageChunkArray               *msg_parseMessageChunks(cJSON *message);
MessageStateArray               *msg_parseMessageState(cJSON *state);
MessageResponse                 *msg_parseMessageResponse(char *response_body);
#ifdef __cplusplus
}
#endif

#endif // MESSAGES_H