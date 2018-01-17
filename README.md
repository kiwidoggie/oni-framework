# oni-framework
Embedded systems C2 software written in C/C#
# Oni-Framework


Oni framework is a platform for embedded device software development. It is used in order to remotely debug/add functionality to an embedded device. As of right now, the main concepts are a fully modular plugin system, with local and remote rpc. This was designed to run at ring-0 privilege level, but with some tweaks can also be used in ring-3 levels. Be warned, this is *NOT* for production use, only for developers who want an easy building platform for research.

  - Currently this project is in development, and may not be fully tested
  - Will need decent knowledge of C
  - Magic

# New Features!

  - Fully robust local and remote RPC system
  - Hot-swappable loading and unloading of plugin modules
  - Self-escalating from ring-3 to ring-0 (BYO-EntryPoint)
  - All used functions are stubbed out in order to ease porting to different platforms
  - File transfer utility


Features that are planned:
  - Debugging framework
  - Remote package management and install
  - Communication between devices

### Installation

Oni-framework just requires that you check out this directory and implement the `util/stubs.h` file with all of the implementations that are required. After this, it should compile just fine with gcc or clang and produce an executable file. You will need to BYO-execution method, but for most embedded devices this can be accomplished via a ssh shell, or a shell over telnet, or any shell for that matter. Other devices may have to load into running memory context, then jump to `main` which should take care of all of the initialization issues.

Install the dependencies and devDependencies and start the server.

### Developers

If you want to contribute, just submit a pull request. Otherwise, there is no real support for this at this point. This may change in the future
