<h1 align="center">PocketFS</h1>
<p align="center">
    <img src="https://img.shields.io/badge/version-0.5.0-blue" />
    <img src="https://img.shields.io/badge/License-MIT-yellow" />
    <img src="https://img.shields.io/badge/Platform-GBA-green" />
    <img src="https://img.shields.io/badge/Language-C-orange" />
    <img src="https://img.shields.io/badge/Contributors-1-blueviolet" />
</p>

> PocketFS is a light file system written in C for the Nintendo Game Boy Advance.

The file system will allow users to store and manage files on their GBA devices, providing a convenient way to organize and access data. The file system will be optimized for the limited resources of the GBA, taking into account the device's memory and processing constraints. It will provide efficient file storage and retrieval, ensuring that games and applications can access data quickly and reliably.

## Features
- Creation of nested directories and sub-directories
- Basic text editor
- Saving functionality

## Installation
Follow the devkitPro's [Getting Started](https://devkitpro.org/wiki/Getting_Started) for the latest instructions on how to set up the necessary tool chain for your OS. 

Using devkitPro Pacman install the **gba-dev** group package.


| Button  | Function                          |
|---------|-----------------------------------|
| Up      | Shift cursor up one               |
| Down    | Shift cursor down one             |
| Left    | Back out of directory             |
| Right   | Enter a directory/edit a file     |
| B       | Create a directory here           |
| A       | Create a file here                |
| Select  | Delete highlighted directory/file |
| Start   | Save all                          |
| L       | Rename directory/file             |



#### Within the text editor

| Button        | Function                                                |
| ------------- | ------------------------------------------------------- |
| L             | Cycle selected character left                           |
| R             | Cycle selected character right                          |
| Up            | Shift cursor up one                                     |
| Down          | Shift cursor down one                                   |
| Left          | Shift cursor left one                                   |
| Right         | Shift cursor right one                                  |
| Select        | Shift-like modifier key to skip 26 characters left/right|
| Start + Select| Save and exit file                                      |
| A             | Print selected character                                |
| B             | Print a space                                           |



<br clear="right"/>

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)
