#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_W 1920
#define WINDOW_H 1030
#define RADIUS 18

#define SPEED 220.0f       // Начальная скорость, пикселей в секунду
#define SPEED_STEP 30.0f   // Изменение скорости стрелками
#define MIN_SPEED 40.0f
#define MAX_SPEED 600.0f

#define TURN_INTERVAL 10.0f
#define TURN_ANGLE 0.52359878f // 30 градусов в радианах

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
        "------------- BALL ------------",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_W,
        WINDOW_H,
        SDL_WINDOW_SHOWN
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

    float speed = SPEED;
    float angle = random_angle();
    float vx = speed * cosf(angle);
    float vy = speed * sinf(angle);

    float turn_timer = 0.0f;
    int paused = 0;
    int running = 1;

    Uint32 previous_time = SDL_GetTicks();

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;

                if (key == SDLK_ESCAPE && event.key.repeat == 0) {
                    running = 0;
                } else if (key == SDLK_SPACE && event.key.repeat == 0) {
                    paused = !paused;
                } else if (key == SDLK_LEFT || key == SDLK_RIGHT) {
                    float new_speed = speed;

                    if (key == SDLK_RIGHT) {
                        new_speed += SPEED_STEP;
                    } else {
                        new_speed -= SPEED_STEP;
                    }

                    if (new_speed < MIN_SPEED) new_speed = MIN_SPEED;
                    if (new_speed > MAX_SPEED) new_speed = MAX_SPEED;

                    // Меняем скорость, сохраняя текущее направление.
                    float scale = new_speed / speed;
                    vx *= scale;
                    vy *= scale;
                    speed = new_speed;
                }
            }
        }

        Uint32 current_time = SDL_GetTicks();
        float dt = (current_time - previous_time) / 1000.0f;
        previous_time = current_time;

        // Ограничиваем шаг, чтобы шарик не пролетал сквозь границы.
        if (dt > 0.05f) {
            dt = 0.05f;
        }

        if (!paused) {
            turn_timer += dt;

            // Каждые 10 секунд поворачиваем шарик на случайные +30° или -30°.
            if (turn_timer >= TURN_INTERVAL) {
                turn_timer -= TURN_INTERVAL;

                float turn = (rand() % 2 == 0)
                    ? -TURN_ANGLE
                    : TURN_ANGLE;

                float old_vx = vx;
                float old_vy = vy;

                vx = old_vx * cosf(turn) - old_vy * sinf(turn);
                vy = old_vx * sinf(turn) + old_vy * cosf(turn);
            }

            x += vx * dt;
            y += vy * dt;

            // Отражение от границ окна.
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
