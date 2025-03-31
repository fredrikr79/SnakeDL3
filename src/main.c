#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define FPS 60

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define GRID_PADDING 2
#define GRID_WIDTH 18
#define GRID_HEIGHT 15
#define TILE_WIDTH 24
#define TILE_HEIGHT 24

enum GRID_TILES { EMPTY, SNAKE, FRUIT };
enum SNAKE_PARTS { TAIL, BODY, HEAD };

#define COLOR_SNAKE 80, 250, 123, SDL_ALPHA_OPAQUE
#define COLOR_FRUIT 255, 85, 85, SDL_ALPHA_OPAQUE
#define COLOR_EMPTY 40, 42, 54, SDL_ALPHA_OPAQUE
#define COLOR_BG 68, 71, 90, SDL_ALPHA_OPAQUE

typedef enum DIRECTION { LEFT, RIGHT, UP, DOWN } DIRECTION;

typedef struct Snake {
  DIRECTION direction;
  Uint8 length;
} Snake;

typedef struct State {
  Uint8 grid[GRID_HEIGHT * GRID_WIDTH];
  Snake *player;
} State;

#define SET_GRID_AT(grid, row, col, value) grid[row * GRID_WIDTH + col] = value
#define GET_GRID_AT(grid, row, col) grid[row * GRID_WIDTH + col]

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

  State *state = SDL_malloc(sizeof(State));
  if (!state) {
    SDL_Log("Failed to allocate state");
    return SDL_APP_FAILURE;
  }
  SDL_memset(state->grid, EMPTY, sizeof(state->grid));

  SET_GRID_AT(state->grid, GRID_WIDTH / 2, GRID_HEIGHT / 2, SNAKE);
  SET_GRID_AT(state->grid, 4, 12, FRUIT);
  SET_GRID_AT(state->grid, 13, 5, FRUIT);

  log_game_grid(state->grid);

  *appstate = state;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if ((event->type == SDL_EVENT_KEY_UP && event->key.scancode == SDL_SCANCODE_ESCAPE) ||
      event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  State *stateptr = appstate;
  Uint8 *game_grid = stateptr->grid;

  SDL_SetRenderDrawColor(renderer, COLOR_BG);
  SDL_RenderClear(renderer);

  SDL_FRect rect;
  rect.w = TILE_WIDTH;
  rect.h = TILE_HEIGHT;
  const int startx = (WINDOW_WIDTH - GRID_WIDTH * (TILE_WIDTH + GRID_PADDING)) / 2;
  const int starty = (WINDOW_HEIGHT - GRID_HEIGHT * (TILE_HEIGHT + GRID_PADDING)) / 2;

  SDL_Log("%d %d\n", startx, starty);
  for (int i = 0; i < GRID_HEIGHT; i++) {
    for (int j = 0; j < GRID_WIDTH; j++) {
      switch (GET_GRID_AT(game_grid, i, j)) {
      case EMPTY:
        SDL_SetRenderDrawColor(renderer, COLOR_EMPTY);
        break;
      case SNAKE:
        SDL_SetRenderDrawColor(renderer, COLOR_SNAKE);
        break;
      case FRUIT:
        SDL_SetRenderDrawColor(renderer, COLOR_FRUIT);
        break;
      }
      rect.x = startx + j * (rect.w + GRID_PADDING);
      rect.y = starty + i * (rect.h + GRID_PADDING);
      // SDL_Log("x: %f, y: %f, w: %f, h: %f", rect.x, rect.y, rect.w, rect.h);
      SDL_RenderFillRect(renderer, &rect);
    }
  }
  // SDL_Log("---");

  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  /* SDL will clean up the window/renderer for us. */
  SDL_free(appstate);
}
