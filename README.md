
<p align="center" style="margin-bottom: -20px;">
<img src="./assets/logo.png" width="30%">
</p>
<p align="center">
    <img src="https://img.shields.io/badge/version-0.6.0-blue" />
    <img src="https://img.shields.io/badge/License-MIT-yellow" />
    <img src="https://img.shields.io/badge/Platform-GBA-green" />
    <img src="https://img.shields.io/badge/Language-C-orange" />
    <img src="https://img.shields.io/badge/Contributors-1-blueviolet" />
</p>

> PocketFS is a light file system written in C for the Nintendo Game Boy Advance.

PocketFS allows users to create, edit, rename, and write to nested directories and .txt files on the Nintendo GBA. These file trees can be either saved to and loaded from the GBA's SRAM or can be imported from a Python script on the host machine.

## Features
- Creation of nested directories and sub-directories with files
- Deletion/Renaming of files and directories
- Basic text editor
- Saving functionality
- Importer Python script to load in a pre-made file tree.

## Installation

Follow the devkitPro's [Getting Started](https://devkitpro.org/wiki/Getting_Started) for the latest instructions on how to set up the necessary tool chain for your OS. Make sure to use devkitPro Pacman to install the **gba-dev** group package.

Once installed, run make to compile into a .gba file.

```bash
make
```

## Usage
You can either run the .gba file in an emulator or transfer to an actual GBA via a flashing kit such as the [Ever Drive](https://krikzz.com/our-products/cartridges/everdrive-gba-mini.html).

If using the [mgba emulator](https://mgba.io/downloads.html), you can then launch the program as follows:

```bash
mgba -4 pocketfs.gba
```

### Controls

<img src="./assets/fs demo.png" width="50%" align="right">

#### Within the file browser

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

<img src="./assets/text editor demo.png" width="50%" align="right">

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

## PocketFS Importer

> Since creating and editing file trees on the GBA can be inconveient, I've provided a Python GUI program to import an entire file tree into the PocketFS.

<img src="./assets/importer demo.png" width="75%" align="center">

### Limitations



- Only supports folders and .txt files
- Doesn't support '{' and '}'. Replaces them with '( and ')'
- There's a maximum of 16 sub-directories/files within one parent
- Each file should contain a maximum of 600, otherwise expect unintended effects

<br clear="right"/>

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)
