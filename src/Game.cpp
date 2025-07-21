#include "Game.h"
#include <SDL.h>
#include "SceneMain.h"
#include "SceneTitle.h"

#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <fstream>

void Game::backgroundUpdate(float deltaTime)
{
    nearStars.offset += nearStars.speed * deltaTime;
    if (nearStars.offset >= 0)
    {
        nearStars.offset -= nearStars.height;
    }
    farStars.offset += farStars.speed * deltaTime;
    if (farStars.offset >= 0)
    {
        farStars.offset -= farStars.height;
    }
}

void Game::renderBackground()
{
    for (int posY = static_cast<int>(farStars.offset);
         posY < getWindowHeight(); posY += farStars.height)
    {
        for (int posX = 0; posX < getWindowWidth(); posX += farStars.width)
        {
            SDL_Rect dstRect = {posX, posY, farStars.width, farStars.height};
            SDL_RenderCopy(renderer, farStars.texture, nullptr, &dstRect);
        }
    }

    for (int posY = static_cast<int>(nearStars.offset);
         posY < getWindowHeight(); posY += nearStars.height)
    {
        for (int posX = 0; posX < getWindowWidth(); posX += nearStars.width)
        {
            SDL_Rect dstRect = {posX, posY, nearStars.width, nearStars.height};
            SDL_RenderCopy(renderer, nearStars.texture, nullptr, &dstRect);
        }
    }
}

void Game::saveData()
{
    // 保存得分榜
    std::ofstream file("assets/save.dat");
    if (!file.is_open())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open save file");
        return;
    }
    for (const auto &i : leaderBoard)
    {
        file << i.first << " " << i.second << std::endl;
    }
    file.close();
}

void Game::loadData()
{
    // 加载文档
    std::ifstream file("assets/save.dat");
    if (!file.is_open())
    {
        SDL_Log("Failed to open save file");
        return;
    }
    leaderBoard.clear();
    int score;
    std::string name;
    while (file >> score >> name)
    {
        leaderBoard.insert({score, name});
    }
    file.close();
}

void Game::insertLeaderBoard(int score, std::string name)
{
    leaderBoard.insert({score, name});
    while (leaderBoard.size() > 8)
    {
        leaderBoard.erase(--leaderBoard.end());
    }
}

void Game::renderTextPox(std::string text, int posX, int posY, bool isLeft)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect;
    if (isLeft)
        dstRect = {posX, posY, surface->w, surface->h};
    else
        dstRect = {getWindowWidth() - surface->w - posX, posY, surface->w, surface->h};

    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

SDL_Point Game::renderTextCentered(std::string text, float posY, bool isTitle)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface;
    if (isTitle)
    {
        surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), color);
    }
    else
    {
        surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = {getWindowWidth() / 2 - surface->w / 2,
                        static_cast<int>(posY * (getWindowHeight() - surface->h)),
                        surface->w,
                        surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    return {dstRect.x + dstRect.w, dstRect.y};
}

Game::Game()
{
}

Game::~Game()
{
    saveData();
    clean();
}

void Game::run()
{
    while (isrunning)
    {
        auto frameStart = SDL_GetTicks();

        SDL_Event event;
        handleEvents(&event);
        update(deltaTime);
        render();
        auto frameEnd = SDL_GetTicks();
        auto diff = frameEnd - frameStart;
        if (diff < frameTime)
        {
            SDL_Delay(frameTime - diff);
            deltaTime = frameTime / 1000.0f;
        }
        else
        {
            deltaTime = diff / 1000.0f;
        }
    }
}

void Game::init()
{
    frameTime = 1000 / FPS;
    // SDL2初始化
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isrunning = false;
    }

    // 创建窗口
    window = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        isrunning = false;
    }

    // 创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        isrunning = false;
    }

    //设置逻辑分辨率
    SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);

    // 初始化SDL_img
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != (IMG_INIT_PNG | IMG_INIT_JPG))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_img could not initialize! SDL_Error: %s\n", SDL_GetError());
        isrunning = false;
    }

    // 初始化SDL_mixer
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not initialize! SDL_Error: %s\n", SDL_GetError());
        isrunning = false;
    }
    // 打开音频设备
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not open audio device! SDL_Error: %s\n", SDL_GetError());
        isrunning = false;
    }
    // 设置音效数量
    Mix_AllocateChannels(32);

    // 设置音量
    Mix_VolumeMusic(MIX_MAX_VOLUME / 20);
    Mix_Volume(-1, MIX_MAX_VOLUME / 10);

    // 字体初始化
    if (TTF_Init() == -1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_ttf could not initialize! SDL_Error: %s\n", SDL_GetError());
        isrunning = false;
    }

    // 初始化背景
    nearStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-A.png");
    if (nearStars.texture == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_img could not initialize! SDL_Error: %s\n", SDL_GetError());
        isrunning = false;
    }
    SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width, &nearStars.height);

    nearStars.width /= 2;
    nearStars.height /= 2;

    farStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-B.png");
    if (farStars.texture == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_img could not initialize! SDL_Error: %s\n", SDL_GetError());
        isrunning = false;
    }
    SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width, &farStars.height);
    farStars.speed = 10;

    farStars.width /= 2;
    farStars.height /= 2;

    titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);

    if (titleFont == nullptr || textFont == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "首页TTF could not initialize! SDL_Error: %s\n", SDL_GetError());
        isrunning = false;
    }

    loadData();

    currentScene = new SceneTitle();
    currentScene->init();
}

void Game::clean()
{
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }
    if (nearStars.texture != nullptr)
    {
        SDL_DestroyTexture(nearStars.texture);
    }
    if (farStars.texture != nullptr)
    {
        SDL_DestroyTexture(farStars.texture);
    }
    if (titleFont != nullptr)
    {
        TTF_CloseFont(titleFont);
    }
    if (textFont != nullptr)
    {
        TTF_CloseFont(textFont);
    }

    // 清理SDL_img
    IMG_Quit();

    // 清理SDL_mixer
    Mix_CloseAudio();
    Mix_Quit();

    // 清理SDL_ttf
    TTF_Quit();

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

void Game::changeScene(Scene *scene)
{
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }
    currentScene = scene;
    currentScene->init();
}

void Game::handleEvents(SDL_Event *event)
{
    // 修改为处理所有事件
    while (SDL_PollEvent(event))
    {
        if (event->type == SDL_QUIT)
        {
            isrunning = false;
        }
        if (event->type == SDL_KEYDOWN)
        {
            if (event->key.keysym.scancode == SDL_SCANCODE_F4)
            {
                isFullscreen = !isFullscreen;
                if (isFullscreen)
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                else
                    SDL_SetWindowFullscreen(window, 0);
            }
        }
        currentScene->handleEvent(event);
    }
}

void Game::update(float deltaTime)
{
    backgroundUpdate(deltaTime);
    currentScene->update(deltaTime);
}

void Game::render()
{
    // 清空
    SDL_RenderClear(renderer);
    // 渲染星空背景
    renderBackground();

    // 渲染当前场景
    currentScene->render();

    // 显示更新
    SDL_RenderPresent(renderer);
}