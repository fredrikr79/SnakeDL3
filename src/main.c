#define SDL_MAIN_USE_CALLBACKS 1
#include "utils/arena.h"
#include "utils/vector.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define STEP_RATE_MS 500

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define GRID_PADDING 4
#define GRID_WIDTH 18
#define GRID_HEIGHT 15
#define TILE_WIDTH 28
#define TILE_HEIGHT 24

enum GRID_TILES { EMPTY, SNAKE, FRUIT };

#define COLOR_SNAKE 80, 250, 123, SDL_ALPHA_OPAQUE
#define COLOR_FRUIT 255, 85, 85, SDL_ALPHA_OPAQUE
#define COLOR_EMPTY 40, 42, 54, SDL_ALPHA_OPAQUE
#define COLOR_BG 68, 71, 90, SDL_ALPHA_OPAQUE

typedef enum DIRECTION { LEFT, RIGHT, UP, DOWN } DIRECTION;

typedef struct Snake {
  DIRECTION direction;
  int length;
  Vector body;
} Snake;

typedef struct State {
  Snake player;
  Vector fruits;
  Uint64 last_frame;
} State;

static Arena arena;

#define INDEX_AT(row, col) ((row) * GRID_WIDTH + (col))
#define ROW_AT(index) ((int)((index) / GRID_WIDTH))
#define COL_AT(index) ((index) % GRID_WIDTH)
#define SET_GRID_AT(grid, row, col, value) (grid)[INDEX_AT((row), (col))] = (value)
#define GET_GRID_AT(grid, row, col) (grid)[INDEX_AT((row), (col))]

void log_game_grid(Uint8 *game_grid) {
  char buf[GRID_WIDTH * 2];
  for (int i = 0; i < GRID_HEIGHT; i++) {
    for (int j = 0; j < GRID_WIDTH; j++) {
      buf[2 * j] = '0' + GET_GRID_AT(game_grid, i, j);
      buf[2 * j + 1] = ' ';
    }
    buf[2 * GRID_WIDTH - 1] = '\0';
    SDL_Log("%s", buf);
  }
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("SnakeDL3", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window,
                                   &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  arena_init(&arena, KB(42));
  State *state = arena_alloc(&arena, 1, sizeof(State), _Alignof(State));
  if (!state) {
    SDL_Log("Failed to allocate state");
    return SDL_APP_FAILURE;
  }

  Vector body;
  vector_new(&body);
  int i = INDEX_AT(GRID_WIDTH / 2, GRID_HEIGHT / 2);
  vector_pusha(&body, (int[]){i, i + 1, i + 2}, 3);
  vector_print(body);
  state->player = (Snake){.direction = UP, .length = 3, .body = body};

  state->last_frame = SDL_GetTicks();
  *appstate = state;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  State *stateptr = (State *)appstate;
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_KEY_UP:
    // VIRTUAL LAYOUT KEYS
    switch (event->key.key) {
    case SDLK_Q:
      return SDL_APP_SUCCESS;
    case SDLK_L:
      stateptr->player.direction = RIGHT;
      break;
    case SDLK_K:
      stateptr->player.direction = UP;
      break;
    case SDLK_H:
      stateptr->player.direction = LEFT;
      break;
    case SDLK_J:
      stateptr->player.direction = DOWN;
      break;
    }
    // PHYSICAL LAYOUT KEYS
    switch (event->key.scancode) {
    case SDL_SCANCODE_ESCAPE:
      return SDL_APP_SUCCESS;
    case SDL_SCANCODE_RIGHT:
    case SDL_SCANCODE_D:
      stateptr->player.direction = RIGHT;
      break;
    case SDL_SCANCODE_UP:
    case SDL_SCANCODE_W:
      stateptr->player.direction = UP;
      break;
    case SDL_SCANCODE_LEFT:
    case SDL_SCANCODE_A:
      stateptr->player.direction = LEFT;
      break;
    case SDL_SCANCODE_DOWN:
    case SDL_SCANCODE_S:
      stateptr->player.direction = DOWN;
      break;
    default:
      break;
    }
  }
  return SDL_APP_CONTINUE;
}

void game_step(State *stateptr) {
  DIRECTION dir = stateptr->player.direction;
  for (int i = 0; i < stateptr->player.length - 1; i++) {
    int cur = stateptr->player.body.data[i];
    switch (dir) {
    case RIGHT:
      stateptr->player.body.data[i] = cur + 1;
      break;
    case UP:
      stateptr->player.body.data[i] = cur - GRID_WIDTH;
      break;
    case LEFT:
      stateptr->player.body.data[i] = cur - 1;
      break;
    case DOWN:
      stateptr->player.body.data[i] = cur + GRID_WIDTH;
      break;
    }

    int next = stateptr->player.body.data[i + 1];
    int delta = next - cur;
    switch (delta) {
    case 1:
      dir = LEFT;
      break;
    case -1:
      dir = RIGHT;
      break;
    case GRID_WIDTH:
      dir = UP;
      break;
    case -GRID_WIDTH:
      dir = DOWN;
      break;
    default:
      SDL_Log("game_step: shouldn't be here");
      break;
    }
  }

  int i = stateptr->player.length - 1;
  int cur = stateptr->player.body.data[i];
  switch (dir) {
  case RIGHT:
    stateptr->player.body.data[i] = cur + 1;
    break;
  case UP:
    stateptr->player.body.data[i] = cur - GRID_WIDTH;
    break;
  case LEFT:
    stateptr->player.body.data[i] = cur - 1;
    break;
  case DOWN:
    stateptr->player.body.data[i] = cur + GRID_WIDTH;
    break;
  }
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  State *stateptr = appstate;

  const Uint64 now = SDL_GetTicks();

  // step multiple times if we are behind
  while (now - stateptr->last_frame >= STEP_RATE_MS) {
    game_step(stateptr);
    stateptr->last_frame += STEP_RATE_MS;
  }

  const int startx = (WINDOW_WIDTH - GRID_WIDTH * (TILE_WIDTH + GRID_PADDING)) / 2;
  const int starty = (WINDOW_HEIGHT - GRID_HEIGHT * (TILE_HEIGHT + GRID_PADDING)) / 2;

  SDL_FRect rect;
  rect.w = TILE_WIDTH;
  rect.h = TILE_HEIGHT;

  SDL_SetRenderDrawColor(renderer, COLOR_BG);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, COLOR_EMPTY);
  for (int i = 0; i < GRID_HEIGHT; i++) {
    for (int j = 0; j < GRID_WIDTH; j++) {
      rect.x = startx + j * (rect.w + GRID_PADDING);
      rect.y = starty + i * (rect.h + GRID_PADDING);
      SDL_RenderFillRect(renderer, &rect);
    }
  }

  SDL_SetRenderDrawColor(renderer, COLOR_SNAKE);
  for (int i = 0; i < stateptr->player.length; i++) {
    int b = stateptr->player.body.data[i];
    rect.x = startx + COL_AT(b) * (rect.w + GRID_PADDING);
    rect.y = starty + ROW_AT(b) * (rect.h + GRID_PADDING);
    SDL_RenderFillRect(renderer, &rect);
  }

  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  /* SDL will clean up the window/renderer for us. */
  vector_free(&((State *)appstate)->player.body);
  arena_free(&arena);
}
