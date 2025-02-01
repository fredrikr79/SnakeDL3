{ pkgs ? import <nixpkgs> {} }: pkgs.mkShell {
    nativeBuildInputs = with pkgs; [
        libgcc
        sdl3
        gnumake
    ];
}
