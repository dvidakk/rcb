#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "messages.h"

void printChunkContentInfo(ChunkContentInfo *info) {
    if (info) {
        printf("\n[Chunk Content Info]\n");
        printf("h: %d\n", info->h);
        if (info->mimetype) printf("mimetype: %s\n", info->mimetype);
        printf("size: %d\n", info->size);
        printf("w: %d\n", info->w);
    }
}

void printRelatesToInReplyTo(RelatesToInReplyTo *in_reply_to) {
    if (in_reply_to) {
        printf("\n[Relates To In Reply To]\n");
        if (in_reply_to->event_id) printf("event_id: %s\n", in_reply_to->event_id);
    }
}
void printContentRelatesTo(ContentRelatesTo *relates_to) {
    if (relates_to) {
        printf("\n[Content Relates To]\n");
        if (relates_to->event_id) printf("event_id: %s\n", relates_to->event_id);
        printf("is_falling_back: %s\n", relates_to->is_falling_back ? "true" : "false");
        if (relates_to->in_reply_to) {
            printf("in_reply_to:\n");
            printRelatesToInReplyTo(relates_to->in_reply_to);
        }
        if (relates_to->rel_type) printf("rel_type: %s\n", relates_to->rel_type);
    }
}

void printChunkContent(ChunkContent *content) {
    if (content) {
        printf("\n[Chunk Content]\n");
        if (content->membership) printf("membership: %s\n", content->membership);
        if (content->avatar_url) printf("avatar_url: %s\n", content->avatar_url);
        if (content->displayname) printf("displayname: %s\n", content->displayname);
        if (content->msgtype) printf("msgtype: %s\n", content->msgtype);
        if (content->body) printf("body: %s\n", content->body);
        if (content->com_reddit_blurred_url) printf("com_reddit_blurred_url: %s\n", content->com_reddit_blurred_url);
        printf("com_reddit_nsfw_image: %s\n", content->com_reddit_nsfw_image ? "true" : "false");
        if (content->info) {
            printf("info:\n");
            printChunkContentInfo(content->info);
        }
        if (content->url) printf("url: %s\n", content->url);
        if (content->relates_to) {
            printf("relates_to:\n");
            printContentRelatesTo(content->relates_to);
        }
    }
}

void printRelations(Relations *relations) {
    if (relations) {
        printf("\n[Relations]\n");
        if (relations->thread) {
            printf("thread:\n");
            printThread(relations->thread);
        }
    }
}

void printThread(Thread *thread) {
    if (thread) {
        printf("\n[Thread]\n");
        if (thread->com_reddit_thread_heroes_user_ids) {
            printf("com_reddit_thread_heroes_user_ids:\n");
            for (int i = 0; i < thread->count; i++) {
                printf("\t%s\n", thread->com_reddit_thread_heroes_user_ids[i]);
            }
        }
        printf("count: %d\n", thread->count);
        printf("current_user_participated: %s\n", thread->current_user_participated ? "true" : "false");
        if (thread->latest_event) {
            printf("latest_event:\n");
            printMessageChunk(thread->latest_event);
        }
    }
}
void printChunkUnsigned(ChunkUnsigned *unsigned_chunk) {
    if (unsigned_chunk) {
        printf("\n[Chunk Unsigned]\n");
        printf("age: %f\n", unsigned_chunk->age);
        if (unsigned_chunk->com_reddit_subreddit_id) printf("com_reddit_subreddit_id: %s\n", unsigned_chunk->com_reddit_subreddit_id);
        if (unsigned_chunk->prev_content) {
            printf("prev_content:\n");
            printChunkContent(unsigned_chunk->prev_content);
        }
        if (unsigned_chunk->prev_sender) printf("prev_sender: %s\n", unsigned_chunk->prev_sender);
        printf("prev_stream_pos: %f\n", unsigned_chunk->prev_stream_pos);
        if (unsigned_chunk->replaces_state) printf("replaces_state: %s\n", unsigned_chunk->replaces_state);
        if (unsigned_chunk->relations) {
            printf("relations:\n");
            printRelations(unsigned_chunk->relations);
        }
    }
}

void printMessageChunk(MessageChunkArray *chunk) {
    //iterate over chunks
    for (int i = 0; i < chunk->size; i++) {
        printf("\n[Message Chunk %d]\n", i);
        printSingleChunk(chunk->chunks[i]);
    }
}

void printSingleChunk(MessageChunk *chunk) {
    if (chunk) {
        // create a divider for each chunk
        printf("\n--------------------------------------------------\n");
        if (chunk->content) {
            printf("content:\n");
            printChunkContent(chunk->content);
        }
        if (chunk->event_id) printf("\tevent_id: %s\n", chunk->event_id);
        printf("\torigin_server_ts: %f\n", chunk->origin_server_ts);
        if (chunk->room_id) printf("\troom_id: %s\n", chunk->room_id);
        if (chunk->sender) printf("\tsender: %s\n", chunk->sender);
        if (chunk->state_key) printf("\tstate_key: %s\n", chunk->state_key);
        if (chunk->type) printf("\ttype: %s\n", chunk->type);
        if (chunk->unsigned_chunk) {
            printf("\tunsigned_chunk:\n");
            printChunkUnsigned(chunk->unsigned_chunk);
        }
    }
}

void printMessageState(MessageState *state) {
    if (state) {
        printf("\n[Message State]\n");
        if (state->chunk) {
            printf("chunk:\n");
            printSingleChunk(state->chunk);
        }
    }
}

void printMessageResponse(MessageResponse *messageResponse) {
    if (messageResponse) {
        printf("\n[Message Response]\n");
        if (messageResponse->start) printf("start: %s\n", messageResponse->start);
        if (messageResponse->start_stream) printf("start_stream: %s\n", messageResponse->start_stream);
        if (messageResponse->end) printf("end: %s\n", messageResponse->end);
        if (messageResponse->chunk) {
            printf("chunk:\n");
            printMessageChunk(messageResponse->chunk);
        }
        if (messageResponse->updates) printf("updates: %s\n", messageResponse->updates);
        if (messageResponse->state) {
            printf("state:\n");
            printMessageState(messageResponse->state);
        }
    }
}