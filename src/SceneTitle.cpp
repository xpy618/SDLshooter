#include "SceneTitle.h"
#include "SceneMain.h"
#include "Game.h"
#include <string>

void SceneTitle::init()
{
    //载入并播放背景音乐
    bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if (bgm == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);
}

void SceneTitle::update(float delatTime)
{
    timer += delatTime;
    if (timer > 1.0f){
        timer -= 1.0f;
    }
}

void SceneTitle::render()
{
    //渲染标题文字
    std::string titleText = "SDL太空战机";
    game.renderTextCentered(titleText, 0.35f, true);

    //渲染普通文字
    if (timer < 0.5f){
        std::string instructText = "按 J 键开始游戏";
        game.renderTextCentered(instructText, 0.8f, false);
    }
}

void SceneTitle::clean()
{
    if (bgm != nullptr){
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
}

void SceneTitle::handleEvent(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN)
    { if (event->key.keysym.scancode == SDL_SCANCODE_J){
            auto sceneMain = new SceneMain();
            game.changeScene(sceneMain);
        }
    }
}
