# Tekken-7-Player-Displayer
> See your opponents name before accepting the match and more...

Displays your opponents name, the character they last used against you and a comment that you can write about them.

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/temporarybranch10october2021/6git.png)

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/master/2git.png)

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/master/5git.png)

## Commands
* ALT+W  puts the game in windowed mode (so you don't have to worry about crashing the game by alt-tabbing)
* ALT+F  puts the game in full-screen mode
* ALT+C  opens the comment window where you can write a comment after playing against an opponent
* ALT+O  opens the list of all the players you played against

## Usage
Run the executable before or after opening Tekken 7.

There are two windows in the GUI that show the name of the opponent. The first one is the main one that you should be paying attention to. The second one is in case the first one fails. Note however that if the first one fails then the opponent won't be saved in the list if you do choose to accept.

Extra information: The second "Opponent name" window always displays your name, this is normal, and only changes when you are matched with someone to that someones name.

## Installation
Download the latest release [here](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/releases) or you can build it yourself by loading the .sln file in Visual Studio.

## Dependencies
It is possible (usually on new computers) that when trying to run the .exe for the first time you might get an error that looks something like this: "VCRUNTIME140_1.dll was not found". To fix this error you can download the 64-bit version of the package called "Visual C++ Redistributable for Visual Studio 2019" (from [here](https://www.sts-tutorial.com/download/credistributable2019) for example). Or alternatively if you want a quicker (but generally not recommended) way you can also manually download the dll (from [here](https://www.dll-files.com/vcruntime140_1.dll.html)) and place it in the following folder: "C:\Windows\System32".

## Issues
* The program is sometimes unable to get the name of the opponent. You can just decline those match requests.
* It is also possible that the program gets the name wrong. But from my (limited) experience this tends to be rare.
