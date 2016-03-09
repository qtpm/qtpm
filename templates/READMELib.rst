[[.Target]]
=================================

Description about your awesome package.

Install
--------------

This is a `qtpm <https://github.com/qtpm/qtpm>`_ package.

.. code-block:: bash

   $ qtpm get github.com/[[.AuthorName]]/[[.TargetSmall]]

Headers and a library file are installed under ``vendor/include`` and ``vendor/lib`` directory.

Example
--------------

.. code-block:: cpp

   #include <iostream>;
   #include <[[.TargetSmall]]/[[.TargetSmall]].h>

   int main() {
       auto obj = [[.Target]]();
       std::cout << obj.awesomeMethod() << std::endl;
   }

Reference
--------------

Write reference here

Develop
--------------

``qtpm`` command helps your development.

.. code-block:: bash

   $ git clone git@github.com:[[.AuthorName]]/[[.TargetSmall]].git
   $ cd [[.TargetSmall]]

   # Run the following command once if this package has required packages
   # -u updates modules they are already downloaded
   $ qtpm get [-u]

   # Build and run test
   $ qtpm test

   # Try install to ``vendor`` folder
   $ qtpm build


Folder Structure and Naming Convention
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: none

   + src/           // Header files under this folder will be published.
   |  |             // Don't add `main.cpp`. qtpm assumes this package is an application, not library.
   |  + private/    // Files under this folder will hide from outer modules.
   + resources/     // .qrc files are linked to application.
   + examples/      // Each .cpp file is build into executable.
   + test/          // Each _test.cpp file is built into executable and launched by qtpm test.
   |                // Use `qtpm add test`` to add test case.
   |                // Other .cpp files are used as utility code. They are linked to test modules.
   + doc/           // (not working yet) QDoc setting file and extra image files and so on.
   + html/          // (not working yet) HTML output of QDoc.
   + vendor/        // External libraries (source and binary)
   + build/         // Work folder.
   + CMakeExtra.txt // This file is included from created CMakeLists.txt.
                    // You can add any extra build options here.

Author
--------------

* [[.AuthorName]]

License
--------------

[[.LicenseName]]

History
--------------

* Write package history here