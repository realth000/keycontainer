# KeyContainer
[中文文档](https://github.com/realth000/keycontainer/blob/master/README_zh_CN.md)
+ Password container application based on Qt5.
+ Do NOT need network accessibility.
+ Avaliable on Windows and Linux.
+ Protect password data in encrypted form.
+ Support export password in encryption and decryption.
+ Will support Android in future.
## How to build KeyContainer
Suppot build KeyContainer using both QMake and CMake.
+ Need Qt5(>= 5.10, suggest 5.15).
+ Need Windows SDK on Windows platform.
+ Need X11 on Linux platform.
### Build via QMake
1. Use QtCreator open ``KeyContainer.pro``
2. Select a kit.
3. Build.
5. Copy the login file ``login.ec`` to the output directory.
### Build via CMake
1. Cd the source code directory.
2. ``mkdir build && cd build``.
3. Export Qt modules path ``QT_CMAKE_PATH``(e.g.``export QT_CMAKE_PATH=/opt/Qt5.15.2/5.15.2/gcc_64/bin/cmake``), this step is not need when building in QtCreator.
4. ``cmake ../``
5. ``make``
6. Copy the login file ``login.ec`` to the output directory.
## License
KeyContainer use the following third-party repos:
### [Qt-AES](https://github.com/bricke/Qt-AES/)
+ Simple AES encrypt method.
+ Code used in saving password data.
### [Qt-Widgets-password-dialog-with-Caps-Lock-check](https://github.com/savolai/Qt-Widgets-password-dialog-with-Caps-Lock-check/)
+ Detection on CapsLock.
+ Code used in the part hint inputing password.
