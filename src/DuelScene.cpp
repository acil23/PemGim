#include "DuelScene.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <algorithm> 
#include <cmath> 
#include "TextRenderer.hpp"
#include "Game.hpp"

DuelScene::DuelScene() 
    : change(false), next(""), gamePtr(nullptr) {}

// ==========================================================
// 1. SCENE LIFECYCLE
// ==========================================================

void DuelScene::onEnter(Game* game) {
    gamePtr = game;
    
    // 1. Music
    if (gamePtr) {
        gamePtr->stopMusic(); 
        gamePtr->playMusic("assets/sounds/fight_song.wav", -1); 
    }

    // 2. Reset State
    change = false;
    next = "";
    isGameOver = false;
    playerWon = false;
    enemyAlive = true;

    // 3. Reset Intro
    isIntro = true;
    introTimer = 0.0f;
    vsScale = 3.0f;
    vsAlpha = 0.0f;

    std::cout << "=== [DuelScene] START LOADING ASSETS ===\n";

    // 4. Load Visual Assets (Background & UI)
    auto loadOrError = [&](std::string path) -> SDL_Texture* {
        SDL_Surface* s = IMG_Load(path.c_str());
        if (!s) {
            std::cerr << "[ERROR] Gagal load: " << path << " -> " << IMG_GetError() << "\n";
            return nullptr;
        }
        SDL_Texture* t = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), s);
        SDL_FreeSurface(s);
        return t;
    };

    bgTexture = loadOrError("assets/images/fight_bg.png");
    healthBarTex = loadOrError("assets/images/health_bar.png");
    vsTex = loadOrError("assets/images/vs.png");

    // === LOAD NEW ASSETS ===
    groundTex = IMG_LoadTexture(gamePtr->getRenderer(), "assets/images/ground.png");
    if(!groundTex) std::cerr << "[Duel] Fail load ground\n";

    bloodTex = IMG_LoadTexture(gamePtr->getRenderer(), "assets/images/blood.png");
    if(!bloodTex) std::cerr << "[Duel] Fail load blood\n";

    dustTex = IMG_LoadTexture(gamePtr->getRenderer(), "assets/images/dust.png");
    if(!dustTex) std::cerr << "[Duel] Fail load dust\n";

    activeEffects.clear(); // Reset efek

    // 5. Setup Player Data
    int selectedCharID = 0;
    if (gamePtr) {
        const auto& ch = gamePtr->getChosenCharacter();
        playerName          = ch.displayName;
        player.hp           = ch.maxHP;
        player.maxHP        = ch.maxHP;
        playerAttackDamage  = ch.baseAttack;
        selectedCharID      = ch.characterID;
    } else {
        playerName          = "Pejuang";
        player.hp           = 100;
        player.maxHP        = 100;
        playerAttackDamage  = 15;
        selectedCharID      = 0;
    }

    enemyHP = 100; 
    enemyMaxHP = 100;

    std::string suffix = (selectedCharID > 0) ? std::to_string(selectedCharID + 1) : "";

    // --- LOAD PLAYER SPRITES ---
    std::cout << "--- Loading Player Sprites ---\n";
    player.texture = loadOrError("assets/images/player_attack" + suffix + ".png");
    if (player.texture) {
        int w, h; SDL_QueryTexture(player.texture, NULL, NULL, &w, &h);
        player.frameCount = 5; player.frameW = w/5; player.frameH = h;
    } else {
        std::cerr << "FATAL: Player texture failed!\n";
    }
    player.currentFrame = 0; player.attacking = false; player.attackTimer = 0.0f;

    playerJumpTex = loadOrError("assets/images/jump" + suffix + ".png");
    if (playerJumpTex) {
        int w, h; SDL_QueryTexture(playerJumpTex, NULL, NULL, &w, &h);
        jumpFrameCount = 5; jumpFrameW = w/5; jumpFrameH = h;
    }

    playerWalkTex = loadOrError("assets/images/walk" + suffix + ".png");
    if (playerWalkTex) {
        int w, h; SDL_QueryTexture(playerWalkTex, NULL, NULL, &w, &h);
        walkFrameCount = 5; walkFrameW = w/5; walkFrameH = h;
    }

    playerBlockTex = loadOrError("assets/images/block" + suffix + ".png");
    if (playerBlockTex) {
        int w, h; SDL_QueryTexture(playerBlockTex, NULL, NULL, &w, &h);
        blockFrameCount = 3; blockFrameW = w/3; blockFrameH = h;
    }


    // --- LOAD ENEMY SPRITES (DEBUGGED) ---
    std::cout << "--- Loading Enemy Sprites ---\n";
    
    // 1. IDLE
    enemy.texture = loadOrError("assets/images/enemy_idle.png");
    if (enemy.texture) {
        int w, h; SDL_QueryTexture(enemy.texture, NULL, NULL, &w, &h);
        enemy.frameCount = 1; enemy.frameW = w; enemy.frameH = h;
    } else {
        std::cerr << "FATAL: Enemy Idle texture failed!\n";
    }
    enemy.currentFrame = 0; enemy.attacking = false; enemy.attackTimer = 0.0f;

    // 2. ATTACK
    enemyAttackTex = loadOrError("assets/images/enemy_attack.png");
    if (enemyAttackTex) {
        int w, h; SDL_QueryTexture(enemyAttackTex, NULL, NULL, &w, &h);
        enemyAttackFrameCount = 5; enemyAttackFrameW = w/5; enemyAttackFrameH = h;
        std::cout << "[DEBUG] Enemy Attack OK.\n";
    }

    // 3. WALK
    enemyWalkTex = loadOrError("assets/images/enemy_walk.png");
    if (!enemyWalkTex) {
        std::cout << "[DEBUG] Enemy Walk NOT FOUND. Fallback to Idle.\n";
        enemyWalkTex = enemy.texture;
    } else {
        std::cout << "[DEBUG] Enemy Walk OK.\n";
    }

    // 4. JUMP
    enemyJumpTex = loadOrError("assets/images/enemy_jump.png");
    if (!enemyJumpTex) {
        std::cout << "[DEBUG] Enemy Jump NOT FOUND. Fallback to Idle.\n";
        enemyJumpTex = enemy.texture;
    } else {
        std::cout << "[DEBUG] Enemy Jump OK.\n";
    }

    // 5. BLOCK
    enemyBlockTex = loadOrError("assets/images/enemy_block.png");
    if (!enemyBlockTex) {
        std::cout << "[DEBUG] Enemy Block NOT FOUND. Fallback to Idle.\n";
        enemyBlockTex = enemy.texture;
    } else {
        std::cout << "[DEBUG] Enemy Block OK.\n";
    }

    std::cout << "=== [DuelScene] ASSETS LOADING FINISHED ===\n";

    // 6. POSISI AWAL
    int winW = 1280; int winH = 720;
    if (gamePtr) gamePtr->getWindowSize(winW, winH);
    float groundY = winH * 0.80f; 
    
    player.x = winW * 0.25f; player.y = groundY;
    enemy.x = winW * 0.75f; enemy.y = groundY;

    // Reset AI
    moveLeft = false; moveRight = false;
    aiDecisionTimer = 0.0f;
    enemyAttackTimer = 1.0f; 
    enemyMovingLeft = false; enemyMovingRight = false;
    enemyJumping = false; enemyBlocking = false;
}

void DuelScene::onExit() {
    if(bgTexture) SDL_DestroyTexture(bgTexture);
    if(healthBarTex) SDL_DestroyTexture(healthBarTex);
    if(vsTex) SDL_DestroyTexture(vsTex);
    
    if(player.texture) SDL_DestroyTexture(player.texture);
    if(playerJumpTex) SDL_DestroyTexture(playerJumpTex);
    if(playerWalkTex) SDL_DestroyTexture(playerWalkTex);
    if(playerBlockTex) SDL_DestroyTexture(playerBlockTex);

    if(enemy.texture) SDL_DestroyTexture(enemy.texture);
    if(enemyAttackTex) SDL_DestroyTexture(enemyAttackTex);
    
    // Hapus texture extra jika bukan copy pointer
    if(enemyWalkTex && enemyWalkTex != enemy.texture) SDL_DestroyTexture(enemyWalkTex);
    if(enemyJumpTex && enemyJumpTex != enemy.texture) SDL_DestroyTexture(enemyJumpTex);
    if(enemyBlockTex && enemyBlockTex != enemy.texture) SDL_DestroyTexture(enemyBlockTex);

    bgTexture = nullptr; healthBarTex = nullptr; vsTex = nullptr;

    if (groundTex) SDL_DestroyTexture(groundTex);
    if (bloodTex) SDL_DestroyTexture(bloodTex);
    if (dustTex) SDL_DestroyTexture(dustTex);
    
    activeEffects.clear();
}

// --- HELPER EFFECT SYSTEM ---

void DuelScene::spawnEffect(float x, float y, std::string type, bool flip) {
    VisualEffect fx;
    fx.x = x;
    fx.y = y;
    fx.timer = 0.0f;
    fx.currentFrame = 0;
    fx.flip = flip;

    if (type == "BLOOD" && bloodTex) {
        fx.texture = bloodTex;
        // Asumsi blood.png: 5 kolom, 3 baris. Kita pakai baris pertama (splash)
        int w, h; SDL_QueryTexture(bloodTex, NULL, NULL, &w, &h);
        fx.frameW = w / 5; 
        fx.frameH = h / 3; 
        fx.maxFrames = 5;
        fx.frameDuration = 0.08f; // Cepat
    } 
    else if (type == "DUST" && dustTex) {
        fx.texture = dustTex;
        // Asumsi dust.png: 4 kolom, 2 baris. Kita pakai baris kedua (smoke puff)
        int w, h; SDL_QueryTexture(dustTex, NULL, NULL, &w, &h);
        fx.frameW = w / 4; 
        fx.frameH = h / 2; 
        fx.maxFrames = 4;
        fx.frameDuration = 0.1f;
    } 
    else return;

    activeEffects.push_back(fx);
}

void DuelScene::updateEffects(float dt) {
    for (auto it = activeEffects.begin(); it != activeEffects.end(); ) {
        it->timer += dt;
        if (it->timer >= it->frameDuration) {
            it->timer = 0.0f;
            it->currentFrame++;
            if (it->currentFrame >= it->maxFrames) {
                it = activeEffects.erase(it); // Hapus jika animasi selesai
                continue;
            }
        }
        ++it;
    }
}

void DuelScene::renderEffects(SDL_Renderer* r) {
    for (const auto& fx : activeEffects) {
        SDL_Rect src;
        src.w = fx.frameW;
        src.h = fx.frameH;
        src.x = fx.currentFrame * fx.frameW;
        
        // Pilih baris sprite (Opsional, sesuaikan dengan gambar)
        if (fx.texture == dustTex) src.y = fx.frameH; // Baris 2 untuk debu lari
        else src.y = 0; // Baris 1 untuk darah

        // Scale up dikit biar keliatan
        float scale = 2.0f; 
        SDL_Rect dst;
        dst.w = (int)(fx.frameW * scale);
        dst.h = (int)(fx.frameH * scale);
        dst.x = (int)(fx.x - dst.w / 2);
        dst.y = (int)(fx.y - dst.h / 2);

        SDL_RenderCopyEx(r, fx.texture, &src, &dst, 0, NULL, fx.flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }
}

void DuelScene::renderGround(SDL_Renderer* r, int winW, int winH) {
    if (!groundTex) return;
    
    int w, h; SDL_QueryTexture(groundTex, NULL, NULL, &w, &h);
    
    // Tile ground horizontally
    // Kita taruh ground di bagian bawah layar
    int groundY = (int)(winH * 0.83f); // Sesuaikan agar kaki player pas di tanah
    
    // Scale ground biar gak terlalu kecil pixelnya
    float scale = 1.5f; 
    int drawW = (int)(w * scale);
    int drawH = (int)(h * scale);

    // Loop gambar sampai memenuhi lebar layar
    for (int x = 0; x < winW; x += drawW) {
        SDL_Rect dst = {x, groundY, drawW, drawH};
        SDL_RenderCopy(r, groundTex, NULL, &dst);
    }
}

// ==========================================================
// 2. INPUT HANDLING
// ==========================================================

void DuelScene::handleEvent(const SDL_Event& e) {
    if (isIntro || isGameOver) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) { change = true; next = "menu"; }
            if (isGameOver && e.key.keysym.sym == SDLK_RETURN) { 
                change = true; next = "post_duel_story"; 
            }
        }
        return;
    }

    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode key = e.key.keysym.sym;
        if (key == SDLK_ESCAPE) { change = true; next = "menu"; }
        else if (key == SDLK_LEFT) moveLeft = true;
        else if (key == SDLK_RIGHT) moveRight = true;
        else if (key == SDLK_a) {
            if (!player.attacking && !jumping && !blocking) startPlayerAttack();
        }
        else if (key == SDLK_w || key == SDLK_SPACE) {
            if (!jumping && !player.attacking && !blocking) startPlayerJump();
        }
        else if (key == SDLK_s) {
            if (!player.attacking && !jumping) startBlock();
        }
        else if (key == SDLK_F1) { enemyHP = 0; enemyAlive = false; }
    }
    else if (e.type == SDL_KEYUP) {
        SDL_Keycode key = e.key.keysym.sym;
        if (key == SDLK_LEFT) moveLeft = false;
        if (key == SDLK_RIGHT) moveRight = false;
        if (key == SDLK_s) stopBlock();
    }
}

// ==========================================================
// 3. LOGIC UPDATE
// ==========================================================

void DuelScene::update(float dt) {
    if (isIntro) {
        introTimer += dt;
        if (introTimer < 0.5f) {
            float t = introTimer / 0.5f; vsScale = 3.0f - (2.0f * t); vsAlpha = t * 255.0f;
        } else if (introTimer < 2.0f) {
            vsScale = 1.0f; vsAlpha = 255.0f;
        } else if (introTimer < 2.5f) {
            float t = (introTimer - 2.0f) / 0.5f; vsScale = 1.0f + (t * 2.0f); vsAlpha = 255.0f - (t * 255.0f);
        } else {
            isIntro = false;
        }
        return; 
    }

    if (!isGameOver) {
        updatePlayerMovement(dt);
        updatePlayerAttack(dt);
        updatePlayerJump(dt);
        updatePlayerWalk(dt);
        updateBlock(dt);
        updateEffects(dt);

        if (enemyAlive) {
            updateEnemyAI(dt); 
        }

        if (enemyHP <= 0 && enemyAlive) {
            enemyHP = 0; enemyAlive = false;
            isGameOver = true; playerWon = true;
        }
        if (player.hp <= 0) {
            player.hp = 0;
            isGameOver = true; playerWon = false;
        }
    }
}

// ==========================================================
// 4. PLAYER LOGIC
// ==========================================================

void DuelScene::updatePlayerMovement(float dt) {
    if (!player.attacking) {
        float dx = 0.0f;
        if (moveLeft)  dx -= playerSpeed * dt;
        if (moveRight) dx += playerSpeed * dt;
        player.x += dx;

        int winW = 1280; int winH = 720;
        if (gamePtr) gamePtr->getWindowSize(winW, winH);

        if (player.x < 50) player.x = 50;
        if (player.x > winW - 50) player.x = winW - 50;
        
        player.y = winH * 0.80f; 

        // --- SPAWN DUST ---
        if ((moveLeft || moveRight) && !jumping) {
            playerDustTimer += dt;
            if (playerDustTimer > 0.3f) { // Muncul debu setiap 0.3 detik
                spawnEffect(player.x, player.y, "DUST", !playerFacingRight);
                playerDustTimer = 0.0f;
            }
        } else {
            playerDustTimer = 0.3f; // Reset biar pas jalan langsung keluar debu
        }
        
        // Update Y Position agar pas di atas Ground baru
        // GroundY di renderGround ada di 0.83f, jadi player harus di sekitar itu
        float groundLevel = winH * 0.83f + 10; // +10 biar kaki agak masuk dikit ke tanah
        player.y = groundLevel;
        
        player.x += dx;
    }
}

void DuelScene::startPlayerAttack() {
    player.attacking = true;
    player.attackTimer = 0.0f;
    player.currentFrame = 0;
    hasHitThisSwing = false;
    if (gamePtr) gamePtr->playSound("assets/sounds/sword_sound.wav");
}

void DuelScene::updatePlayerAttack(float dt) {
    if (!player.attacking) return;
    player.attackTimer += dt;
    int frame = (int)(player.attackTimer / player.attackFrameDuration);
    if (frame >= player.frameCount) {
        player.attacking = false; player.currentFrame = 0;
    } else {
        player.currentFrame = frame;
        checkHitAndDamageEnemy();
    }
}

void DuelScene::checkHitAndDamageEnemy() {
    if (!enemyAlive || hasHitThisSwing) return;
    
    float dist = std::abs(player.x - enemy.x);
    // Hit range 100px
    if (dist < 100) {
        if (enemyBlocking) {
            std::cout << "Enemy Blocked!\n";
            if (gamePtr) gamePtr->playSound("assets/sounds/sword_block_sound.wav");
            if(player.x < enemy.x) player.x -= 30; else player.x += 30;
        } else {
            float bloodY = enemy.y - (rand() % 50 + 40); 
            spawnEffect(enemy.x, bloodY, "BLOOD", !playerFacingRight);
            enemyHP -= playerAttackDamage;
            if (enemyHP < 0) enemyHP = 0;
            std::cout << "Hit Enemy! " << enemyHP << "\n";
        }
        hasHitThisSwing = true;
    }
}

void DuelScene::startPlayerJump() { jumping = true; jumpTimer = 0.0f; }
void DuelScene::updatePlayerJump(float dt) { 
    if (!jumping) return;
    jumpTimer += dt;
    int frame = (int)(jumpTimer / jumpFrameDuration);
    if (frame >= jumpFrameCount) { jumping = false; jumpYOffset = 0; }
    else { 
        jumpCurrentFrame = frame; 
        static const int OFFS[] = {0, -20, -40, -20, 0}; 
        if(frame < 5) jumpYOffset = OFFS[frame] * 2; 
    }
}

void DuelScene::updatePlayerWalk(float dt) {
    if (moveLeft || moveRight) {
        walkTimer += dt;
        if (walkTimer > 0.09f) {
            walkTimer = 0;
            walkCurrentFrame = (walkCurrentFrame + 1) % walkFrameCount;
        }
    } else walkCurrentFrame = 0;
}

void DuelScene::startBlock() { blocking = true; blockTimer = 0.0f; blockCurrentFrame = 0; }
void DuelScene::stopBlock() { blocking = false; }
void DuelScene::updateBlock(float dt) {
    if(blocking && blockCurrentFrame < 1) blockCurrentFrame = 1; 
}

void DuelScene::updateFacing() {
    playerFacingRight = (player.x <= enemy.x);
    enemyFacingRight = (enemy.x > player.x);
}

// ==========================================================
// 5. ENEMY AI (AGGRESSIVE VERSION)
// ==========================================================

void DuelScene::startEnemyAttack() {
    if (!enemyAlive) return;
    enemy.attacking = true;
    enemy.attackTimer = 0.0f;
    enemy.currentFrame = 0;
    hasEnemyHitThisSwing = false;
    // Debug
    // std::cout << "AI: Attack Start!\n";
}

void DuelScene::updateEnemyAttack(float dt) {
    if (!enemy.attacking) return;
    
    enemy.attackTimer += dt;
    int frame = (int)(enemy.attackTimer / enemy.attackFrameDuration);
    
    if (frame >= enemyAttackFrameCount) {
        enemy.attacking = false;
        enemy.currentFrame = 0;
        // Random cooldown 1.0 - 2.0 detik agar tidak terlalu robot
        enemyAttackTimer = 1.0f + ((rand() % 10) / 10.0f); 
    } else {
        enemy.currentFrame = frame;
        checkEnemyHitAndDamagePlayer();
    }
}

void DuelScene::checkEnemyHitAndDamagePlayer() {
    if (hasEnemyHitThisSwing) return;
    float dist = std::abs(player.x - enemy.x);
    
    // Frame hit biasanya frame 2 atau 3
    if (enemy.currentFrame >= 2 && dist < 100) {
        if (blocking) {
            if (gamePtr) gamePtr->playSound("assets/sounds/sword_block_sound.wav");
        } else {
            // MUNCULKAN DARAH DI POSISI PLAYER
            float bloodY = player.y - (rand() % 50 + 40);
            spawnEffect(player.x, bloodY, "BLOOD", enemyFacingRight);
            player.hp -= enemyAttackDamage;
            if(player.hp < 0) player.hp = 0;
            std::cout << "AI Hit Player! HP: " << player.hp << "\n";
        }
        hasEnemyHitThisSwing = true;
    }
}

void DuelScene::updateEnemyAI(float dt) {
    // 1. Update Animations Logic
    updateEnemyAttack(dt);
    enemyJumpLogic(dt);
    enemyBlockLogic(dt);
    enemyMoveLogic(dt);

    // Jika sedang menyerang atau block, AI tidak mikir jalan
    if (enemy.attacking || enemyBlocking) return;

    // 2. Reduce Cooldown
    enemyAttackTimer -= dt;

    // 3. AI Decision (Brain)
    float dist = std::abs(player.x - enemy.x);
    
    // -- PERBAIKAN: LOGIKA LEBIH SIMPEL & AGRESIF --
    
    // Case A: Player Dekat (< 100px)
    if (dist < 100) {
        enemyMovingLeft = false;
        enemyMovingRight = false;

        // Kalau cooldown habis -> SERANG!
        if (enemyAttackTimer <= 0.0f) {
            startEnemyAttack();
        }
        // Kalau belum siap serang, coba Block jika player nyerang
        else if (player.attacking && !enemyBlocking) {
            if (rand() % 100 < 50) { // 50% chance reflex block
                enemyBlocking = true; 
                enemyBlockTimer = 0.5f; 
            }
        }
    } 
    // Case B: Player Jauh -> KEJAR!
    else {
        if (player.x < enemy.x) {
            enemyMovingLeft = true; enemyMovingRight = false;
        } else {
            enemyMovingRight = true; enemyMovingLeft = false;
        }
    }
}

void DuelScene::enemyMoveLogic(float dt) {
    if (enemy.attacking || enemyBlocking) return;
    
    float speed = 140.0f; // Kecepatan jalan musuh
    float dx = 0.0f;
    if (enemyMovingLeft) dx -= speed * dt;
    if (enemyMovingRight) dx += speed * dt;
    
    enemy.x += dx;
    
    // Update Walk Animation
    if (dx != 0) {
        enemyWalkTimer += dt;
        if (enemyWalkTimer > 0.1f) {
            enemyWalkTimer = 0;
            enemyWalkFrame = (enemyWalkFrame + 1) % 5; // Asumsi 5 frame walk
        }
    } else {
        enemyWalkFrame = 0;
    }

    // --- ENEMY DUST ---
    if (dx != 0.0f && !enemyJumping) {
        enemyDustTimer += dt;
        if (enemyDustTimer > 0.3f) {
            spawnEffect(enemy.x, enemy.y, "DUST", enemyFacingRight);
            enemyDustTimer = 0.0f;
        }
    }
    
    // Sesuaikan Y Enemy juga
    int winW, winH; gamePtr->getWindowSize(winW, winH);
    enemy.y = winH * 0.83f + 10;
}

void DuelScene::enemyJumpLogic(float dt) {
    if(!enemyJumping) return;
    enemyJumpTimer += dt;
    if(enemyJumpTimer > 0.6f) { enemyJumping = false; enemyJumpYOffset = 0; }
    else {
        if(enemyJumpTimer < 0.3f) enemyJumpYOffset = -40; else enemyJumpYOffset = 0;
    }
}

void DuelScene::enemyBlockLogic(float dt) {
    if(!enemyBlocking) return;
    enemyBlockTimer -= dt;
    if(enemyBlockTimer <= 0.0f) enemyBlocking = false;
}

// ==========================================================
// 6. RENDER IMPLEMENTATION
// ==========================================================

void DuelScene::drawFancyHealthBar(SDL_Renderer* r, TextRenderer* t, 
                                   int x, int y, int hp, int maxHp, 
                                   const std::string& name, bool isLeft) 
{
    if (!healthBarTex) return;

    int barW, barH;
    SDL_QueryTexture(healthBarTex, NULL, NULL, &barW, &barH);
    int displayW = 400; 
    int displayH = (int)((float)displayW / barW * barH);

    // Draw Fill
    int marginX = (int)(displayW * 0.295f);
    int marginY = (int)(displayH * 0.43f);
    int fillMaxWidth = (int)(displayW * 0.63f);
    int fillHeight = (int)(displayH * 0.15f);

    float ratio = (maxHp > 0) ? (float)hp / (float)maxHp : 0.0f;
    int currentFillW = (int)(fillMaxWidth * ratio);

    if (isLeft) SDL_SetRenderDrawColor(r, 50, 200, 50, 255);
    else SDL_SetRenderDrawColor(r, 200, 50, 50, 255);

    SDL_Rect fillRect;
    if (isLeft) fillRect = {x + marginX, y + marginY, currentFillW, fillHeight};
    else fillRect = {x + displayW - marginX - currentFillW, y + marginY, currentFillW, fillHeight};
    SDL_RenderFillRect(r, &fillRect);

    // Draw Frame
    SDL_Rect dst = {x, y, displayW, displayH};
    SDL_RendererFlip flip = isLeft ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(r, healthBarTex, NULL, &dst, 0, NULL, flip);

    // Draw Text
    t->setFontSize(18);
    int textY = y + (displayH / 4); 
    if (isLeft) {
        t->drawText(r, name, x + 30, textY - 30, {255, 255, 255, 255});
    } else {
        int nameW = t->measureWidth(r, name);
        t->drawText(r, name, x + displayW - nameW - 30, textY - 30, {255, 255, 255, 255});
    }
}

void DuelScene::render(SDL_Renderer* renderer, TextRenderer* text) {
    int winW = 1280, winH = 720;
    if (gamePtr) gamePtr->getWindowSize(winW, winH);

    // BG
    if (bgTexture) SDL_RenderCopy(renderer, bgTexture, NULL, NULL);
    else { SDL_SetRenderDrawColor(renderer, 100,80,50,255); SDL_RenderClear(renderer); }

    renderGround(renderer, winW, winH);

    updateFacing();

    // --- PLAYER RENDER ---
    if (player.texture) {
        SDL_Texture* tex = player.texture;
        // Default frame size dari load data
        int fw = player.frameW; 
        int fh = player.frameH;
        int fIdx = 0;

        if (player.attacking) {
            // Texture attack biasanya sudah diset frameW-nya dengan benar saat load
            fIdx = player.currentFrame;
        }
        else if (jumping && playerJumpTex) { 
            tex = playerJumpTex; 
            // Cek dimensi texture jump asli
            int tw, th; SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
            // Jika texture jump beda file, hitung frame baru
            fw = tw / 5; fh = th; 
            fIdx = jumpCurrentFrame;
        }
        else if (blocking && playerBlockTex) { 
            tex = playerBlockTex; 
            int tw, th; SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
            fw = tw / 3; fh = th;
            fIdx = blockCurrentFrame;
        }
        else if ((moveLeft||moveRight) && playerWalkTex) { 
            tex = playerWalkTex; 
            int tw, th; SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
            fw = tw / 5; fh = th;
            fIdx = walkCurrentFrame;
        }
        
        SDL_Rect src = {fIdx * fw, 0, fw, fh};
        
        float scale = 1.5f;
        SDL_Rect dst = {(int)player.x - (int)(src.w*scale)/2, (int)player.y - (int)(src.h*scale) + (jumping ? jumpYOffset : 0), (int)(src.w*scale), (int)(src.h*scale)};
        SDL_RenderCopyEx(renderer, tex, &src, &dst, 0, NULL, playerFacingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
    }

    // --- ENEMY RENDER (PERBAIKAN BLOCK FRAME) ---
    if (enemyAlive) {
        SDL_Texture* enTex = enemy.texture; // Default Idle
        
        int tw, th;
        SDL_QueryTexture(enTex, NULL, NULL, &tw, &th);
        
        int eFw = tw; 
        int eFh = th;
        int eIdx = 0;
        int yOff = 0; // Reset Y Offset

        if (enemy.attacking && enemyAttackTex) { 
            enTex = enemyAttackTex; 
            SDL_QueryTexture(enTex, NULL, NULL, &tw, &th);
            eFw = tw / 5; eFh = th; 
            eIdx = enemy.currentFrame;
        }
        else if (enemyBlocking && enemyBlockTex) { 
            enTex = enemyBlockTex;
            SDL_QueryTexture(enTex, NULL, NULL, &tw, &th);
            
            if (enTex == enemy.texture) { 
                // Fallback ke Idle
                eFw = tw; eIdx = 0; 
            } else { 
                // PERBAIKAN DISINI: Bagi 3 karena block ada 3 frame
                eFw = tw / 3; 
                eFh = th;
                // Kita pakai frame ke-0 atau ke-2 (terakhir) agar terlihat menahan
                eIdx = 1; 
            } 
        }
        else if (enemyJumping && enemyJumpTex) { 
            enTex = enemyJumpTex; 
            SDL_QueryTexture(enTex, NULL, NULL, &tw, &th);
            
            yOff = enemyJumpYOffset; // Terapkan offset lompat

            if (enTex == enemy.texture) { 
                eFw = tw; eIdx = 0; 
            } else { 
                eFw = tw / 5; eFh = th; // Asumsi Jump ada 5 frame
                // Pilih frame tengah (2) saat di udara
                eIdx = 2; 
            } 
        }
        else if ((enemyMovingLeft||enemyMovingRight) && enemyWalkTex) { 
            enTex = enemyWalkTex; 
            SDL_QueryTexture(enTex, NULL, NULL, &tw, &th);
            
            if (enTex == enemy.texture) {
                eFw = tw; eIdx = 0; 
            } else {
                eFw = tw / 5; // Walk ada 5 frame
                eIdx = enemyWalkFrame;
            }
        }

        SDL_Rect srcE = {eIdx * eFw, 0, eFw, eFh};
        
        float scale = 1.5f;
        SDL_Rect dstE = {(int)enemy.x - (int)(srcE.w*scale)/2, (int)enemy.y - (int)(srcE.h*scale) + yOff, (int)(srcE.w*scale), (int)(srcE.h*scale)};
        SDL_RenderCopyEx(renderer, enTex, &srcE, &dstE, 0, NULL, enemyFacingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
    }

    renderEffects(renderer);

    // UI & Intro
    drawFancyHealthBar(renderer, text, 20, 20, player.hp, player.maxHP, playerName, true);
    drawFancyHealthBar(renderer, text, winW - 420, 20, enemyHP, enemyMaxHP, "Jawara Quraysh", false);

    if (isIntro && vsTex) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0,0,0,150);
        SDL_Rect fs = {0,0,winW,winH}; SDL_RenderFillRect(renderer, &fs);
        
        int w,h; SDL_QueryTexture(vsTex,NULL,NULL,&w,&h);
        int dw = (int)(w*vsScale), dh = (int)(h*vsScale);
        SDL_Rect dst = {(winW-dw)/2, (winH-dh)/2, dw, dh};
        SDL_SetTextureAlphaMod(vsTex, (Uint8)std::clamp(vsAlpha,0.0f,255.0f));
        SDL_RenderCopy(renderer, vsTex, NULL, &dst);
    }

    if (isGameOver) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0,0,0,200);
        SDL_Rect fs = {0,0,winW,winH}; SDL_RenderFillRect(renderer, &fs);
        text->setFontSize(60);
        if(playerWon) text->drawText(renderer, "VICTORY!", winW/2 - 100, winH/2 - 50, {0,255,0,255});
        else text->drawText(renderer, "DEFEATED...", winW/2 - 120, winH/2 - 50, {255,0,0,255});
        text->setFontSize(24);
        text->drawText(renderer, "[ENTER] Continue", winW/2 - 80, winH/2 + 50, {255,255,255,255});
    }
}