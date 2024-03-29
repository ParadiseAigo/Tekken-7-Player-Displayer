﻿using System;
using System.Collections.Generic;
using System.Text;

namespace Tekken_7_Player_Displayer
{
    class Pointers
    {
        public const string TEKKEN_WINDOW_NAME = "TEKKEN 7 ";
        public const string TEKKEN_EXE_NAME = "TekkenGame-Win64-Shipping";
        public const string PLAYERLIST_PATH = "Tekken Player List.txt";
        public const string TEKKEN_STEAM_APP_ID = "389730";

        public const string STEAM_API_MODULE_NAME = "steam_api64.dll";
        public const string STEAM_API_MODULE_EDITED_NAME = "steam_api64_o.dll";
        public const string TEKKEN_MODULE_NAME = "TekkenGame-Win64-Shipping.exe";

        public const int SCREEN_MODE_FULLSCREEN = 0;
        public const int SCREEN_MODE_BORDERLESS = 1;
        public const int SCREEN_MODE_WINDOWED = 2;

        public static readonly string[] ALL_CHARACTERS = { "Paul", "Law", "King", "Yoshimitsu", "Hwoarang", "Xiaoyu", "Jin", "Bryan", "Heihachi", "Kazuya", "Steve", "Jack-7", "Asuka", "Devil Jin", "Feng", "Lili", "Dragunov", "Leo", "Lars", "Alisa", "Claudio", "Katarina", "Lucky Chloe", "Shaheen", "Josie", "Gigas", "Kazumi", "Devil Kazumi", "Nina", "Master Raven", "Lee", "Bob", "Akuma", "Kuma", "Panda", "Eddy", "Eliza", "Miguel", "Soldier", "Kid Kazuya", "Jack-#", "Young Heihachi", "Dummy A", "Geese", "Noctis", "Anna", "Lei", "Marduk", "Armor King", "Julia", "Negan", "Zafina", "Ganryu", "Leroy Smith", "Fahkumram", "Kunimitsu", "Lidia" };
        public static readonly string[] ALL_RANKS = { "Beginner", "1st Kyu", "2nd Kyu", "3rd Kyu", "4th Kyu", "5th Kyu", "6th Kyu", "7th Kyu", "8th Kyu", "9th Kyu", "1st Dan", "2nd Dan", "3rd Dan", "Initiate", "Mentor", "Expert", "Grand master", "Brawler", "Marauder", "Fighter", "Vanguard", "Warrior", "Vindicator", "Juggernaut", "Usurper", "Vanquisher", "Destroyer", "Savior", "Overlord", "Genbu", "Byakko", "Seiryu", "Suzaku", "Mighty Ruler", "Revered Ruler", "Divine Ruler", "Eternal Ruler", "Fujin", "Raijin", "Yaksa", "Ryujin", "Emperor", "Tekken King", "Tekken God", "TTG", "TGP", "TGO" };

        // steam id of the opponent
        public static readonly long STEAM_ID_BETTER_STATIC_POINTER = 0x2FC50;  //to get the real static pointer: needs to be added to the module name, example: "steam_api64_o.dll"+2FC50
        public static readonly int[] STEAM_ID_BETTER_POINTER_OFFSETS = { 8, 0x10, 0, 0x10, 0x28, 0x88, 0 };

        // steam id of the user
        public static readonly long STEAM_ID_USER_STATIC_POINTER = 0x2FF78;  //to get the real static pointer: needs to be added to the module name, example: "steam_api64_o.dll"+2FC50
        public static readonly int[] STEAM_ID_USER_POINTER_OFFSETS = { };

        // steam name of opponent after having accepted
        public static readonly long OPPONENT_STRUCT_NAME_STATIC_POINTER = 0x34D55A0;  //to get the real static pointer: needs to be added to the module name
        public static readonly int[] OPPONENT_STRUCT_NAME_POINTER_OFFSETS = { 0, 0x8, 0x11C };

        // character of opponent after having accepted
        public static readonly long OPPONENT_STRUCT_CHARACTER_STATIC_POINTER = 0x34D55A0;  //to get the real static pointer: needs to be added to the module name
        public static readonly int[] OPPONENT_STRUCT_CHARACTER_POINTER_OFFSETS = { 0, 0x8, 0x10 };

        public static readonly long SCREEN_MODE_STATIC_POINTER = 0x347DED8;  //to get the real static pointer: needs to be added to the module name
        public static readonly int[] SCREEN_MODE_POINTER_OFFSETS = { };

        public static readonly long SECONDS_REMAINING_MESSAGE_STATIC_POINTER = 0x34CE9E0;  //to get the real static pointer: needs to be added to the module name
        public static readonly int[] SECONDS_REMAINING_MESSAGE_POINTER_OFFSETS = { 0x58, 0x330, 0xC8, 0x30, 0x30, 0xF18 };
    }
}



