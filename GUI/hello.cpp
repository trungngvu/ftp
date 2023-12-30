#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <functional>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TEXT_WIDTH = 150; // Width of each text item
const int TEXT_HEIGHT = 30; // Height of each text item
const int ITEMS_PER_ROW = SCREEN_WIDTH / TEXT_WIDTH;

bool initSDL(SDL_Window *&window, SDL_Renderer *&renderer);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const string &text, int x, int y);
int getItemIndexAtPosition(int x, int y, int scrollOffsetX, int scrollOffsetY);
vector<string> itemList;

struct ContextMenuOption
{
    const char *label;
    function<void()> callback; // Function to be called when the option is selected
};

int main(int argc, char *argv[])
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    if (!initSDL(window, renderer))
    {
        return -1;
    }

    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return -1;
    }

    TTF_Font *font = TTF_OpenFont("./font/Roboto-Regular.ttf", 24);

    if (!font)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return -1;
    }

    for (int i = 1; i <= 100; ++i)
    {
        itemList.push_back("Item " + to_string(i));
    }

    int scrollOffsetX = 0;
    int scrollOffsetY = 0;
    bool quit = false;
    SDL_Event e;

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_MOUSEWHEEL)
            {
                // Invert the scrolling direction
                if (e.wheel.y > 0)
                {
                    scrollOffsetY -= TEXT_HEIGHT;
                }
                else if (e.wheel.y < 0)
                {
                    scrollOffsetY += TEXT_HEIGHT;
                }

                // Ensure vertical scrolling is within valid range
                if (scrollOffsetY < 0)
                {
                    scrollOffsetY = 0;
                }
                else if (scrollOffsetY > TEXT_HEIGHT * (itemList.size() / ITEMS_PER_ROW - 1))
                {
                    scrollOffsetY = TEXT_HEIGHT * (itemList.size() / ITEMS_PER_ROW - 1);
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    int clickedIndex = getItemIndexAtPosition(e.button.x + scrollOffsetX, e.button.y + scrollOffsetY, scrollOffsetX, scrollOffsetY);

                    if (clickedIndex != -1)
                    {
                        // Handle click on the item with index clickedIndex
                        printf("Clicked on item %d\n", clickedIndex + 1);
                    }
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    // Right mouse button click handling
                    int clickedIndex = getItemIndexAtPosition(e.button.x + scrollOffsetX, e.button.y + scrollOffsetY, scrollOffsetX, scrollOffsetY);

                    if (clickedIndex != -1)
                    {
                        // Handle right-click on the item with index clickedIndex
                        printf("Right-clicked on item %d\n", clickedIndex + 1);
                        // Add your custom right-click handling logic here
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Render the visible portion of the list
        int xPos = 50 - scrollOffsetX;
        int yPos = 50 - scrollOffsetY;

        for (size_t i = 0; i < itemList.size(); ++i)
        {
            if (i % ITEMS_PER_ROW == 0 && i != 0)
            {
                // Move to the next row
                xPos = 50 - scrollOffsetX;
                yPos += TEXT_HEIGHT;
            }

            if (xPos >= 0 && xPos < SCREEN_WIDTH && yPos >= 0 && yPos < SCREEN_HEIGHT)
            {
                renderText(renderer, font, itemList[i], xPos, yPos);
            }

            xPos += TEXT_WIDTH;
        }

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

bool initSDL(SDL_Window *&window, SDL_Renderer *&renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("SDL List Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window)
    {
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void renderText(SDL_Renderer *renderer, TTF_Font *font, const string &text, int x, int y)
{
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);

    if (!textSurface)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (!textTexture)
    {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

int getItemIndexAtPosition(int x, int y, int scrollOffsetX, int scrollOffsetY)
{
    // Adjust the clicked position with scroll offsets
    x += scrollOffsetX;
    y += scrollOffsetY;

    // Check if the click is outside the item area
    if (x < 50 || y < 50)
        return -1;

    // Calculate the row and column of the clicked item
    int col = (x - 50) / TEXT_WIDTH;
    int row = (y - 50) / TEXT_HEIGHT;

    // Calculate the index of the clicked item
    int index = row * ITEMS_PER_ROW + col;

    // Check if the index is within the valid range
    if (index >= 0 && index < static_cast<int>(itemList.size()))
    {
        return index;
    }
    else
    {
        return -1;
    }
}
