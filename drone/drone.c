#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_W 1920
#define WINDOW_H 1020

#define SPEED 220.0f
#define SPEED_STEP 30.0f
#define MIN_SPEED 40.0f
#define MAX_SPEED 600.0f

#define TURN_INTERVAL 10.0f
#define TURN_ANGLE 0.52359878f  // 30 градусов в радианах

#define PI 3.14159265f

static float random_angle(void) {
    return (float)rand() / (float)RAND_MAX * 2.0f * PI;
}

static void transform_point(
    float x, float y,
    float center_x, float center_y,
    float angle,
    SDL_Point *point
) {
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);

    point->x = (int)(center_x + x * cos_angle - y * sin_angle);
    point->y = (int)(center_y + x * sin_angle + y * cos_angle);
}

static void draw_filled_circle(SDL_Renderer *renderer, int cx, int cy, int radius) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
}

static void draw_thick_line(
    SDL_Renderer *renderer,
    int x1, int y1, int x2, int y2,
    int thickness
) {
    int half = thickness / 2;

    if (abs(x2 - x1) >= abs(y2 - y1)) {
        for (int offset = -half; offset <= half; offset++) {
            SDL_RenderDrawLine(renderer, x1, y1 + offset, x2, y2 + offset);
        }
    } else {
        for (int offset = -half; offset <= half; offset++) {
            SDL_RenderDrawLine(renderer, x1 + offset, y1, x2 + offset, y2);
        }
    }
}

static void draw_filled_polygon(
    SDL_Renderer *renderer,
    const SDL_Point *points,
    int count
) {
    int min_y = points[0].y;
    int max_y = points[0].y;

    for (int i = 1; i < count; i++) {
        if (points[i].y < min_y) min_y = points[i].y;
        if (points[i].y > max_y) max_y = points[i].y;
    }

    for (int y = min_y; y <= max_y; y++) {
        float intersections[8];
        int intersection_count = 0;

        for (int i = 0; i < count; i++) {
            SDL_Point a = points[i];
            SDL_Point b = points[(i + 1) % count];

            if ((a.y <= y && b.y > y) || (b.y <= y && a.y > y)) {
                float x = a.x +
                    (float)(y - a.y) * (float)(b.x - a.x) /
                    (float)(b.y - a.y);

                if (intersection_count < 8) {
                    intersections[intersection_count++] = x;
                }
            }
        }

        for (int i = 0; i < intersection_count - 1; i++) {
            for (int j = i + 1; j < intersection_count; j++) {
                if (intersections[j] < intersections[i]) {
                    float temp = intersections[i];
                    intersections[i] = intersections[j];
                    intersections[j] = temp;
                }
            }
        }

        for (int i = 0; i + 1 < intersection_count; i += 2) {
            SDL_RenderDrawLine(
                renderer,
                (int)intersections[i], y,
                (int)intersections[i + 1], y
            );
        }
    }
}

static void draw_drone(
    SDL_Renderer *renderer,
    int center_x, int center_y,
    float angle
) {
    // Четыре ротора расположены по диагоналям.
    const float rotor_positions[4][2] = {
        {-25.0f, -25.0f},
        { 25.0f, -25.0f},
        { 25.0f,  25.0f},
        {-25.0f,  25.0f}
    };

    SDL_Point rotors[4];

    for (int i = 0; i < 4; i++) {
        transform_point(
            rotor_positions[i][0], rotor_positions[i][1],
            (float)center_x, (float)center_y,
            angle, &rotors[i]
        );

        draw_thick_line(
            renderer, center_x, center_y,
            rotors[i].x, rotors[i].y, 6
        );
    }

    // Корпуса роторов.
    for (int i = 0; i < 4; i++) {
        draw_filled_circle(renderer, rotors[i].x, rotors[i].y, 9);
    }

    // Центральный корпус с небольшим носом, направленным вперёд.
    const float body_shape[][2] = {
        { 13.0f,   0.0f},
        {  4.0f,  -8.0f},
        {-10.0f,  -7.0f},
        {-13.0f,   0.0f},
        {-10.0f,   7.0f},
        {  4.0f,   8.0f}
    };

    SDL_Point body[6];

    for (int i = 0; i < 6; i++) {
        transform_point(
            body_shape[i][0], body_shape[i][1],
            (float)center_x, (float)center_y,
            angle, &body[i]
        );
    }

    draw_filled_polygon(renderer, body, 6);
}

int main(void) {
    srand((unsigned)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Ошибка SDL_Init: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "-------------- DRONE ---------------",
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

        if (dt > 0.05f) {
            dt = 0.05f;
        }

        if (!paused) {
            turn_timer += dt;

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

            // Отскок от границ окна с учётом размера силуэта.
            if (x < 30.0f) {
                x = 30.0f;
                vx = fabsf(vx);
            } else if (x > WINDOW_W - 30.0f) {
                x = WINDOW_W - 30.0f;
                vx = -fabsf(vx);
            }

            if (y < 30.0f) {
                y = 30.0f;
                vy = fabsf(vy);
            } else if (y > WINDOW_H - 30.0f) {
                y = WINDOW_H - 30.0f;
                vy = -fabsf(vy);
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 180, 40, 255);
        draw_drone(renderer, (int)x, (int)y, atan2f(vy, vx));

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
