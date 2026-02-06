// Transposition table

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

struct TableEntry {
    ull key = 0;
    int evaluation = NO_EVAL;
    char depth = 0, type = NONE;
    int16_t bestMove = 0;

    TableEntry() {
        key = 0;
        evaluation = NO_EVAL;
        depth = 0;
        type = NONE;
        bestMove = 0;
    }

    TableEntry(ull key_, int evaluation_, char depth_, char type_, int16_t bestMove_) {
        key = key_;
        evaluation = evaluation_;
        depth = depth_;
        type = type_;
        bestMove = bestMove_;
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

    inline pair<int, Move> get(Board &board, ull key, int depth, int alpha, int beta, int depthFromRoot) {
        // if (tableSize == 0)
        //     return {NO_EVAL, Move()};
        int index = (__uint128_t(key) * __uint128_t(tableSize)) >> 64;
        if (table[index].type == NONE)
            return {NO_EVAL, Move()};
        if (table[index].key != key)
            return {NO_EVAL, Move()};
        if (table[index].depth < depth)
            return {NO_EVAL, Move(table[index].bestMove)};

        int eval = NO_EVAL;
        if (table[index].type == EXACT)
            eval = table[index].evaluation;

        if (table[index].type == LOWER_BOUND &&
            table[index].evaluation >= beta) // when calculated TT node we got alpha>=beta
            eval = table[index].evaluation;

        if (table[index].type == UPPER_BOUND && table[index].evaluation < alpha)
            eval = table[index].evaluation;

        if (abs(eval) >= MATE_SCORE_MAX_PLY && eval != NO_EVAL){
            if (eval > 0)
                eval -= depthFromRoot;
            else
                eval += depthFromRoot;
        }

        return {eval, Move(table[index].bestMove)};
    }

    inline TableEntry getEntry(Board &board, ull key, int depthFromRoot) {
        // if (tableSize == 0)
        //     return TableEntry();
        int index = (__uint128_t(key) * __uint128_t(tableSize)) >> 64;
        if (table[index].type == NONE)
            return TableEntry();
        if (table[index].key != key)
            return TableEntry();

        auto entry = table[index];

        if (abs(entry.evaluation) >= MATE_SCORE_MAX_PLY && entry.evaluation != NO_EVAL){
            if (entry.evaluation > 0)
                entry.evaluation -= depthFromRoot;
            else
                entry.evaluation += depthFromRoot;
        }

        return entry;
    }

    inline int getDepth(Board &board, ull key) {
        // if (tableSize == 0)
        //     return -10;
        int index = (__uint128_t(key) * __uint128_t(tableSize)) >> 64;
        if (table[index].type == NONE)
            return -10;
        if (table[index].key != key)
            return -10;
        // if(table[index].depth<depth)
        // 	return {NO_EVAL,table[index].bestMove};

        return table[index].depth;
    }

    inline int getNodeType(ull key) {
        // if (tableSize == 0)
        //     return NONE;
        int index = (__uint128_t(key) * __uint128_t(tableSize)) >> 64;
        if (table[index].key != key)
            return NONE;
        return table[index].type;
    }

    inline void prefetch(ull key) {
        // if (tableSize == 0)
        //     return;
        __builtin_prefetch(&table[(__uint128_t(key) * __uint128_t(tableSize)) >> 64]);

    }
};

TranspositionTable transpositionTable;