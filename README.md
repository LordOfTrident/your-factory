<p align="center">
	<h1 align="center">Your Factory</h2>
	<p align="center">A simple tycoon game written using C and SDL2</p>
</p>
<p align="center">
	<a href="./LICENSE">
		<img alt="License" src="https://img.shields.io/badge/license-GPL-blue?color=7aca00"/>
	</a>
	<a href="https://github.com/LordOfTrident/your-factory/issues">
		<img alt="Issues" src="https://img.shields.io/github/issues/LordOfTrident/your-factory?color=0088ff"/>
	</a>
	<a href="https://github.com/LordOfTrident/your-factory/pulls">
		<img alt="GitHub pull requests" src="https://img.shields.io/github/issues-pr/LordOfTrident/your-factory?color=0088ff"/>
	</a>
	<br><br><br>
<!--	<img width="450px" src="res/clip.gif"/> -->
</p>

## Table of contents
* [Introduction](#introduction)
* [Quickstart](#quickstart)
* [Milestones](#milestones)
* [Controls](#controls)
* [Bugs](#bugs)
* [Dependencies](#dependencies)
* [Make](#make)

## Introduction
A very fun isometric tycoon factory game, where you can build a factory on your land and expand the
land! This game is still in early stages

## Quickstart
```sh
$ make
$ ./bin/app
```

## Milestones
- [X] A simple tilemap where you can place and delete blocks
- [X] Gold system
- [X] Block placing UI
- [ ] Main menu
- [X] Editable ground blocks
- [ ] Droppers, things moving on the conveyors
- [ ] Save/load system

## Controls
| Key         | Action                |
| ----------- | --------------------- |
| W           | Move cursor up        |
| A           | Move cursor left      |
| S           | Move cursor down      |
| D           | Move cursor right     |
| R           | Rotate the block      |
| 1           | Deleting mode         |
| 2           | Placing mode          |
| 3           | Changing mode         |
| Enter       | Place/Delete          |
| ESC         | Exit the current mode |
| Up arrow    | Move the camera up    |
| Left arrow  | Move the camera left  |
| Down arrow  | Move the camera down  |
| Right arrow | Move the camera right |

## Bugs
If you find any bugs, please create an issue and report them.

## Dependencies
- [SDL2](https://www.libsdl.org/)

## Make
Run `make all` to see all the make rules.
