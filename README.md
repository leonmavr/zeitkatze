zeitkatze
=========

Zeitkatze is time cat -- literally
```
=(^.^)= =(ˇ.ˇ)= =(o.o)m =(*.*)= =(^.^)= =(ˇ.ˇ)= =(o.o)m =(*.*)= =(^.^)= =(ˇ.ˇ)= =(o.o)m =(*.*)=
```

## About
What's the simplest stopwatch in a linux console?
Well, just type `time cat` to start, and push ctrl-c to stop.
Zeitkatze is the same thing, with just a little extra.

It continuously updates the total elapsed time display, you can hit ctrl-c once to display a split time,
and if you quit (hitting ctrl-d, or ctrl-c twice in succession, or q), zeitkatze prints the total time and
exits. You can measure a new split time (or "lap time") by pressing ctrl-c once or by hitting Enter.


## Compilation and usage
To compile:
```
cd zeitkatze
make
```
To clean:
```
make clean
```
You can use it by simply running the executable. It accepts certain command line options described in the table below.
```
./zeitkatze
```

| Short specifier | Long specifier            | Argument type | Default | Description                                                                              |
|:--------------- |:--------------------------|:--------------|:--------|:-----------------------------------------------------------------------------------------|
| -n              | --no-color                |  None         | On      |Disables colors when formatting the times on the terminal                                 |   
| -p              | --precision               |  integer      | 2       |How many decimal places to show when measuring time                                       | 
| -o              | --one-line                |  None         | Off     |Whether or not to print the output in one line - in this case only prints lap times       | 
| -h              | --help                    |  None         | Off     |Print usage instructions and exit                                                         | 

Other features/tips:
* It also supports configuration via the environment variable `ZEITKATZE_COLOR`. Note that the option `-n` or `--no-color` overrides this variable. If `ZEITKATZE_COLOR` is set to 0 (e.g. by adding `ZEITKATZE_COLOR=0` in your rc), then colors are disabled.
* You can define custom cat emotes at `~/.config/zeitkatze/cats.txt`. This file is created by default and can be overwritten. For a nicer output, make sure that all cats take the same number of characters (you can pad with spaces if necessary).
* If you mess up your config file that contains the cat emotes (`~/.config/zeitkatze/cats.txt`), you can reset it by calling the method `ResetEmotes()` on your `Zeitkatze` instance.

<p float="left">
    <img src="https://raw.githubusercontent.com/leonmavr/zeitkatze/master/assets/demo.gif" width="450" />
    <img src="https://raw.githubusercontent.com/leonmavr/zeitkatze/master/assets/demo_one_line.gif" width="450" />
</p>

### Future ideas
- [x] Measure seconds with custom precision
- [x] Read cat emoticons from file
- [ ] Write times to file in a compact format
- [x] Option for one-line output
- [ ] Unicode support
- [ ] Fancier output with ncurses
- [ ] Unit test it by sending signals/keystrokes to the app
