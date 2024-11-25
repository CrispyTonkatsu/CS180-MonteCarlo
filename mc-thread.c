#include <stdio.h>
#include <stdlib.h>

#include "mc-head.h"

int RoyalFlush(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 1) {
    printf("Not enough draw piles. Needs: 1\n");
    return 0;
  }

  const int value = 0;
  const int suit = 2;

  int first_suit = 0;
  bool found[] = {false, false, false, false, false};

  for (int i = 0; i < 5; i++) {
    int *card = DrawPoolDrawCard(draw_piles[0], pack);

    if (i == 0) {
      first_suit = card[suit];
    } else if (card[suit] != first_suit) {
      return false;
    }

    if (card[value] == 1) {
      found[4] = true;
    } else if (card[value] >= 10) {
      found[card[value] - 10] = true;
    } else {
      return false;
    }
  }

  for (int i = 0; i < 5; i++) {
    if (!found[i]) {
      return false;
    }
  }

  return true;
}

int Fifty50(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  (void) draw_pile_count;
  int *card = DrawPoolDrawCard(draw_piles[0], pack);
  return card[1] == 0;
}

int (*const events[])(CardPack *, DrawPool **, int) = {
    &RoyalFlush,
    &Fifty50,
};

// TODO: Get rid of this and get it into the EventDetails struct when we can finally read it off somewhere
int const event_iterations[] = {
    2500000,
    1,
};

void *EventRun(void *data) {
  // Interpreting the data (This should never fail)
  EventDetails *details = (EventDetails *) data;

  // Initialization of RNG
  randData rng_machine;
  ThreadSeedRNG(&rng_machine);

  // Getting the shuffled cards from the details
  DrawPool **draw_piles = EventDetailsGetShuffledCards(details, &rng_machine);

  // Running the event
  int successes = 0;
  for (int i = 0; i < event_iterations[details->event_number - 1]; i++) {
    for (int j = 0; j < details->draw_pile_count; j++) {
      DrawPoolShuffle(draw_piles[j], &rng_machine);
    }

    successes += events[details->event_number - 1](details->pack, draw_piles, details->draw_pile_count);
  }
  printf("Successes: %d\n", successes);

  // Cleanup of the hands used for the simulation
  for (int i = 0; i < details->draw_pile_count; i++) {
    DrawPoolDelete(draw_piles[i]);
  }
  free(draw_piles);

  // Returning the success count for this thread
  int *output = calloc(1, sizeof(int));
  *output = successes;
  return (void *) output;
}

// Pokemon Formulas
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

float TypeChartXAttackY(int typeX, int typeY) {
  // If there's no attacking type, does not contribute to outcome
  // -1 specfically to not screw up first attacking type
  if (typeX == -1) {
    return -1.0f;
  }
  // If there's no defending type, does not contribute to outcome
  // 1 specifically to not screw up first defending type
  if (typeY == -1) {
    return 1.0f;
  }
  int hashIndex = PKMN_TypeChart[typeX][typeY];
  return PKMN_TypeChartHash[hashIndex];
}

float TypeChartXAttackYFull(int typeX1, int typeX2, int typeY1, int typeY2) {
  float effectX1Y = TypeChartXAttackY(typeX1, typeY1) * TypeChartXAttackY(typeX1, typeY2);
  float effectX2Y = typeX2 == -1 ? -1.0f : TypeChartXAttackY(typeX2, typeY1) * TypeChartXAttackY(typeX2, typeY2);

  float effect = effectX1Y > effectX2Y ? effectX1Y : effectX2Y;

  return effect;
}
