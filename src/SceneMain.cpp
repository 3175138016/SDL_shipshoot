#include "SceneMain.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "SceneTitle.h"
#include "SceneEnd.h"

#include <random>
#include <string>

SceneMain::~SceneMain()
{
    // 释放资源
}

// 更新场景主函数
void SceneMain::update(float deltaTime)
{
    keyboardControl(deltaTime);
    updatePlayerProjectiles(deltaTime);
    updateEnemyProjectiles(deltaTime);

    spawEnemy();              // 生成敌人
    updateEnemies(deltaTime); // 更新敌人

    updatePlayer(deltaTime); // 更新玩家

    updateExplostions(deltaTime); // 更新爆炸效果

    // 掉落物品
    updateItems(deltaTime);

    if (isDead)
    {
        changeSceneDelayed(deltaTime, 2.0f);
    }
}

void SceneMain::render()
{
    // 渲染玩家子弹
    renderPlayerProjectiles();
    // 渲染敌人子弹
    renderEnemyProjectiles();
    // 渲染玩家
    if (!isDead)
    {
        SDL_Rect playerRect = {static_cast<int>(player.position.x),
                               static_cast<int>(player.position.y),
                               player.width,
                               player.height};

        SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect);
    }

    // 渲染敌人
    renderEnemies();

    // 渲染物品
    renderItems();

    // 渲染爆炸特效
    renderExplostions();

    // 渲染UIhealth
    renderUI();
}

void SceneMain::handleEvent(SDL_Event *event)
{
    // 按ESC键返回主菜单
    if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            auto scneneTitle = new SceneTitle();
            game.changeScene(scneneTitle);
        }
    }
}

void SceneMain::init()
{
    // 载入bgm
    bgm = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
    if (bgm == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load bgm: %s\n", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);

    // 读入uihealth
    uiHealth = IMG_LoadTexture(game.getRenderer(), "assets/image/Health UI Black.png");
    if (uiHealth == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load uiHealth: %s\n", SDL_GetError());
    }

    // 读入ttf
    scoreFont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);
    if (scoreFont == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s\n", TTF_GetError());
    }

    // 读取音效资源
    sounds["player_shott"] = Mix_LoadWAV("assets/sound/laser_shoot4.wav");
    sounds["enemy_shott"] = Mix_LoadWAV("assets/sound/xs_laser.wav");
    sounds["player_explode"] = Mix_LoadWAV("assets/sound/explosion1.wav");
    sounds["enemy_explode"] = Mix_LoadWAV("assets/sound/explosion3.wav");
    sounds["hit"] = Mix_LoadWAV("assets/sound/eff11.wav");
    sounds["get_item"] = Mix_LoadWAV("assets/sound/eff5.wav");

    // 随机数种子
    std::random_device rd;
    gen = std::mt19937(rd());
    // 创建随机数分布
    dis = std::uniform_real_distribution<float>(0.0f, 1.0f);

    player.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
    if (player.texture == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load player texture: %s\n", SDL_GetError());
    }

    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);
    player.width /= 5;
    player.height /= 5;
    player.position.x = game.getWindowWidth() / 2 - player.width / 2;
    player.position.y = game.getWindowHeight() - player.height;

    // 初始化模板
    // 子弹
    ProjectilePlayerTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/laser-1.png");
    SDL_QueryTexture(ProjectilePlayerTemplate.texture, NULL, NULL, &ProjectilePlayerTemplate.width, &ProjectilePlayerTemplate.height);
    ProjectilePlayerTemplate.width /= 4;
    ProjectilePlayerTemplate.height /= 4;

    // 敌人
    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-1.png");
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width, &enemyTemplate.height);
    enemyTemplate.width /= 4;
    enemyTemplate.height /= 4;
    // 敌人子弹
    projectileEnemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bullet-1.png");
    SDL_QueryTexture(projectileEnemyTemplate.texture, NULL, NULL, &projectileEnemyTemplate.width, &projectileEnemyTemplate.height);
    projectileEnemyTemplate.width /= 2;
    projectileEnemyTemplate.height /= 2;

    // 爆炸
    explosionTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/effect/explosion.png");

    SDL_QueryTexture(explosionTemplate.texture, NULL, NULL, &explosionTemplate.width, &explosionTemplate.height);
    explosionTemplate.totalFrame = explosionTemplate.width / explosionTemplate.height;
    explosionTemplate.height *= 2;
    explosionTemplate.width = explosionTemplate.height;

    // 物品
    itemLifeTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_life.png");
    SDL_QueryTexture(itemLifeTemplate.texture, NULL, NULL, &itemLifeTemplate.width, &itemLifeTemplate.height);
    itemLifeTemplate.width /= 4;
    itemLifeTemplate.height /= 4;
}
void SceneMain::clean()
{
    if (uiHealth != nullptr)
    {
        SDL_DestroyTexture(uiHealth);
    }
    // 清理容器
    for (auto &i : sounds)
    {
        if (i.second != nullptr)
            Mix_FreeChunk(i.second);
    }
    sounds.clear();
    for (auto &i : projectilesPlayer)
    {
        if (i != nullptr)
        {
            delete i;
        }
    }
    projectilesPlayer.clear();
    for (auto &i : enemies)
    {
        if (i != nullptr)
        {
            delete i;
        }
    }
    enemies.clear();
    for (auto &i : projectilesEnemy)
    {
        if (i != nullptr)
        {
            delete i;
        }
    }
    projectilesEnemy.clear();
    for (auto &i : explosions)
    {
        if (i != nullptr)
        {
            delete i;
        }
    }
    explosions.clear();
    for (auto &i : items)
    {
        if (i != nullptr)
        {
            delete i;
        }
    }
    items.clear();
    // 清理模板
    if (player.texture != nullptr)
    {
        SDL_DestroyTexture(player.texture);
    }
    if (ProjectilePlayerTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(ProjectilePlayerTemplate.texture);
    }
    if (enemyTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(enemyTemplate.texture);
    }
    if (projectileEnemyTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(projectileEnemyTemplate.texture);
    }
    if (explosionTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(explosionTemplate.texture);
    }
    if (itemLifeTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(itemLifeTemplate.texture);
    }
    // 清理bgm
    if (bgm != nullptr)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
    if (scoreFont != nullptr)
    {
        TTF_CloseFont(scoreFont);
    }
}

void SceneMain::keyboardControl(float deltaTime)
{
    if (isDead)
        return;
    auto keyboardState = SDL_GetKeyboardState(NULL);

    // 移动
    if (keyboardState[SDL_SCANCODE_LEFT] || keyboardState[SDL_SCANCODE_A])
    {
        player.position.x -= deltaTime * player.speed;
    }
    if (keyboardState[SDL_SCANCODE_RIGHT] || keyboardState[SDL_SCANCODE_D])
    {
        player.position.x += deltaTime * player.speed;
    }
    if (keyboardState[SDL_SCANCODE_UP] || keyboardState[SDL_SCANCODE_W])
    {
        player.position.y -= deltaTime * player.speed;
    }
    if (keyboardState[SDL_SCANCODE_DOWN] || keyboardState[SDL_SCANCODE_S])
    {
        player.position.y += deltaTime * player.speed;
    }

    // 限制飞机宽度
    if (player.position.x < 0)
    {
        player.position.x = 0;
    }
    if (player.position.x > game.getWindowWidth() - player.width)
    {
        player.position.x = game.getWindowWidth() - player.width;
    }
    if (player.position.y < 0)
    {
        player.position.y = 0;
    }
    if (player.position.y > game.getWindowHeight() - player.height)
    {
        player.position.y = game.getWindowHeight() - player.height;
    }

    // 发射子弹
    if (keyboardState[SDL_SCANCODE_J])
    {
        // 发射子弹
        auto currentTime = SDL_GetTicks();
        if (currentTime - player.lastshootTime >= player.coolDown)
        {
            // 创建子弹
            shootPlayer();
            player.lastshootTime = currentTime;
        }
    }
}

void SceneMain::shootPlayer()
{
    auto projectile = new ProjectilePlayer(ProjectilePlayerTemplate);
    projectile->position.x = player.position.x + player.width / 2 - projectile->width / 2;
    projectile->position.y = player.position.y;
    projectilesPlayer.push_back(projectile);
    Mix_PlayChannel(0, sounds["player_shott"], 0);
}

void SceneMain::updatePlayerProjectiles(float deltaTime)
{
    int margin = 32; // 子弹超出屏幕外边界的距离

    for (auto it = projectilesPlayer.begin(); it != projectilesPlayer.end();)
    {
        auto projectile = *it;
        projectile->position.y -= deltaTime * projectile->speed;
        // 检查子弹是否超出屏幕
        if (projectile->position.y + margin < 0)
        {
            it = projectilesPlayer.erase(it);
            delete projectile;
        }
        else
        {
            bool hit = false;
            // 检查子弹是否击中敌人
            for (auto enemy : enemies)
            {
                SDL_Rect projectileRect = {static_cast<int>(projectile->position.x),
                                           static_cast<int>(projectile->position.y),
                                           projectile->width,
                                           projectile->height};
                SDL_Rect enemyRect = {static_cast<int>(enemy->position.x),
                                      static_cast<int>(enemy->position.y),
                                      enemy->width,
                                      enemy->height};
                if (SDL_HasIntersection(&projectileRect, &enemyRect))
                {
                    enemy->currentHealth -= projectile->damage;
                    delete projectile;
                    it = projectilesPlayer.erase(it);
                    hit = true;
                    Mix_PlayChannel(0, sounds["hit"], 0);
                    break;
                }
            }
            if (!hit)
                it++;
        }
    }
}

void SceneMain::renderPlayerProjectiles()
{
    for (auto projectile : projectilesPlayer)
    {
        SDL_Rect projectileRect = {static_cast<int>(projectile->position.x),
                                   static_cast<int>(projectile->position.y),
                                   projectile->width,
                                   projectile->height};
        SDL_RenderCopy(game.getRenderer(), projectile->texture, NULL, &projectileRect);
    }
}

void SceneMain::renderEnemyProjectiles()
{

    for (auto projectile : projectilesEnemy)
    {
        SDL_Rect projectileRect = {static_cast<int>(projectile->position.x),
                                   static_cast<int>(projectile->position.y),
                                   projectile->width,
                                   projectile->height};
        // SDL_RenderCopy(game.getRenderer(), projectile->texture, NULL, &projectileRect);
        float angle = atan2(projectile->direction.y, projectile->direction.x) * 180 / M_PI - 90;
        SDL_RenderCopyEx(game.getRenderer(), projectile->texture, NULL, &projectileRect, angle, NULL, SDL_FLIP_NONE);
    }
}
void SceneMain::spawEnemy()
{
    if (dis(gen) > 1.0 / 60.0f)
    {
        return;
    }
    Enemy *enemy = new Enemy(enemyTemplate);
    enemy->position.x = dis(gen) * (game.getWindowWidth() - enemy->width);
    enemy->position.y = -enemy->height;
    enemies.push_back(enemy);
}

void SceneMain::changeSceneDelayed(float deltaTime, float delay)
{
    timerEnd += deltaTime;
    if (timerEnd >= delay)
    {
        auto sceneEnd = new SceneEnd();
        game.changeScene(sceneEnd);
    }
}

void SceneMain::updateEnemies(float deltaTime)
{
    auto currentTime = SDL_GetTicks();
    for (auto it = enemies.begin(); it != enemies.end();)
    {
        auto enemy = *it;
        enemy->position.y += deltaTime * enemy->speed;
        if (enemy->position.y > game.getWindowHeight())
        {
            it = enemies.erase(it);
            delete enemy;
        }
        else
        {
            // 敌人血量
            if (enemy->currentHealth <= 0)
            {
                enemyExplode(enemy);
                it = enemies.erase(it);
                Mix_PlayChannel(-1, sounds["enemy_explode"], 0);

                continue;
            }
            // 敌人发射子弹

            if (currentTime - enemy->lastshootTime >= enemy->coolDown && (!isDead))
            {
                shootEnemy(enemy);
                enemy->lastshootTime = currentTime;
            }
            it++;
        }
    }
}

void SceneMain::updateEnemyProjectiles(float deltaTime)
{
    auto margin = 32; // 子弹超出屏幕外边界的距离
    for (auto it = projectilesEnemy.begin(); it != projectilesEnemy.end();)
    {
        auto projectile = *it;
        projectile->position.y += deltaTime * projectile->speed * projectile->direction.y;
        projectile->position.x += deltaTime * projectile->speed * projectile->direction.x;
        if (projectile->position.y > game.getWindowHeight() + margin ||
            projectile->position.y < -margin ||
            projectile->position.x > game.getWindowWidth() + margin ||
            projectile->position.x < -margin)
        {
        }

        if (projectile->position.y > game.getWindowHeight() + margin ||
            projectile->position.y < -margin ||
            projectile->position.x > game.getWindowWidth() + margin ||
            projectile->position.x < -margin)
        {
            it = projectilesEnemy.erase(it);
            delete projectile;
        }
        else
        {
            SDL_Rect projectileRect = {
                static_cast<int>(projectile->position.x),
                static_cast<int>(projectile->position.y),
                projectile->width,
                projectile->height};
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height};
            if (SDL_HasIntersection(&projectileRect, &playerRect) && (!isDead))
            {
                player.currentHealth -= projectile->damage;
                delete projectile;
                it = projectilesEnemy.erase(it);
                Mix_PlayChannel(-1, sounds["hit"], 0);
            }
            else
            {
                it++;
            }
        }
    }
}

void SceneMain::updatePlayer(float)
{
    if (isDead)
        return;
    if (player.currentHealth <= 0)
    {
        isDead = true;
        auto currentTime = SDL_GetTicks();
        Explosion *explosion = new Explosion(explosionTemplate);
        explosion->position.x = player.position.x + player.width / 2 - explosion->width / 2;
        explosion->position.y = player.position.y + player.height / 2 - explosion->height / 2;
        explosion->startTime = currentTime;
        explosions.push_back(explosion);
        Mix_PlayChannel(0, sounds["player_explode"], 0);
        game.setFinallScore(score);
        return;
    }
    // 飞机与 敌人的对撞检测
    SDL_Rect playerRect = {
        static_cast<int>(player.position.x),
        static_cast<int>(player.position.y),
        player.width,
        player.height};
    for (auto enemy : enemies)
    {
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height};

        if (SDL_HasIntersection(&enemyRect, &playerRect))
        {
            player.currentHealth--;
            enemy->currentHealth = 0;
        }
    }
}

void SceneMain::updateExplostions(float)
{
    for (auto it = explosions.begin(); it != explosions.end();)
    {
        auto explosion = *it;
        auto currentTime = SDL_GetTicks();
        explosion->currentFrame = (currentTime - explosion->startTime) * explosion->FPS / 1000;
        if (explosion->currentFrame >= explosion->totalFrame)
        {
            it = explosions.erase(it);
            delete explosion;
            continue;
        }
        else
        {
            it++;
        }
    }
}

void SceneMain::renderExplostions()
{
    for (auto &explosion : explosions)
    {

        SDL_Rect src = {
            explosion->currentFrame * explosion->width,
             0,
            explosion->width / 2,
            explosion->height / 2};
        SDL_Rect dst = {
            static_cast<int>(explosion->position.x),
            static_cast<int>(explosion->position.y),
            explosion->width,
            explosion->height};

        SDL_RenderCopy(game.getRenderer(), explosion->texture, &src, &dst);
    }
}

void SceneMain::renderEnemies()
{
    for (auto &enemy : enemies)
    {
        SDL_Rect enemyRect = {static_cast<int>(enemy->position.x),
                              static_cast<int>(enemy->position.y),
                              enemy->width,
                              enemy->height};
        SDL_RenderCopy(game.getRenderer(), enemy->texture, NULL, &enemyRect);
    }
}

void SceneMain::shootEnemy(Enemy *enemy)
{
    auto projectile = new ProjectileEnemy(projectileEnemyTemplate);
    projectile->position.x = enemy->position.x + enemy->width / 2 - projectile->width / 2;
    projectile->position.y = enemy->position.y + enemy->height / 2 - projectile->height / 2;
    projectile->direction = getDirection(enemy); // 获取子弹方向
    projectilesEnemy.push_back(projectile);
    Mix_PlayChannel(-1, sounds["enemy_shott"], 0);
}
// 获取子弹方向
SDL_FPoint SceneMain::getDirection(Enemy *enemy)
{
    auto x = (player.position.x + player.width / 2) - (enemy->position.x + enemy->width / 2);
    auto y = (player.position.y + player.height / 2) - (enemy->position.y + enemy->height / 2);
    auto length = sqrt(x * x + y * y);
    if (length == 0)
    {
        return SDL_FPoint{1, 1};
    }
    x /= length;
    y /= length;
    return SDL_FPoint{x, y};
}

void SceneMain::enemyExplode(Enemy *enemy)
{
    // 创建爆炸

    auto currentTime = SDL_GetTicks();
    auto explosion = new Explosion(explosionTemplate);
    explosion->position.x = enemy->position.x + enemy->width / 2 - explosion->width / 2;
    explosion->position.y = enemy->position.y + enemy->height / 2 - explosion->height / 2;
    explosion->startTime = currentTime;
    explosions.push_back(explosion);
    // 掉落物品
    if (dis(gen) < 0.5f)
        dropItem(enemy);
    score += enemy->score;
    delete enemy;
}

void SceneMain::dropItem(Enemy *enemy)
{
    // 掉落物品

    Item *item = new Item(itemLifeTemplate);
    item->position.x = enemy->position.x + enemy->width / 2 - item->width / 2;
    item->position.y = enemy->position.y + enemy->height / 2 - item->height / 2;
    float angle = dis(gen) * 2 * M_PI;
    item->direction.x = cos(angle);
    item->direction.y = sin(angle);

    items.push_back(item);
}

void SceneMain::updateItems(float deltaTime)
{
    for (auto it = items.begin(); it != items.end();)
    {
        auto item = *it;
        // 更新位置
        item->position.x += item->direction.x * item->speed * deltaTime;
        item->position.y += item->direction.y * item->speed * deltaTime;
        // 判断反弹
        if (item->bounce > 0)
        {
            if (item->position.x <= 0 || item->position.x + item->width >= game.getWindowWidth())
            {
                item->direction.x = -item->direction.x;
                item->bounce--;
            }
            if (item->position.y <= 0 || item->position.y + item->height >= game.getWindowHeight())
            {
                item->direction.y = -item->direction.y;
                item->bounce--;
            }
        }
        // 超出屏幕范围则删除
        if (item->position.x + item->width < 0 ||
            item->position.x > game.getWindowWidth() ||
            item->position.y + item->height < 0 ||
            item->position.y > game.getWindowHeight())
        {
            it = items.erase(it);
            delete item;
        }
        else
        {
            if (isDead)
                break;
            SDL_Rect itemRect = {
                static_cast<int>(item->position.x),
                static_cast<int>(item->position.y),
                item->width,
                item->height};
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height};

            if (SDL_HasIntersection(&itemRect, &playerRect))
            {
                // 碰撞检测
                playerGetItem(item);
                it = items.erase(it);
                delete item;
            }
            else
            {
                it++;
            }
        }
    }
}

void SceneMain::playerGetItem(Item *item)
{
    score += item->score;
    if (item->type == ItemType::Life)
    {
        player.currentHealth++;
        if (player.currentHealth > player.maxHealth)
            player.currentHealth = player.maxHealth;
    }
    Mix_PlayChannel(0, sounds["get_item"], 0);
}

void SceneMain::renderItems()
{
    for (auto &item : items)
    {
        SDL_Rect itemRect = {static_cast<int>(item->position.x),
                             static_cast<int>(item->position.y),
                             item->width,
                             item->height};
        SDL_RenderCopy(game.getRenderer(), item->texture, NULL, &itemRect);
    }
}

void SceneMain::renderUI()
{

    int x = 10, y = 10;
    int offset = 40;
    int size = 32;
    SDL_SetTextureColorMod(uiHealth, 100, 100, 100); // 颜色减淡
    for (int i = 0; i < player.maxHealth; i++)
    {
        SDL_Rect rect = {x + (i % player.lineLife) * offset, y + i / player.lineLife * offset, size, size};
        SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &rect);
    }

    SDL_SetTextureColorMod(uiHealth, 255, 255, 255); // 颜色正常
    for (int i = 0; i < player.currentHealth; i++)
    {
        SDL_Rect rect = {x + (i % player.lineLife) * offset, y + i / player.lineLife * offset, size, size};
        SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &rect);
    }

    // 渲染分数
    auto text = "SCORE: " + std::to_string(score);

    SDL_Color fontColor = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderUTF8_Solid(scoreFont, text.c_str(), fontColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game.getRenderer(), surface);
    SDL_Rect rect = {game.getWindowWidth() - 10 - surface->w, 10, surface->w, surface->h};
    SDL_RenderCopy(game.getRenderer(), texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
