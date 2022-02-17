zeitkatze
=========
[![C/C++ CI](https://github.com/leonmavr/zeitkatze/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/leonmavr/zeitkatze/actions/workflows/c-cpp.yml)
[![AUR version](https://img.shields.io/aur/version/zeitkatze)](https://aur.archlinux.org/packages/zeitkatze/)

Zeitkatze is time cat -- literally
```
=(^.^)= =(ˇ.ˇ)= =(o.o)m =(*.*)= =(^.^)= =(ˇ.ˇ)= =(o.o)m =(*.*)= =(^.^)= =(ˇ.ˇ)= =(o.o)m =(*.*)=
```

## 1. About
What's the simplest stopwatch in a linux console?
Well, just type `time cat` to start, and push ctrl-c to stop.
Zeitkatze is the same thing, with just a little extra.

It continuously updates the total elapsed time display, you can hit ctrl-c once to display a split time,
and if you quit (hitting ctrl-d, or ctrl-c twice in succession, or q), zeitkatze prints the total time and
exits. You can measure a new split time (or "lap time") by pressing ctrl-c once or by hitting Enter.

## 2. Installation

### 2.1 For Arch-based distros
If you run an Arch-based distro (Arch, Manjaro, Artix, etc.), you can install it
from the AUR, e.g. via the `yay` package wrapper:
```
yay -S zeitkatze
```

### 2.2 Manual installation
#### 2.2.1 Installation
Make sure you're in the root directory (same level as the `Makefile`). Then you
can install it directly with one command. It will compile the project for you
and install it at `$PREFIX/bin`, where `PREFIX` is defined in the `Makefile` --
typically `/usr`. Execute `sudo make install` to install it if you've cloned the
repo and you're at the root level. Alternatively, open up and terminal and paste
the following to install it:
```
cd `mktemp -d`
wget https://github.com/leonmavr/zeitkatze/archive/refs/heads/master.zip
unzip master.zip
cd zeitkatze-master
sudo make install
```

#### 2.2.2 Uninstallation
To uninstall it manually you can execute the following from any terminal:
```
sudo rm `which zeitkatze`
rm -rf ~/.config/zeitkatze
```
Or from the root level of the repository:
```
sudo make uninstall
```

## 3. Development
To execute any of the following steps, it's assumed that you're in the root
directory (same directory as the `Makefile`).  

To compile:
```
make
```
To clean:
```
make clean
```
You can use it by simply running the executable.
It accepts certain command line options described in section 4 -- Usage.
```
./zeitkatze
```
Don't forget that the naming coding coding conventions follow the C++ Google
standard ([example](https://github.com/chromium/chromium/blob/main/media/ffmpeg/ffmpeg_decoding_loop.h)) and the spacing and indentation convention the clang format.
You can execute the script `scripts/clang_format.sh` to format all source files.

## 4. Usage

| Short specifier | Long specifier            | Argument type | Default | Description                                                                              |
|:--------------- |:--------------------------|:--------------|:--------|:-----------------------------------------------------------------------------------------|
| -n              | --no-color                |  None         | Off     |Disables colors when formatting the times on the terminal                                 |   
| -p              | --precision               |  integer      | 2       |How many decimal places to show when measuring time                                       | 
| -o              | --one-line                |  None         | Off     |Whether or not to print the output in one line - in this case only prints lap times       | 
| -r              | --reset-emotes            |  None         | Off     |If true, re-writes (and overwrites) the default cat emotes at file `~/.config/zeitkatze/cats.txt`                                                                                                                                     | 
| -h              | --help                    |  None         | Off     |Print usage instructions and exit                                                         | 

Other features/tips:
* It also supports configuration via the environment variable `ZEITKATZE_COLOR`. Note that the
option `-n` or `--no-color` overrides this variable. If `ZEITKATZE_COLOR` is set to 0 (e.g. by
adding `ZEITKATZE_COLOR=0` in your rc), then colors are disabled.
* You can define custom cat emotes at `~/.config/zeitkatze/cats.txt`. This file is created by
default and can be overwritten. For a nicer output, make sure that all cats take the same number
of characters (you can pad with spaces if necessary).
* If you mess up your config file that contains the cat emotes (`~/.config/zeitkatze/cats.txt`),
you can reset it by calling the method `ResetEmotes()` on your `Zeitkatze` instance.

<p float="left">
    <img src="https://raw.githubusercontent.com/leonmavr/zeitkatze/master/assets/demo.gif" width="450" />
    <img src="https://raw.githubusercontent.com/leonmavr/zeitkatze/master/assets/demo_one_line.gif" width="450" />
</p>

### 5. Future ideas
- [x] Measure seconds with custom precision
- [x] Read cat emoticons from file
- [ ] Write times to file in a compact format
- [x] Add a command line specifier that calls ResetMethod() on Zeitkatze
- [x] Option for one-line output
- [ ] Animated cats with multithreading?
- [ ] Unicode support
- [ ] Fancier output with ncurses
- [ ] Unit test it by sending signals/keystrokes to the app

### 6. Issues
* None at the moment

### 7. Bugs
* ~~After running zeitkatze in a terminal, it changes its properties.
A side-effect of this is that if you try to enter a user password in
same terminal, it will not work: `sudo: no password was provided
sudo: a password is required`~~ Fixed as of [691b1de](https://github.com/leonmavr/zeitkatze/commit/f3c87b11023dc8c4818dbc054d1690cffb2f04c4).
