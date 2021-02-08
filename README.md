# Tekken-7-Player-Displayer
> See your opponents name before accepting the match and more...

Displays your opponents name, the character they last used against you and a comment that you can write about them.
It will display them both in-game and in the console.

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/master/1git.png)

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/master/2git.png)

## GUI
Now also comes with a GUI!

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/master/4git.png)

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/master/5git.png)

## Commands
* ALT+W  puts the game in windowed mode (so you don't have to worry about crashing the game by alt-tabbing)
* ALT+F  puts the game in full-screen mode
* ALT+O  opens the list of all the players you played against
* ALT+C  opens the comment window where you can write a comment after playing against an opponent

## Installation
Download the executable [here](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/releases/download/v1.0.0/Tekken-7-Player-Displayer.zip) or you can build it yourself by loading the .sln file in Visual Studio.

## Usage
Run the executable before or after opening Tekken 7, but not during a match and sometimes not while you are in the waiting room.
To be sure the program is loaded correctly your name should be displayed in the terminal like in the picture:

![](https://github.com/ParadiseAigo/Tekken-7-Player-Displayer/blob/master/3git.png)

If you get the message "New (not in list)" as the opponents name, it means the program didn't get their name but it did get their Steam id, and it was not found in the player list (list of people you played against). You can play against them and they will be saved, so you can identify them next time using only their Steam id.

If you get the message "Failed to get any info." as the opponents name it's best not to accept and instead wait for the next person because they won't be saved in the list if you choose to play against them.

## Issues
* The in-game display sometimes doesn't update fast enough and it ends up not displaying correct information. My recommendation is to always search in windowed mode and take your feedback from the terminal and switch back to full-screen before the match.
* There is a 1 out of 5 chance or so that the program is unable to get the name of the opponent. You can just decline those match requests.
* Your game language has to be in English, if it isn't the in-game display will not work, but the program window will still display the correct information.
* Sometimes some Steam names can cause the program to behave in untended ways. (rare)
