#pragma once
#include "Scene.hpp"
#include <string>
#include <SDL.h>
#include <vector>

// Struktur untuk menampung data dasar petarung (Player/Enemy)
struct Fighter {
    float x, y;
    int hp, maxHP;
    
    // Sprite Utama (biasanya Idle atau Attack tergantung state)
    SDL_Texture* texture = nullptr;

    // Animasi Frame
    int frameW = 0;
    int frameH = 0;
    int frameCount = 1;
    int currentFrame = 0;

    // Attack State
    bool attacking = false;
    float attackTimer = 0.0f;
    float attackFrameDuration = 0.07f; 
};

class DuelScene : public Scene {
public:
    DuelScene();

    // --- SCENE LIFECYCLE ---
    void onEnter(Game* game) override;
    void onExit() override;
    void handleEvent(const SDL_Event& e) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, TextRenderer* text) override;

    // --- SCENE MANAGEMENT ---
    bool wantsSceneChange() const override { return change; }
    const char* nextSceneName() const override { return next.c_str(); }

private:
    Game* gamePtr = nullptr;
    bool change = false;
    std::string next = "";

    // ==================================================
    // 1. CORE GAME STATE (Menang/Kalah/Intro)
    // ==================================================
    bool isGameOver = false;
    bool playerWon = false;
    
    // Intro VS Animation
    bool isIntro = true;
    float introTimer = 0.0f;
    float vsScale = 5.0f;
    float vsAlpha = 0.0f;

    // ==================================================
    // 2. RESOURCES (TEXTURES & ASSETS)
    // ==================================================
    SDL_Texture* bgTexture = nullptr;      // Background Gurun
    SDL_Texture* healthBarTex = nullptr;   // Bingkai HP
    SDL_Texture* vsTex = nullptr;          // Icon VS

    // ==================================================
    // 3. PLAYER DATA & STATE
    // ==================================================
    Fighter player;
    std::string playerName;
    int playerAttackDamage;
    float playerSpeed = 200.0f;
    
    // Movement Flags
    bool playerFacingRight = true;
    bool moveLeft = false;
    bool moveRight = false;

    // Logic Hit
    bool hasHitThisSwing = false;

    // --- Player Special Animations ---
    
    // Jump
    bool jumping = false;
    int jumpCurrentFrame = 0;
    float jumpTimer = 0.0f;
    float jumpFrameDuration = 0.12f;
    int jumpFrameCount = 5;
    int jumpYOffset = 0; 
    SDL_Texture* playerJumpTex = nullptr;
    int jumpFrameW = 0, jumpFrameH = 0;

    // Walk
    SDL_Texture* playerWalkTex = nullptr;
    int walkFrameW = 0, walkFrameH = 0;
    int walkFrameCount = 5;     
    int walkCurrentFrame = 0;
    float walkTimer = 0.0f;
    float walkFrameDuration = 0.09f;

    // Block
    SDL_Texture* playerBlockTex = nullptr;
    int blockFrameW = 0, blockFrameH = 0;
    int blockFrameCount = 3;
    int blockCurrentFrame = 0;
    float blockTimer = 0.0f;
    float blockFrameDuration = 0.10f;
    bool blocking = false;
    bool blockForward = true;

    // ==================================================
    // 4. ENEMY DATA & AI (YANG BARU)
    // ==================================================
    Fighter enemy;
    int enemyHP;
    int enemyMaxHP;
    bool enemyAlive;
    bool enemyFacingRight = false;

    // Enemy Attack Specifics
    SDL_Texture* enemyAttackTex = nullptr;
    int enemyAttackFrameW = 0;
    int enemyAttackFrameH = 0;
    int enemyAttackFrameCount = 5;
    float enemyAttackCooldown = 1.0f;
    float enemyAttackTimer = 0.0f;
    bool hasEnemyHitThisSwing = false;
    int enemyAttackDamage = 12;

    // --- Enemy AI State (NEW) ---
    bool enemyMovingLeft = false;
    bool enemyMovingRight = false;
    bool enemyJumping = false;
    bool enemyBlocking = false;

    // AI Timers
    float aiDecisionTimer = 0.0f;
    float enemyJumpTimer = 0.0f;
    int enemyJumpYOffset = 0;
    float enemyBlockTimer = 0.0f;
    float enemyWalkTimer = 0.0f;
    int enemyWalkFrame = 0;

    // Enemy Extra Textures
    SDL_Texture* enemyWalkTex = nullptr;
    SDL_Texture* enemyBlockTex = nullptr;
    SDL_Texture* enemyJumpTex = nullptr;

    // === 1. VISUAL EFFECT SYSTEM ===
    struct VisualEffect {
        float x, y;
        SDL_Texture* texture; // Pointer ke texture (blood/dust)
        int frameW, frameH;
        int currentFrame;
        int maxFrames;
        float timer;
        float frameDuration;
        bool flip; // Agar debu bisa menghadap kiri/kanan
    };
    
    std::vector<VisualEffect> activeEffects; // List efek yang sedang tampil

    // Resources Baru
    SDL_Texture* groundTex = nullptr;
    SDL_Texture* bloodTex = nullptr;
    SDL_Texture* dustTex = nullptr;

    // Timer untuk spawn debu saat lari
    float playerDustTimer = 0.0f;
    float enemyDustTimer = 0.0f;

    // ==================================================
    // 5. HELPER FUNCTIONS
    // ==================================================
    
    // -- Player Logic --
    void updatePlayerMovement(float dt);
    void updateFacing();
    
    void startPlayerAttack();
    void updatePlayerAttack(float dt);
    void checkHitAndDamageEnemy();
    
    void startPlayerJump();
    void updatePlayerJump(float dt);
    
    void updatePlayerWalk(float dt);
    
    void startBlock();
    void stopBlock();
    void updateBlock(float dt);

    // -- Enemy AI Logic (NEW) --
    void startEnemyAttack();
    void updateEnemyAttack(float dt);
    void checkEnemyHitAndDamagePlayer();
    
    void updateEnemyAI(float dt);     // Otak utama AI
    void enemyMoveLogic(float dt);    // Jalan
    void enemyJumpLogic(float dt);    // Lompat
    void enemyBlockLogic(float dt);   // Tangkis

    void spawnEffect(float x, float y, std::string type, bool flip = false);
    void updateEffects(float dt);
    void renderEffects(SDL_Renderer* r);
    void renderGround(SDL_Renderer* r, int winW, int winH);

    // -- UI Rendering --
    void drawFancyHealthBar(SDL_Renderer* r, TextRenderer* t, 
                            int x, int y, int hp, int maxHp, 
                            const std::string& name, bool isLeft);
};