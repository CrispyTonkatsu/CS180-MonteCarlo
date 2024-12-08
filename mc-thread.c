#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mc-head.h"

// Playing card events implementation

int RoyalFlush(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 1) {
    printf("Not enough draw piles. Needs: 1\n");
    return 0;
  }

  const int value = 0;
  const int suit = 2;
  const int draw_count = 5;
  const int min_for_royal = 10;

  int first_suit = 0;
  bool found[] = {false, false, false, false, false};

  for (int i = 0; i < draw_count; i++) {
    int *card = DrawPoolDrawCard(draw_piles[0], pack);

    if (i == 0) {
      first_suit = card[suit];
    } else if (card[suit] != first_suit) {
      return false;
    }

    if (card[value] == 1) {
      found[4] = true;
    } else if (card[value] >= min_for_royal) {
      found[card[value] - min_for_royal] = true;
    } else {
      return false;
    }
  }

  for (int i = 0; i < draw_count; i++) {
    if (!found[i]) {
      return false;
    }
  }

  return true;
}

int FourOfAKind(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 1) {
    printf("Not enough draw piles. Needs: 1\n");
    return 0;
  }

  const int draw_count = 5;

  int found[13] = {0}; // NOLINT *magic*

  for (int i = 0; i < draw_count; i++) {
    int *card = DrawPoolDrawCard(draw_piles[0], pack);
    found[card[0] - 1]++;
  }

  for (int i = 0; i < 13; i++) { // NOLINT *magic*
    if (found[i] >= 4) {
      return true;
    }
  }

  return false;
}

int FourOrTwo(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 1) {
    printf("Not enough draw piles. Needs: 1\n");
    return 0;
  }

  const int draw_count = 5;

  int found[13] = {0}; // NOLINT *magic*

  for (int i = 0; i < draw_count; i++) {
    int *card = DrawPoolDrawCard(draw_piles[0], pack);
    found[card[0] - 1]++;
  }

  for (int i = 0; i < 13; i++) { // NOLINT *magic*
    if (found[i] >= 4) {
      return true;
    }
  }

  return false;
}

int FourSuitsOrFacePair(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 1) {
    printf("Not enough draw piles. Needs: 1\n");
    return 0;
  }

  int *card = NULL;
  bool suits_found[] = {false, false, false, false}; // NOLINT *magic*
  int faces_found[] = {0, 0, 0};

  for (int i = 0; i < 7; i++) { // NOLINT *magic*
    card = DrawPoolDrawCard(draw_piles[0], pack);
    suits_found[card[2]] = true;

    if (card[3] == 1) {
      faces_found[card[0] - 11]++; // NOLINT *magic*
    }
  }

  bool all_suits = true;
  for (int i = 0; i < 4; i++) {
    if (!suits_found[i]) {
      all_suits = false;
      break;
    }
  }

  if (all_suits) {
    return true;
  }

  int face_pairs_remaining = 2;
  for (int i = 0; i < 3; i++) {
    if (faces_found[i] == 2) {
      face_pairs_remaining--;
    }
  }
  if (face_pairs_remaining == 0) {
    return true;
  }

  return false;
}

int Switch30(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 1) {
    printf("Not enough draw piles. Needs: 1\n");
    return 0;
  }

  int switches_remaining = 30; // NOLINT *magic*
  int *card = DrawPoolDrawCard(draw_piles[0], pack);
  bool prev_red = card[1];
  while (card != NULL) {
    if (card[1] != prev_red) {
      switches_remaining--;
      prev_red = card[1];

      if (switches_remaining == 0) {
        return true;
      }
    }
    card = DrawPoolDrawCard(draw_piles[0], pack);
  }

  return false;
}

// Pokemon events implementation
int EachPlayerDrawsStage(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 3) {
    printf("Not enough draw piles. Needs: 3\n");
    return 0;
  }

  bool stages_found[] = {false, false, false};

  for (int i = 0; i < 3; i++) {
    int *cards[] = {
        DrawPoolDrawCard(draw_piles[i], pack),
        DrawPoolDrawCard(draw_piles[i], pack),
        DrawPoolDrawCard(draw_piles[i], pack),
    };

    if (cards[0][3] == cards[1][3] && cards[1][3] == cards[2][3]) {
      if (stages_found[cards[0][3]]) {
        return false;
      }
      stages_found[cards[0][3]] = true;
    } else {
      return false;
    }
  }

  return true;
}

bool TeamBattleInvulCheck(int *team_0[2], int *team_1[2]) {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      float dealt = TypeChartXAttackYFull(team_0[i][1], team_0[i][2], team_1[j][1], team_1[j][2]);
      float recieved = TypeChartXAttackYFull(team_1[j][1], team_1[j][2], team_0[i][1], team_0[i][2]);

      if (dealt == 0.0 || recieved == 0.0) {
        return true;
      }
    }
  }

  return false;
}

int NoEffectAttack(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 3) {
    printf("Not enough draw piles. Needs: 3\n");
    return 0;
  }

  int *player0[] = {
      DrawPoolDrawCard(draw_piles[0], pack),
      DrawPoolDrawCard(draw_piles[0], pack),
  };

  int *player1[] = {
      DrawPoolDrawCard(draw_piles[1], pack),
      DrawPoolDrawCard(draw_piles[1], pack),
  };

  int *player2[] = {
      DrawPoolDrawCard(draw_piles[2], pack),
      DrawPoolDrawCard(draw_piles[2], pack),
  };

  int **teams[3] = {player0, player1, player2};

  for (int i = 0; i < 2; i++) {
    for (int j = i; j < 3; j++) {
      if (TeamBattleInvulCheck(teams[i], teams[j])) {
        return true;
      }
    }
  }

  return false;
}

int P1ChainOrZeroButP2No(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 2) {
    printf("Not enough draw piles. Needs: 2\n");
    return 0;
  }

  const int cards_to_draw = 7;

  int *players[2][cards_to_draw];

  int chain_count[] = {0, 0};
  int zero_count[] = {0, 0};

  for (int i = 0; i < 2; i++) {
    int **player = players[i];

    for (int j = 0; j < cards_to_draw; j++) {
      player[j] = DrawPoolDrawCard(draw_piles[i], pack);

      // Error might be here
      if (player[j][3] == 0) {
        zero_count[i]++;
      }

      bool evo_stages[] = {false, false, false};
      evo_stages[player[j][3]] = true;

      for (int k = 0; k < j; k++) {
        if (player[k][0] == player[j][0]) {
          evo_stages[player[k][3]] = true;
        }
      }

      if (evo_stages[0] == true && evo_stages[0] == evo_stages[1] && evo_stages[1] == evo_stages[2]) {
        chain_count[i]++;
      }
    }
  }

  if ((chain_count[1] >= 1) || (zero_count[1] >= 2)) {
    return false;
  }

  if ((chain_count[0] >= 1) || (zero_count[0] >= 2)) {
    return true;
  }

  return false;
}

int WaterOnlyPokemonHundred(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 1) {
    printf("Not enough draw piles. Needs: 1\n");
    return 0;
  }

  int water_only = 0;
  int water_and_other = 0;

  for (int i = 0; i < 100; i++) {
    int *card = DrawPoolDrawCard(draw_piles[0], pack);

    // It was vague whether the pokemon that was water + sth else had to be stage 0, but this provides the super small
    // result that matches with the rubric
    if ((card[1] == 10 || card[2] == 10) && card[2] != -1) {
      water_and_other++;
      continue;
    }

    if (card[3] == 0) {
      continue;
    }

    if (card[1] == 10 && card[2] == -1) {
      water_only++;
      continue;
    }
  }

  if (water_only >= 8 && water_and_other == 0) {
    return true;
  }

  return false;
}

// Dungeon events implementation

inline int TileIndex(int *tile, int x, int y) { return tile[(x % 3) + (y * 3)]; }

int ZeroCross(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {
  if (draw_pile_count < 1) {
    printf("Not enough draw piles. Needs: 1\n");
    return 0;
  }

  const int tile_count = 5;

  int *tiles[tile_count];
  for (int i = 0; i < tile_count; i++) {
    tiles[i] = DrawPoolDrawCard(draw_piles[0], pack);
  }

  // Vertical check
  for (int i = 0; i < tile_count; i += 2) {
    for (int j = 0; j < 3; j++) {
      if (TileIndex(tiles[i], 1, j)) {
        return false;
      }
    }
  }

  // Horizontal check
  for (int i = 1; i < tile_count - 1; i++) {
    for (int j = 0; j < 3; j++) {
      if (TileIndex(tiles[i], j, 1)) {
        return false;
      }
    }
  }

  return true;
}

// Stub to copy the signature around
/*int Event(CardPack *pack, DrawPool **draw_piles, int draw_pile_count) {}*/

int (*const events[])(CardPack *, DrawPool **, int) = {
    // Playing card events
    &RoyalFlush,
    &FourOfAKind,
    &FourSuitsOrFacePair,
    &Switch30,
    // Pokemon events
    &EachPlayerDrawsStage,
    &NoEffectAttack,
    &P1ChainOrZeroButP2No,
    &WaterOnlyPokemonHundred,
    // Dungeon events (The nice ones)
    &ZeroCross,
};

// TODO: Get rid of this and get it into the EventDetails struct when we can finally read it off somewhere
int const event_iterations[] = {
    12500000,
    12500000,
    12500000,
    12500000,
    12500000,
    12500000,
    12500000,
    12500000,
    12500000,
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

// Playing card attributes
enum PC52_ATT { PC52_value, PC52_color, PC52_suit, PC52_face };

// Pokemon Formulas
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
