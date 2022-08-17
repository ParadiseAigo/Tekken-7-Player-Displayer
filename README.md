# Tekken-7-Player-Displayer
> See your opponents name before accepting the match and more...

Displays your opponents name, their location, the character they last used against you and a comment that you can write about them.

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/ip-17082022/7git.png)

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/master/2git.png)

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/master/5git.png)

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/master/8git.png)

## Installation
Download the latest release [here](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/releases) and leave it in any folder on your computer. If you want to build it yourself: clone it via Visual Studio. :)

## Usage
Simply run the executable before or after (doesn't matter) opening Tekken 7.

If you restart Tekken 7 you must also restart Player-Displayer first (or at least close it). (If you don't, Steam might not let you start Tekken again because it thinks Tekken is still running, which happens because Steam thinks Player-Displayer is Tekken 7) 

## Commands
* ALT+W  puts the game in windowed mode (so you don't have to worry about crashing the game by alt-tabbing)
* ALT+F  puts the game in full-screen mode
* ALT+C  opens the comment window where you can write a comment after playing against an opponent
* ALT+O  opens the list of all the players you played against
* ALT+S  turn off silent mode, or in other words: turn on in-game feedback (program is in silent mode by default)

## Dependencies
It is possible (usually on new computers) that when trying to run the .exe for the first time you might get an error that looks something like this: "VCRUNTIME140_1.dll was not found". To fix this error you can download the 64-bit version of the package called "Visual C++ Redistributable for Visual Studio 2019" (from [here](https://www.sts-tutorial.com/download/credistributable2019) for example). Or alternatively if you want a quicker (but generally not recommended) way you can also manually download the dll (from [here](https://www.dll-files.com/vcruntime140_1.dll.html)) and place it in the following folder: "C:\Windows\System32".

## Issues
* The program is sometimes unable to get the name of the opponent. You can just decline those match requests.
* It is also possible that the program gets the name wrong. But from my (limited) experience this tends to be rare.
* Your game language has to be in English, if it isn't the in-game display will not work, but the program window will still display the correct information.
