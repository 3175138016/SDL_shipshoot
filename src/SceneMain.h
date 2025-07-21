#ifndef SCENEMAIN_H
#define SCENEMAIN_H

#include "Scene.h"
#include "Object.h"
#include <list>
#include <random>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <map>

class Game;

class SceneMain : public Scene
{
public:
    ~SceneMain();
    void update(float deltaTime) override;
    void render() override;
    void handleEvent(SDL_Event *event) override;
    void init() override;
    void clean() override;

private:
    Player player;

    Mix_Music *bgm;
    SDL_Texture *uiHealth;
    TTF_Font *scoreFont;

    int score = 0;
    float timerEnd = 0.0f;

    bool isDead = false;
    // 创建每个物体的模板
    ProjectilePlayer ProjectilePlayerTemplate;
    Enemy enemyTemplate;
    ProjectileEnemy projectileEnemyTemplate;
    Explosion explosionTemplate;

    Item itemLifeTemplate;

    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;

    std::list<ProjectilePlayer *> projectilesPlayer;
    std::list<Enemy *> enemies;
    std::list<ProjectileEnemy *> projectilesEnemy;
    std::list<Explosion *> explosions;
    std::list<Item *> items;
    std::map<std::string, Mix_Chunk *> sounds;

    // 渲染
    void renderPlayerProjectiles();
    void renderEnemyProjectiles();
    void renderExplostions();
    void renderItems();
    void renderEnemies();
    void renderUI();

    // 更新
    void updatePlayerProjectiles(float deltaTime);
    void updateEnemies(float deltaTime);
    void updateEnemyProjectiles(float deltaTime);
    void updatePlayer(float deltaTime);
    void updateExplostions(float deltaTime);
    void updateItems(float deltaTime);
    void keyboardControl(float deltaTime);
    void spawEnemy();
    void changeSceneDelayed(float deltaTime,float delay);

    // 其他

    void shootPlayer();
    void shootEnemy(Enemy *enemy);
    SDL_FPoint getDirection(Enemy *enemy);
    void enemyExplode(Enemy *enemy);
    void dropItem(Enemy *enemy);
    void playerGetItem(Item *item);
};

#endif