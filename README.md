<h1 align="center">PocketFS</h1>
<p align="center">
    <img src="https://img.shields.io/badge/version-0.5.0-blue" />
    <img src="https://img.shields.io/badge/License-MIT-yellow" />
    <img src="https://img.shields.io/badge/Platform-GBA-green" />
    <img src="https://img.shields.io/badge/Language-C-orange" />
    <img src="https://img.shields.io/badge/Contributors-1-blueviolet" />
</p>

>PocketFS is a light file system written in C for the Nintendo Game Boy Advance.

The file system will allow users to store and manage files on their GBA devices, providing a convenient way to organize and access data. The file system will be optimized for the limited resources of the GBA, taking into account the device's memory and processing constraints. It will provide efficient file storage and retrieval, ensuring that games and applications can access data quickly and reliably.

## Features
- Creation of nested directories and sub-directories
- Arbitrary files storing key meta-data
- Basic text editor

## Installation
Follow the [installation guide](https://devkitpro.org/wiki/devkitPro_pacman) to set up the devkitPro tool chain for your OS. Make sure to download the **gba-dev** group package, instructions listed at the bottom of that page.

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
<div>
<img src="https://i.imgur.com/aMDGG1k.png" width="50%" align="right">

<table>
    <tr>
        <th>Button</th>
        <th>Function</th>
    </tr>
    <tr>
        <td>Down</td>
        <td>Shift cursor down one</td>
    </tr>
    <tr>
        <td>Up</td>
        <td>Shift cursor up one</td>
    </tr>
    <tr>
        <td>Left</td>
        <td>Back out of directory</td>
    </tr>
    <tr>
        <td>Right</td>
        <td>Enter a directory/edit a file</td>
    </tr>
    <tr>
        <td>B</td>
        <td>Create a directory here</td>
    </tr>
    <tr>
        <td>A</td>
        <td>Create a file here</td>
    </tr>
    <tr>
        <td>Select</td>
        <td>Delete highlighted directory/file</td>
    </tr>
</table>
</div>
<br clear="right"/>

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)
