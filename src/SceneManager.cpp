#include "SceneManager.hpp"
#include "Game.hpp"
#include "EncyclopediaScene.hpp"
// Include semua scene
#include "MenuScene.hpp"
#include "CharacterSelectScene.hpp"
#include "DuelScene.hpp"
#include "StoryScene.hpp"
#include "EndingScene.hpp" // Opsional, jika masih ada sisa file lama

#include <iostream>
#include <memory>
#include <vector>

// ==========================================
// HELPER FUNCTION
// ==========================================
// Membantu membuat StoryScene dengan cepat tanpa mengetik ulang kode setup
std::unique_ptr<Scene> createStory(const std::string& bgPath, 
                                   const std::string& title, 
                                   const std::vector<std::string>& lines, 
                                   const std::string& nextScene) 
{
    auto s = std::make_unique<StoryScene>();
    s->setup(bgPath, title, lines, nextScene);
    return s;
}

// ==========================================
// SCENE FACTORY
// ==========================================
std::unique_ptr<Scene> SceneManager::createSceneByName(const std::string& name) {
    
    // --- 1. CORE SCENES ---
    if (name == "menu") {
        return std::make_unique<MenuScene>();
    } 
    else if (name == "char_select") {
        return std::make_unique<CharacterSelectScene>();
    } 
    else if (name == "duel") {
        return std::make_unique<DuelScene>();
    }else if (name == "encyclopedia") {
        return std::make_unique<EncyclopediaScene>();
    }

    // --- 2. INTRO FLOW (BABAK 1 - 3) ---
    
    // Babak 1
    if (name == "badar_intro") {
        return createStory(
            "assets/images/Scene1.png", 
            "BABAK 1: PEMICU AWAL",
            {
                "Nabi Muhammad SAW bergerak dengan sekitar 313 kaum Muslimin.",
                "Dengan perlengkapan minim, beliau berniat mencegat kafilah dagang kaya Abu Sufyan.",
                "Ini adalah langkah strategis sebagai tekanan ekonomi pada Makkah yang telah menindas Muslimin."
            }, 
            "story_2_eskalasi"
        );
    }

    // Babak 2
    if (name == "story_2_eskalasi") {
        return createStory(
            "assets/images/Scene2.png", 
            "BABAK 2: ESKALASI",
            {
                "Abu Sufyan berhasil lolos dan meminta bantuan ke Makkah.",
                "Makkah merespon dengan amarah. Mereka mengirim pasukan besar: 1.000 prajurit elit.",
                "Dipimpin oleh Abu Jahal yang sombong, mereka berniat menghancurkan Muslimin di lembah Badr."
            }, 
            "story_3_tantangan"
        );
    }

    // Babak 3 (Intro Mubarazah)
    if (name == "story_3_tantangan") {
        return createStory(
            "assets/images/Scene3.png", 
            "BABAK 3: TANTANGAN MUBARAZAH",
            {
                "Ketegangan memuncak. Tiga ksatria elit Quraysh—Utbah, Shaybah, dan Al-Walid—maju ke tengah medan.",
                "Mereka menolak berduel dengan kaum Ansar Madinah. Dengan angkuh mereka berteriak:",
                "'Wahai Muhammad! Keluarkan untuk kami lawan yang setara dari darah daging kami sendiri!'",
                "Rasulullah SAW menatap barisan pasukan, lalu berseru tegas:",
                "'Bangkitlah wahai Ubaydah! Bangkitlah wahai Hamzah! Bangkitlah wahai Ali!'"
            }, 
            "char_select" // Lanjut ke pemilihan karakter
        );
    }

    // --- 3. DYNAMIC STORY: PRE-DUEL ---
    // Scene ini muncul SETELAH memilih karakter, SEBELUM masuk DuelScene
    if (name == "pre_duel_story") {
        // Cek karakter siapa yang dipilih player
        std::string charName = game->getChosenCharacter().displayName;
        
        if (charName.find("Ali") != std::string::npos) {
            return createStory(
                "assets/images/Scene3.1(Ali).png", 
                "ALI BIN ABI THALIB",
                {
                    "Kamu maju sebagai Ali bin Abi Thalib, ksatria termuda.",
                    "Lawanmu adalah Al-Walid bin Utbah, sosok muda yang beringas dan penuh amarah.",
                    "Pertarungan ini menuntut kelincahan dan kecepatan."
                }, 
                "duel"
            );
        } 
        else if (charName.find("Hamzah") != std::string::npos) {
            return createStory(
                "assets/images/Scene3.2(Hamzah).png", 
                "HAMZAH BIN ABDUL MUTHALIB",
                {
                    "Kamu maju sebagai Hamzah, Sang Singa Allah.",
                    "Bulu burung unta di dadamu menjadi tanda keberanian yang dikenal kawan maupun lawan.",
                    "Lawanmu adalah Shaybah bin Rabi'ah, petarung senior yang tangguh."
                }, 
                "duel"
            );
        } 
        else { // Ubaydah
            return createStory(
                "assets/images/Scene3.3(Ubaidah).png", 
                "UBAYDAH BIN HARITS",
                {
                    "Kamu maju sebagai Ubaydah, yang tertua di antara ketiganya.",
                    "Lawanmu adalah Utbah bin Rabi'ah, pemimpin utama pasukan musuh.",
                    "Ini adalah duel kehormatan antara para pemimpin."
                }, 
                "duel"
            );
        }
    }

    // --- 4. DYNAMIC STORY: POST-DUEL ---
    // Scene ini muncul SETELAH DuelScene selesai (baik menang/kalah, kita tampilkan kisah aslinya)
    if (name == "post_duel_story") {
        std::string charName = game->getChosenCharacter().displayName;

        if (charName.find("Ali") != std::string::npos) {
            return createStory(
                "assets/images/Scene3.1(Ali).png", 
                "AKHIR MUBARAZAH: ALI",
                {
                    "Tidak ada basa-basi. Saat Al-Walid mengayunkan pedangnya, kamu menghindar dengan kelincahan luar biasa.",
                    "Celah terbuka! Dengan satu sambaran kilat, pedangmu menebas bahu musuh.",
                    "Al-Walid roboh seketika. Kemenangan mutlak dan tercepat!"
                }, 
                "story_5_penutup"
            );
        } 
        else if (charName.find("Hamzah") != std::string::npos) {
            return createStory(
                "assets/images/Scene3.2(Hamzah).png", 
                "AKHIR MUBARAZAH: HAMZAH",
                {
                    "Pengalaman tempurmu berbicara. Kamu tidak memberi Shaybah kesempatan bernapas.",
                    "Dengan tenaga penuh, kamu menangkis serangannya hingga ia terhuyung.",
                    "Lalu menyelesaikan duel dengan satu tebasan mematikan yang presisi."
                }, 
                "story_5_penutup"
            );
        } 
        else { // Ubaydah
            return createStory(
                "assets/images/Scene3.3(Ubaidah).png", 
                "AKHIR MUBARAZAH: UBAYDAH",
                {
                    "Duel berjalan alot dan keras. Pedang beradu berkali-kali.",
                    "Di satu momen krusial, kalian berdua menyerang bersamaan!",
                    "Pedang Utbah melukai kakimu hingga parah, namun pedangmu juga berhasil melukai kepalanya.",
                    "Kalian berdua jatuh terkapar, tak berdaya di atas pasir panas."
                }, 
                "story_5_penutup"
            );
        }
    }

    // --- 5. ENDING FLOW (BABAK 5 - 7) ---

    // Babak 5 (Penutup Mubarazah)
    if (name == "story_5_penutup") {
        return createStory(
            "assets/images/Scene4.png", 
            "PENUTUP MUBARAZAH",
            {
                "Melihat Ubaydah dan Utbah sama-sama terkapar, Ali dan Hamzah yang telah menang segera berlari.",
                "Mereka menghabisi Utbah seketika, lalu dengan cepat memopong tubuh Ubaydah yang terluka parah kembali ke barisan Muslim.",
                "Tiga jagoan Quraysh tewas di pasir Badr. Keheningan mencekam menyelimuti pasukan musuh."
            }, 
            "story_6_perang_total"
        );
    }

    // Babak 6 (Perang Total)
    if (name == "story_6_perang_total") {
        return createStory(
            "assets/images/Scene5.png", 
            "PERANG TOTAL",
            {
                "Setelah mental Quraysh jatuh akibat Mubarazah, perang total pecah.",
                "Meski kalah jumlah (313 vs 1000), Muslimin bertempur dengan disiplin tinggi.",
                "Strategi menguasai sumber air dan bantuan Malaikat membuat barisan musuh kocar-kacir."
            }, 
            "story_7_hasil"
        );
    }

    // Babak 7 (Hasil Akhir)
    if (name == "story_7_hasil") {
        return createStory(
            "assets/images/Scene6.png", 
            "KEMENANGAN GEMILANG",
            {
                "Muslimin menang telak. 70 tokoh Quraysh tewas (termasuk Abu Jahal), dan 70 lainnya ditawan.",
                "Di pihak Muslimin, hanya 14 orang yang gugur sebagai syuhada.",
                "Ini adalah titik balik kejayaan Islam yang akan dikenang selamanya."
            }, 
            "menu" // Selesai, kembali ke menu utama
        );
    }

    // --- ERROR HANDLING ---
    std::cerr << "[SceneManager] Unknown scene: " << name << "\n";
    return nullptr;
}

void SceneManager::changeScene(const std::string& name) {
    auto newScene = createSceneByName(name);
    if (newScene) {
        if (currentScene) {
            currentScene->onExit();
        }
        currentScene = std::move(newScene);
        currentScene->onEnter(game);
        std::cout << "[SceneManager] Switched to scene: " << name << "\n";
    } else {
        std::cerr << "[SceneManager] ERROR: Scene '" << name << "' not found/implemented yet!\n";
    }
}