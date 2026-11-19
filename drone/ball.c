#include <SDL2/SDL.h>

int main(void)
{
    const int width = 800;
    const int height = 500;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Arrows: speed and size | SPACE: pause",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS
    );

    if (window == NULL) {
        SDL_Log("SDL_CreateWindow: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_SOFTWARE
        );
    }

    if (renderer == NULL) {
        SDL_Log("SDL_CreateRenderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    float x = width / 2.0f;
    float y = height / 2.0f;
    float speed = 200.0f;
    float radius = 25.0f;

    // Направление под углом 30° вверх-вправо.
    // На экране ось Y направлена вниз, поэтому dirY отрицательный.
    float dirX = 0.8660254f;
    float dirY = -0.5f;

    int paused = 0;
    int running = 1;

    Uint64 lastTime = SDL_GetPerformanceCounter();
    double frequency = (double)SDL_GetPerformanceFrequency();

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN &&
                       event.key.keysym.sym == SDLK_SPACE &&
                       event.key.repeat == 0) {
                paused = !paused;
            }
        }

        Uint64 now = SDL_GetPerformanceCounter();
        float dt = (float)((now - lastTime) / frequency);
        lastTime = now;

        if (dt > 0.05f) {
            dt = 0.05f;
        }

        const Uint8 *keys = SDL_GetKeyboardState(NULL);

        // Влево/вправо — скорость
        if (keys[SDL_SCANCODE_RIGHT]) {
            speed += 300.0f * dt;
        }
        if (keys[SDL_SCANCODE_LEFT]) {
            speed -= 300.0f * dt;
        }

        // Вверх/вниз — размер шарика
        if (keys[SDL_SCANCODE_UP]) {
            radius += 60.0f * dt;
        }
        if (keys[SDL_SCANCODE_DOWN]) {
            radius -= 60.0f * dt;
        }

        if (speed < 0.0f) speed = 0.0f;
        if (speed > 800.0f) speed = 800.0f;
        if (radius < 5.0f) radius = 5.0f;
        if (radius > 100.0f) radius = 100.0f;

        if (!paused) {
            x += speed * dirX * dt;
            y += speed * dirY * dt;

            // Отскок от левого и правого краёв
            if (x + radius >= width) {
                x = width - radius;
                dirX = -dirX;
            } else if (x - radius <= 0) {
                x = radius;
                dirX = -dirX;
            }

            // Отскок от верхнего и нижнего краёв
            if (y + radius >= height) {
                y = height - radius;
                dirY = -dirY;
            } else if (y - radius <= 0) {
                y = radius;
                dirY = -dirY;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        int r = (int)radius;
        int centerX = (int)x;
        int centerY = (int)y;

        for (int dy = -r; dy <= r; dy++) {
            int dx = 0;

            while ((dx + 1) * (dx + 1) + dy * dy <= r * r) {
                dx++;
            }

            SDL_RenderDrawLine(
                renderer,
                centerX - dx, centerY + dy,
                centerX + dx, centerY + dy
            );
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
