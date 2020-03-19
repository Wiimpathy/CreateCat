# Create WiiFlow categories

createcat is a command line tool that write games categories to categories_lite.ini.

The program is scanning the cached .db files then it's parsing databases and write categories when found.
You can have your own main categories. By default, plugins games is using cat_plugin.ini in data folder.
Wii/GC is using cat_wiitdb.ini, the same categories from the master pack.

In the docs folder, you'll find all available genres/subgenres.
So if you want to customize your categories for plugins, you'd have to modify the cat#= line in cat_plugin.ini
and the corresponding cat# in the default categories_lite.ini from the data folder.


For WINDOWS a batch file called GOWIN.bat is provided. Open command line, then type :
GOWIN.bat E:\wiiflow\cache\lists for example.

You can modify the options in the batch by removing the comment. For example, use the fuzzy search :
set OPTION2=-fuzzy
Off course, you can use the createcat.exe directly too but that only work for a single .db file not a folder.


For LINUX, you can use the GOLINUX.sh script. GOLINUX.sh "myfile.db" or GOLINUX.sh "myfolder"
You can modify the options in the scripts.



## Usage

createcat.exe Path Option

### File

The path to the WiiFlow .db file.

### Option

-fuzzy :      Enable fuzzy search. Slower search and probable wrong results.

-mobysearch : Search also in the Mobygames database.

-skiphidden : Skip searching previously hidden games.

-debug :      Show debug information.

Examples :

  createcat.exe e:\WiiFlow\cache\lists\usb1_53454761.db -fuzzy

  createcat.exe e:\WiiFlow\cache\lists\usb1_53454761.db -fuzzy -mobysearch -debug


