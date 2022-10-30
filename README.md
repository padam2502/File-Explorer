# FILE EXPLORER

A fully functional Linux based File Explorer application in GUI and command modes to navigate the entire file system
and perform various operations like create, update, delete, copy, move on files and directories using commands.


## Description

File Explorer supports working in two modes -
1. Normal mode (default mode) - used to explore the current directory and navigate the filesystem
2. Command mode - used to enter shell commands

### Normal mode:
Normal mode is the default mode of the application. It supports following functionalities -
1. Display a list of directories and files in the current folder
    - Every file in the directory is displayed on a new line with the following attributes for each file :<br>
        i. File Name<br>
        ii. File Size<br>
        iii. Ownership (user and group) and Permissions<br>
        iv. Last modified<br>
    - The file explorer shows entries “.” and “..” for current and parent directory respectively.

    - The file explorer handles scrolling in the case of vertical overflow using keys ```k``` & ```l```.
    - User can navigate up and down in the file list using the corresponding ```up``` and ```down``` arrow keys
2. Open directories and files when `enter` key is pressed :

    i. Directory : Clear the screen and navigate into the directory and shows the directory contents as specified in point 1.<br>
    ii. File : Open the file.
3. Traversal<br>

    i. Go back - `Left `arrow key takes the user to the previously visited directory.<br>
    ii. Go forward - `Right` arrow key takes the user to the next directory.<br>
    iii. Up one level - `Backspace` key takes the user up one level.<br>
    iv. Home -` h` key takes the user to the home folder (the folder where the
    application was started).<br>

### Command Mode:

The application enters the Command button whenever `: `(colon) key is pressed. 

1. In the
command mode, the user is able to enter the following different commands:
    - Copy - `copy <source_file(s)> <destination_directory>`

    - Move - `move <source_file(s)> <destination_directory>`

    - Rename - `rename <old_filename> <new_filename>`

    - Create File - `create_file <file_name> <destination_path>`

    - Create Directory - `create_dir <dir_name> <destination_path>`

    - Delete File - `delete_file <file_path>`

    - Delete Directory - `delete_dir <dir_path>`

    - Goto - `goto <location>`
    - Search -` search <file_name>` or `search <directory_name>`


2. On pressing `ESC` key, the application goes back to Normal Mode

3. On pressing `q` key, the application exit.



## Assumptions

* All the commands work with both Absolute and Relative paths, in absolute path whole
path will be given where as in relative path, path from root folder will be given in input.

## Execution
 1. `g++ main.cpp`
 2. `./a.out`
