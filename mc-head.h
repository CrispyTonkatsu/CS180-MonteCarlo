#pragma once

#include <stdbool.h>

// CardPack methods

typedef struct {
  int *card_data;
  int card_count;
  int field_count;
} CardPack; // All possible cards in the event

CardPack *CardPackCreate(char *card_file);

int CardPackIndex(CardPack *pack, int card_id, int card_field);

int *CardPackGetCard(CardPack *pack, int card_id);

void CardPackDelete(CardPack *pack);

// DrawPool Struct

typedef struct {
  int card_count;
  int *card_ids;
  int top_index;
} DrawPool;

DrawPool *DrawPoolCreate(const char *hand_file);

DrawPool *DrawPoolClone(DrawPool *other);

void DrawPoolShuffle(DrawPool *other);

void DrawPoolDelete(DrawPool *self);

// EventDetails Struct

typedef struct {
  CardPack *pack;
  int draw_pile_count;
  DrawPool **draw_piles;
  int event_number;
  int thread_count;
  // TODO: Confirm whether we are getting this number
  int iteration_count; // Come up with it for now: 2.5 Million
} EventDetails;

EventDetails *EventDetailsCreate(
    char *card_file, char (*draw_pool_files)[128], int draw_pool_count, int event_number, int thread_count);

void EventDetailsDelete(EventDetails *self);

// Event Running functions

void *EventRun(void *data);

// Pokemon Data
enum PC52_ATT { PC52_value, PC52_color, PC52_suit, PC52_face };

enum PKMN_ATT { PKMN_evoline, PKMN_type1, PKMN_type2, PKMN_evostage };

//  Multiplier  (Effectiveness)             (EXAMPLE, with types)
//  ****************************************************************
//  1x          (regular effectiveness)     (grass against fighting)
//  2x          (super effective!)          (water against fire)
//  0.25x       (not very effective)        (normal against steel)
//  0           (immune)                    (ground against flying)

const float PKMN_TypeChartHash[4] = {1, 2, 0.5f, 0};

// The integers in the table below correspond to the hash above
// aka, use the ints from the table to access the floats above
const int PKMN_TypeChart[18][18] = {
    {0, 0, 0, 0, 0, 2, 0, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 2, 2, 0, 1, 2, 3, 1, 0, 0, 0, 0, 2, 1, 0, 1, 2},
    {0, 1, 0, 0, 0, 2, 1, 0, 2, 0, 0, 1, 2, 0, 0, 0, 0, 0},
    {0, 0, 0, 2, 2, 2, 0, 2, 3, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {0, 0, 3, 1, 0, 1, 2, 0, 1, 1, 0, 2, 1, 0, 0, 0, 0, 0},
    {0, 2, 1, 0, 2, 0, 1, 0, 2, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 2, 2, 2, 0, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 0, 1, 2},
    {3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 2, 2, 2, 0, 2, 0, 1, 0, 0, 1},
    {0, 0, 0, 0, 0, 2, 1, 0, 1, 2, 2, 1, 0, 0, 1, 2, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 2, 2, 0, 0, 0, 2, 0, 0},
    {0, 0, 2, 2, 1, 1, 2, 0, 2, 2, 1, 2, 0, 0, 0, 2, 0, 0},
    {0, 0, 1, 0, 3, 0, 0, 0, 0, 0, 1, 2, 2, 0, 0, 2, 0, 0},
    {0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 3, 0},
    {0, 0, 1, 0, 1, 0, 0, 0, 2, 2, 2, 1, 0, 0, 2, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 3},
    {0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 2, 2},
    {0, 1, 0, 2, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 1, 1, 0}};

float TypeChartXAttackY(int typeX, int typeY);

float TypeChartXAttackYFull(int typeX1, int typeX2, int typeY1, int typeY2);
