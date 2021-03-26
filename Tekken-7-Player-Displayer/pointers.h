#ifndef TEKKEN7_POINTERS_H
#define TEKKEN7_POINTERS_H

#define TEKKEN_WINDOW_NAME "TEKKEN 7 "
#define TEKKEN_EXE_NAME "TekkenGame-Win64-Shipping.exe"

#define ALL_CHARACTERS {"Paul", "Law", "King", "Yoshimitsu", "Hwoarang", "Xiaoyu", "Jin", "Bryan", "Heihachi", "Kazuya", "Steve", "Jack-7", "Asuka", "Devil Jin", "Feng", "Lili", "Dragunov", "Leo", "Lars", "Alisa", "Claudio", "Katarina", "Lucky Chloe", "Shaheen", "Josie", "Gigas", "Kazumi", "Devil Kazumi", "Nina", "Master Raven", "Lee", "Bob", "Akuma", "Kuma", "Panda", "Eddy", "Eliza", "Miguel", "Soldier", "Kid Kazuya", "Jack-#", "Young Heihachi", "Dummy A", "Geese", "Noctis", "Anna", "Lei", "Marduk", "Armor King", "Julia", "Negan", "Zafina", "Ganryu", "Leroy Smith", "Fahkumram", "Kunimitsu", "Lidia"}

// no longer needed: name is obtained from the web using the steam id
#define OPPONENT_NAME_STATIC_POINTER 0x1434CB2B0 // changed last update
#define OPPONENT_NAME_POINTER_OFFSETS {}

#define FIGHT_THIS_PLAYER_MESSAGE_STATIC_POINTER 0x1434C9960 // changed last update
#define FIGHT_THIS_PLAYER_MESSAGE_POINTER_OFFSETS {0x48, 0, 0x30, 0xE8, 0x5B7}

#define SECONDS_REMAINING_MESSAGE_STATIC_POINTER 0x1434C9960 // changed last update
#define SECONDS_REMAINING_MESSAGE_POINTER_OFFSETS {0x48, 0, 0x30, 0xE8, 0x14C310}

#define OPPONENT_FOUND_MESSAGE_STATIC_POINTER 0x1434C9960 // changed last update
#define OPPONENT_FOUND_MESSAGE_POINTER_OFFSETS {0x48, 0, 0x30, 0xE8, 0x14CAF5}

#define DISCONNECTION_RATE_MESSAGE_STATIC_POINTER 0x1434C9960 // changed last update
#define DISCONNECTION_RATE_MESSAGE_POINTER_OFFSETS {0x48, 0, 0x30, 0xE8, 0xA2A98}

#define SCREEN_MODE_STATIC_POINTER 0x143478DE8 // changed last update
#define SCREEN_MODE_POINTER_OFFSETS {}

//#define STEAM_ID_STATIC_POINTER 0x1434D12C0 // changed last update
//#define STEAM_ID_POINTER_OFFSETS {0x50, 0x1D8, 0x238, 0x18, 0x98}

#define STEAM_API_MODULE_NAME L"steam_api64.dll"
#define STEAM_API_MODULE_EDITED_NAME L"steam_api64_o.dll"
#define STEAM_ID_BETTER_STATIC_POINTER 0x2FC50  //to get the real static pointer: needs to be added to the module name, example: "steam_api64_o.dll"+2FC50
#define STEAM_ID_BETTER_POINTER_OFFSETS {8, 0x10, 0, 0x10, 0x28, 0x88, 0}

#define STEAM_ID_USER_STATIC_POINTER 0x2FF78  //to get the real static pointer: needs to be added to the module name, example: "steam_api64_o.dll"+2FC50
#define STEAM_ID_USER_POINTER_OFFSETS {}

//#define OPPONENT_STRUCT_SIGNATURE_STATIC_POINTER 0x1434D0510 // changed last update
//#define OPPONENT_STRUCT_SIGNATURE_POINTER_OFFSETS {0, 0x8, 0}

#define OPPONENT_STRUCT_NAME_STATIC_POINTER 0x1434D0510 // changed last update
#define OPPONENT_STRUCT_NAME_POINTER_OFFSETS {0, 0x8, 0x11C}

#define OPPONENT_STRUCT_CHARACTER_STATIC_POINTER 0x1434D0510 // changed last update
#define OPPONENT_STRUCT_CHARACTER_POINTER_OFFSETS {0, 0x8, 0x10}

//#define OPPONENT_STRUCT_SIGNATURE 0x1427D6848 // changed last update

#define SCREEN_MODE_FULLSCREEN 0
#define SCREEN_MODE_BORDERLESS 1
#define SCREEN_MODE_WINDOWED 2

#endif
