# UXDemo for MiniGUI v3.2.x

This is a comprehensive demonstration of MiniGUI and its key components,
including mGEff, mGPlus, mGNCS, and mGNCS4Touch.

UXDemo implements a set user interfaces with special effects, which are
similar to the popular modern smart phones.

## Prerequisites

  * MiniGUI: v3.2.0 or later
  * MiniGUI Resource: v3.0.12 or later
  * mGPlus: v1.4.0 or later
  * mGEff: v1.2.0 or later
  * mGNCS: V1.2.0 or later
  * mGNCS4Touch: V1.2.0 or later

  * System library dependencies:
    * sqlite3-dev
    * libxml2-dev

  * 3rd-party libraries:
    * chipmunk-5.3.1 (included in `3rd-party` directory in build-minigui-3.2)

## Building

We strongly recommend you to build this demo by using the fetching and building
scripts of build-minigui-3.2:

    https://github.com/VincenWei/build-minigui-3.2

Or you can use the following commands to configure, make, install, and run it,
if you have installed the dependent libraries listed above.

Run

    $ ./autogen.sh; ./configure;
    $ make
    $ sudo make install
    $ cd /usr/local/bin
    $ ./mguxdemo

## Copying

    Copyright (C) 2018, Beijing FMSoft Technologies Co., Ltd.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

