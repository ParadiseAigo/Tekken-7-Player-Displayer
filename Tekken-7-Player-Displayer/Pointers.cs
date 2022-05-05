﻿using System;
using System.Collections.Generic;
using System.Text;

namespace Tekken_7_Player_Displayer_csharp
{
    class Pointers
    {
        public static readonly string TEKKEN_WINDOW_NAME = "TEKKEN 7 ";
        public static readonly string TEKKEN_EXE_NAME = "TekkenGame-Win64-Shipping";
        public static readonly string PLAYERLIST_PATH = "Tekken Player List.txt";

        public static readonly string STEAM_API_MODULE_NAME = "steam_api64.dll";
        public static readonly string STEAM_API_MODULE_EDITED_NAME = "steam_api64_o.dll";
        public static readonly string TEKKEN_MODULE_NAME = "TekkenGame-Win64-Shipping.exe";

        public static readonly string[] ALL_CHARACTERS = { "Paul", "Law", "King", "Yoshimitsu", "Hwoarang", "Xiaoyu", "Jin", "Bryan", "Heihachi", "Kazuya", "Steve", "Jack-7", "Asuka", "Devil Jin", "Feng", "Lili", "Dragunov", "Leo", "Lars", "Alisa", "Claudio", "Katarina", "Lucky Chloe", "Shaheen", "Josie", "Gigas", "Kazumi", "Devil Kazumi", "Nina", "Master Raven", "Lee", "Bob", "Akuma", "Kuma", "Panda", "Eddy", "Eliza", "Miguel", "Soldier", "Kid Kazuya", "Jack-#", "Young Heihachi", "Dummy A", "Geese", "Noctis", "Anna", "Lei", "Marduk", "Armor King", "Julia", "Negan", "Zafina", "Ganryu", "Leroy Smith", "Fahkumram", "Kunimitsu", "Lidia" };

        public static readonly long STEAM_ID_BETTER_STATIC_POINTER = 0x2FC50;  //to get the real static pointer: needs to be added to the module name, example: "steam_api64_o.dll"+2FC50
        public static readonly int[] STEAM_ID_BETTER_POINTER_OFFSETS = { 8, 0x10, 0, 0x10, 0x28, 0x88, 0 };

        public static readonly long STEAM_ID_USER_STATIC_POINTER = 0x2FF78;  //to get the real static pointer: needs to be added to the module name, example: "steam_api64_o.dll"+2FC50
        public static readonly int[] STEAM_ID_USER_POINTER_OFFSETS = { };

        public static readonly long SCREEN_MODE_STATIC_POINTER = 0x347ADD8;  //to get the real static pointer: needs to be added to the module name
        public static readonly int[] SCREEN_MODE_POINTER_OFFSETS = { };

        public static readonly long SECONDS_REMAINING_MESSAGE_STATIC_POINTER = 0x34CB8E0;  //to get the real static pointer: needs to be added to the module name
        public static readonly int[] SECONDS_REMAINING_MESSAGE_POINTER_OFFSETS = { 0x58, 0x330, 0x40, 0x30, 0x158, 0x750 };

        public static readonly long OPPONENT_STRUCT_NAME_STATIC_POINTER = 0x34D24A0;  //to get the real static pointer: needs to be added to the module name
        public static readonly int[] OPPONENT_STRUCT_NAME_POINTER_OFFSETS = { 0, 0x8, 0x11C };

        public static readonly long OPPONENT_STRUCT_CHARACTER_STATIC_POINTER = 0x34D24A0;  //to get the real static pointer: needs to be added to the module name
        public static readonly int[] OPPONENT_STRUCT_CHARACTER_POINTER_OFFSETS = { 0, 0x8, 0x10 };

        public static readonly int SCREEN_MODE_FULLSCREEN = 0;
        public static readonly int SCREEN_MODE_BORDERLESS = 1;
        public static readonly int SCREEN_MODE_WINDOWED = 2;
    }
}
