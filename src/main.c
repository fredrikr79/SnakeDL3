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

void log_game_grid(int *game_grid) {
    char *buf = SDL_malloc(sizeof(char) * grid_width * 2 + 1);
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
            game_grid[i * grid_width + j] = EMPTY;
        }
    }
    game_grid[16 * grid_width + 12] = SNAKE;
    game_grid[12 * grid_width + 4] = FRUIT;
    game_grid[5 * grid_width + 18] = FRUIT;

    *appstate = game_grid;
    log_game_grid(*appstate);

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
    const double now = ((double)SDL_GetTicks()) / 1000.0;
    const float red = (float)(0.5 + 0.5 * SDL_sin(now));
    const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
    const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

    SDL_SetRenderDrawColorFloat(renderer, red, green, blue,
                                SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /* SDL will clean up the window/renderer for us. */
}
