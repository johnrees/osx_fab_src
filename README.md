# Mac OS X 10.9 Fab Modules Installation Notes

This is simply a slightly modified clone of the original fabmodules code [http://kokompe.cba.mit.edu/downloads.html](http://kokompe.cba.mit.edu/downloads.html), differences can be found on this page  [https://github.com/johnrees/osx_fab_src/commit/8462ff40c25e7935e39587f93bba59cfe2d76631#commitcomment-4842875]([https://github.com/johnrees/osx_fab_src/commit/8462ff40c25e7935e39587f93bba59cfe2d76631#commitcomment-4842875)

## Dependencies (WXPython)

before you install dependencies with `brew install`, download development (2.9) build of wxpython from [http://www.wxpython.org/download.php](http://www.wxpython.org/download.php) with 2.8 you will probably experience an error like this

```
ImportError: dlopen(/usr/local/lib/wxPython-unicode-2.8.12.1/lib/python2.7/site-packages/wx-2.8-mac-unicode/wx/_core_.so, 2): no suitable image found.  Did find:
  /usr/local/lib/wxPython-unicode-2.8.12.1/lib/python2.7/site-packages/wx-2.8-mac-unicode/wx/_core_.so: no matching architecture in universal wrapper
```

## Installing fab modules

```
git clone http://github.com/johnrees/osx_fab_src
cd osx_fab_src
make fab
make install
```

`usr/local/bin` should already be in your PATH as you are using homebrew.
