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
exits. You can measure a new split time (or "lap time") by pressing ctrl-c once.

And it has colors! Enabled by default, you can disable them with the environment variable
`ZEITKATZE_COLOR`. Just set it to `0` (`export ZEITKATZE_COLOR=0` does that, or add
`ZEITKATZE_COLOR=0` to your shell rc). There's even a command line switch to override it:
`--color` or `-c` to enable colors, `--no-color` or `-n` to disable.


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
You can use it by simply running the executable. Colors are enabled by default and seconds are measured with a precision of two decimals:
```
./zeitkatze
```
<img src="https://raw.githubusercontent.com/leonmavr/zeitkatze/master/assets/demo.gif" height="275" />

It supports the following command line options

| Short specifier | Long specifier            | Argument type | Default | Description                                                                              |
|:--------------- |:--------------------------|:--------------|:--------|:-----------------------------------------------------------------------------------------|
| -n              | --no-color                |  None         | On      |Disables colors when formatting the times on the terminal                                 |   
| -p              | --precision               |  integer      | 2       |How many decimal places to show when measuring time                                       | 
                     
