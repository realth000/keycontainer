///* Josip Medved <jmedved@jmedved.com> * www.medo64.com * MIT License */

//// 2019-07-05: Initial version

///*
//To use it, just assign instance to a long living variable, register a key sequence, and hook into activated signal:

//_hotkey = new Hotkey();
//_hotkey->registerHotkey(QKeySequence { "Ctrl+Shift+F1" });
//connect(_hotkey, SIGNAL(activated()), this, SLOT(onActivated()));
//*/

//#ifndef GLOBAL_SHORTCUT_H
//#define GLOBAL_SHORTCUT_H
//#include <QAbstractNativeEventFilter>
//#include <QObject>
//#include <QKeySequence>

//#if defined(Q_OS_WIN)
//#include <windows.h>
//#elif defined(Q_OS_LINUX)
//#include <QX11Info>
//#include <xcb/xcb.h>
//#include <X11/keysym.h>
//#include <X11/Xlib.h>
////workaround for compiler errors
//#undef Bool
//#undef CursorShape
//#undef Expose
//#undef KeyPress
//#undef KeyRelease
//#undef FocusIn
//#undef FocusOut
//#undef FontChange
//#undef None
//#undef Status
//#undef Unsorted
//#else
//#error "Only Linux and Windows are supported!"
//#endif
//namespace GSC {class globalShortCut;}

//class globalShortCut : public QObject, QAbstractNativeEventFilter
//{
//    Q_OBJECT
//    /*! Creates a new instance */
//    explicit globalShortCut(QObject* parent = nullptr);

//    /*! Destroys the instance */
//    ~globalShortCut() override;

//    /*! Registers hotkey.
//         * \param keySequence Hotkey. */
//    bool registerShortCut(int Modifier, int key);

//    /*! Disables currently registered hotkey. */
//    bool unregisterShortCut();

//signals:

//    /*! Signals hotkey has been activated */
//    void activated();

//protected:

//#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
//    virtual bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
//#else
//    virtual bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;
//#endif

//private:
//    bool _isRegistered = false;
//    void nativeInit();
//    bool nativeRegisterShortCut(QKeyCombination keyCombination);
//    bool nativeUnregisterShortCut();
//    QString fromPressKeyToString(int key);
//#if defined(Q_OS_WIN)
//    static std::atomic<WPARAM> _globalHotkeyCounter;
//    int _hotkeyId = 0; //0x0000 through 0xBFFF
//#elif defined(Q_OS_LINUX)
//    uint16_t _hotkeyMods;
//    xcb_keycode_t _hotkeyKey;
//#endif

//public slots:
//};

//#endif // GLOBAL_SHORTCUT_H
