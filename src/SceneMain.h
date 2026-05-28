#ifndef SCENE_MAIN_H
#define SCENE_MAIN_H

#include "Scene.h"
#include "Object.h"
#include <list>
#include <random>
#include <map>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

class Game;

class SceneMain : public Scene{
public:
    ~SceneMain();

    void update(float deltaTime) override;
    void render() override;
    void handleEvent(SDL_Event* event) override;
    void init() override;
    void clean() override;

private:
    Player player;
    Mix_Music* bgm;
    SDL_Texture* uiHealth;
    TTF_Font* scoreFont;
    int score = 0;
    float timerEnd = 0.0f;

    bool isDead = false;
    bool hasShield = false;
    std::mt19937 gen; //随机数生成器
    std::uniform_real_distribution<float> dis; //随机数分布
    int playerShieldWidth = 0;  // 新增：存储缩放后的防护罩宽度
    int playerShieldHeight = 0; // 新增：存储缩放后的防护罩高度

    //创建每个物体的模板
    Enemy EnemyTemplate;
    ProjectilePlayer ProjectilePlayerTemplate;
    ProjectileEnemy ProjectileEnemyTemplate;
    Explosion ExplosionTemplate;
    Item ItemLifeTemplate;
    Item ItemShieldTemplate;
    SDL_Texture* playerShieldTexture = nullptr;

    //创建容器
    std::list<Enemy*> enemies;
    std::list<ProjectilePlayer*> projectilesPlayer;
    std::list<ProjectileEnemy*> projectilesEnemy;
    std::list<Explosion*> explosions;
    std::list<Item*> items;
    std::map<std::string, Mix_Chunk*> sounds;

    //渲染相关
    void renderPlayer();
    void renderPlayerProjectiles();
    void renderEnemyProjectiles();
    void renderEnemies();
    void renderExplosions();
    void renderUI();
    void renderItems();

    //更新相关
    void updatePlayerProjectiles(float deltaTime);
    void updateEnemyProjectiles(float deltaTime);
    void updateEnemies(float deltaTime);
    void updatePlayer();
    void updateExplosions(float deltaTime);
    void updateItems(float deltaTime);
    void spawnEnemy();
    void changeSceneDelayed(float deltaTime, float delay);

    //其他
    void keyboardControl(float deltaTime);
    void shootPlayer();
    void shootEnemy(Enemy* enemy);
    SDL_FPoint getDirection(Enemy* enemy);
    void enemyExplode(Enemy* enemy);
    void dropItem(Enemy* enemy);
    void playerGetItem(Item* item);
    
};


#endif // SCENE_MAIN_H