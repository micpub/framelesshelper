# FramelessHelper

## Usage

Copy *winnativeeventfilter.h* and *winnativeeventfilter.cpp* to your project and use `WinNativeEventFilter::install();` or `WinNativeEventFilter::addFramelessWindow(reinterpret_cast<HWND>(myWidget->winId()));` before any windows (widgets and Qt Quick windows) show up, just like what the example project does.

## Tested Platforms

- Windows 7, MinGW
- Windows 10, MSVC

## Notice

- Any widgets (or Qt Quick elements) in the titlebar area will not be resposible because the mouse events are intercepted. Try if `WinNativeEventFilter::setWindowData()` helps.
- Don't change the window flags (for example, enable the Qt::FramelessWindowHint flag) because it will break the functionality of this code. I'll get rid of the window frame (including the titlebar of course) in Win32 native events.
- All traditional Win32 APIs are replaced by their DPI-aware ones, if there is one.
- Start from Windows 10, normal windows usually have a one pixel width border line, I don't add it because not everyone like it. You can draw one manually if you really need it.
- The frame shadow will get lost if the window is totally transparent. It can't be solved unless you draw the frame shadow manually.
- On Windows 7, if you disabled the Windows Aero, the frame shadow will be disabled as well because it's DWM's resposibility to draw the frame shadow.
- Only top level windows can be frameless. Applying this code to child windows or widgets will result in unexpected behavior.
- The border width (8 if not scaled), border height (8 if not scaled) and titlebar height (30 if not scaled) are acquired by Win32 APIs and are the same with other standard windows, and thus you should not modify them.
- You can also copy all the code to `[virtual protected] bool QWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)` or `[virtual protected] bool QWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)`, it's the same with install a native event filter to the application.
- If you want to use your own border width, border height, titlebar height or minimum window size, just use raw numbers, no need to scale them according to DPI, this code will do the scaling automatically.
- For UNIX platforms, things are much easier. Just use the `startSystemMove` and `startSystemResize` APIs introduced in Qt 5.15.

## References

- <https://github.com/rossy/borderless-window>
- <https://github.com/Bringer-of-Light/Qt-Nice-Frameless-Window>
- <https://github.com/dfct/TrueFramelessWindow>
- <https://github.com/qtdevs/FramelessHelper>
- <https://docs.microsoft.com/en-us/archive/blogs/wpfsdk/custom-window-chrome-in-wpf>

## Special Thanks

Thanks **Lucas** for testing this code in many various conditions.