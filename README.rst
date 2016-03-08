qtpm - Qt Package Manager Prototype
=======================================

Install
----------

.. code-block:: bash

   $ go get github.com/qtpm/qtpm

Usage
----------

**Create application template**

.. code-block:: bash

   $ mkdir helloworld
   $ qtpm init
   $ ls
   CMakeExtra.txt   LICENSE.rst build       include     qtpackage.toml  src     test        vendor
   $ qtpm build 

**Create library template**

.. code-block:: bash

   $ mkdir awesomesdk
   $ cd awsomesdk
   $ qtpm init app
   CMakeExtra.txt   LICENSE.rst build       include     qtpackage.toml  src     test        vendor
   $ qtpm build 

**Add files**

.. code-block:: bash

   $ qtpm add class MyDialog@QDialog
   $ qtpm add test TestMyDialog

**Change license**

.. code-block:: bash

   $ qtpm add license mit

Supported license names are mentioned at the below section.

Qt Location
--------------

It uses CMake behind qtpm command to build. By default, Qt should be in default (``CMAKE_PREFIX_PATH``). If you put Qt out of the folder,
there are two ways to specify the Qt location.

1. qtpm sees environment variable ``QTDIR``:

   .. code-block:: bash

      $ QTDIR=~/Qt/5.5/clang_64 qtpm build

2. put ``qtpackage.user.toml`` that contains the following contents:

   .. code-block:: none

      qtdir = 'C:\Qt\5.5\mingw492_32'

If you don't use the both settings and Qt is not in ``CMAKE_PREFIX_PATH``, qtpm tries to search any locations.

Name Convention
--------------------

This tool behaves according to the convention over any configuration.

* Source and header files are under ``src`` folder.
* Tests are under ``test`` folder.
* Resources are under ``resource`` folder.
* One project folder includes one executable file or one shared library as the output.
* If there is ``src/main.cpp``, qtpm generates executable, otherwise shared library
* Each test classes are implemented in ``test/*_test.cpp`` files (no header files) and compiled into executable.
* Other ``.cpp`` files in ``test`` are treated test utility. They are linked with each test executables.

Project File
-----------------

Project file is written in TOML format.

* ``name``: Project name.
* ``author``: Author name.
* ``license``: License name.
* ``requires``: Dependency packages like ``'github.com/shibukawa/qtobubus'`` (this feature is not implemented yet).
* ``qtmodules``: Required qt modules like ``Widgets``, ``Xml``.
* ``version``: Version number like ``[1, 0, 0]``.

Supported License Files
---------------------------

The following names can use with ``qtpm add license`` command:

* ``apache-v2.0``   : Apache License v2.0
* ``artistic-v2.0`` : The Artistic License v2.0
* ``bsd-2``         : Simplified BSD License (BSD 2-clause)
* ``bsd-3``         : Modified BSD License (BSD 3-clause)
* ``epl-v1.0``      : Eclipse Public License (EPL) v1.0
* ``gnu-agpl-v3.0`` : GNU Affero General Public License (AGPL) v3.0
* ``gnu-gpl-v2.0``  : GNU General Public License (GPL) v2.0
* ``gnu-gpl-v3.0``  : GNU General Public License (GPL) v3.0
* ``gnu-lgpl-v2.1`` : GNU Lesser General Public License (LGPL) v2.1
* ``gnu-lgpl-v3.0`` : GNU Lesser General Public License (LGPL) v3.0
* ``mit``           : The MIT License (MIT)
* ``mpl-v2.0``      : Mozilla Public License (MPL) v2.0
* ``unlicense``     : The Unlicense (Public Domain)

You can uses the following names as alias:

* ``apache2``       -> apache-v2.0
* ``bsd``           -> bsd-3
* ``eclipse``       -> epl-v1.0
* ``gpl``           -> gnu-gpl-v3.0
* ``gpl2``          -> gnu-gpl-v2.0
* ``gpl3``          -> gnu-gpl-v3.0
* ``lgpl``          -> gnu-lgpl-v3.0
* ``lgpl2``         -> gnu-lgpl-v2.1
* ``lgpl3``         -> gnu-lgpl-v3.0
* ``mozilla``       -> mpl-v2.0
* ``perl``          -> artistic-v2.0
* ``public domain`` -> unlicense
* ``x``             -> mit

License
--------------

MIT
