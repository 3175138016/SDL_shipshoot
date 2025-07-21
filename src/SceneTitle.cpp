#include "SceneTitle.h"
#include "Game.h"
#include "SceneMain.h"

#include <string>

void SceneTitle::init()
{
    // 播放背景音乐，循环播放
    bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if (bgm == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);
}

void SceneTitle::clean()
{
    if (bgm != nullptr)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
        bgm = nullptr;
    }
}

void SceneTitle::render()
{
    // 渲染标题文字
    std::string titleText = std::string("SDL太空射击");
    game.renderTextCentered(titleText, 0.4f, true);
    //  渲染普通文字
    if (timer < 0.5f)
    {
        std::string instructions = "按J开始游戏";
        game.renderTextCentered(instructions, 0.8f, false);
    }
}

void SceneTitle::update(float deltaTime)
{
    timer += deltaTime;
    if (timer > 1.0f)
    {
        timer -= 1.0f;
    }
}

void SceneTitle::handleEvent(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.scancode == SDL_SCANCODE_J)
        {
            auto sceneMain = new SceneMain();
            game.changeScene(sceneMain);
        }
    }
}
