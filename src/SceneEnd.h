#ifndef SCENEEND_H
#define SCENEEND_H
#include "Scene.h"
#include<string>
#include<SDL_mixer.h>
class SceneEnd : public Scene
{
public:
    void init() override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;
    void handleEvent(SDL_Event *event) override;

private:
    float blink = 1.0f;
    bool isTyping = true;
    std::string name;
    Mix_Music *bgm;

    void renderPhase1();
    void renderPhase2();
    void removeLastUTF8Char(std::string &str);
};

#endif