#ifndef GAME_H
#define GAME_H

#include "Scene.h"
#include "Object.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <map>

class Game
{
public:
    static Game &getInstance()
    {
        static Game instance;
        return instance;
    }
    ~Game();
    void run();
    void init();
    void clean();
    void changeScene(Scene *scene);

    void handleEvents(SDL_Event *event);
    void update(float deltaTime);
    void render();

    SDL_Window *getWindow() { return window; }
    SDL_Renderer *getRenderer() { return renderer; }
    int getWindowWidth() { return windowWidth; }
    int getWindowHeight() { return windowHeight; }

    void setFinallScore(int &score) { finallScore = score; }
    int getFinallScore() { return finallScore; }
    void renderTextPox(std::string text, int posX, int posY, bool isLeft = true);

    // 渲染工具函数

    SDL_Point renderTextCentered(std::string text, float posY, bool isTitle);
    std::multimap<int, std::string, std::greater<int>> &getLeaderBoard() { return leaderBoard; }

    void insertLeaderBoard(int score, std::string name);

private:
    Game();
    // 删除拷贝构造函数
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    int finallScore = 0;

    TTF_Font *titleFont;
    TTF_Font *textFont;

    bool isrunning = true;
    Scene *currentScene = nullptr;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    int windowWidth = 600;
    int windowHeight = 800;

    int FPS = 60;
    Uint32 frameTime;
    float deltaTime;

    Background nearStars;
    Background farStars;
    void backgroundUpdate(float deltaTime);
    void renderBackground();

    std::multimap<int, std::string, std::greater<int>> leaderBoard;
    void saveData();
    void loadData();
    bool isFullscreen= false;
};

#endif