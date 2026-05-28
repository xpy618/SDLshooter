#ifndef SCENEEND_H
#define SCENEEND_H

#include "Scene.h"
#include <string>
#include <SDL_mixer.h>

class SceneEnd : public Scene
{
public:
    void init() override;
    void update(float delatTime) override;
    void render() override;
    void clean() override;
    void handleEvent(SDL_Event* event) override;
private:
    bool isTyping = true;
    std::string name = "";
    float blinkTimer = 1.0f;
    Mix_Music* bgm;

    void renderPhase1();
    void renderPhase2();

    void removeLastUTF8char(std::string& str);
};

#endif // SCENEEND_H