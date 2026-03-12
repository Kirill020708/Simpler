// Transposition table

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

struct Flag {
    int8_t flags = 0;

    Flag() {}
    Flag(int8_t type, int8_t ttpv) {
        flags = type + (ttpv << 2);
    }

    int8_t type() {
        return flags&3;
    }

    int8_t ttpv() {
        return (flags>>2)&1;
    }
};

struct __attribute__ ((packed)) TableEntry {
    uint16_t key = 0;
    int16_t score = NO_EVAL;
    int16_t eval = NO_EVAL;
    int16_t move = 0;
    int8_t depth = 0;
    Flag flag;

    TableEntry() {
        key = 0;
        score = NO_EVAL;
        depth = 0;
        move = 0;
        flag = Flag(NONE, 0);
    }

    TableEntry(uint16_t key_, int score_, int eval_, char depth_, char type_, int16_t bestMove_, bool ttpv_) {
        key = key_;
        score = score_;
        eval = eval_;
        depth = depth_;
        move = bestMove_;
        flag = Flag(type_, ttpv_);
    }
};

struct Cluster {
    TableEntry entries[3];

    inline void write(TableEntry ttEntry) {
        int lowestDepthEntry = 0, emptyEntry = -1;

        for (int entry = 0; entry < 3; entry++) {
            int8_t type = entries[entry].flag.type();
            if (type != NONE &&
                entries[entry].key == ttEntry.key) {

                if (entries[entry].depth > ttEntry.depth)
                    return;
                if (entries[entry].depth == ttEntry.depth && type == EXACT)
                    return;
                entries[entry] = ttEntry;
                return;
            }

            if (type == NONE)
                emptyEntry = entry;
            else if (entries[lowestDepthEntry].depth > entries[entry].depth)
                lowestDepthEntry = entry;
        }

        if (emptyEntry != -1)
            entries[emptyEntry] = ttEntry;
        else
            entries[lowestDepthEntry] = ttEntry;
    }

    inline void writeStaticEval(uint16_t key, int16_t eval) {
        int lowestDepthEntry = 0, emptyEntry = -1;

        for (int entry = 0; entry < 3; entry++) {
            int8_t type = entries[entry].flag.type();
            if (type != NONE &&
                entries[entry].key == key) {

                entries[entry].eval = eval;
                return;
            }
        }
    }

    inline TableEntry get(uint16_t key) {
        for (int entry = 0; entry < 3; entry++) {
            int8_t type = entries[entry].flag.type();
            if (type != NONE &&
                entries[entry].key == key) {

                return entries[entry];
            }
        }
        return TableEntry();
    }
};

bool alwaysReplace = false;

struct TranspositionTable {
    ll tableSize = 0;
    vector<Cluster> table;
    int b16 = 0b1111'1111'1111'1111;

    // mutex TTmutex;

    inline void write(Board &board, ull key, int score, int eval, int depth, int type, int age, Move bestMove, int depthFromRoot, bool ttpv) {
        // if (tableSize == 0)
        //     return;
        if (abs(score) >= MATE_SCORE_MAX_PLY){
            if (score > 0)
                score += depthFromRoot;
            else
                score -= depthFromRoot;
        }
        int index = (__uint128_t(key) * __uint128_t(tableSize)) >> 64;
        uint16_t key16 = key & b16;
        // if (table[index].flag.type() != NONE) {
        //     if (table[index].key == key16) {
        //         if (table[index].depth > depth)
        //             return;
        //         if (table[index].depth == depth && table[index].flag.type() == EXACT)
        //             return;
        //     }
        // }
        // TTmutex.lock();
        table[index].write({key16, score, eval, char(depth), char(type), bestMove.move, ttpv});
        // TTmutex.unlock();
    }

    inline void writeStaticEval(ull key, int eval) {
        int index = (__uint128_t(key) * __uint128_t(tableSize)) >> 64;
        uint16_t key16 = key & b16;
        table[index].writeStaticEval(key16, eval);
    }

    inline TableEntry get(Board &board, ull key, int depthFromRoot) {
        // if (tableSize == 0)
        //     return TableEntry();
        int index = (__uint128_t(key) * __uint128_t(tableSize)) >> 64;
        uint16_t key16 = key & b16;
        // if (table[index].flag.type() == NONE)
        //     return TableEntry();
        // if (table[index].key != key16)
        //     return TableEntry();

        auto entry = table[index].get(key16);
        if (entry.flag.type() != NONE) {
            if (abs(entry.score) >= MATE_SCORE_MAX_PLY && entry.score != NO_EVAL){
                if (entry.score > 0)
                    entry.score -= depthFromRoot;
                else
                    entry.score += depthFromRoot;
            }
        }

        return entry;
    }

    inline void prefetch(ull key) {
        // if (tableSize == 0)
        //     return;
        __builtin_prefetch(&table[(__uint128_t(key) * __uint128_t(tableSize)) >> 64]);

    }

    int getHashfull() {
        int hits = 0;
        // for (int i = 0; i < min(1000ll, tableSize); i++)
        //     hits += (table[i].flag.type() != NONE);
        if (tableSize && tableSize < 1000)
            hits = hits * 1000 / tableSize;
        return hits;
    }
};

TranspositionTable transpositionTable;