#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"

#include "image/Weapons/101001.h" // AKM
#include "image/Weapons/101002.h" // M16A4
#include "image/Weapons/101003.h" // SCAR-L
#include "image/Weapons/101004.h" // M416
#include "image/Weapons/101005.h" // GROZA
#include "image/Weapons/101006.h" // AUG
#include "image/Weapons/101007.h" // QBZ
#include "image/Weapons/101008.h" // M762
#include "image/Weapons/101009.h" // Mk47 Mutant
#include "image/Weapons/101010.h" // G36C
#include "image/Weapons/101011.h" // AC-VAL
#include "image/Weapons/101012.h" // Honey Badger
#include "image/Weapons/1011001.h" // FAMAS
#include "image/Weapons/101101.h" // ASM Abakan
#include "image/Weapons/101102.h" // ACE32
#include "image/Weapons/102001.h" // UZI
#include "image/Weapons/102002.h" // UMP45
#include "image/Weapons/102003.h" // Vector
#include "image/Weapons/102004.h" // Thompson
#include "image/Weapons/102005.h" // PP-19 Bizon
#include "image/Weapons/102007.h" // MP5K
#include "image/Weapons/102008.h" // JS9
#include "image/Weapons/102105.h" // P90
#include "image/Weapons/103001.h" // Kar98K
#include "image/Weapons/103002.h" // M24
#include "image/Weapons/103003.h" // AWM
#include "image/Weapons/103004.h" // SKS
#include "image/Weapons/103005.h" // VSS
#include "image/Weapons/103006.h" // Mini-14
#include "image/Weapons/103007.h" // Mk14 EBR
#include "image/Weapons/103008.h" // Win94
#include "image/Weapons/103009.h" // SLR
#include "image/Weapons/103010.h" // QBU
#include "image/Weapons/103011.h" // Mosin Nagant
#include "image/Weapons/103012.h" // AMR
#include "image/Weapons/103100.h" // Mk12
#include "image/Weapons/103901.h" // Kar98K (skin)
#include "image/Weapons/103902.h" // M24 (skin)
#include "image/Weapons/103903.h" // AWM (skin)
#include "image/Weapons/104001.h" // S686
#include "image/Weapons/104002.h" // S1897
#include "image/Weapons/104003.h" // S12K
#include "image/Weapons/104004.h" // DBS
#include "image/Weapons/104100.h" // SPAS-12
#include "image/Weapons/105001.h" // M249
#include "image/Weapons/105002.h" // DP-28
#include "image/Weapons/105010.h" // MG3
#include "image/Weapons/106001.h" // P92
#include "image/Weapons/106002.h" // P1911
#include "image/Weapons/106003.h" // R1895
#include "image/Weapons/106004.h" // P18C
#include "image/Weapons/106005.h" // R45
#include "image/Weapons/106006.h" // Sawed-Off
#include "image/Weapons/106007.h" // Flare Gun
#include "image/Weapons/106008.h" // Scorpion
#include "image/Weapons/106010.h" // Desert Eagle
#include "image/Weapons/106011.h" // MP7
#include "image/Weapons/107001.h" // Crossbow
#include "image/Weapons/107006.h" // RPG-7
#include "image/Weapons/107007.h" // Crossbow (skin)
#include "image/Weapons/107008.h" // Bow
#include "image/Weapons/108001.h" // Machete
#include "image/Weapons/108002.h" // Sickle
#include "image/Weapons/108003.h" // Crowbar
#include "image/Weapons/108004.h" // Pan
#include "image/Weapons/602001.h" // Stun Grenade
#include "image/Weapons/602002.h" // Smoke Grenade
#include "image/Weapons/602003.h" // Molotov Cocktail
#include "image/Weapons/602004.h" // Frag Grenade
#include "image/Weapons/Empty.h" // Empty

#include "image/Throwables/missile1.h" // Frag Grenade
#include "image/Throwables/missile2.h" // Molotov Cocktail
#include "image/Throwables/missile3.h" // Smoke Grenade
#include "image/Throwables/missile4.h" // Stun Grenade
#include "image/Throwables/missile5.h" // Sticky Bomb

bool LoadTextureFromMemory(const stbi_uc* data, size_t size, GLuint* out_texture, int* out_width, int* out_height) {
    int image_width, image_height, channels;
    stbi_uc* image_data = stbi_load_from_memory(data, size, &image_width, &image_height, &channels, STBI_rgb_alpha);
    if (image_data == nullptr) return false;

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

static std::unordered_map<int, GLuint> weaponTextureCache;

GLuint LoadWeaponTexture(int weaponID, const stbi_uc* imageData, size_t imageSize) {
    auto it = weaponTextureCache.find(weaponID);
    if (it != weaponTextureCache.end()) {
        return it->second;
    }
    
    GLuint texture = 0;
    int width, height;
    if (LoadTextureFromMemory(imageData, imageSize, &texture, &width, &height)) {
        weaponTextureCache[weaponID] = texture;
        return texture;
    }
    return 0;
}