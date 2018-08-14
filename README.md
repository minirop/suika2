![demo](https://github.com/ktabata/suika2/raw/master/doc/icon.png "icon") Suika 2
=================================================================================

Suika 2 is a simple game engine that runs on both Windows and Mac. You can enjoy producing impressive visual novels using Suika 2.

## Description

In a strict sense, Suika 2 runs on Windows, Mac, Linux and Android. It's portable!

## Portability

Suika 2 is consist of platform independent part and hardware abstraction layer (HAL). Platform independent part is written in ANSI C with some GCC pragmas. HALs are currently written in C, Objective-C and Java.

If you want to port Suika 2 to a new target platform, it is only necessary to write a HAL.

## Demo

![demo](https://github.com/ktabata/suika2/raw/master/doc/screenshot.png "screenshot")

## Usage

* On Windows:
    * `git clone` the repository or download the zip file.
	* Open `suika2` folder in Explorer.
	* Double click `suika.exe`

* On Mac:
    * `git clone` the repository or download the zip file.
    * Open `suika2-mac` folder in Finder.
	* Copy `Suika` to the `/Applications` folder.

* On Linux:
    * `git clone` the repository or download the zip file.
	* Enter `suika2` directory in terminal.
	* run `./suika-linux`

## Contribution

Please write a issue with a light heart. Any and all feedback is welcome.

## Build

See build/BUILD.md

## License

MIT license.

## Author

[ktabata](https://github.com/ktabata)