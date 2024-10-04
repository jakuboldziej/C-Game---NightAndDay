#include "game.h"
#include "gameobject.h"
#include "player.h"

Player *player;
// GameObject *player;

SDL_Renderer *Game::renderer = nullptr;

Game::Game() {};
Game::~Game() {};

SDL_Rect startRect;
SDL_Rect quitRect;

void Game::init(const char *title, int xpos, int ypos, bool fullscreen)
{
  int flags = 0;
  if (fullscreen)
  {
    flags = SDL_WINDOW_FULLSCREEN;
    windowWidth = 1920;
    windowHeight = 1080;
  }

  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
  {
    std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
    SDL_Quit();
  }

  if (TTF_Init() == -1)
  {
    std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    SDL_Quit();
  }

  window = SDL_CreateWindow(title, xpos, ypos, windowWidth, windowHeight, flags);
  if (window)
  {
    std::cout << "Window created!" << std::endl;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer)
  {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    std::cout << "Renderer created!" << std::endl;
  }

  player = new Player("E:/programming/C++/SDL2/NightAndDay/assets/Samurai/Samurai_Spritelist.png", 0, 720 - 128, true, 6);
  // player = new GameObject("E:/programming/C++/SDL2/NightAndDay/assets/Samurai/Samurai_Spritelist.png", 0, 720 - 128, true, 6);

  isRunning = true;
  gameState = MENU;
}

void Game::handleEvents()
{
  SDL_Point mousePosition;

  SDL_Event event;
  SDL_PollEvent(&event);
  if (gameState == MENU)
  {
    if (event.type == SDL_KEYDOWN)
    {
      switch (event.key.keysym.sym)
      {
      case SDLK_RETURN:
        gameState = PLAY;
        break;
      case SDLK_ESCAPE:
        isRunning = false;
        break;
      default:
        break;
      }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
      SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
      if (SDL_PointInRect(&mousePosition, &startRect))
      {
        gameState = PLAY;
      }
      else if (SDL_PointInRect(&mousePosition, &quitRect))
      {
        isRunning = false;
      }
    }
  }
  else if (gameState == PLAY)
  {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
    {
      gameState = PAUSE;
    }
  }
  else if (gameState == PAUSE)
  {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
    {
      gameState = PLAY;
    }
  }

  if (event.type == SDL_QUIT)
  {
    isRunning = false;
  }
}

void Game::update()
{
  if (gameState == PLAY)
  {
    const Uint8 *state = SDL_GetKeyboardState(nullptr);

    if (state[SDL_SCANCODE_W])
    {
      // jump
      player->move(0, -1);
    }
    else if (state[SDL_SCANCODE_A])
    {
      player->move(-1, 0);
      player->play("Run");
      player->flip = SDL_FLIP_HORIZONTAL;
    }
    else if (state[SDL_SCANCODE_D])
    {
      player->move(1, 0);
      player->play("Run");
      player->flip = SDL_FLIP_NONE;
    }
    else
      player->play("Idle");

    player->update();
    // player->printInfo("Player");
  }
}

void Game::render()
{
  SDL_RenderClear(renderer);

  if (gameState == MENU)
  {
    int paddingW = 300;
    int paddingH = 50;

    TTF_Font *font = TTF_OpenFont("C:/Windows/Fonts/VGASYSE.FON", 72);
    SDL_Color white = {255, 255, 255};

    // Menu Title
    SDL_Surface *titleSurface = TTF_RenderText_Solid(font, "MAIN MENU", white);
    SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {(windowWidth - titleSurface->w - paddingW) / 2, 100, titleSurface->w + paddingW, titleSurface->h + paddingH};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Start Option
    SDL_Surface *startSurface = TTF_RenderText_Solid(font, "Start", white);
    SDL_Texture *startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
    startRect = {(windowWidth - startSurface->w - paddingW + 200) / 2, 300, startSurface->w + paddingW - 200, startSurface->h + paddingH};
    SDL_RenderCopy(renderer, startTexture, NULL, &startRect);

    // Quit Option
    SDL_Surface *quitSurface = TTF_RenderText_Solid(font, "Quit", white);
    SDL_Texture *quitTexture = SDL_CreateTextureFromSurface(renderer, quitSurface);
    quitRect = {(windowWidth - quitSurface->w - paddingW + 200) / 2, 500, quitSurface->w + paddingW - 200, quitSurface->h + paddingH};
    SDL_RenderCopy(renderer, quitTexture, NULL, &quitRect);

    SDL_FreeSurface(titleSurface);
    SDL_FreeSurface(startSurface);
    SDL_FreeSurface(quitSurface);
    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(startTexture);
    SDL_DestroyTexture(quitTexture);
    TTF_CloseFont(font);
  }
  else if (gameState == PLAY)
  {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    player->render();
  }
  else if (gameState == PAUSE)
  {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  }

  SDL_RenderPresent(renderer);
}

void Game::clean()
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
}