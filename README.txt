# Introduction
This project was originally developed in 2022. Due to my dissatisfaction with how HTML/CSS/JavaScript worked, I decided to create my own internet browser which instead uses Lua. I am mainly dissatisfied at the inneficiencies which splitting a page into 3 languages brings, along with the fragmentation of JavaScript (its ecosystem and the many frameworks).

The code is quite bad, contains a few hacks and is probably poorly optimised. But with this project I have learned how to implement Lua into a project and how to render a graphical interface in SDL. I am uploading it for archival purposes.

# Compiling
The program is compiled with MSVC and Visual Studio 2022. Include and link the following libraries:
* lua
* SDL2
* SDL2_image
* SDL2_ttf

I have also included a pre-compiled version in the releases section, but beware that this program is prone to crashes, undefind behaviour, bugs or other problems.

# Features
The Lua API provides methods to create text and buttons with links to other files, as well as positioning them within the page. Check the page.lua file for a demonstration.

# Contact me
If someone would like to continue to work on this idea, please contact me at the e-mail address on my profile! 

# Acknowledgments
Lazy Foo's SDL2 tutorials: https://lazyfoo.net/tutorials/SDL/
Titillium font