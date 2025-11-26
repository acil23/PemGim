#pragma once
#include "Scene.hpp"
#include <string>
#include <SDL.h>

// Fighter struct (lihat penjelasan sebelumnya)
struct Fighter {
    float x;
    float y;

    int hp;
    int maxHP;

    SDL_Texture* texture = nullptr;

    int frameW = 0;
    int frameH = 0;
    int frameCount = 1;
    int currentFrame = 0;
    bool attacking = false;
    float attackTimer = 0.0f;
    float attackFrameDuration = 0.07f; // detik per frame
};

class DuelScene : public Scene {
public:
    DuelScene()
        : change(false), next(""),
          gamePtr(nullptr),
          playerSpeed(200.0f),
          enemyHP(100),
          enemyMaxHP(100),
          playerAttackDamage(15),
          enemyAlive(true)
    {}

    void onEnter(Game* game) override;
    void onExit() override {}

    void handleEvent(const SDL_Event& e) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, TextRenderer* text) override;

    bool wantsSceneChange() const override { return change; }
    const char* nextSceneName() const override { return next.c_str(); }

private:
    Game* gamePtr;

    bool change;
    std::string next;

    // pemain & musuh
    Fighter player;
    Fighter enemy;

    // info pemain dari pilihan characterSelect
    std::string playerName;
    int playerAttackDamage;

    // musuh state
    int enemyHP;
    int enemyMaxHP;
    bool enemyAlive;
    bool hasHitThisSwing = false;

    bool playerFacingRight = true;
    bool enemyFacingRight  = false;

    // gerak player kiri/kanan
    bool moveLeft = false;
    bool moveRight = false;
    float playerSpeed; // pixel per detik

    // --- Jump state ---
    bool jumping = false;
    int  jumpCurrentFrame = 0;
    float jumpTimer = 0.0f;
    float jumpFrameDuration = 0.12f; // detik per frame
    int  jumpFrameCount = 5;
    int jumpYOffset = 0; 
    // Tekstur jump (spritesheet 5 frame) & ukurannya
    SDL_Texture* playerJumpTex = nullptr;
    int jumpFrameW = 0;
    int jumpFrameH = 0;

    // --- Walk animation ---
    SDL_Texture* playerWalkTex = nullptr;
    int walkFrameW = 0;
    int walkFrameH = 0;
    int walkFrameCount = 5;     
    int walkCurrentFrame = 0;
    float walkTimer = 0.0f;
    float walkFrameDuration = 0.09f; // 9-11 ms per frame = halus, atur selera

    // --- Block animation ---
    SDL_Texture* playerBlockTex = nullptr;
    int blockFrameW = 0;
    int blockFrameH = 0;
    int blockFrameCount = 3;
    int blockCurrentFrame = 0;
    float blockTimer = 0.0f;
    float blockFrameDuration = 0.10f; // 0.1 s per frame -> agak pelan

    bool blocking = false;
    bool blockForward = true; // untuk anim ping-pong 0-1-2-1-0

    bool isGameOver = false; // Flag baru
    bool playerWon = false;

    // helper
    void drawHealthBar(SDL_Renderer* renderer,
                       TextRenderer* text,
                       int x, int y,
                       int w, int h,
                       int hp,
                       int hpMax,
                       const char* label,
                       SDL_Color barColor,
                       SDL_Color borderColor,
                       SDL_Color textColor);

    void updatePlayerMovement(float dt);
    void startPlayerAttack();
    void updatePlayerAttack(float dt);
    void updateFacing();
    void checkHitAndDamageEnemy();
    // Helper
    void startPlayerJump();
    void updatePlayerJump(float dt);
    void updatePlayerWalk(float dt);

    void startBlock();
    void stopBlock();
    void updateBlock(float dt);
};
