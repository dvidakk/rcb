#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <cJSON.h>

#include "messages.h"

ChunkContentInfo *parseChunkContentInfo(cJSON *info) {
    ChunkContentInfo *chunkContentInfo = malloc(sizeof(ChunkContentInfo));
    cJSON *h = cJSON_GetObjectItem(info, "h");
    chunkContentInfo->h = h ? h->valueint : 0;
    cJSON *mimetype = cJSON_GetObjectItem(info, "mimetype");
    chunkContentInfo->mimetype = mimetype ? mimetype->valuestring : NULL;
    cJSON *size = cJSON_GetObjectItem(info, "size");
    chunkContentInfo->size = size ? size->valueint : 0;
    cJSON *w = cJSON_GetObjectItem(info, "w");
    chunkContentInfo->w = w ? w->valueint : 0;
    return chunkContentInfo;
}

ChunkContent *parseChunkContent(cJSON *content) {
    ChunkContent *chunkContent = malloc(sizeof(ChunkContent));

    cJSON *membership = cJSON_GetObjectItem(content, "membership");
    chunkContent->membership = membership ? membership->valuestring : NULL;

    cJSON *avatar_url = cJSON_GetObjectItem(content, "avatar_url");
    chunkContent->avatar_url = avatar_url ? avatar_url->valuestring : NULL;

    cJSON *displayname = cJSON_GetObjectItem(content, "displayname");
    chunkContent->displayname = displayname ? displayname->valuestring : NULL;

    cJSON *msgtype = cJSON_GetObjectItem(content, "msgtype");
    chunkContent->msgtype = msgtype ? msgtype->valuestring : NULL;

    cJSON *body = cJSON_GetObjectItem(content, "body");
    chunkContent->body = body ? body->valuestring : NULL;

    cJSON *com_reddit_blurred_url = cJSON_GetObjectItem(content, "com.reddit.blurred_url");
    chunkContent->com_reddit_blurred_url = com_reddit_blurred_url ? com_reddit_blurred_url->valuestring : NULL;

    cJSON *com_reddit_nsfw_image = cJSON_GetObjectItem(content, "com.reddit.nsfw_image");
    chunkContent->com_reddit_nsfw_image = com_reddit_nsfw_image ? com_reddit_nsfw_image->valueint : 0;

    cJSON *info = cJSON_GetObjectItem(content, "info");
    chunkContent->info = info ? parseChunkContentInfo(info) : NULL;

    cJSON *url = cJSON_GetObjectItem(content, "url");
    chunkContent->url = url ? url->valuestring : NULL;

    cJSON *relates_to = cJSON_GetObjectItem(content, "m.relates_to");
    chunkContent->relates_to = relates_to ? parseContentRelatesTo(relates_to) : NULL;

    cJSON *reason = cJSON_GetObjectItem(content, "reason");
    chunkContent->reason = reason ? reason->valuestring : NULL;

    return chunkContent;
}

ChunkUnsigned *parseChunkUnsigned(cJSON *unsigned_chunk) {
    ChunkUnsigned *chunkUnsigned = malloc(sizeof(ChunkUnsigned));

    cJSON *age = cJSON_GetObjectItem(unsigned_chunk, "age");
    chunkUnsigned->age = age ? age->valuedouble : 0;

    cJSON *com_reddit_subreddit_id = cJSON_GetObjectItem(unsigned_chunk, "com_reddit_subreddit_id");
    chunkUnsigned->com_reddit_subreddit_id = com_reddit_subreddit_id ? com_reddit_subreddit_id->valuestring : NULL;

    cJSON *prev_content = cJSON_GetObjectItem(unsigned_chunk, "prev_content");
    chunkUnsigned->prev_content = prev_content ? parseChunkContent(prev_content) : NULL;

    cJSON *prev_sender = cJSON_GetObjectItem(unsigned_chunk, "prev_sender");
    chunkUnsigned->prev_sender = prev_sender ? prev_sender->valuestring : NULL;

    cJSON *prev_stream_pos = cJSON_GetObjectItem(unsigned_chunk, "prev_stream_pos");
    chunkUnsigned->prev_stream_pos = prev_stream_pos ? prev_stream_pos->valuedouble : 0;

    cJSON *replaces_state = cJSON_GetObjectItem(unsigned_chunk, "replaces_state");
    chunkUnsigned->replaces_state = replaces_state ? replaces_state->valuestring : NULL;

    cJSON *relations = cJSON_GetObjectItem(unsigned_chunk, "m.relations");
    chunkUnsigned->relations = relations ? parseRelations(relations) : NULL;

    cJSON *com_reddit_is_moderator = cJSON_GetObjectItem(unsigned_chunk, "com.reddit.is_moderator");
    chunkUnsigned->com_reddit_is_moderator = com_reddit_is_moderator ? com_reddit_is_moderator->valueint : 0;

    cJSON *redacted_by = cJSON_GetObjectItem(unsigned_chunk, "redacted_by");
    chunkUnsigned->redacted_by = redacted_by ? redacted_by->valuestring : NULL;

    cJSON *redacted_because = cJSON_GetObjectItem(unsigned_chunk, "redacted_because");
    chunkUnsigned->redacted_because = redacted_because ? parseSingleChunk(redacted_because) : NULL;

    return chunkUnsigned;
}

struct MessageChunk* parseSingleChunk(cJSON* message) {
    MessageChunk *messageChunk = malloc(sizeof(MessageChunk));
    //printf("Message: %s\n", cJSON_Print(message));

    cJSON *content = cJSON_GetObjectItem(message, "content");
    messageChunk->content = content ? parseChunkContent(content) : NULL;

    cJSON *event_id = cJSON_GetObjectItem(message, "event_id");
    messageChunk->event_id = event_id ? strdup(event_id->valuestring) : NULL;

    cJSON *origin_server_ts = cJSON_GetObjectItem(message, "origin_server_ts");
    messageChunk->origin_server_ts = origin_server_ts ? origin_server_ts->valuedouble : 0;

    cJSON *room_id = cJSON_GetObjectItem(message, "room_id");
    messageChunk->room_id = room_id ? strdup(room_id->valuestring) : NULL;

    cJSON *sender = cJSON_GetObjectItem(message, "sender");
    messageChunk->sender = sender ? strdup(sender->valuestring) : NULL;

    cJSON *state_key = cJSON_GetObjectItem(message, "state_key");
    messageChunk->state_key = state_key ? strdup(state_key->valuestring) : NULL;

    cJSON *type = cJSON_GetObjectItem(message, "type");
    messageChunk->type = type ? strdup(type->valuestring) : NULL;

    cJSON *unsigned_chunk = cJSON_GetObjectItem(message, "unsigned");
    messageChunk->unsigned_chunk = unsigned_chunk ? parseChunkUnsigned(unsigned_chunk) : NULL;

    cJSON *redacts = cJSON_GetObjectItem(message, "redacts");
    messageChunk->redacts = redacts ? strdup(redacts->valuestring) : NULL;
    
    return messageChunk;
}

MessageChunkArray* parseMessageChunk(cJSON* messages) {
    int size = cJSON_GetArraySize(messages);
    MessageChunk** messageChunks = malloc(size * sizeof(MessageChunk*));

    for (int i = 0; i < size; i++) {
        cJSON* message = cJSON_GetArrayItem(messages, i);
        messageChunks[i] = parseSingleChunk(message);
    }

    MessageChunkArray* result = malloc(sizeof(MessageChunkArray));
    result->chunks = messageChunks;
    result->size = size;

    return result;
}

MessageStateArray *parseMessageState(cJSON *state) {
    int size = cJSON_GetArraySize(state);
    MessageChunk** messageStates = malloc(size * sizeof(MessageChunk*));

    for (int i = 0; i < size; i++) {
        cJSON* message = cJSON_GetArrayItem(state, i);
        messageStates[i] = parseSingleChunk(message);
    }

    MessageStateArray* result = malloc(sizeof(MessageStateArray));
    result->chunks = messageStates;
    result->size = size;

    return result;
}

MessageResponse *parseMessageResponse(char *response_body) {
    cJSON *root = cJSON_Parse(response_body);
    MessageResponse *messageResponse = malloc(sizeof(MessageResponse));
    cJSON *start = cJSON_GetObjectItem(root, "start");
    messageResponse->start = start ? start->valuestring : NULL;
    cJSON *start_stream = cJSON_GetObjectItem(root, "start_stream");
    messageResponse->start_stream = start_stream ? start_stream->valuestring : NULL;
    cJSON *end = cJSON_GetObjectItem(root, "end");
    messageResponse->end = end ? end->valuestring : NULL;
    cJSON *chunk = cJSON_GetObjectItem(root, "chunk");
    messageResponse->chunk = chunk ? parseMessageChunk(chunk) : NULL;
    cJSON *updates = cJSON_GetObjectItem(root, "updates");
    messageResponse->updates = updates ? updates->valuestring : NULL;
    cJSON *state = cJSON_GetObjectItem(root, "state");
    messageResponse->state = state ? parseMessageState(state) : NULL;
    return messageResponse;
}

Thread *parseThread(cJSON *threadJson) {
    Thread *thread = malloc(sizeof(Thread));
    cJSON *com_reddit_thread_heroes_user_ids = cJSON_GetObjectItem(threadJson, "com.reddit.thread_heroes_user_ids");
    int size = cJSON_GetArraySize(com_reddit_thread_heroes_user_ids);
    thread->com_reddit_thread_heroes_user_ids = malloc(size * sizeof(char *));
    for (int i = 0; i < size; i++) {
        cJSON *user_id = cJSON_GetArrayItem(com_reddit_thread_heroes_user_ids, i);
        thread->com_reddit_thread_heroes_user_ids[i] = user_id->valuestring;
    }
    thread->count = size;
    cJSON *current_user_participated = cJSON_GetObjectItem(threadJson, "current_user_participated");
    thread->current_user_participated = current_user_participated ? current_user_participated->valueint : 0;
    cJSON *latest_event = cJSON_GetObjectItem(threadJson, "latest_event");
    thread->latest_event = latest_event ? parseSingleChunk(latest_event) : NULL;
    return thread;
}

RelationsHide* parseRelationsHide(cJSON *hide) {
    RelationsHide *relationsHide = malloc(sizeof(RelationsHide));
    cJSON *hideItem = cJSON_GetObjectItem(hide, "hide");
    relationsHide->hide = hideItem ? hideItem->valueint : 0;
    return relationsHide;
}

RelationsDisplaySettings *parseDisplaySettings(cJSON *display_settings) {
    RelationsDisplaySettings *displaySettings = malloc(sizeof(RelationsDisplaySettings));
    cJSON *display_origin_server_ts = cJSON_GetObjectItem(display_settings, "display_origin_server_ts");
    displaySettings->display_origin_server_ts = display_origin_server_ts ? display_origin_server_ts->valuedouble : 0;
    cJSON *distinguish_host = cJSON_GetObjectItem(display_settings, "distinguish_host");
    displaySettings->distinguish_host = distinguish_host ? distinguish_host->valueint : 0;
    return displaySettings;
}

Relations *parseRelations(cJSON *relationsJson) {
    Relations *relations = malloc(sizeof(Relations));

    cJSON *thread = cJSON_GetObjectItem(relationsJson, "m.thread");
    relations->thread = thread ? parseThread(thread) : NULL;

    cJSON *hide_user_content = cJSON_GetObjectItem(relationsJson, "com.reddit.hide_user_content");
    relations->hide_user_content = hide_user_content ? parseRelationsHide(hide_user_content) : NULL;

    cJSON *display_settings = cJSON_GetObjectItem(relationsJson, "com.reddit.display_settings");
    relations->display_settings = display_settings ? parseDisplaySettings(display_settings) : NULL;
    return relations;
}

RelatesToInReplyTo *parseRelatesToInReplyTo(cJSON *in_reply_to) {
    RelatesToInReplyTo *relatesToInReplyTo = malloc(sizeof(RelatesToInReplyTo));
    cJSON *event_id = cJSON_GetObjectItem(in_reply_to, "event_id");
    relatesToInReplyTo->event_id = event_id ? event_id->valuestring : NULL;
    return relatesToInReplyTo;
}

ContentRelatesTo *parseContentRelatesTo(cJSON *relates_to) {
    ContentRelatesTo *contentRelatesTo = malloc(sizeof(ContentRelatesTo));
    cJSON *rel_type = cJSON_GetObjectItem(relates_to, "rel_type");
    contentRelatesTo->rel_type = rel_type ? rel_type->valuestring : NULL;
    cJSON *event_id = cJSON_GetObjectItem(relates_to, "event_id");
    contentRelatesTo->event_id = event_id ? event_id->valuestring : NULL;
    cJSON *m_in_reply_to = cJSON_GetObjectItem(relates_to, "m.in_reply_to");
    contentRelatesTo->in_reply_to = parseRelatesToInReplyTo(m_in_reply_to);
    cJSON *is_falling_back = cJSON_GetObjectItem(relates_to, "is_falling_back");
    contentRelatesTo->is_falling_back = is_falling_back ? is_falling_back->valueint : 0;
    cJSON *key = cJSON_GetObjectItem(relates_to, "key");
    contentRelatesTo->key = key ? key->valuestring : NULL;
    return contentRelatesTo;
}
