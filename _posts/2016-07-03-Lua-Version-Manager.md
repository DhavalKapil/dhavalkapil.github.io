---

layout: post
title: "Lua Version Manager"
author: "Dhaval Kapil"
excerpt: "Introducing luaver. Ease your Lua, Luarocks, LuaJIT installation"
keywords: lua, luarocks, luajit, version, manage, luaver, environment, install, installing, switch

---

This post is about installing and maintaining multiple versions of Lua, LuaJIT, and Luarocks using luaver. This is perhaps the easiest and the most systematic way to go about installing any of the above.

> ## Introducing Lua Version Manager (luaver)

Lua Version Manager or [luaver](https://dhavalkapil.com/luaver/) allows you to easily install and switch between multiple versions of lua, luajit, and luarocks in a seamless and consistent manner. The source code is on [Github](https://github.com/DhavalKapil/luaver).

> ### Motivation

I was working on a few projects involving lua such as [elasticsearch-lua](github.com/DhavalKapil/elasticsearch-lua) and the [sailor](https://github.com/sailorproject/sailor) web framework. Therein, I frequently had to shift between different versions of Lua to manage dependencies as well as for testing purposes. The following issues motivated me to create [luaver](https://dhavalkapil.com/luaver/):

1. Building different lua versions required some effort as I had to frequently shift between them.

2. Luarocks installation depends on the Lua version. Switching lua versions can mess up the installed rocks.

3. To solve these issues I used workaround methods such as editing the source code of some existing rocks.

4. Sometimes, these code changes broke the entire rock. In such cases, I had to remove all existing rocks, rebuild luarocks and then reinstall the needed rocks.

I was already familiar with NodeJS and Ruby and understood how such problems were addressed by [nvm](https://github.com/creationix/nvm) and [rvm](https://github.com/rvm/rvm).

> ## Installing Lua, LuaJIT, Luarocks with luaver

Using luaver it is very easy to install Lua, LuaJIT or Luarocks. It works by modifying your environment variables. Hence, every terminal session can have a separate environment.

> ### Installing luaver

First of all, you would need to install luaver itself.

```sh
curl https://raw.githubusercontent.com/DhavalKapil/luaver/master/install.sh -o install.sh && . ./install.sh
```

You might need to manually setup a ~/.bashrc or ~/.zshrc file.

> ### Installing Lua

To install lua you can simply specify the version you want to install:

```sh
luaver install 5.3.1  # Installs lua-5.3.1
```

Verify your installation by running:

```sh
lua -v
```

You might need to install `libreadline-dev` as a dependency for lua. To install older 32-bit lua versions on 64-bit machines, you will require some additional header files. You can get them by installing `lib32ncurses5-dev`.

> ### Installing LuaJIT

```sh
luaver install-luajit 2.0.2  # Installs luajit-2.0.2
```

> ### Installing Luarocks

```sh
luaver install-luarocks 2.3.0  # Installs luarocks-2.3.0
```

> ### Switching between versions

You can easily switch between different versions:

```sh
luaver use 5.3.2           # Switches to lua version 5.3.2
luaver use-luajit 2.0.0    # Switches to luajit version 2.0.0
luaver use-luarocks 2.3.0  # Switches to luarocks version 2.3.0
```

The switch will be instantaneous and without any glitches. Consistency will be maintained between lua and luarocks. Rocks are installed separately for different versions of luarocks and lua.

> ### Setting default version

You can also set default version of lua, luajit and luarocks that will be active whenever you start a new terminal session.

```sh
luaver set-default 5.3.2           # Set lua-5.3.2 as the default version
luaver set-default-luajit 2.0.0    # Set lua-2.0.0 as the default version
luaver set-default-luarocks 2.3.0  # Set lua-2.3.0 as the default version
```

> ### Listing all installed versions

```sh
luaver list           # Lists all installed lua versions
luaver list-luajit    # Lists all installed luajit versions
luaver list-luarocks  # Lists all installed luarocks versions
```

> ### Getting currently used versions of Lua, LuaJIT, Luarocks

The following command will give you the currently used versions:

```sh
luaver current
```

For complete usage run:

```sh
luaver help
```

> ### Facing any issue?

If you face any issue don't hesitate to [file an issue](https://github.com/DhavalKapil/luaver/issues) on the Github repository.

> ### Any suggestions, improvements?

luaver is still in its early stages. Feel free to submit a [pull request](https://github.com/DhavalKapil/luaver/pulls)! However if you are planning on some big thing, do [discuss](https://github.com/DhavalKapil/luaver/issues/new) it beforehand.
