#include "SceneEnd.h"
#include "Game.h"
#include "SceneMain.h"
void SceneEnd::init()
{
    // 载入背景音乐
    bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if (bgm == NULL)
    {
        SDL_LogError(SDL_LOG_PRIORITY_ERROR,  "Mix_LoadMUS failed:%s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);

    if (!SDL_IsTextInputActive())
    {
        SDL_StartTextInput();
    }
    if (!SDL_IsTextInputActive())
    {
        SDL_LogError(SDL_LOG_PRIORITY_ERROR,  "SDL_StartTextInput failed:%s", SDL_GetError());
    }
}

void SceneEnd::update(float deltaTime)
{
    blink -= deltaTime;
    if (blink <= 0)
    {
        blink += 1.0f;
    }
}

void SceneEnd::render()
{
    if (isTyping)
    {
        renderPhase1();
    }
    else
    {
        renderPhase2();
    }
}

void SceneEnd::handleEvent(SDL_Event *event)
{
    if (isTyping)
    {
        if (event->type == SDL_TEXTINPUT)
        {
            name += event->text.text;
        }
        if (event->type == SDL_KEYDOWN)
        {
            if (event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
            {
                if (!name.empty())
                    removeLastUTF8Char(name);
            }
            if (event->key.keysym.scancode == SDL_SCANCODE_RETURN || event->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            {
                isTyping = false;
                if (name.empty())
                    name = "匿名玩家";
                game.insertLeaderBoard(game.getFinallScore(), name);
                SDL_StopTextInput();
                // TODO:save score
            }
        }
    }
    else
    {
        if (event->type == SDL_KEYDOWN)
        {

            if (event->key.keysym.scancode == SDL_SCANCODE_J || event->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            {
                game.changeScene(new SceneMain());
            }
        }
    }
}

void SceneEnd::clean()
{
   if(bgm != nullptr)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
        bgm = nullptr;
    }
    
}

void SceneEnd::renderPhase1()
{
    std::string scoreText = "最终得分" + std::to_string(game.getFinallScore());
    std::string gameOver = "GAME OVER";
    std::string instrutionText = "请输入你的名字，按回车键确认";
    game.renderTextCentered(scoreText, 0.1f, false);
    game.renderTextCentered(gameOver, 0.4f, true);
    game.renderTextCentered(instrutionText, 0.6f, false);
    if (!name.empty())
    {
        SDL_Point p = game.renderTextCentered(name, 0.8f, false);
        if (blink < 0.5f)
            game.renderTextPox("_", p.x, p.y);
    }
    else
    {
        if (blink < 0.5f)
            game.renderTextCentered("_", 0.8f, false);
    }
}
void SceneEnd::renderPhase2()
{
    game.renderTextCentered("得分榜", 0.07f, true);
    int i = 1;
    int posY = 0.25 * game.getWindowHeight();
    for (auto &item : game.getLeaderBoard())
    {
        std::string name = std::to_string(i) + ". " + item.second;
        std::string score = std::to_string(item.first);
        game.renderTextPox(name, 100, posY);
        game.renderTextPox(score, 100, posY, false);
        posY += 45;
        i++;
    }

    if (blink < 0.5f)
        game.renderTextCentered("按J键重新开始游戏", 0.85f, false);
}

void SceneEnd::removeLastUTF8Char(std::string &str)
{
    if ((str.back() & 0b10000000) == 0b10000000)
    {
        str.pop_back();
        while ((str.back() & (0b11000000)) != 0b11000000)
        {
            str.pop_back();
        }
    }
    str.pop_back();
}
