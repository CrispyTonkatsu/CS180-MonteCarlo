#pragma once

#include <stdbool.h>
#include "ThreadSafe_PRNG.h"

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

void CardPrint(int* card, int fields);

// DrawPool Struct

typedef struct {
  int card_count;
  int *card_ids;
  int top_index;
} DrawPool;

DrawPool *DrawPoolCreate(const char *hand_file);

DrawPool *DrawPoolClone(DrawPool *other);

void DrawPoolShuffle(DrawPool *self, randData *rng_machine);

int *DrawPoolDrawCard(DrawPool *self, CardPack* pack);

void DrawPoolDelete(DrawPool *self);

// EventDetails Struct

typedef struct {
  CardPack *pack;
  DrawPool **draw_piles;
  int draw_pile_count;
  int event_number;

  // HACK:
  // Come up with this number for now: 2.5 Million.
  // Eventually it will be a diff value per event
  int iteration_count;
} EventDetails;

EventDetails *EventDetailsCreate(char *card_file, char (*draw_pool_files)[128], int draw_pool_count, int event_number);

DrawPool **EventDetailsGetShuffledCards(EventDetails *details, randData *rng_machine);

void EventDetailsDelete(EventDetails *self);

// Event Running functions

/**
 * @brief This function will run the event number as provided by EventDetails
 *
 * @param data This is the EventDetails which describe the entire event
 * @return The amount of successes for the event
 */
void *EventRun(void *data);

// Pokemon Functions
float TypeChartXAttackY(int typeX, int typeY);

float TypeChartXAttackYFull(int typeX1, int typeX2, int typeY1, int typeY2);
