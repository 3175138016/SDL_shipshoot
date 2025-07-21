#ifndef _OBJECT_H
#define _OBJECT_H
#include <SDL.h>

enum class ItemType
{
    Life,
    Shield,
    Time,
};
struct Player
{
    // 玩家材质
    SDL_Texture *texture = nullptr;

    // 玩家位置
    SDL_FPoint position = {0, 0};

    // 玩家大小
    int width = 0;
    int height = 0;

    // 玩家血量
    int maxHealth = 3;
    int currentHealth = maxHealth;

    // 玩家速度
    int speed = 400;
    Uint32 coolDown = 100;    // 子弹冷却时间
    Uint32 lastshootTime = 0; // 上次发射子弹时间
    int lineLife = 10;
};

struct ProjectilePlayer
{
    // 玩家子弹材质
    SDL_Texture *texture = nullptr;

    // 玩家子弹位置
    SDL_FPoint position = {0, 0};

    // 玩家子弹大小
    int width = 0;
    int height = 0;
    // 玩家子弹伤害
    int damage = 1;

    // 玩家子弹速度
    int speed = 800;
};

struct Enemy
{
    // 敌人材质
    SDL_Texture *texture = nullptr;

    // 敌人位置
    SDL_FPoint position = {0, 0};

    // 敌人大小
    int width = 0;
    int height = 0;

    // 敌人速度
    int speed = 200;
    // 敌人血量
    int currentHealth = 2;
    int score = 10;

    Uint32 coolDown = 2000;   // 子弹冷却时间
    Uint32 lastshootTime = 0; // 上次发射子弹时间
};
struct ProjectileEnemy
{
    // 敌人子弹材质
    SDL_Texture *texture = nullptr;

    // 敌人子弹位置
    SDL_FPoint position = {0, 0};
    SDL_FPoint direction = {0, 0};
    // 敌人子弹大小
    int width = 0;
    int height = 0;

    // 敌人子弹速度
    int speed = 400;
    // 敌人子弹伤害
    int damage = 1;
};

// 爆炸结构体
struct Explosion
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int currentFrame = 0;
    int totalFrame = 0;
    Uint32 startTime = 0; // 当前帧
    Uint32 FPS = 10;
};

// 物品
struct Item
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint position = {0, 0};
    // 方向
    SDL_FPoint direction = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 100;
    // 反弹次数
    int bounce = 3;
    int score = 5;

    // 物品类型
    ItemType type = ItemType::Life;
};

struct Background
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint position = {0, 0};
    float offset = 0;
    int width = 0;
    int height = 0;
    int speed = 30;
};

#endif