#include "EncyclopediaScene.hpp"
#include "Game.hpp"
#include <SDL_image.h>
#include <iostream>
#include <sstream>


EncyclopediaScene::EncyclopediaScene() : change(false), next("") {
    // SETUP KONTEN (Hardcoded sesuai request)
    
    // Halaman 1
    pages.push_back({
        "Latar Belakang",
        "Perang Badr tidak dimulai dengan niat penaklukkan, melainkan keadilan ekonomi. Pada 17 Ramadan 2 H, Rasulullah SAW memimpin 313 kaum Muslimin untuk menghadang kafilah dagang Abu Sufyan. Ini adalah upaya strategis untuk menekan ekonomi Quraysh dan menuntut ganti rugi atas harta kaum Muhajirin yang dirampas paksa.",
        "assets/images/badar1.png" // GANTI mp4 jadi png/png dulu
    });

    // Halaman 2
    pages.push_back({
        "Eskalasi Kekuatan",
        "Abu Sufyan lolos, namun Makkah membunyikan genderang perang. Abu Jahal memimpin 1.000 pasukan elit (100 kuda, 700 unta). Muslimin hanya 313 orang (2 kuda, 70 unta). Secara matematis ini misi bunuh diri, namun Muslimin memiliki senjata terkuat: Iman yang kokoh.",
        "assets/images/badar2.png" // Siapkan gambar ini
    });

    // Halaman 3
    pages.push_back({
        "Mubarazah (Duel)",
        "Tradisi Arab mengharuskan duel pembuka. Tiga ksatria Quraysh (Utbah, Shaybah, Al-Walid) menantang. Rasulullah mengutus Ali, Hamzah, dan Ubaydah. Ali dan Hamzah menang telak. Ubaydah terluka namun lawannya tewas. Skor 3-0 meruntuhkan mental musuh.",
        "assets/images/badar3.png"
    });

    // Halaman 4
    pages.push_back({
        "Pertempuran & Bantuan",
        "Perang total pecah. Meski kalah jumlah 1 banding 3, Muslimin bertempur disiplin menguasai sumber air. Rasulullah berdoa khusyuk, dan Allah menurunkan 'Seribu Malaikat' yang turut memukul mundur musuh. Pasukan Quraysh menjadi gentar.",
        "assets/images/badar4.png"
    });

    // Halaman 5
    pages.push_back({
        "Kemenangan Mutlak",
        "Badr berakhir dengan kemenangan Islam. 70 tokoh Quraysh tewas (termasuk Abu Jahal), 70 ditawan. Muslimin hanya kehilangan 14 syuhada. Ini adalah proklamasi bahwa Islam adalah kekuatan baru yang tidak bisa diremehkan.",
        "assets/images/badar5.png"
    });
}

void EncyclopediaScene::onEnter(Game* game) {
    gamePtr = game;
    change = false;
    currentPage = 0;
    isTurning = false;
    animFrame = 0; // Frame buku terbuka (pojok kiri atas spritesheet)

    // Load Assets
    // 1. Spritesheet Buku
    SDL_Surface* surf = IMG_Load("assets/images/book.png");
    if (surf) {
        bookSpriteSheet = SDL_CreateTextureFromSurface(game->getRenderer(), surf);
        // Spritesheet format: 4 kolom, 2 baris
        bookFrameW = surf->w / 4;
        bookFrameH = surf->h / 2;
        SDL_FreeSurface(surf);
    } else {
        std::cerr << "[Ency] Gagal load book.png\n";
    }

    // Load Media Halaman Pertama
    loadPageMedia(0);

    // --- SETUP AUDIO ---
    // Load Musik
    bgMusic = Mix_LoadMUS("assets/sounds/story_song.wav");
    if (bgMusic == nullptr) {
        std::cerr << "[Ency] Gagal load music: " << Mix_GetError() << "\n";
    } else {
        // Play Music (Looping: -1 artinya ulang terus menerus)
        Mix_PlayMusic(bgMusic, -1);
        // Set Volume (0-128), set ke 60 biar ga terlalu berisik
        Mix_VolumeMusic(60); 
    }

    std::cout << "[Ency] Masuk Ensiklopedia.\n";
}

void EncyclopediaScene::loadPageMedia(int index) {
    if (currentMediaTex) {
        SDL_DestroyTexture(currentMediaTex);
        currentMediaTex = nullptr;
    }
    
    // Load gambar ilustrasi
    std::string path = pages[index].mediaPath;
    SDL_Surface* s = IMG_Load(path.c_str());
    if (s) {
        currentMediaTex = SDL_CreateTextureFromSurface(gamePtr->getRenderer(), s);
        SDL_FreeSurface(s);
    } else {
        // Kalau gambar belum ada, jangan crash, biarkan kosong/hitam
        // std::cerr << "[Ency] Media not found: " << path << "\n";
    }
}

void EncyclopediaScene::onExit() {
    // Bersihkan Texture
    if (bookSpriteSheet) SDL_DestroyTexture(bookSpriteSheet);
    if (currentMediaTex) SDL_DestroyTexture(currentMediaTex);
    if (arrowTex) SDL_DestroyTexture(arrowTex);
    
    bookSpriteSheet = nullptr;
    currentMediaTex = nullptr;

    // --- BERSIHKAN AUDIO ---
    // Stop musik jika sedang main
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
    // Hapus dari memori
    if (bgMusic) {
        Mix_FreeMusic(bgMusic);
        bgMusic = nullptr;
    }
}

void EncyclopediaScene::startPageTurn(int direction) {
    // Jika sedang animasi, abaikan
    if (isTurning) return;

    // Cek batas halaman
    int targetPage = currentPage + direction;
    if (targetPage < 0 || targetPage >= (int)pages.size()) return;

    isTurning = true;
    turnDirection = direction;
    animTimer = 0.0f;
    
    // Mulai animasi
    // Jika Next: Frame 0 -> 7
    // Jika Prev: Frame 7 -> 0 (Mundur)
    animFrame = (direction == 1) ? 0 : 7;
}

void EncyclopediaScene::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        SDL_Keycode key = e.key.keysym.sym;
        if (key == SDLK_ESCAPE) {
            change = true;
            next = "menu";
        }
        else if (key == SDLK_RIGHT || key == SDLK_d) {
            startPageTurn(1); // Next
        }
        else if (key == SDLK_LEFT || key == SDLK_a) {
            startPageTurn(-1); // Prev
        }
    }
    
    // Mouse Interaction (Simple regions)
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        int w, h;
        gamePtr->getWindowSize(w, h);

        // Klik Kiri Layar -> Prev
        if (mx < w / 4) startPageTurn(-1);
        // Klik Kanan Layar -> Next
        else if (mx > w * 3 / 4) startPageTurn(1);
    }
}

void EncyclopediaScene::update(float deltaTime) {
    // Update Animasi Buku
    if (isTurning) {
        animTimer += deltaTime;
        // Kecepatan animasi: 0.08 detik per frame
        if (animTimer >= 0.08f) {
            animTimer = 0.0f;
            
            if (turnDirection == 1) {
                animFrame++;
                if (animFrame > 7) {
                    // Animasi Selesai Next
                    isTurning = false;
                    animFrame = 0; // Kembali ke diam
                    currentPage++;
                    loadPageMedia(currentPage);
                }
            } else {
                animFrame--;
                if (animFrame < 0) {
                    // Animasi Selesai Prev
                    isTurning = false;
                    animFrame = 0; 
                    currentPage--;
                    loadPageMedia(currentPage);
                }
            }
        }
    } else {
        // Jika tidak turning, pastikan frame di posisi diam (Frame 0)
        animFrame = 0; 
    }

    // Simulasi Animasi Video (Kalau nanti pakai sprite sequence)
    // videoTimer += deltaTime;
}

// Helper Wrap Text (Sama kayak StoryScene tapi warna default hitam)
std::vector<std::string> EncyclopediaScene::wrapText(TextRenderer* text, SDL_Renderer* r, const std::string& str, int maxWidth) {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string word, line;
    while (iss >> word) {
        std::string test = line.empty() ? word : line + " " + word;
        if (text->measureWidth(r, test) > maxWidth) {
            result.push_back(line);
            line = word;
        } else {
            line = test;
        }
    }
    if (!line.empty()) result.push_back(line);
    return result;
}

void EncyclopediaScene::render(SDL_Renderer* renderer, TextRenderer* text) {
    int w, h;
    gamePtr->getWindowSize(w, h);

    // 1. Background Gelap
    SDL_SetRenderDrawColor(renderer, 25, 15, 10, 255);
    SDL_RenderClear(renderer);

    // Variabel posisi konten
    int leftPageX, rightPageX, contentY;
    int contentW, contentH;

    if (bookSpriteSheet) {
        // --- 1. RENDER BUKU (VISUAL) ---
        float targetBookHeight = h * 1.8f; 
        float scale = targetBookHeight / (float)bookFrameH;

        int col = animFrame % 4;
        int row = animFrame / 4;
        SDL_Rect src = { col * bookFrameW, row * bookFrameH, bookFrameW, bookFrameH };
        
        SDL_Rect dst;
        dst.h = (int)targetBookHeight;
        dst.w = (int)(bookFrameW * scale);
        
        dst.x = (w - dst.w) / 2;
        int bookShiftY = (int)(h * 0.12f); 
        dst.y = ((h - dst.h) / 2) - bookShiftY;

        SDL_RenderCopy(renderer, bookSpriteSheet, &src, &dst);

        // --- 2. SETUP AREA KONTEN ---
        
        int centerX = w / 2;
        int centerY = h / 2;
        int visualBookCenterY = centerY - bookShiftY;

        // Area Teks (Kiri) - Tetap Ramping biar rapi
        contentW = (int)(w * 0.25f); 
        contentH = (int)(h * 0.55f); 
        contentY = visualBookCenterY - (contentH / 2) + 20;

        int spineGap = 40; 
        leftPageX = centerX - contentW - spineGap; 
        rightPageX = centerX + spineGap; 

        // 3. Render Konten
        if (!isTurning) {
            // -- HALAMAN KIRI: TEKS --
            int titleSize = 28; 
            text->setFontSize(titleSize);
            SDL_Color inkColor = {40, 30, 15, 255}; 
            
            std::vector<std::string> titleLines = wrapText(text, renderer, pages[currentPage].title, contentW);
            int curY = contentY;

            for (const auto& line : titleLines) {
                text->drawText(renderer, line, leftPageX, curY, inkColor);
                curY += (titleSize + 5);
            }

            SDL_SetRenderDrawColor(renderer, 100, 70, 40, 150);
            SDL_RenderDrawLine(renderer, leftPageX, curY + 10, leftPageX + contentW, curY + 10);
            curY += 25;

            int bodySize = 20; 
            text->setFontSize(bodySize);
            std::string content = pages[currentPage].content;
            std::vector<std::string> bodyLines = wrapText(text, renderer, content, contentW);
            
            for (const auto& line : bodyLines) {
                if (curY + bodySize > contentY + contentH) break;
                text->drawText(renderer, line, leftPageX, curY, inkColor);
                curY += (bodySize + 4);
            }

            // -- HALAMAN KANAN: MEDIA --
            if (currentMediaTex) {
                int texW, texH;
                SDL_QueryTexture(currentMediaTex, nullptr, nullptr, &texW, &texH);

                // --- BATAS UKURAN GAMBAR (BOUNDING BOX) ---
                
                // Lebar Maksimal: Samakan dengan lebar area teks (27% lebar layar)
                // Jangan pakai dst.w karena itu lebar buku raksasa
                int maxW = (int)(w * 0.27f); 
                
                // Tinggi Maksimal: 50% tinggi layar (biar aman di dalam kertas)
                int maxH = (int)(h * 0.50f); 

                // --- LOGIKA SCALING (ASPECT RATIO PRESERVE) ---
                float aspect = (float)texH / (float)texW;
                
                int drawW = maxW;
                int drawH = (int)(drawW * aspect);

                // Jika setelah dihitung tingginya masih terlalu besar, kecilkan lagi based on height
                if (drawH > maxH) {
                    drawH = maxH;
                    drawW = (int)(drawH / aspect);
                }

                // --- POSISI GAMBAR ---
                
                // Cari titik tengah halaman kanan
                // Titik tengah halaman kanan adalah: Tengah Layar + Offset ke Kanan
                int rightPageCenterX = (w / 2) + (int)(w * 0.18f); // Offset manual kira-kira 18% layar ke kanan
                
                // Atau hitung pakai contentWidth sebelumnya biar simetris dengan teks
                // int rightPageCenterX = rightPageX + (contentW / 2); (Kalau mau pakai variabel sebelumnya)

                SDL_Rect mediaDst;
                mediaDst.w = drawW;
                mediaDst.h = drawH;
                
                // Center Horizontal di halaman kanan
                mediaDst.x = rightPageX + (contentW - drawW) / 2; 
                
                // Center Vertikal di tengah layar (sedikit naik biar ada ruang caption)
                int centerY = h / 2;
                mediaDst.y = centerY - (drawH / 2) - (int)(h * 0.05f); // Naik 5%

                // --- RENDER FRAME ---
                
                // Shadow
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 40);
                SDL_Rect shadow = {mediaDst.x + 5, mediaDst.y + 5, mediaDst.w, mediaDst.h};
                SDL_RenderFillRect(renderer, &shadow);

                // Border Putih (Polaroid)
                SDL_SetRenderDrawColor(renderer, 245, 240, 230, 255);
                SDL_Rect border = {mediaDst.x - 5, mediaDst.y - 5, mediaDst.w + 10, mediaDst.h + 30};
                SDL_RenderFillRect(renderer, &border);

                // Gambar
                SDL_RenderCopy(renderer, currentMediaTex, nullptr, &mediaDst);

                // Caption
                text->setFontSize(16);
                std::string cap = "Visualisasi Sejarah";
                int capW = text->measureWidth(renderer, cap);
                text->drawText(renderer, cap, mediaDst.x + (mediaDst.w - capW)/2, mediaDst.y + mediaDst.h + 5, {80, 60, 40, 255});
            }
            
            // -- HALAMAN NOMOR (PERBAIKAN POSISI) --
            std::string pageNum = std::to_string(currentPage + 1) + " / " + std::to_string(pages.size());
            
            // Taruh di pojok kanan bawah BUKU (bukan area teks)
            // dst.y + dst.h adalah ujung bawah buku. Kita naikkan sedikit.
            // Margin kanan 18% dari lebar buku, Margin bawah 13% dari tinggi buku
            int pageNumX = dst.x + dst.w - (int)(dst.w * 0.18f);
            int pageNumY = dst.y + dst.h - (int)(dst.h * 0.13f);

            text->drawText(renderer, pageNum, pageNumX, pageNumY, inkColor);
        }
    }

    // 4. Hint Controls
    text->setFontSize(16);
    text->drawText(renderer, "[ARROWS/MOUSE] Balik Halaman   [ESC] Menu Utama", 40, h - 40, {200, 200, 200, 150});
}