// Transposition table

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

struct TableEntry {
    ull key = 0;
    int score = NO_EVAL;
    char depth = 0, type = NONE;
    int16_t move = 0;

    TableEntry() {
        key = 0;
        score = NO_EVAL;
        depth = 0;
        type = NONE;
        move = 0;
    }

    TableEntry(ull key_, int score_, char depth_, char type_, int16_t bestMove_) {
        key = key_;
        score = score_;
        depth = depth_;
        type = type_;
        move = bestMove_;
    }
};

bool alwaysReplace = false;

struct TranspositionTable {
    ll tableSize = 0;
    vector<TableEntry> table;

    // mutex TTmutex;

    inline void write(Board &board, ull key, int evaluation, int depth, int type, int age, Move bestMove, int depthFromRoot) {
        // if (tableSize == 0)
        //     return;
        if (abs(evaluation) >= MATE_SCORE_MAX_PLY){
            if (evaluation > 0)
                evaluation += depthFromRoot;
            else
                evaluation -= depthFromRoot;
        }
        int index = (__uint128_t(key) * __uint128_t(tableSize)) >> 64;
        if (table[index].type != NONE) {
            if (table[index].key == key) {
                if (table[index].depth > depth)
                    return;
                if (table[index].depth == depth && table[index].type == EXACT)
                    return;
            }
        }
        // TTmutex.lock();
        table[index] = {key, evaluation, char(depth), char(type), bestMove.move};
        // TTmutex.unlock();
    }

    inline TableEntry get(Board &board, ull key, int depthFromRoot) {
        // if (tableSize == 0)
        //     return TableEntry();
        int index = (__uint128_t(key) * __uint128_t(tableSize)) >> 64;
        if (table[index].type == NONE)
            return TableEntry();
        if (table[index].key != key)
            return TableEntry();

        auto entry = table[index];

        if (abs(entry.score) >= MATE_SCORE_MAX_PLY && entry.score != NO_EVAL){
            if (entry.score > 0)
                entry.score -= depthFromRoot;
            else
                entry.score += depthFromRoot;
        }

        return entry;
    }

    inline void prefetch(ull key) {
        // if (tableSize == 0)
        //     return;
        __builtin_prefetch(&table[(__uint128_t(key) * __uint128_t(tableSize)) >> 64]);

    }
};

TranspositionTable transpositionTable;