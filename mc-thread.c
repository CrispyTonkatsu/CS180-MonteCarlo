#include <stdio.h>
#include <stdlib.h> /* exit, malloc          */
#include <string.h>

#include "ThreadSafe_PRNG.h"
#include "mc-head.h"

void *EventRun(void *data) {
  EventDetails *details = (EventDetails *) data;
  return NULL;
}

// Pokemon Formulas
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
