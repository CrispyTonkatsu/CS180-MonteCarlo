#include <stdio.h> /* fodetails->thread_count, fscanf, fclose */
#include <stdlib.h> /* exit, malloc          */

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "mc-head.h"

void GetSimSetup(const char *filename, char filepaths[][128], int *numFiles, int *numThreads, int *eventNumber) {
  FILE *fp = NULL;

  // Open the file in text/translated mode
  fp = fopen(filename, "rt");
  if (!fp) {
    printf("Can't open file: %s\n", filename);
    exit(-1);
  }

  int total_read = 0;
  total_read += fscanf(fp, "%d", eventNumber);
  total_read += fscanf(fp, "%d", numFiles);
  total_read += fscanf(fp, "%d", numThreads);
  for (int i = 0; i < *numFiles; ++i) {
    total_read += fscanf(fp, "%s", filepaths[i]);
  }

  if (total_read < 3 + *numFiles) {
    printf("Read less than 3 numbers");
  }

  fclose(fp);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Insufficient parameters supplied\n");
    return -1;
  }

  char filepaths[10][128];

  int numFiles = -1;
  int numThreads = -1;
  int eventNumber = -1;

  GetSimSetup(argv[1], filepaths, &numFiles, &numThreads, &eventNumber);

  // Setup for the threads
  EventDetails *event_details = EventDetailsCreate(filepaths[0], &filepaths[1], numFiles - 1, eventNumber);
  if (event_details == NULL) {
    printf("Failed to get all required data.\nRead above messages to see why.\n");
    return -1;
  }
  pthread_t *threads = calloc(numThreads, sizeof(pthread_t));

  // Code to create and give the pointer to the threads
  for (int i = 0; i < numThreads; i++) {
    pthread_create(&threads[i], NULL, &EventRun, event_details);
  }

  // Wait for child threads using pthread_join
  int success_count = 0;
  for (int i = 0; i < numThreads; i++) {
    int *a = NULL;
    pthread_join(threads[i], (void *) &a);
    success_count += *a;
    free(a);
  }

  float successes = (float) success_count;
  float total_iter = 2500000.f; // NOLINT *magic*
  float total_threads = (float) numThreads;
  printf("Output of simulation: %f", (successes / (total_iter * total_threads)));

  // Cleanup
  free(threads);
  EventDetailsDelete(event_details);

  return 0;
}

// EventDetails Implementation

EventDetails *EventDetailsCreate(char *card_file, char (*draw_pool_files)[128], int draw_pool_count, int event_number) {
  CardPack *cards = CardPackCreate(card_file);
  if (cards == NULL) {
    printf("Failed to create the card database.\n");
    return NULL;
  }

  EventDetails *event_details = calloc(1, sizeof(EventDetails));
  // Non-allocated memory
  event_details->event_number = event_number;
  // Allocated memory
  event_details->pack = cards;
  event_details->draw_piles = calloc(draw_pool_count, sizeof(DrawPool *));
  event_details->draw_pile_count = draw_pool_count;

  DrawPool **hands = event_details->draw_piles;

  for (int i = 0; i < draw_pool_count; i++) {
    hands[i] = DrawPoolCreate(draw_pool_files[i]);
    if (hands[i] == NULL) {
      printf("Failed to read draw pool %d count.\n", i);
      EventDetailsDelete(event_details);
      return NULL;
    }
  }

  return event_details;
}

DrawPool **EventDetailsGetShuffledCards(EventDetails *details, randData *rng_machine) {
  DrawPool **output = calloc(details->draw_pile_count, sizeof(DrawPool *));

  for (int i = 0; i < details->draw_pile_count; i++) {
    output[i] = DrawPoolClone(details->draw_piles[i]);
    DrawPoolShuffle(output[i], rng_machine);
  }

  return output;
}

void EventDetailsDelete(EventDetails *self) {
  if (self != NULL) {
    CardPackDelete(self->pack);
    if (self->draw_piles != NULL) {
      for (int i = 0; i < self->draw_pile_count; i++) {
        DrawPoolDelete(self->draw_piles[i]);
      }
      free(self->draw_piles);
    }
    free(self);
  }
}

// CardPack implementation

bool CardPackReadDimensions(FILE *card_file, int *card_count, int *field_count) {
  int read_count = fscanf(card_file, "%d", card_count);
  if (read_count <= 0) {
    printf("Failed to read card count.\n");
    return false;
  }

  read_count = fscanf(card_file, "%d", field_count);
  if (read_count <= 0) {
    printf("Failed to read card count.\n");
    return false;
  }

  return true;
}

bool CardPackReadCards(CardPack *pack, FILE *card_file) {
  int current_value = 0;

  while (!feof(card_file)) {
    int read_count = fscanf(card_file, "%d", &pack->card_data[current_value]);
    if (read_count < 0) {
      printf("Failed to read card data (%d, %d).\n", current_value / pack->field_count, current_value);
    }

    current_value++;
    int card_read_count = current_value / pack->field_count;
    if (card_read_count == pack->card_count) {
      break;
    }
  }

  int total_cards = current_value / pack->field_count;
  if (total_cards < pack->card_count) {
    printf("There were cards missing\n");
    return false;
  }

  return true;
}

CardPack *CardPackCreate(char *card_file_path) {
  if (card_file_path == NULL) {
    printf("Null card file path.\n");
    return NULL;
  }

  int change_dir = chdir("./decks/");
  if (change_dir == -1) {
    printf("Null card folder path.\n");
    return NULL;
  }

  FILE *card_file = fopen(card_file_path, "r");
  if (card_file == NULL) {
    printf("Could not open the card file.\n");
    return NULL;
  }

  int card_count = 0;
  int field_count = 0;
  if (!CardPackReadDimensions(card_file, &card_count, &field_count)) {
    printf("Failed to read card pack dimensions\n");
    fclose(card_file);
    return NULL;
  }

  CardPack *pack = calloc(1, sizeof(CardPack));
  pack->card_count = card_count;
  pack->field_count = field_count;
  pack->card_data = calloc((card_count * field_count), sizeof(int));

  if (!CardPackReadCards(pack, card_file)) {
    printf("Failed to read the card list\n");
    CardPackDelete(pack);
    fclose(card_file);
    return NULL;
  }

  change_dir = chdir("../");
  fclose(card_file);
  return pack;
}

int CardPackIndex(CardPack *pack, int card_id, int card_field) {
  return pack->card_data[(card_id * pack->field_count) + (card_field % pack->field_count)];
}

int *CardPackGetCard(CardPack *pack, int card_id) { return &(pack->card_data[card_id * pack->field_count]); }

void CardPackDelete(CardPack *pack) {
  if (pack) {
    if (pack->card_data) {
      free(pack->card_data);
    }
    free(pack);
  }
}

// DrawPool Implementation

DrawPool *DrawPoolCreate(const char *hand_file_path) {
  if (hand_file_path == NULL) {
    printf("Null hand file path.\n");
    return NULL;
  }

  int change_dir = chdir("./decks/");
  if (change_dir == -1) {
    printf("Null card folder path.\n");
    return NULL;
  }

  FILE *hand_file = fopen(hand_file_path, "r");
  if (hand_file == NULL) {
    printf("Could not open the card file.\n");
    return NULL;
  }

  int hand_size = 0;

  int read_count = fscanf(hand_file, "%d", &hand_size);
  if (read_count <= 0) {
    printf("Failed to read hand size.\n");
    fclose(hand_file);
    return false;
  }

  int *hand_ids = calloc(hand_size, sizeof(int));
  int current_value = 0;

  while (!feof(hand_file)) {
    int read_count = fscanf(hand_file, "%d", &hand_ids[current_value]);
    if (read_count < 0) {
      printf("Failed to read card data (%d).\n", current_value);
    }

    current_value++;
    if (current_value == hand_size) {
      break;
    }
  }

  if (current_value < hand_size) {
    printf("There were cards missing\n");
    fclose(hand_file);
    free(hand_ids);
    return NULL;
  }

  DrawPool *draw_pool = calloc(1, sizeof(DrawPool));
  draw_pool->card_ids = hand_ids;
  draw_pool->card_count = hand_size;
  draw_pool->top_index = 0;

  change_dir = chdir("../");
  fclose(hand_file);
  return draw_pool;
}

DrawPool *DrawPoolClone(DrawPool *other) {
  if (other == NULL) {
    return NULL;
  }

  DrawPool *output = calloc(1, sizeof(DrawPool));

  int *cards_clone = calloc(other->card_count, sizeof(int));
  memcpy(cards_clone, other->card_ids, sizeof(int) * other->card_count);

  output->card_ids = cards_clone;
  output->card_count = other->card_count;
  output->top_index = 0;

  return output;
}

void DrawPoolShuffle(DrawPool *self, randData *rng_machine) {
  for (int i = 0; i < self->card_count - 1; i++) {
    int swap_position = RandomInt(i, self->card_count - 1, rng_machine);
    int swap_value = self->card_ids[swap_position];

    self->card_ids[swap_position] = self->card_ids[i];
    self->card_ids[i] = swap_value;
  }
  self->top_index = 0;
}

int *DrawPoolDrawCard(DrawPool *self, CardPack *pack) {
  int *output = CardPackGetCard(pack, self->card_ids[self->top_index]);
  self->top_index++;
  return output;
}

void DrawPoolDelete(DrawPool *self) {
  if (self != NULL) {
    if (self->card_ids != NULL) {
      free(self->card_ids);
    }
    free(self);
  }
}
