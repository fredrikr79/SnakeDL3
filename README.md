# SnakeDL3
a game of snake written in the brand new SDL3/C


## running the game

this game uses [nix](https://nixos.org/) to manage dependencies.

after downloading nix, you can simply clone the repository, enter the directory
with direnv enabled and you will automatically get all the dependencies needed.

if you don't have direnv, you can run 
```bash
    $ nix-shell .
```
to enter a shell with the required dependencies.

after this, you can run the game by typing
```bash
    $ make run
```
