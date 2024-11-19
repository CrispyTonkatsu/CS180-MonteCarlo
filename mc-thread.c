#include <stdio.h>

#include "mc-head.h"

int EventTest(EventDetails *details) {
  printf("%d", details->draw_pile_count);
  return 1;
}

int (*const events[])(EventDetails *) = {
    EventTest,
};

int const event_iterations[] = {
    0,
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
  events[0](details);

  // Cleanup of the hands used for the simulation
  for(int i = 0; i<details->draw_pile_count; i++){
    DrawPoolDelete(draw_piles[i]);
  }

  // Returning the success count for this thread
  return (void *) 0;
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
