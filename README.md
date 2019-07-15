# UXDemo for MiniGUI v3.2.x

This is a comprehensive demonstration of MiniGUI and its key components,
including mGEff, mGPlus, mGNCS, and mGNCS4Touch.

UXDemo implements a set user interfaces with special effects, which are
similar to the popular modern smart phones.

## Prerequisites

  * MiniGUI: v3.2.2 or later
  * MiniGUI Resource: v3.0.12 or later
  * mGPlus: v1.4.0 or later
  * mGEff: v1.2.0 or later
  * mGNCS: V1.2.2 or later
  * mGNCS4Touch: V1.2.2 or later

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

    $ ./autogen.sh; # if you got the source from GitHub repo
    $ ./configure;
    $ make
    $ sudo make install
    $ cd /usr/local/bin
    $ ./mguxdemo

## Copying

Copyright (C) 2018, 2019, Beijing FMSoft Technologies Co., Ltd.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

### Special Statement

The above open source or free software license does
not apply to any entity in the Exception List published by
Beijing FMSoft Technologies Co., Ltd.

If you are or the entity you represent is listed in the Exception List,
the above open source or free software license does not apply to you
or the entity you represent. Regardless of the purpose, you should not
use the software in any way whatsoever, including but not limited to
downloading, viewing, copying, distributing, compiling, and running.
If you have already downloaded it, you MUST destroy all of its copies.

The Exception List is published by FMSoft and may be updated
from time to time. For more information, please see
<https://www.fmsoft.cn/exception-list>.

