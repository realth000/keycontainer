# KeyContainer
 [English version README.md](https://github.com/realth000/keycontainer/blob/master/README.md)
## 描述
+ 基于Qt5的密码保存工具。
+ 不需要联网。
+ 支持Windows及Linux。
+ 加密保存密码数据。
+ 支持密码以明文及加密的形式导出。
+ 将会支持Android平台。

## 编译KeyContainer
可通过QMake或CMake编译KeyContainer。
+ 需要Qt5（>=5.10，推荐5.15）。
+ Windows上需要Windows SDK。
+ Linux上需要系统使用X11。
### 使用QMake编译
1. 使用QtCreator打开``KeyContainer.pro``。
2. 选择一个kit。
3. 编译（构建）。
4. 复制登录所需的密码文件``login.ec``到编译输出目录。
5. 默认密码:``1``
### 使用CMake编译
1. 进入源码目录。
2. 创建build文件夹并进入``mkdir build && cd build``。
3. export Qt的CMake module目录，例如``export QT_CMAKE_PATH=/opt/Qt5.15.2/5.15.2/gcc_64/lib/cmake``，如果使用QtCreator，跳过此步。
4. ``cmake ../``
5. ``make``
6. 复制登录所需的密码文件``login.ec``到编译输出目录。
7. 默认密码:``1``
## License
KeyContainer使用了以下第三方库：
### [Qt-AES](https://github.com/bricke/Qt-AES/)
+ 提供了AES加密/解密方法。
+ 在保存密码数据的部分中使用了这些代码。
### [Qt-Widgets-password-dialog-with-Caps-Lock-check](https://github.com/savolai/Qt-Widgets-password-dialog-with-Caps-Lock-check/)
+ 检测大写锁定的状态。
+ 在输入密码的部分使用了这些代码
