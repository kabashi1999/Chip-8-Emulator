#include <SDL2/SDL.h>

class platform
{
private:
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    SDL_Texture *texture{};

public:
    platform(char *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~platform();

    void Update(void const *buffer, int pitch);
    bool ProcessInput(uint8_t *keys);
};
