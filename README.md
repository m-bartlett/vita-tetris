# ViTetris

A Tetris-like homebrew game with 3D graphics for Playstation Vita.

This is written completely from scratch in C using the [vitaGL library](https://github.com/Rinnegatamante/vitaGL/) by [Rinnegatamante](https://github.com/Rinnegatamante).

<p align="center">
  <span>
    <img
      width="250px"
      alt="Screenshot 1"
      title="Screenshot 1"
      src="https://github.com/m-bartlett/vita-tetris/assets/85039141/62095254-35c7-4df2-b1b1-95537dabb9c6">
    <img
      width="250px"
      alt="Screenshot 2"
      title="Screenshot 2"
      src="https://github.com/m-bartlett/vita-tetris/assets/85039141/806a3fc6-eba3-4b5b-b508-c15f051927cd">
    <img
      width="250px"
      alt="Screenshot 3"
      title="Screenshot 3"
      src="https://github.com/m-bartlett/vita-tetris/assets/85039141/ef6853b9-fda0-43a3-94ae-6e6c090436c4">

  </span>
</p>

### 🚀 See this project's [FuHEN Homebrew Contest 2023 submission](https://fuhen.homebrew-contest.com/submissions/52/)
<br/>

## Disclaimer
This homebrew application is in no way affiliated with The Tetris Company, or any other third parties.

All product and company names are trademarks&trade; or registered trademarks&reg; of their respectve
holders. Use of them does not imply any affiliation with or endorsement by them. Any content
featured and shared here is done so without consent or knowledge of the copyright holders involved
unless otherwise noted. Anything featured here does not represent the official works in any way and
is made and distributed for free purely for educational, entertainment, and research purposes, which
are protected under Section 107 of the Copyright Act.

Please support official Tetris&reg; releases.


## Install

>**Note**
>This homebrew game, like any others that utilize the vitaGL library, require you to extract libshacccg.suprx on your Vita to compile shaders. Please see this guide for how to do this:<br/>
><https://cimmerian.gitbook.io/vita-troubleshooting-guide/shader-compiler/extract-libshacccg.suprx>
><br/>The simplest way as of writing to extract libshacccg.suprx is to install and run this app:<br/>
><https://vitadb.rinnegatamante.it/#/info/997>
<br/>


Simply download the [latest release .vpk](https://github.com/m-bartlett/vita-tetris/releases/latest/download/ViTetris.vpk) to your Vita and install by your preferred means. You should see a ViTetris bubble on the homescreen.

![ViTetris bubble on homescreen](https://github.com/m-bartlett/vita-tetris/assets/85039141/c1c51e02-d493-412a-b660-13d63aa36c94)



## Building & Development

Run the included script [./sdk/bootstrap-vita-sdk](./sdk/bootstrap-vita-sdk) which will automatically setup the Vita SDK for local builds. Run either `bootstrap-vita-sdk vita` or `bootstrap-vita-sdk Vita3K` to create isolated SDK environments configured to make builds for running on the Vita itself or the Vita3K emulator respectively. If you are running the script for the first time, you will see output indicating the progress of the SDK environment setup. If you run the scrip after the setting up the environment(s), an interactive subshell will be launch with the appropriate environment variables set. With the environment configured, it will be possible to run `make` in this repo's root to build the .vpk file.

E.g.
```console
$ ./sdk/bootstrap-vita-sdk 3k
(✜ⴰ[  ]ⴰ✤) Spawning environment in child shell.

(✜ⴰ[3K]ⴰ✤)$ make
arm-vita-eabi-gcc -g -Wl,-q -O2 -ftree-vectorize  -Werror -c -o src/lib/linalg.o src/lib/linalg.c
    ...
vita-elf-create ViTetris.elf ViTetris.velf
vita-make-fself ViTetris.velf eboot.bin
vita-mksfoex -s TITLE_ID=VGLTETRIS "ViTetris" param.sfo
vita-pack-vpk \
        --sfo=param.sfo \
        --eboot=eboot.bin \
        --add "sce_sys=sce_sys" \
        --add "src/graphics/texture=texture" \
        --add "src/graphics/shader=shader" \
        ViTetris.vpk
```

To build and run in the Vita3K emulator, run `Vita3K` which will be in `$PATH` after running `bootstrap-vita-sdk 3k` and configure it with the firmware and font packages as instructed (you will need to download these from the internet). Once Vita3K is configured with those files, run `make 3k` to either trigger an initial installation of the built `.vpk` file, or update the assets of an existing installation of ViTetris in the emulator.

[Vita SDK bootstrapper demo](https://github.com/m-bartlett/vita-tetris/assets/85039141/d81cf31d-21e4-435c-a40d-8c061e6cee97)


For a more detailed explanation for what library configuration is necessary to support building for Vita3K, see [Setting Up the Vita SDK to Compile for the Vita3K Emulator](Setting%20Up%20the%20Vita%20SDK%20to%20Compile%20for%20the%20Vita3K%20Emulator.md).
