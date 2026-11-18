#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_W 1920
#define WINDOW_H 1080
#define RADIUS 18
#define SPEED 220.0f  // пикселей в секунду

static float random_angle(void) {
    return (float)rand() / (float)RAND_MAX * 6.28318530718f;
}

static void draw_ball(SDL_Renderer *renderer, int x, int y) {
    for (int dy = -RADIUS; dy <= RADIUS; dy++) {
        for (int dx = -RADIUS; dx <= RADIUS; dx++) {
            if (dx * dx + dy * dy <= RADIUS * RADIUS) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

int main(void) {
    srand((unsigned)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Ошибка SDL_Init: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Случайно движущийся шарик",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN
    );
    if (!window) {
        SDL_Log("Ошибка создания окна: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED
    );
    if (!renderer) {
        SDL_Log("Ошибка создания рендерера: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    float x = WINDOW_W / 2.0f;
    float y = WINDOW_H / 2.0f;
    float angle = random_angle();
    float vx = SPEED * cosf(angle);
    float vy = SPEED * sinf(angle);

    Uint32 previous_time = SDL_GetTicks();
    Uint32 last_turn_time = previous_time;
    int running = 1;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN &&
                 event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            }
        }

        Uint32 current_time = SDL_GetTicks();
        float dt = (current_time - previous_time) / 1000.0f;
        previous_time = current_time;

        // Ограничиваем шаг, чтобы шарик не «перепрыгивал» через окно.
        if (dt > 0.05f) dt = 0.05f;

        // Периодически выбираем новое случайное направление.
        if (current_time - last_turn_time >= 1000) {
            angle = random_angle();
            vx = SPEED * cosf(angle);
            vy = SPEED * sinf(angle);
            last_turn_time = current_time;
        }

        x += vx * dt;
        y += vy * dt;

        // Отскок от границ окна.
        if (x < RADIUS) {
            x = RADIUS;
            vx = fabsf(vx);
        } else if (x > WINDOW_W - RADIUS) {
            x = WINDOW_W - RADIUS;
            vx = -fabsf(vx);
        }

        if (y < RADIUS) {
            y = RADIUS;
            vy = fabsf(vy);
        } else if (y > WINDOW_H - RADIUS) {
            y = WINDOW_H - RADIUS;
            vy = -fabsf(vy);
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 180, 40, 255);
        draw_ball(renderer, (int)x, (int)y);

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
