## Setting up vitaGL with Vita3K Emulator Support

The included script [./sdk/bootstrap-vita-sdk](./sdk/bootstrap-vita-sdk) will handle the below setup steps for you automatically. However the configuration details are explained for any who are curious.

### vitaShaRK (**vita** **Sha**ders **R**untime **K**ompiler)

From the [vitaGL README](https://github.com/Rinnegatamante/vitaGL), relevant macros we need are:

> * `HAVE_VITA3K_SUPPORT=1` Disables several features in order to make vitaGL compatible with Vita3K. Requires vitaShaRK compiled with https://github.com/Rinnegatamante/vitaShaRK/blob/master/source/vitashark.c#L25 uncommented.

Accordingly, we must compile [`vitaShaRK`](https://github.com/Rinnegatamante/vitaShaRK) with [line 25 uncommented in vitashark.c](https://github.com/Rinnegatamante/vitaShaRK/blob/master/source/vitashark.c#L25) and install into your vita sdk environment. Specifically, the line should read:

```console
$ sed -n '25p'  vitaShaRK/source/vitashark.c
#define DISABLE_SHACCCG_EXTENSIONS // Uncomment this to make vitaShaRK not depend on SceShaccCgExt
```

Note the lack of prefixing `//` comment indicator.

If you want to, make a copy of your current `$VITASDK/arm-vita-eabi/lib/libvitashark.a` library before compiling and install with shacccg extensions diabled to restore later.

Now, simply compile and install to your `$VITASDK`:
```console
$ cd vitaShaRK
$ make && make install
cp libvitashark.a /home/m/Projects/vita/vitetris/sdk/arm-vita-eabi/lib/
cp source/vitashark.h /home/m/Projects/vita/vitetris/sdk/arm-vita-eabi/include/
```

### vitaGL

With `libvitashark.a` compiled without shacccg extensions, proceed with compiling vitaGL with the
`HAVE_VITA3K_SUPPORT=1` macro defined as mentioned in the quote from the [vitaGL README](https://github.com/Rinnegatamante/vitaGL) above.

The Makefile at [vitaGL/Makefile](https://github.com/Rinnegatamante/vitaGL/blob/master/Makefile) is nice enough to allow providing this macro as an environment variable in:

```Makefile
ifeq ($(HAVE_VITA3K_SUPPORT),1)
CFLAGS += -DHAVE_VITA3K_SUPPORT -DDISABLE_HW_ETC1
endif
```

So simply run:
```console
$ git clone https://github.com/Rinnegatamante/vitaGL
$ cd vitaGL
$ make clean
$ export HAVE_VITA3K_SUPPORT=1
$ make && make install
$ cd ..
$ rm -rf vitaGL
```

### Testing in Vita3K

As of writing I am using vitaGL v0.3 and vitaShaRK v1.4. Here are the md5 hashes of the compiled libraries as they are on my machine:

```
4736dd7770aace4fe92417c67ef7162e  $VITASDK/arm-vita-eabi/lib/libvitaGL.a
4a283aba225e2f756ecb260442b61a49  $VITASDK/arm-vita-eabi/lib/libvitashark.a
```

With the libraries prepared, compile one of the vitaGL sampes (e.g. [glesgear](https://github.com/Rinnegatamante/vitaGL/tree/master/samples/glesgear)) and try opening the resulting .vpk in Vita3k.

It may take up to a minute depending on your machine, but just observe the titlebar of the Vita3K window to watch its progress compiling shaders and then running the app, and you should be greeted with the classic gear demo complete with vita button input support:

![image](https://user-images.githubusercontent.com/85039141/218005951-8900aa37-7092-4909-9774-f4e70ce0a9b5.png)

If Vita3K can run the demo, your $VITASDK environment is correctly setup to support creating executables that are compatible with Vita3K.