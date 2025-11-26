#include "DuelScene.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <algorithm> // std::max
#include "TextRenderer.hpp"
#include "Game.hpp"

void DuelScene::onEnter(Game* game) {
    gamePtr = game;
    
    // Play fight music
    if (gamePtr) {
        gamePtr->stopMusic();
        gamePtr->playMusic("../assets/sounds/fight_song.wav", -1);
    }
    change = false;
    next = "";

    enemyAlive = true;

    // Ambil data karakter terpilih dari Game
    int selectedCharID = 0; // default char 1
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

    // Musuh
    enemyHP     = 100;
    enemyMaxHP  = 100;

    // Posisi awal (kita taruh di ground kira2)
    // Nanti kita ambil window size buat penempatan musuh
    int winW = 1280;
    int winH = 720;
    if (gamePtr) {
        gamePtr->getWindowSize(winW, winH);
    }

    // --- Load sprite player (spritesheet serangan) ---
    {
        // Build filename based on character ID
        std::string attackFile = "../assets/images/player_attack";
        if (selectedCharID > 0) {
            attackFile += std::to_string(selectedCharID + 1); // char 0->player_attack.png, 1->player_attack2.png, 2->player_attack3.png
        }
        attackFile += ".png";
        
        SDL_Surface* surf = IMG_Load(attackFile.c_str());
        if (!surf) {
            std::cerr << "[DuelScene] Failed to load " << attackFile << ": " << IMG_GetError() << "\n";
        } else {
            player.texture = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), surf);

            // kita tau gambar punya N frame attack, asumsikan 5
            player.frameCount = 5;
            player.frameW = surf->w / player.frameCount;
            player.frameH = surf->h;
            player.currentFrame = 0;
            player.attacking = false;
            player.attackTimer = 0.0f;

            SDL_FreeSurface(surf);
        }
    }

    // --- Load sprite enemy (1 frame idle) ---
    {
        SDL_Surface* surf = IMG_Load("../assets/images/enemy_idle.png");
        if (!surf) {
            std::cerr << "[DuelScene] Failed to load enemy_idle.png: " << IMG_GetError() << "\n";
        } else {
            enemy.texture = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), surf);
            enemy.frameCount = 1;
            enemy.frameW = surf->w;
            enemy.frameH = surf->h;
            enemy.currentFrame = 0;
            enemy.attacking = false;
            enemy.attackTimer = 0.0f;
            enemy.hp = enemyHP;
            enemy.maxHP = enemyMaxHP;

            SDL_FreeSurface(surf);
        }
    }

    // --- Load sprite enemy attack (5 frame) ---
    {
        SDL_Surface* surf = IMG_Load("../assets/images/enemy_attack.png");
        if (!surf) {
            std::cerr << "[DuelScene] Failed to load enemy_attack.png: " << IMG_GetError() << "\n";
        } else {
            enemyAttackTex = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), surf);
            enemyAttackFrameCount = 5;
            enemyAttackFrameW = surf->w / enemyAttackFrameCount;
            enemyAttackFrameH = surf->h;
            SDL_FreeSurface(surf);
        }
    }
    enemyAttackTimer = enemyAttackCooldown; // start ready to attack

    // --- Load sprite player jump (5 frame) ---
    {
        std::string jumpFile = "../assets/images/jump";
        if (selectedCharID > 0) {
            jumpFile += std::to_string(selectedCharID + 1);
        }
        jumpFile += ".png";
        
        SDL_Surface* s = IMG_Load(jumpFile.c_str());
        if (!s) {
            std::cerr << "[DuelScene] load " << jumpFile << " fail: " << IMG_GetError() << "\n";
        } else {
            playerJumpTex = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), s);
            jumpFrameCount = 5;
            jumpFrameW = s->w / jumpFrameCount;
            jumpFrameH = s->h;
            SDL_FreeSurface(s);
        }
        jumping = false; jumpCurrentFrame = 0; jumpTimer = 0.0f;
    }

    // --- Load sprite player walk (6 frame) ---
    {
        std::string walkFile = "../assets/images/walk";
        if (selectedCharID > 0) {
            walkFile += std::to_string(selectedCharID + 1);
        }
        walkFile += ".png";
        
        SDL_Surface* s = IMG_Load(walkFile.c_str()); // simpan walk.png ke path ini
        if (!s) {
            std::cerr << "[DuelScene] Failed to load " << walkFile << ": " << IMG_GetError() << "\n";
        } else {
            playerWalkTex = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), s);
            walkFrameCount = 5;                 // sesuai sheet yang kamu kirim
            walkFrameW = s->w / walkFrameCount;
            walkFrameH = s->h;
            SDL_FreeSurface(s);
        }
        walkCurrentFrame = 0;
        walkTimer = 0.0f;
    }

    // --- Load sprite player block (3 frame) ---
    {
        std::string blockFile = "../assets/images/block";
        if (selectedCharID > 0) {
            blockFile += std::to_string(selectedCharID + 1);
        }
        blockFile += ".png";
        
        SDL_Surface* s = IMG_Load(blockFile.c_str()); // simpan block.png ke path ini
        if (!s) {
            std::cerr << "[DuelScene] Failed to load " << blockFile << ": "
                    << IMG_GetError() << "\n";
        } else {
            playerBlockTex = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), s);
            blockFrameCount = 3;
            blockFrameW = s->w / blockFrameCount;
            blockFrameH = s->h;
            SDL_FreeSurface(s);
        }
        blocking = false;
        blockCurrentFrame = 0;
        blockTimer = 0.0f;
        blockForward = true;
    }


    // letakkan player di kiri tanah
    player.x = winW * 0.25f;
    player.y = winH * 0.65f; // kira-kira lantai

    // letakkan musuh di kanan tanah
    enemy.x = winW * 0.65f;
    enemy.y = winH * 0.65f;

    moveLeft = false;
    moveRight = false;

    std::cout << "\n[DuelScene]\n";
    std::cout << "Kamu bermain sebagai: " << playerName << "\n";
    std::cout << "Kontrol:\n";
    std::cout << "  LEFT/RIGHT: Gerak\n";
    std::cout << "  A: Serang\n";
    std::cout << "  ENTER: akhiri duel jika musuh kalah\n";
    std::cout << "  ESC: mundur & kembali menu\n";
    std::cout << "---------------------------------\n";
}

void DuelScene::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode key = e.key.keysym.sym;

        if (key == SDLK_ESCAPE) {
            change = true;
            next  = "menu";
            std::cout << "[Duel] Mundur. Kembali ke menu.\n";
        }
        else if (key == SDLK_RETURN) {
            if (!enemyAlive || enemyHP <= 0) {
                std::cout << "[Duel] Musuh tumbang. Lanjut ke ending.\n";
                change = true;
                next  = "ending";
            } else {
                std::cout << "[Duel] Musuh masih berdiri...\n";
            }
        }
        else if (key == SDLK_LEFT) {
            moveLeft = true;
        }
        else if (key == SDLK_RIGHT) {
            moveRight = true;
        }
        else if (key == SDLK_a) {
            // mulai animasi serangan (kalau belum menyerang)
            if (!player.attacking && !jumping && !blocking) {
                startPlayerAttack();
            }
        }
        else if (key == SDLK_w || key == SDLK_SPACE) {
            if (!jumping && !player.attacking && !blocking) {
                startPlayerJump();
            }
        }else if (key == SDLK_s) {
            // mulai block kalau tidak sedang attack/jump
            if (!player.attacking && !jumping) {
                startBlock();
            }
        }
    }else if (e.type == SDL_KEYUP) {
        SDL_Keycode key = e.key.keysym.sym;

        if (key == SDLK_LEFT) {
            moveLeft = false;
        }
        else if (key == SDLK_RIGHT) {
            moveRight = false;
        }else if (key == SDLK_s) {
            stopBlock();
        }
    }

    if (isGameOver) {
        if (e.type == SDL_KEYDOWN) {
             if (e.key.keysym.sym == SDLK_RETURN) {
                 change = true;
                 next = "post_duel_story"; // Baik menang/kalah, lihat cerita asli
             } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                 change = true;
                 next = "menu";
             }
        }
        return; // Stop input lain (gerak/serang)
    }
}

void DuelScene::startPlayerAttack() {
    player.attacking = true;
    player.attackTimer = 0.0f;
    player.currentFrame = 1; // mulai di frame serang, bukan idle 0
    hasHitThisSwing = false; 
    
    // Play sword sound
    if (gamePtr) {
        gamePtr->playSound("../assets/sounds/sword_sound.wav");
    }
}

void DuelScene::update(float dt) {
    // player gerak kiri/kanan
    updatePlayerMovement(dt);

    // update animasi serangan
    updatePlayerAttack(dt);

    updatePlayerJump(dt);
    updatePlayerWalk(dt);
    updateBlock(dt);
    
    // enemy attack logic
    updateEnemyAttack(dt);

    // Cek Menang
    if (enemyHP <= 0 && enemyAlive) {
        enemyAlive = false;
        isGameOver = true;
        playerWon = true;
    }
    // Cek Kalah
    if (player.hp <= 0 && !isGameOver) {
        isGameOver = true;
        playerWon = false;
    }

    // // cek kalau musuh udah habis HP
    // if (enemyHP <= 0 && enemyAlive) {
    //     enemyHP = 0;
    //     enemyAlive = false;
    //     std::cout << "[Duel] Musuh kalah!\n";
    // }

    // // cek kalau player jatuh (HP <= 0)
    // if (player.hp <= 0) {
    //     std::cout << "[Duel] Kamu tumbang! (sementara langsung ending)\n";
    //     change = true;
    //     next = "ending";
    // }
}

void DuelScene::updatePlayerMovement(float dt) {
    if (!player.attacking) {
        float dx = 0.0f;
        if (moveLeft)  dx -= playerSpeed * dt;
        if (moveRight) dx += playerSpeed * dt;
        player.x += dx;

        int winW = 1280, winH = 720;
        if (gamePtr) gamePtr->getWindowSize(winW, winH);

        float spriteScale = std::clamp(winH / 720.0f, 1.0f, 2.4f);
        float playerVisualW = player.frameW * spriteScale;

        // batas layar kiri/kanan saja
        float minX = playerVisualW * 0.5f + 10.0f;
        float maxX = winW - playerVisualW * 0.5f - 10.0f;

        if (player.x < minX) player.x = minX;
        if (player.x > maxX) player.x = maxX;

        // y tetap di tanah
        float groundY = winH * 0.84f;
        player.y = groundY;
    }
}

void DuelScene::updateFacing() {
    // Player menghadap ke musuh
    playerFacingRight = (player.x <= enemy.x);
    // Musuh menghadap ke player
    enemyFacingRight = (enemy.x > player.x);
}


void DuelScene::updatePlayerAttack(float dt) {
    if (!player.attacking) return;

    player.attackTimer += dt;

    // ganti frame berdasarkan timer
    // attackFrameDuration = detik per frame
    int frameIndex = (int)(player.attackTimer / player.attackFrameDuration);

    if (frameIndex >= player.frameCount) {
        // animasi selesai -> balik idle
        player.attacking = false;
        player.currentFrame = 0;
        return;
    } else {
        player.currentFrame = frameIndex;
    }

    // selama frame serangan aktif, cek tabrakan dengan musuh
    checkHitAndDamageEnemy();
}

    void DuelScene::startPlayerJump() {
        if (!playerJumpTex) return;
        jumping = true;
        jumpTimer = 0.0f;
        jumpCurrentFrame = 0;
        
        // Play jump sound
        if (gamePtr) {
            gamePtr->playSound("../assets/sounds/jump.wav");
        }
    }

    void DuelScene::updatePlayerJump(float dt) {
        if (!jumping) return;

        jumpTimer += dt;

        int idx = (int)(jumpTimer / jumpFrameDuration);
        if (idx >= jumpFrameCount) {
            jumping = false;
            jumpTimer = 0.0f;
            jumpCurrentFrame = 0;
            jumpYOffset = 0;              // balik ke normal
            return;
        }
        jumpCurrentFrame = idx;

        // --- offset visual per frame (0..4) ---
        // target: frame-3 paling tinggi.
        // angka ini masih di-"scale" sesuai tinggi layar.
        static const int OFFS[5] = { 0, -10, -28, -10, 0 };

        int winW = 1280, winH = 720;
        if (gamePtr) gamePtr->getWindowSize(winW, winH);
        float spriteScale = std::clamp(winH / 720.0f, 1.0f, 2.4f);

        jumpYOffset = int(OFFS[jumpCurrentFrame] * spriteScale);
    }

    void DuelScene::updatePlayerWalk(float dt) {
     // Jalan hanya jika: tidak menyerang, tidak lompat, dan ada input gerak
        bool walkingNow = (!player.attacking && !jumping && (moveLeft || moveRight));

        if (!walkingNow) {
            // reset ke frame 0 saat berhenti
            walkCurrentFrame = 0;
            walkTimer = 0.0f;
            return;
        }

        walkTimer += dt;
        if (walkTimer >= walkFrameDuration) {
            walkTimer -= walkFrameDuration;
            walkCurrentFrame = (walkCurrentFrame + 1) % walkFrameCount;
        }
    }


    void DuelScene::startBlock() {
        if (!playerBlockTex) return;
        blocking = true;
        blockTimer = 0.0f;
        blockCurrentFrame = 0;
        blockForward = true;
    }

    void DuelScene::stopBlock() {
        blocking = false;
        blockTimer = 0.0f;
        blockCurrentFrame = 0;
    }
    static const int BLOCK_HOLD_FRAME = 1; // frame tengah yang mau ditahan
    void DuelScene::updateBlock(float dt) {
        if (!playerBlockTex) return;

        blockTimer += dt;

        if (blocking) {
            // Saat tombol S masih ditekan:
            // Naikkan animasi dari 0 -> BLOCK_HOLD_FRAME, lalu tahan di situ.
            if (blockCurrentFrame < BLOCK_HOLD_FRAME) {
                if (blockTimer >= blockFrameDuration) {
                    blockTimer -= blockFrameDuration;
                    blockCurrentFrame++;
                    if (blockCurrentFrame > BLOCK_HOLD_FRAME) {
                        blockCurrentFrame = BLOCK_HOLD_FRAME;
                    }
                }
            } else {
                // Sudah sampai frame hold -> tetap di sana
                blockCurrentFrame = BLOCK_HOLD_FRAME;
            }
        } else {
            // S sudah dilepas:
            // Kalau masih di atas 0, animasikan turun pelan ke 0.
            if (blockCurrentFrame > 0) {
                if (blockTimer >= blockFrameDuration) {
                    blockTimer -= blockFrameDuration;
                    blockCurrentFrame--;
                    if (blockCurrentFrame < 0) blockCurrentFrame = 0;
                }
            } else {
                // sudah kembali ke frame 0, reset timer saja
                blockTimer = 0.0f;
            }
        }
    }







void DuelScene::checkHitAndDamageEnemy() {
    if (!enemyAlive) return;
    if (hasHitThisSwing) return;

    int winW = 1280, winH = 720;
    if (gamePtr) gamePtr->getWindowSize(winW, winH);
    float spriteScale = std::clamp(winH / 720.0f, 1.0f, 2.4f);

    SDL_Rect enemyBox;
    enemyBox.w = int(enemy.frameW * spriteScale);
    enemyBox.h = int(enemy.frameH * spriteScale);
    enemyBox.x = int(enemy.x - enemyBox.w / 2);
    enemyBox.y = int(enemy.y - enemyBox.h);

    // --- hitbox serang arah kanan / kiri sesuai facing ---
    const int baseW = 80, baseH = 40, offset = 20, up = 60;
    SDL_Rect attackBox;
    attackBox.w = int(baseW * spriteScale);
    attackBox.h = int(baseH * spriteScale);
    attackBox.y = int(player.y - up * spriteScale);

    if (playerFacingRight) {
        attackBox.x = int(player.x + offset * spriteScale);
    } else {
        attackBox.x = int(player.x - offset * spriteScale - attackBox.w);
    }

    bool overlap = !( attackBox.x + attackBox.w < enemyBox.x ||
                      attackBox.x > enemyBox.x + enemyBox.w ||
                      attackBox.y + attackBox.h < enemyBox.y ||
                      attackBox.y > enemyBox.y + enemyBox.h );

    if (overlap) {
        enemyHP -= playerAttackDamage;
        if (enemyHP < 0) enemyHP = 0;
        hasHitThisSwing = true;
        std::cout << "[Hit] " << playerName << " memukul! HP musuh: " << enemyHP << "\n";
    }
}

void DuelScene::drawHealthBar(SDL_Renderer* renderer,
                              TextRenderer* text,
                              int x, int y,
                              int w, int h,
                              int hp,
                              int hpMax,
                              const char* label,
                              SDL_Color barColor,
                              SDL_Color borderColor,
                              SDL_Color textColor)
{
    if (hp < 0) hp = 0;
    if (hp > hpMax) hp = hpMax;

    SDL_Rect borderRect { x, y, w, h };

    SDL_SetRenderDrawColor(renderer,
        borderColor.r,
        borderColor.g,
        borderColor.b,
        borderColor.a);
    SDL_RenderDrawRect(renderer, &borderRect);

    float ratio = (hpMax > 0) ? (float)hp / (float)hpMax : 0.0f;
    int fillW = (int)(w * ratio);

    SDL_Rect fillRect { x+1, y+1, fillW-2 < 0 ? 0 : fillW-2, h-2 };

    SDL_SetRenderDrawColor(renderer,
        barColor.r,
        barColor.g,
        barColor.b,
        barColor.a);
    SDL_RenderFillRect(renderer, &fillRect);

    std::string hpText = std::string(label) + ": " +
                         std::to_string(hp) + "/" + std::to_string(hpMax);

    text->drawText(renderer,
        hpText,
        x,
        y - 22,
        textColor);
}

void DuelScene::render(SDL_Renderer* renderer, TextRenderer* text) {
    int winW = 1280, winH = 720;
    if (gamePtr) gamePtr->getWindowSize(winW, winH);

    // layout responsif
    float groundY = winH * 0.84f;
    float spriteScale = std::clamp(winH / 720.0f, 1.0f, 2.4f);

    // jaga posisi relatif
    player.y = groundY;
    enemy.y  = groundY;

    updateFacing();

    SDL_SetRenderDrawColor(renderer, 20, 60, 20, 255);
    SDL_RenderClear(renderer);

    // ===== PLAYER =====
    if (player.texture) {
        SDL_Texture* tex = nullptr;
        SDL_Rect src{}, dst{};

        int winW = 1280, winH = 720;
        if (gamePtr) gamePtr->getWindowSize(winW, winH);
        float spriteScale = std::clamp(winH / 720.0f, 1.0f, 2.4f);

        // default size
        dst.w = int(player.frameW * spriteScale);
        dst.h = int(player.frameH * spriteScale);

        // PRIORITAS: ATTACK > JUMP > BLOCK > WALK > IDLE
        if (player.attacking) {
            tex = player.texture; // attack sheet
            src = { player.currentFrame * player.frameW, 0, player.frameW, player.frameH };
        }
        else if (jumping && playerJumpTex) {
            tex = playerJumpTex;
            src = { jumpCurrentFrame * jumpFrameW, 0, jumpFrameW, jumpFrameH };
            dst.w = int(jumpFrameW * spriteScale);
            dst.h = int(jumpFrameH * spriteScale);
        }
        else if (blocking && playerBlockTex) {
            tex = playerBlockTex;
            src = { blockCurrentFrame * blockFrameW, 0, blockFrameW, blockFrameH };
            dst.w = int(blockFrameW * spriteScale);
            dst.h = int(blockFrameH * spriteScale);
        }
        else if ((moveLeft || moveRight) && playerWalkTex) {
            tex = playerWalkTex;
            src = { walkCurrentFrame * walkFrameW, 0, walkFrameW, walkFrameH };
            dst.w = int(walkFrameW * spriteScale);
            dst.h = int(walkFrameH * spriteScale);
        }
        else {
            tex = player.texture; // idle
            src = { 0, 0, player.frameW, player.frameH };
        }

        dst.x = int(player.x - dst.w / 2);
        int extraY = (jumping ? jumpYOffset : 0);
        dst.y = int(player.y - dst.h + extraY);

        SDL_RenderCopyEx(
            renderer,
            tex,
            &src,
            &dst,
            0.0,
            nullptr,
            playerFacingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
        );
    }



    // ===== ENEMY =====
    if (enemyAlive) {
        SDL_Texture* enemyTex = nullptr;
        SDL_Rect srcE;
        SDL_Rect dstE;
        
        // Choose texture based on attack state
        if (enemy.attacking && enemyAttackTex) {
            enemyTex = enemyAttackTex;
            srcE.x = enemy.currentFrame * enemyAttackFrameW;
            srcE.y = 0;
            srcE.w = enemyAttackFrameW;
            srcE.h = enemyAttackFrameH;
            dstE.w = int(enemyAttackFrameW * spriteScale);
            dstE.h = int(enemyAttackFrameH * spriteScale);
        } else if (enemy.texture) {
            enemyTex = enemy.texture;
            srcE.x = 0;
            srcE.y = 0;
            srcE.w = enemy.frameW;
            srcE.h = enemy.frameH;
            dstE.w = int(enemy.frameW * spriteScale);
            dstE.h = int(enemy.frameH * spriteScale);
        }
        
        if (enemyTex) {
            dstE.x = int(enemy.x - dstE.w / 2);
            dstE.y = int(enemy.y - dstE.h);
            
            SDL_RenderCopyEx(renderer, enemyTex, &srcE, &dstE,
                     0.0, nullptr,
                     enemyFacingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
        }
    }

    if (isGameOver) {
        // Gelapkan layar
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_Rect full = {0,0,winW,winH};
        SDL_RenderFillRect(renderer, &full);

        text->setFontSize(48);
        if (playerWon) {
            text->drawText(renderer, "MENANG!", winW/2 - 100, winH/2 - 50, {0, 255, 0, 255});
            text->setFontSize(24);
            text->drawText(renderer, "[ENTER] Lanjut Cerita", winW/2 - 120, winH/2 + 20, {255,255,255,255});
        } else {
            text->drawText(renderer, "TERLUKA PARAH...", winW/2 - 150, winH/2 - 50, {255, 0, 0, 255});
            text->setFontSize(24);
            text->drawText(renderer, "[ENTER] Lihat Kisah Asli  [ESC] Menu", winW/2 - 180, winH/2 + 20, {255,255,255,255});
        }
    }

    // ===== HP bars (tetap) =====
    const int barWidth = 250, barHeight = 20, marginTop = 50, marginSide = 40;

    drawHealthBar(renderer, text,
                  marginSide, marginTop, barWidth, barHeight,
                  player.hp, player.maxHP, playerName.c_str(),
                  SDL_Color{50,200,50,255}, SDL_Color{255,255,255,255}, SDL_Color{255,255,255,255});

    drawHealthBar(renderer, text,
                  winW - marginSide - barWidth, marginTop, barWidth, barHeight,
                  enemyHP, enemyMaxHP, "Musuh",
                  SDL_Color{200,50,50,255}, SDL_Color{255,255,255,255}, SDL_Color{255,255,255,255});

    // ===== bantuan kontrol =====
    int instrY = winH - 100;
    text->drawText(renderer, "[LEFT/RIGHT] Gerak", 20, instrY, SDL_Color{220,220,220,255});
    text->drawText(renderer, "[A] Serang", 20, instrY+24, SDL_Color{220,220,220,255});
    text->drawText(renderer,"[W/SPACE] Lompat",20,instrY + 96,SDL_Color{220,220,220,255});
    text->drawText(renderer, "[ENTER] Akhiri duel jika musuh tumbang", 20, instrY+48, SDL_Color{220,220,220,255});
    text->drawText(renderer, "[ESC] Mundur & kembali ke menu", 20, instrY+72, SDL_Color{220,220,220,255});

    text->drawText(renderer, "Duel!", winW/2 - 20, 10, SDL_Color{255,255,255,255});
}

void DuelScene::startEnemyAttack() {
    if (!enemyAlive) return;
    enemy.attacking = true;
    enemy.attackTimer = 0.0f;
    enemy.currentFrame = 1;
    hasEnemyHitThisSwing = false;
    std::cout << "[Enemy] Musuh menyerang!\n";
}

void DuelScene::updateEnemyAttack(float dt) {
    if (!enemyAlive) return;
    
    // cooldown timer for attacks
    if (!enemy.attacking) {
        enemyAttackTimer += dt;
        if (enemyAttackTimer >= enemyAttackCooldown) {
            enemyAttackTimer = 0.0f;
            startEnemyAttack();
        }
        return;
    }
    
    // update attack animation
    enemy.attackTimer += dt;
    int frameIndex = (int)(enemy.attackTimer / enemy.attackFrameDuration);
    
    if (frameIndex >= enemyAttackFrameCount) {
        enemy.attacking = false;
        enemy.currentFrame = 0;
        return;
    } else {
        enemy.currentFrame = frameIndex;
    }
    
    // check if enemy hit player
    checkEnemyHitAndDamagePlayer();
}

void DuelScene::checkEnemyHitAndDamagePlayer() {
    if (hasEnemyHitThisSwing) return;
    if (blocking) {
        // player is blocking, no damage
        if (!hasEnemyHitThisSwing) {
            std::cout << "[Blocked] Kamu berhasil memblokir serangan!\n";
        if (gamePtr) {
            gamePtr->playSound("../assets/sounds/sword_block_sound.wav");
        }
            hasEnemyHitThisSwing = true;
        }
        return;
    }
    
    int winW = 800, winH = 480;
    if (gamePtr) gamePtr->getWindowSize(winW, winH);
    float spriteScale = std::clamp(winH / 720.0f, 1.0f, 2.4f);
    
    // player hitbox
    SDL_Rect playerBox;
    playerBox.w = int(player.frameW * spriteScale);
    playerBox.h = int(player.frameH * spriteScale);
    playerBox.x = int(player.x - playerBox.w / 2);
    playerBox.y = int(player.y - playerBox.h);
    
    // enemy attack hitbox (mirrored from player attack)
    const int baseW = 80, baseH = 40, offset = 20, up = 60;
    SDL_Rect attackBox;
    attackBox.w = int(baseW * spriteScale);
    attackBox.h = int(baseH * spriteScale);
    attackBox.y = int(enemy.y - up * spriteScale);
    
    if (enemyFacingRight) {
        attackBox.x = int(enemy.x + offset * spriteScale);
    } else {
        attackBox.x = int(enemy.x - offset * spriteScale - attackBox.w);
    }
    
    bool overlap = !( attackBox.x + attackBox.w < playerBox.x ||
                      attackBox.x > playerBox.x + playerBox.w ||
                      attackBox.y + attackBox.h < playerBox.y ||
                      attackBox.y > playerBox.y + playerBox.h );
    
    if (overlap) {
        player.hp -= enemyAttackDamage;
        if (player.hp < 0) player.hp = 0;
        hasEnemyHitThisSwing = true;
        std::cout << "[Hit] Musuh memukul! HP kamu: " << player.hp << "\n";
    }
}

