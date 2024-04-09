
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

<h4>Within the file browser</h4>
    
| <table> <thead> <tr> <th>Button</th> <th>Function</th> </tr> </thead> <tbody> <tr> <td>Up</td> <td>Shift cursor up one</td> </tr> <tr> <td>Down</td> <td>Shift cursor down one</td> </tr> <tr> <td>Left</td> <td>Back out of directory</td> </tr> <tr> <td>Right</td> <td>Enter a directory/edit a file</td> </tr> <tr> <td>B</td> <td>Create a directory here</td> </tr> <tr> <td>A</td> <td>Create a file here</td> </tr> <tr> <td>Select</td> <td>Delete highlighted directory/file</td> </tr> <tr> <td>Start</td> <td>Save all</td> </tr> <tr> <td>L</td> <td>Rename directory/file</td> </tr> </tbody></table> | <img src="./assets/fs demo.png" width="100%"> |
| ------------- | ------------- |

<h4>Within the text editor</h4>

| <table> <thead> <tr> <th>Button</th> <th>Function</th> </tr> </thead> <tbody> <tr> <td>L</td> <td>Cycle selected character left</td> </tr> <tr> <td>R</td> <td>Cycle selected character right</td> </tr> <tr> <td>Up</td> <td>Shift cursor up one</td> </tr> <tr> <td>Down</td> <td>Shift cursor down one</td> </tr> <tr> <td>Left</td> <td>Shift cursor left one</td> </tr> <tr> <td>Right</td> <td>Shift cursor right one</td> </tr> <tr> <td>Select</td> <td>Shift-like modifier key to skip 26 characters left/right</td> </tr> <tr> <td>Start + Select</td> <td>Save and exit file</td> </tr> <tr> <td>A</td> <td>Print selected character</td> </tr> <tr> <td>B</td> <td>Print a space</td> </tr> </tbody></table> | <img src="./assets/text editor demo.png" width="100%"> |
| ------------- | ------------- |

## PocketFS Importer

> Since creating and editing file trees on the GBA can be inconveient, I've provided a Python GUI program to import an entire file tree into the PocketFS.



### Limitations

<table> <tr> <td style="text-align:left;"> <ul> <li style="text-align:left;">Only supports folders and .txt files</li> <li style="text-align:left;">Doesn't support '( and ')' characters. Replaces them with '&#40;' and '&#41;'</li> <li style="text-align:left;">There's a maximum of 16 sub-directories/files within one parent</li> <li style="text-align:left;">Each file should contain a maximum of 600, otherwise expect unintended effects</li> </ul> </td> <td> <img src="./assets/importer demo.png" width="100%"> </td> </tr></table>

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)
