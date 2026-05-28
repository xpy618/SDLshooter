#include "Game.h"
#include "SceneMain.h"
#include "SceneTitle.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <fstream>

Game::Game()
{
}

Game::~Game()
{
    savedata();
    clean();
}

void Game::run()
{
    while (isRunning)
    {
        //控制帧率
        auto frameStart = SDL_GetTicks();
        SDL_Event event;
        handleEvent(&event);      
        update(deltaTime);
        render();     
        auto frameEnd = SDL_GetTicks();
        auto diff = frameEnd - frameStart;
        if (diff < frameTime)
        {
            SDL_Delay(frameTime - diff);
            deltaTime = frameTime / 1000.0f;
        }
        else{
            deltaTime = diff / 1000.0f;
        }
    }
}

void Game::init()
{
    frameTime = 1000 / FPS;
    //SDL初始化
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){   
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;     
    }

    //创建窗口
    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }

    //创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }

    //设置逻辑分辨率
    SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
    
    //初始化SDL_Image
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }

    //初始化SDL_mixer
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG)){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }

    //打开音频设备
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }

    //设置音效数量
    Mix_AllocateChannels(32);

    //设置音量
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_Volume(-1, MIX_MAX_VOLUME / 8);

    //初始化SDL_ttf
    if (TTF_Init() == -1){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }

    //初始化背景卷轴
    nearStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-A.png");
    if (nearStars.texture == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width, &nearStars.height);
    nearStars.height /= 2;
    nearStars.width /= 2;

    farStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-B.png");
    if (farStars.texture == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width, &farStars.height);
    farStars.height /= 2;
    farStars.width /= 2;
    farStars.speed = 20;

    //载入字体
    titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);
    if (titleFont == nullptr || textFont == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }

    //载入得分
    loaddata();

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

    //清理SDL_Image
    IMG_Quit();

    //清理SDL_mixer
    Mix_CloseAudio();
    Mix_Quit();
    //清理SDL_ttf
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::changeScene(Scene * scene)
{
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }
    currentScene = scene;
    currentScene->init();
}

void Game::handleEvent(SDL_Event *event)
{
    while (SDL_PollEvent(event))
    {
        if (event->type == SDL_QUIT)
        {
            isRunning = false;
        }
        if (event->type == SDL_KEYDOWN){
            if (event->key.keysym.scancode == SDL_SCANCODE_F4){
                isFullscreen = !isFullscreen;
                if (isFullscreen){
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                } else{
                    SDL_SetWindowFullscreen(window, 0);
                }
            }
        }
        currentScene->handleEvent(event);
    }
}

void Game::update(float deltaTime)
{
    backgroundupdate(deltaTime);
    currentScene->update(deltaTime);
}

void Game::render()
{
    //清空
    SDL_RenderClear(renderer);
    //渲染星空背景
    renderbackground();
    currentScene->render();
    //显示更新
    SDL_RenderPresent(renderer);
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
    int y = static_cast<int>((getWindowHeight() - surface->h) * posY);
    SDL_Rect rect = {getWindowWidth() / 2 - surface->w / 2,
                    y,
                    surface->w,
                    surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    return {rect.x+ rect.w, y};
}

void Game::renderTextPos(std::string text, int posX, int posY, bool isLeft)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;
    if (isLeft)
    {
        rect = {posX, posY, surface->w, surface->h}; 
    }else{
        rect = {getWindowWidth() - surface->w - posX, posY, surface->w, surface->h};
    }
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Game::backgroundupdate(float deltaTime)
{
    nearStars.offset += nearStars.speed * deltaTime;
    if (nearStars.offset >= 0)
    {
        nearStars.offset = static_cast<float>(-nearStars.height);
    }
    farStars.offset += farStars.speed * deltaTime;
    if (farStars.offset >= 0)
    {
        farStars.offset = static_cast<float>(-farStars.height);
    }
}

void Game::renderbackground()
{
    for(int posY = static_cast<int>(farStars.offset); posY < getWindowHeight(); posY += farStars.height)
    {
        for(int posX = 0; posX < getWindowWidth(); posX += farStars.width)
        {
            SDL_Rect dstRect = {posX, posY, farStars.width, farStars.height};
            SDL_RenderCopy(renderer, farStars.texture, NULL, &dstRect);
        }
    }
    for( int posY = static_cast<int>(nearStars.offset); posY < getWindowHeight(); posY += nearStars.height)
    {
        for(int posX = 0; posX < getWindowWidth(); posX += nearStars.width)
        {
            SDL_Rect dstRect = {posX, posY, nearStars.width, nearStars.height};
            SDL_RenderCopy(renderer, nearStars.texture, NULL, &dstRect);
        }
    }
}

void Game::savedata()
{
    //保存得分榜数据
    std::ofstream file("assets/save.dat");
    if (!file.is_open()){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Failed to open save.dat");
        return;
    }
    for (const auto &entry : leaderBoard)
    {
        file << entry.first << " " << entry.second << std::endl;
    }
}

void Game::loaddata()
{
    //加载得分榜数据
    std::ifstream file("assets/save.dat");
    if (!file.is_open()){
        SDL_Log("Failed to open save.dat");
        return;
    }
    leaderBoard.clear();
    int score;
    std::string name;
    while (file >> score >> name)
    {
        leaderBoard.insert({score, name});
    }
}

void Game::insertleaderBoard(int score, std::string name)
{
    leaderBoard.insert({score, name});
    if (leaderBoard.size() > 8)
    {
        leaderBoard.erase(--leaderBoard.end());
    }
}