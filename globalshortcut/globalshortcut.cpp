//#include "globalshortcut/globalshortcut.h"
//#include <QCoreApplication>
//#include <QDebug>

//globalShortCut::globalShortCut(QObject* parent)
//    : QObject(parent)
//{

//    nativeInit();
//    qApp->installNativeEventFilter(this);
//}


//globalShortCut::~globalShortCut() {
//    if (_isRegistered) { unregisterShortCut(); }
//    qApp->removeNativeEventFilter(this);
//}

//// 注册快捷键
//bool globalShortCut::registerShortCut(int Modifier, int key) {
//    // 快捷键已被占用
//    if (_isRegistered) {
//        qDebug().noquote() << "[Hotkey]" << "Hotkey already registered!";
//        return false;
//    }

////    // 修饰键不是一个，拒绝注册
////    if (keySplitted.length() != 2) {
////        qDebug().noquote() << "[Hotkey]" << "Must have only one key combination!";
////        return false;
////    }

//    bool successful = nativeRegisterShortCut(QKeyCombination((Qt::KeyboardModifiers)Modifier, (Qt::Key)key));
//    if (successful) {
//        _isRegistered = true;
//    } else {
//        qDebug().noquote() << "[Hotkey]" << "Failed to register hotkey (" + fromPressKeyToString(Modifier) + fromPressKeyToString(key) + ")!";
//    }
//    return successful;
//}

//bool globalShortCut::unregisterShortCut() {
//    // 没有被注册
//    if (!_isRegistered) {
//        qDebug().noquote() << "[Hotkey]" << "Hotkey not registered!";
//        return false;
//    }

//    bool successful = nativeUnregisterShortCut();
//    if (successful) {
//        _isRegistered = false;
//    } else {
//        qDebug().noquote() << "[Hotkey]" << "Failed to deregister hotkey!";
//    }
//    return successful;
//}

//#if defined(Q_OS_WIN)

//std::atomic<WPARAM> globalShortCut::_globalHotkeyCounter(0);

//void globalShortCut::nativeInit() {
//    _hotkeyId = static_cast<int>(_globalHotkeyCounter++);
//}

//bool globalShortCut::nativeRegisterShortCut(QKeyCombination keyCombination) {
//    if (_hotkeyId > 0xBFFF) {
//        qDebug().noquote() << "[Hotkey]" << "No more Hotkey IDs!"; //cannot be bothered to track IDs as this is unlikely
//        return false;
//    }

//    Qt::KeyboardModifiers KM = keyCombination.keyboardModifiers();
//    Qt::Key K = keyCombination.key();

//    uint modValue = 0;
//    if (KM & Qt::AltModifier)     { modValue += MOD_ALT; }
//    if (KM & Qt::ControlModifier) { modValue += MOD_CONTROL; }
//    if (KM & Qt::ShiftModifier)   { modValue += MOD_SHIFT; }

//    if (KM & ~(Qt::AltModifier | Qt::ControlModifier | Qt::ShiftModifier | Qt::MetaModifier)) {
//        qDebug().noquote().nospace() << "[Hotkey] " << "Unrecognized modifiers (" << KM << ")!";
//        return false;
//    }

//    uint keyValue;
//    if (((K >= Qt::Key_A) && (K <= Qt::Key_Z)) || ((K >= Qt::Key_0) && (K <= Qt::Key_9))) {
//        keyValue = K;
//    } else if ((K >= Qt::Key_F1) && (K <= Qt::Key_F24)) {
//        keyValue = VK_F1 + (K - Qt::Key_F1);
//    } else {
//        qDebug().noquote().nospace() << "[Hotkey] " << "Unrecognized key (" << K << ")!";
//        return false;
//    }

//    return RegisterHotKey(nullptr, _hotkeyId, modValue, keyValue);
//}

//bool globalShortCut::nativeUnregisterShortCut() {
//    return UnregisterHotKey(nullptr, _hotkeyId);
//}

//QString globalShortCut::fromPressKeyToString(int key)
//{
//    switch (key) {
//    case 0x01000000:
//        return "Escape";
//    case 0x01000001:
//        return "Tab";
//    case 0x01000002:
//        return "Backtab";
//    case 0x01000003:
//        return "Backspace";
//    case 0x01000004:
//        return "Return";
//    case 0x01000005:
//        return "Enter";
//    case 0x01000006:
//        return "Insert";
//    case 0x01000007:
//        return "Delete";
//    case 0x01000008:
//        return "Pause";
//    case 0x01000009:
//        return "Print";
//    case 0x0100000a:
//        return "SysReq";
//    case 0x0100000b:
//        return "Clear";
//    case 0x01000010:
//        return "Home";
//    case 0x01000011:
//        return "End";
//    case 0x01000012:
//        return "Left";
//    case 0x01000013:
//        return "Up";
//    case 0x01000014:
//        return "Right";
//    case 0x01000015:
//        return "Down";
//    case 0x01000016:
//        return "PageUp";
//    case 0x01000017:
//        return "PageDown";
//    case 0x01000020:
//        return "Shift";
//    case 0x01000021:
//        return "Control";
//    case 0x01000022:
//        return "Meta";
//    case 0x01000023:
//        return "Alt";
//    case 0x01001103:
//        return "AltGr";
//    case 0x01000024:
//        return "CapsLock";
//    case 0x01000025:
//        return "NumLock";
//    case 0x01000026:
//        return "ScrollLock";
//    case 0x01000030:
//        return "F1";
//    case 0x01000031:
//        return "F2";
//    case 0x01000032:
//        return "F3";
//    case 0x01000033:
//        return "F4";
//    case 0x01000034:
//        return "F5";
//    case 0x01000035:
//        return "F6";
//    case 0x01000036:
//        return "F7";
//    case 0x01000037:
//        return "F8";
//    case 0x01000038:
//        return "F9";
//    case 0x01000039:
//        return "F10";
//    case 0x0100003a:
//        return "F11";
//    case 0x0100003b:
//        return "F12";
//    case 0x0100003c:
//        return "F13";
//    case 0x0100003d:
//        return "F14";
//    case 0x0100003e:
//        return "F15";
//    case 0x0100003f:
//        return "F16";
//    case 0x01000040:
//        return "F17";
//    case 0x01000041:
//        return "F18";
//    case 0x01000042:
//        return "F19";
//    case 0x01000043:
//        return "F20";
//    case 0x01000044:
//        return "F21";
//    case 0x01000045:
//        return "F22";
//    case 0x01000046:
//        return "F23";
//    case 0x01000047:
//        return "F24";
//    case 0x01000048:
//        return "F25";
//    case 0x01000049:
//        return "F26";
//    case 0x0100004a:
//        return "F27";
//    case 0x0100004b:
//        return "F28";
//    case 0x0100004c:
//        return "F29";
//    case 0x0100004d:
//        return "F30";
//    case 0x0100004e:
//        return "F31";
//    case 0x0100004f:
//        return "F32";
//    case 0x01000050:
//        return "F33";
//    case 0x01000051:
//        return "F34";
//    case 0x01000052:
//        return "F35";
//    case 0x01000053:
//        return "L";
//    case 0x01000054:
//        return "R";
//    case 0x01000055:
//        return "Menu";
//    case 0x01000056:
//        return "L";
//    case 0x01000057:
//        return "R";
//    case 0x01000058:
//        return "Help";
//    case 0x01000059:
//        return "L";
//    case 0x01000060:
//        return "R";
//    case 0x20:
//        return "Space";
//    case 0x21:
//        return "Exclam";
//    case 0x22:
//        return "QuoteDbl";
//    case 0x23:
//        return "NumberSign";
//    case 0x24:
//        return "Dollar";
//    case 0x25:
//        return "Percent";
//    case 0x26:
//        return "Ampersand";
//    case 0x27:
//        return "Apostrophe";
//    case 0x28:
//        return "ParenLeft";
//    case 0x29:
//        return "ParenRight";
//    case 0x2a:
//        return "Asterisk";
//    case 0x2b:
//        return "Plus";
//    case 0x2c:
//        return "Comma";
//    case 0x2d:
//        return "Minus";
//    case 0x2e:
//        return "Period";
//    case 0x2f:
//        return "Slash";
//    case 0x30:
//        return "0";
//    case 0x31:
//        return "1";
//    case 0x32:
//        return "2";
//    case 0x33:
//        return "3";
//    case 0x34:
//        return "4";
//    case 0x35:
//        return "5";
//    case 0x36:
//        return "6";
//    case 0x37:
//        return "7";
//    case 0x38:
//        return "8";
//    case 0x39:
//        return "9";
//    case 0x3a:
//        return "Colon";
//    case 0x3b:
//        return "Semicolon";
//    case 0x3c:
//        return "Less";
//    case 0x3d:
//        return "Equal";
//    case 0x3e:
//        return "Greater";
//    case 0x3f:
//        return "Question";
//    case 0x40:
//        return "At";
//    case 0x41:
//        return "A";
//    case 0x42:
//        return "B";
//    case 0x43:
//        return "C";
//    case 0x44:
//        return "D";
//    case 0x45:
//        return "E";
//    case 0x46:
//        return "F";
//    case 0x47:
//        return "G";
//    case 0x48:
//        return "H";
//    case 0x49:
//        return "I";
//    case 0x4a:
//        return "J";
//    case 0x4b:
//        return "K";
//    case 0x4c:
//        return "L";
//    case 0x4d:
//        return "M";
//    case 0x4e:
//        return "N";
//    case 0x4f:
//        return "O";
//    case 0x50:
//        return "P";
//    case 0x51:
//        return "Q";
//    case 0x52:
//        return "R";
//    case 0x53:
//        return "S";
//    case 0x54:
//        return "T";
//    case 0x55:
//        return "U";
//    case 0x56:
//        return "V";
//    case 0x57:
//        return "W";
//    case 0x58:
//        return "X";
//    case 0x59:
//        return "Y";
//    case 0x5a:
//        return "Z";
//    case 0x5b:
//        return "BracketLeft";
//    case 0x5c:
//        return "Backslash";
//    case 0x5d:
//        return "BracketRight";
//    case 0x5e:
//        return "AsciiCircum";
//    case 0x5f:
//        return "Underscore";
//    case 0x60:
//        return "QuoteLeft";
//    case 0x7b:
//        return "BraceLeft";
//    case 0x7c:
//        return "Bar";
//    case 0x7d:
//        return "BraceRight";
//    case 0x7e:
//        return "AsciiTilde";
//    case 0x0a0:
//        return "nobreakspace";
//    case 0x0a1:
//        return "exclamdown";
//    case 0x0a2:
//        return "cent";
//    case 0x0a3:
//        return "sterling";
//    case 0x0a4:
//        return "currency";
//    case 0x0a5:
//        return "yen";
//    case 0x0a6:
//        return "brokenbar";
//    case 0x0a7:
//        return "section";
//    case 0x0a8:
//        return "diaeresis";
//    case 0x0a9:
//        return "copyright";
//    case 0x0aa:
//        return "ordfeminine";
//    case 0x0ab:
//        return "guillemotleft";
//    case 0x0ac:
//        return "notsign";
//    case 0x0ad:
//        return "hyphen";
//    case 0x0ae:
//        return "registered";
//    case 0x0af:
//        return "macron";
//    case 0x0b0:
//        return "degree";
//    case 0x0b1:
//        return "plusminus";
//    case 0x0b2:
//        return "twosuperior";
//    case 0x0b3:
//        return "threesuperior";
//    case 0x0b4:
//        return "acute";
//    case 0x0b5:
//        return "mu";
//    case 0x0b6:
//        return "paragraph";
//    case 0x0b7:
//        return "periodcentered";
//    case 0x0b8:
//        return "cedilla";
//    case 0x0b9:
//        return "onesuperior";
//    case 0x0ba:
//        return "masculine";
//    case 0x0bb:
//        return "guillemotright";
//    case 0x0bc:
//        return "onequarter";
//    case 0x0bd:
//        return "onehalf";
//    case 0x0be:
//        return "threequarters";
//    case 0x0bf:
//        return "questiondown";
//    case 0x0c0:
//        return "Agrave";
//    case 0x0c1:
//        return "Aacute";
//    case 0x0c2:
//        return "Acircumflex";
//    case 0x0c3:
//        return "Atilde";
//    case 0x0c4:
//        return "Adiaeresis";
//    case 0x0c5:
//        return "Aring";
//    case 0x0c6:
//        return "AE";
//    case 0x0c7:
//        return "Ccedilla";
//    case 0x0c8:
//        return "Egrave";
//    case 0x0c9:
//        return "Eacute";
//    case 0x0ca:
//        return "Ecircumflex";
//    case 0x0cb:
//        return "Ediaeresis";
//    case 0x0cc:
//        return "Igrave";
//    case 0x0cd:
//        return "Iacute";
//    case 0x0ce:
//        return "Icircumflex";
//    case 0x0cf:
//        return "Idiaeresis";
//    case 0x0d0:
//        return "ETH";
//    case 0x0d1:
//        return "Ntilde";
//    case 0x0d2:
//        return "Ograve";
//    case 0x0d3:
//        return "Oacute";
//    case 0x0d4:
//        return "Ocircumflex";
//    case 0x0d5:
//        return "Otilde";
//    case 0x0d6:
//        return "Odiaeresis";
//    case 0x0d7:
//        return "multiply";
//    case 0x0d8:
//        return "Ooblique";
//    case 0x0d9:
//        return "Ugrave";
//    case 0x0da:
//        return "Uacute";
//    case 0x0db:
//        return "Ucircumflex";
//    case 0x0dc:
//        return "Udiaeresis";
//    case 0x0dd:
//        return "Yacute";
//    case 0x0de:
//        return "THORN";
//    case 0x0df:
//        return "ssharp";
//    case 0x0f7:
//        return "division";
//    case 0x0ff:
//        return "ydiaeresis";
//    case 0x01001120:
//        return "key";
//    case 0x01001137:
//        return "Codeinput";
//    case 0x0100113c:
//        return "SingleCandidate";
//    case 0x0100113d:
//        return "MultipleCandidate";
//    case 0x0100113e:
//        return "PreviousCandidate";
//    case 0x0100117e:
//        return "switch";
//    case 0x01001121:
//        return "Kanji";
//    case 0x01001122:
//        return "Muhenkan";
//    case 0x01001123:
//        return "Henkan";
//    case 0x01001124:
//        return "Romaji";
//    case 0x01001125:
//        return "Hiragana";
//    case 0x01001126:
//        return "Katakana";
//    case 0x01001127:
//        return "Katakana";
//    case 0x01001128:
//        return "Zenkaku";
//    case 0x01001129:
//        return "Hankaku";
//    case 0x0100112a:
//        return "Hankaku";
//    case 0x0100112b:
//        return "Touroku";
//    case 0x0100112c:
//        return "Massyo";
//    case 0x0100112d:
//        return "Lock";
//    case 0x0100112e:
//        return "Shift";
//    case 0x0100112f:
//        return "Shift";
//    case 0x01001130:
//        return "toggle";
//    case 0x01001131:
//        return "Hangul";
//    case 0x01001132:
//        return "Start";
//    case 0x01001133:
//        return "End";
//    case 0x01001134:
//        return "Hanja";
//    case 0x01001135:
//        return "Jamo";
//    case 0x01001136:
//        return "Romaja";
//    case 0x01001138:
//        return "Jeonja";
//    case 0x01001139:
//        return "Banja";
//    case 0x0100113a:
//        return "PreHanja";
//    case 0x0100113b:
//        return "PostHanja";
//    case 0x0100113f:
//        return "Special";
//    case 0x01001250:
//        return "Grave";
//    case 0x01001251:
//        return "Acute";
//    case 0x01001252:
//        return "Circumflex";
//    case 0x01001253:
//        return "Tilde";
//    case 0x01001254:
//        return "Macron";
//    case 0x01001255:
//        return "Breve";
//    case 0x01001256:
//        return "Abovedot";
//    case 0x01001257:
//        return "Diaeresis";
//    case 0x01001258:
//        return "Abovering";
//    case 0x01001259:
//        return "Doubleacute";
//    case 0x0100125a:
//        return "Caron";
//    case 0x0100125b:
//        return "Cedilla";
//    case 0x0100125c:
//        return "Ogonek";
//    case 0x0100125d:
//        return "Iota";
//    case 0x0100125e:
//        return "Sound";
//    case 0x0100125f:
//        return "Sound";
//    case 0x01001260:
//        return "Belowdot";
//    case 0x01001261:
//        return "Hook";
//    case 0x01001262:
//        return "Horn";
//    case 0x01001263:
//        return "Stroke";
//    case 0x01001264:
//        return "Abovecomma";
//    case 0x01001265:
//        return "Abovereversedcomma";
//    case 0x01001266:
//        return "Doublegrave";
//    case 0x01001267:
//        return "Belowring";
//    case 0x01001268:
//        return "Belowmacron";
//    case 0x01001269:
//        return "Belowcircumflex";
//    case 0x0100126a:
//        return "Belowtilde";
//    case 0x0100126b:
//        return "Belowbreve";
//    case 0x0100126c:
//        return "Belowdiaeresis";
//    case 0x0100126d:
//        return "Invertedbreve";
//    case 0x0100126e:
//        return "Belowcomma";
//    case 0x0100126f:
//        return "Currency";
//    case 0x01001280:
//        return "a";
//    case 0x01001281:
//        return "A";
//    case 0x01001282:
//        return "e";
//    case 0x01001283:
//        return "E";
//    case 0x01001284:
//        return "i";
//    case 0x01001285:
//        return "I";
//    case 0x01001286:
//        return "o";
//    case 0x01001287:
//        return "O";
//    case 0x01001288:
//        return "u";
//    case 0x01001289:
//        return "U";
//    case 0x0100128a:
//        return "Schwa";
//    case 0x0100128b:
//        return "Schwa";
//    case 0x0100128c:
//        return "Greek";
//    case 0x01001290:
//        return "Lowline";
//    case 0x01001291:
//        return "Aboveverticalline";
//    case 0x01001292:
//        return "Belowverticalline";
//    case 0x01001293:
//        return "Longsolidusoverlay";
//    case 0x01000061:
//        return "Back";
//    case 0x01000062:
//        return "Forward";
//    case 0x01000063:
//        return "Stop";
//    case 0x01000064:
//        return "Refresh";
//    case 0x01000070:
//        return "VolumeDown";
//    case 0x01000071:
//        return "VolumeMute";
//    case 0x01000072:
//        return "VolumeUp";
//    case 0x01000073:
//        return "BassBoost";
//    case 0x01000074:
//        return "BassUp";
//    case 0x01000075:
//        return "BassDown";
//    case 0x01000076:
//        return "TrebleUp";
//    case 0x01000077:
//        return "TrebleDown";
//    case 0x01000080:
//        return "MediaPlay";
//    case 0x01000081:
//        return "MediaStop";
//    case 0x01000082:
//        return "MediaPrevious";
//    case 0x01000083:
//        return "MediaNext";
//    case 0x01000084:
//        return "MediaRecord";
//    case 0x1000085:
//        return "MediaPause";
//    case 0x1000086:
//        return "MediaTogglePlayPause";
//    case 0x01000090:
//        return "HomePage";
//    case 0x01000091:
//        return "Favorites";
//    case 0x01000092:
//        return "Search";
//    case 0x01000093:
//        return "Standby";
//    case 0x01000094:
//        return "OpenUrl";
//    case 0x010000a0:
//        return "LaunchMail";
//    case 0x010000a1:
//        return "LaunchMedia";
//    case 0x010000a2:
//        return "Launch0";
//    case 0x010000a3:
//        return "Launch1";
//    case 0x010000a4:
//        return "Launch2";
//    case 0x010000a5:
//        return "Launch3";
//    case 0x010000a6:
//        return "Launch4";
//    case 0x010000a7:
//        return "Launch5";
//    case 0x010000a8:
//        return "Launch6";
//    case 0x010000a9:
//        return "Launch7";
//    case 0x010000aa:
//        return "Launch8";
//    case 0x010000ab:
//        return "Launch9";
//    case 0x010000ac:
//        return "LaunchA";
//    case 0x010000ad:
//        return "LaunchB";
//    case 0x010000ae:
//        return "LaunchC";
//    case 0x010000af:
//        return "LaunchD";
//    case 0x010000b0:
//        return "LaunchE";
//    case 0x010000b1:
//        return "LaunchF";
//    case 0x0100010e:
//        return "LaunchG";
//    case 0x0100010f:
//        return "LaunchH";
//    case 0x010000b2:
//        return "MonBrightnessUp";
//    case 0x010000b3:
//        return "MonBrightnessDown";
//    case 0x010000b4:
//        return "KeyboardLightOnOff";
//    case 0x010000b5:
//        return "KeyboardBrightnessUp";
//    case 0x010000b6:
//        return "KeyboardBrightnessDown";
//    case 0x010000b7:
//        return "PowerOff";
//    case 0x010000b8:
//        return "WakeUp";
//    case 0x010000b9:
//        return "Eject";
//    case 0x010000ba:
//        return "ScreenSaver";
//    case 0x010000bb:
//        return "WWW";
//    case 0x010000bc:
//        return "Memo";
//    case 0x010000bd:
//        return "LightBulb";
//    case 0x010000be:
//        return "Shop";
//    case 0x010000bf:
//        return "History";
//    case 0x010000c0:
//        return "AddFavorite";
//    case 0x010000c1:
//        return "HotLinks";
//    case 0x010000c2:
//        return "BrightnessAdjust";
//    case 0x010000c3:
//        return "Finance";
//    case 0x010000c4:
//        return "Community";
//    case 0x010000c5:
//        return "AudioRewind";
//    case 0x010000c6:
//        return "BackForward";
//    case 0x010000c7:
//        return "ApplicationLeft";
//    case 0x010000c8:
//        return "ApplicationRight";
//    case 0x010000c9:
//        return "Book";
//    case 0x010000ca:
//        return "CD";
//    case 0x010000cb:
//        return "Calculator";
//    case 0x010000cc:
//        return "ToDoList";
//    case 0x010000cd:
//        return "ClearGrab";
//    case 0x010000ce:
//        return "Close";
//    case 0x010000cf:
//        return "Copy";
//    case 0x010000d0:
//        return "Cut";
//    case 0x010000d1:
//        return "Display";
//    case 0x010000d2:
//        return "DOS";
//    case 0x010000d3:
//        return "Documents";
//    case 0x010000d4:
//        return "Excel";
//    case 0x010000d5:
//        return "Explorer";
//    case 0x010000d6:
//        return "Game";
//    case 0x010000d7:
//        return "Go";
//    case 0x010000d8:
//        return "iTouch";
//    case 0x010000d9:
//        return "LogOff";
//    case 0x010000da:
//        return "Market";
//    case 0x010000db:
//        return "Meeting";
//    case 0x010000dc:
//        return "MenuKB";
//    case 0x010000dd:
//        return "MenuPB";
//    case 0x010000de:
//        return "MySites";
//    case 0x010000df:
//        return "News";
//    case 0x010000e0:
//        return "OfficeHome";
//    case 0x010000e1:
//        return "Option";
//    case 0x010000e2:
//        return "Paste";
//    case 0x010000e3:
//        return "Phone";
//    case 0x010000e4:
//        return "Calendar";
//    case 0x010000e5:
//        return "Reply";
//    case 0x010000e6:
//        return "Reload";
//    case 0x010000e7:
//        return "RotateWindows";
//    case 0x010000e8:
//        return "RotationPB";
//    case 0x010000e9:
//        return "RotationKB";
//    case 0x010000ea:
//        return "Save";
//    case 0x010000eb:
//        return "Send";
//    case 0x010000ec:
//        return "Spell";
//    case 0x010000ed:
//        return "SplitScreen";
//    case 0x010000ee:
//        return "Support";
//    case 0x010000ef:
//        return "TaskPane";
//    case 0x010000f0:
//        return "Terminal";
//    case 0x010000f1:
//        return "Tools";
//    case 0x010000f2:
//        return "Travel";
//    case 0x010000f3:
//        return "Video";
//    case 0x010000f4:
//        return "Word";
//    case 0x010000f5:
//        return "Xfer";
//    case 0x010000f6:
//        return "ZoomIn";
//    case 0x010000f7:
//        return "ZoomOut";
//    case 0x010000f8:
//        return "Away";
//    case 0x010000f9:
//        return "Messenger";
//    case 0x010000fa:
//        return "WebCam";
//    case 0x010000fb:
//        return "MailForward";
//    case 0x010000fc:
//        return "Pictures";
//    case 0x010000fd:
//        return "Music";
//    case 0x010000fe:
//        return "Battery";
//    case 0x010000ff:
//        return "Bluetooth";
//    case 0x01000100:
//        return "WLAN";
//    case 0x01000101:
//        return "UWB";
//    case 0x01000102:
//        return "AudioForward";
//    case 0x01000103:
//        return "AudioRepeat";
//    case 0x01000104:
//        return "AudioRandomPlay";
//    case 0x01000105:
//        return "Subtitle";
//    case 0x01000106:
//        return "AudioCycleTrack";
//    case 0x01000107:
//        return "Time";
//    case 0x01000108:
//        return "Hibernate";
//    case 0x01000109:
//        return "View";
//    case 0x0100010a:
//        return "TopMenu";
//    case 0x0100010b:
//        return "PowerDown";
//    case 0x0100010c:
//        return "Suspend";
//    case 0x0100010d:
//        return "ContrastAdjust";
//    case 0x01000110:
//        return "TouchpadToggle";
//    case 0x01000111:
//        return "TouchpadOn";
//    case 0x01000112:
//        return "TouchpadOff";
//    case 0x01000113:
//        return "MicMute";
//    case 0x01000114:
//        return "Red";
//    case 0x01000115:
//        return "Green";
//    case 0x01000116:
//        return "Yellow";
//    case 0x01000117:
//        return "Blue";
//    case 0x01000118:
//        return "ChannelUp";
//    case 0x01000119:
//        return "ChannelDown";
//    case 0x0100011a:
//        return "Guide";
//    case 0x0100011b:
//        return "Info";
//    case 0x0100011c:
//        return "Settings";
//    case 0x0100011d:
//        return "MicVolumeUp";
//    case 0x0100011e:
//        return "MicVolumeDown";
//    case 0x01000120:
//        return "New";
//    case 0x01000121:
//        return "Open";
//    case 0x01000122:
//        return "Find";
//    case 0x01000123:
//        return "Undo";
//    case 0x01000124:
//        return "Redo";
//    case 0x0100ffff:
//        return "MediaLast";
//    case 0x01ffffff:
//        return "unknown";
//    case 0x01100004:
//        return "Call";
//    case 0x01100020:
//        return "Camera";
//    case 0x01100021:
//        return "CameraFocus";
//    case 0x01100000:
//        return "Context1";
//    case 0x01100001:
//        return "Context2";
//    case 0x01100002:
//        return "Context3";
//    case 0x01100003:
//        return "Context4";
//    case 0x01100006:
//        return "Flip";
//    case 0x01100005:
//        return "Hangup";
//    case 0x01010002:
//        return "No";
//    case 0x01010000:
//        return "Select";
//    case 0x01010001:
//        return "Yes";
//    case 0x01100007:
//        return "ToggleCallHangup";
//    case 0x01100008:
//        return "VoiceDial";
//    case 0x01100009:
//        return "LastNumberRedial";
//    case 0x01020003:
//        return "Execute";
//    case 0x01020002:
//        return "Printer";
//    case 0x01020005:
//        return "Play";
//    case 0x01020004:
//        return "Sleep";
//    case 0x01020006:
//        return "Zoom";
//    case 0x0102000a:
//        return "Exit";
//    case 0x01020001:
//        return "Cancel";
//    default:
//        return "";
//    }

//}

//#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
//bool globalShortCut::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result){
//#else
//bool globalShortCut::nativeEventFilter(const QByteArray& eventType, void* message, long* result){
//#endif
//    MSG* msg = static_cast<MSG*>(message);
//    if (msg->message == WM_HOTKEY) {
//        if (msg->wParam == static_cast<WPARAM>(_hotkeyId)) {
//            emit activated();
//            return true;
//        }
//    }
//    return false;
//}

//#endif

///*
//quint32 globalShortCut::nativeKeycode(Qt::Key key)
//{
//    switch (key)
//    {
//    case Qt::Key_Escape:
//        return VK_ESCAPE;
//    case Qt::Key_Tab:
//    case Qt::Key_Backtab:
//        return VK_TAB;
//    case Qt::Key_Backspace:
//        return VK_BACK;
//    case Qt::Key_Return:
//    case Qt::Key_Enter:
//        return VK_RETURN;
//    case Qt::Key_Insert:
//        return VK_INSERT;
//    case Qt::Key_Delete:
//        return VK_DELETE;
//    case Qt::Key_Pause:
//        return VK_PAUSE;
//    case Qt::Key_Print:
//        return VK_PRINT;
//    case Qt::Key_Clear:
//        return VK_CLEAR;
//    case Qt::Key_Home:
//        return VK_HOME;
//    case Qt::Key_End:
//        return VK_END;
//    case Qt::Key_Left:
//        return VK_LEFT;
//    case Qt::Key_Up:
//        return VK_UP;
//    case Qt::Key_Right:
//        return VK_RIGHT;
//    case Qt::Key_Down:
//        return VK_DOWN;
//    case Qt::Key_PageUp:
//        return VK_PRIOR;
//    case Qt::Key_PageDown:
//        return VK_NEXT;
//    case Qt::Key_F1:
//        return VK_F1;
//    case Qt::Key_F2:
//        return VK_F2;
//    case Qt::Key_F3:
//        return VK_F3;
//    case Qt::Key_F4:
//        return VK_F4;
//    case Qt::Key_F5:
//        return VK_F5;
//    case Qt::Key_F6:
//        return VK_F6;
//    case Qt::Key_F7:
//        return VK_F7;
//    case Qt::Key_F8:
//        return VK_F8;
//    case Qt::Key_F9:
//        return VK_F9;
//    case Qt::Key_F10:
//        return VK_F10;
//    case Qt::Key_F11:
//        return VK_F11;
//    case Qt::Key_F12:
//        return VK_F12;
//    case Qt::Key_F13:
//        return VK_F13;
//    case Qt::Key_F14:
//        return VK_F14;
//    case Qt::Key_F15:
//        return VK_F15;
//    case Qt::Key_F16:
//        return VK_F16;
//    case Qt::Key_F17:
//        return VK_F17;
//    case Qt::Key_F18:
//        return VK_F18;
//    case Qt::Key_F19:
//        return VK_F19;
//    case Qt::Key_F20:
//        return VK_F20;
//    case Qt::Key_F21:
//        return VK_F21;
//    case Qt::Key_F22:
//        return VK_F22;
//    case Qt::Key_F23:
//        return VK_F23;
//    case Qt::Key_F24:
//        return VK_F24;
//    case Qt::Key_Space:
//        return VK_SPACE;
//    case Qt::Key_Asterisk:
//        return VK_MULTIPLY;
//    case Qt::Key_Plus:
//        return VK_ADD;
//    case Qt::Key_Comma:
//        return VK_SEPARATOR;
//    case Qt::Key_Minus:
//        return VK_SUBTRACT;
//    case Qt::Key_Slash:
//        return VK_DIVIDE;

//        // numbers
//    case Qt::Key_0:
//    case Qt::Key_1:
//    case Qt::Key_2:
//    case Qt::Key_3:
//    case Qt::Key_4:
//    case Qt::Key_5:
//    case Qt::Key_6:
//    case Qt::Key_7:
//    case Qt::Key_8:
//    case Qt::Key_9:
//        return key;

//        // letters
//    case Qt::Key_A:
//    case Qt::Key_B:
//    case Qt::Key_C:
//    case Qt::Key_D:
//    case Qt::Key_E:
//    case Qt::Key_F:
//    case Qt::Key_G:
//    case Qt::Key_H:
//    case Qt::Key_I:
//    case Qt::Key_J:
//    case Qt::Key_K:
//    case Qt::Key_L:
//    case Qt::Key_M:
//    case Qt::Key_N:
//    case Qt::Key_O:
//    case Qt::Key_P:
//    case Qt::Key_Q:
//    case Qt::Key_R:
//    case Qt::Key_S:
//    case Qt::Key_T:
//    case Qt::Key_U:
//    case Qt::Key_V:
//    case Qt::Key_W:
//    case Qt::Key_X:
//    case Qt::Key_Y:
//    case Qt::Key_Z:
//        return key;

//    default:
//        return 0;
//    }
//}
//quint32 globalShortCut::nativeModifiers(Qt::KeyboardModifiers modifiers)
//{
//    // MOD_ALT, MOD_CONTROL, (MOD_KEYUP), MOD_SHIFT, MOD_WIN
//    quint32 native = 0;
//    if (modifiers & Qt::ShiftModifier)
//        native |= MOD_SHIFT;
//    if (modifiers & Qt::ControlModifier)
//        native |= MOD_CONTROL;
//    if (modifiers & Qt::AltModifier)
//        native |= MOD_ALT;
//    if (modifiers & Qt::MetaModifier)
//        native |= MOD_WIN;
//    // TODO: resolve these?
//    //if (modifiers & Qt::KeypadModifier)
//    //if (modifiers & Qt::GroupSwitchModifier)
//    return native;
//}
//*/
