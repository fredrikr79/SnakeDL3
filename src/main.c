#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static int width = 640;
static int height = 480;
static int grid_width = 32;
static int grid_height = 24;

enum TILES { EMPTY, SNAKE, FRUIT };

typedef enum DIRECTION { LEFT, RIGHT, UP, DOWN } DIRECTION;

typedef struct State {
    int *grid;
    DIRECTION player_face;
} State;

void log_game_grid(int *game_grid) {
    char *buf = SDL_malloc(sizeof(char) * grid_width * 2);
    for (int i = 0; i < grid_height; i++) {
        for (int j = 0; j < grid_width; j++) {
            buf[2 * j] = '0' + game_grid[i * grid_width + j];
            buf[2 * j + 1] = ' ';
        }
        buf[2 * grid_width] = '\0';
        SDL_Log("%s", buf);
    }
    SDL_free(buf);
}

void set_grid_at(int *grid, int row, int col, int value) {
    grid[row * grid_width + col] = value;
}

int get_grid_at(int *grid, int row, int col) {
    return grid[row * grid_width + col];
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", width, height,
                                     0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    int *game_grid = SDL_malloc(sizeof(int) * grid_width * grid_height);
    for (int i = 0; i < grid_height; i++) {
        for (int j = 0; j < grid_width; j++) {
            set_grid_at(game_grid, i, j, EMPTY);
        }
    }

    set_grid_at(game_grid, 12, 16, SNAKE);
    set_grid_at(game_grid, 4, 12, FRUIT);
    set_grid_at(game_grid, 18, 5, FRUIT);

    State *state = SDL_malloc(sizeof(State));
    state->grid = game_grid;
    state->player_face = UP;
    *appstate = state;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if ((event->type == SDL_EVENT_KEY_UP &&
         event->key.scancode == SDL_SCANCODE_ESCAPE) ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    State *stateptr = appstate;
    int *game_grid = stateptr->grid;

    SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(renderer);

    SDL_FRect rect;
    rect.w = (float)width / grid_width;
    rect.h = (float)height / grid_height;
    for (int i = 0; i < grid_height; i++) {
        for (int j = 0; j < grid_width; j++) {
            switch (get_grid_at(game_grid, i, j)) {
            case EMPTY:
                SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0,
                                            SDL_ALPHA_OPAQUE_FLOAT);
                break;
            case SNAKE:
                SDL_SetRenderDrawColorFloat(renderer, 255, 255, 255,
                                            SDL_ALPHA_OPAQUE_FLOAT);
                break;
            case FRUIT:
                SDL_SetRenderDrawColorFloat(renderer, 255, 0, 0,
                                            SDL_ALPHA_OPAQUE_FLOAT);
                break;
            }
            rect.x = j * rect.w;
            rect.y = i * rect.h;
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /* SDL will clean up the window/renderer for us. */
}
