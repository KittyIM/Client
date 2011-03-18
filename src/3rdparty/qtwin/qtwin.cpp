/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#include "qtwin.h"
#include <QLibrary>
#include <QApplication>
#include <QWidget>
#include <QList>
#include <QPointer>

#ifdef Q_WS_WIN

#include <qt_windows.h>

#define DWM_BB_ENABLE                 0x00000001
#define DWM_BB_BLURREGION             0x00000002
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004
#define WM_DWMCOMPOSITIONCHANGED      0x031E

typedef struct _DWM_BLURBEHIND
{
  DWORD dwFlags;
  BOOL fEnable;
  HRGN hRgnBlur;
  BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;

typedef struct _MARGINS
{
  int cxLeftWidth;
  int cxRightWidth;
  int cyTopHeight;
  int cyBottomHeight;
} MARGINS, *PMARGINS;

typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL* pfEnabled);
typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS* pMarInset);
typedef HRESULT (WINAPI *PtrDwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);
typedef HRESULT (WINAPI *PtrDwmGetColorizationColor)(DWORD *pcrColorization, BOOL *pfOpaqueBlend);

static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled= 0;
static PtrDwmEnableBlurBehindWindow pDwmEnableBlurBehindWindow = 0;
static PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = 0;
static PtrDwmGetColorizationColor pDwmGetColorizationColor = 0;

class WindowNotifier: public QWidget
{
  public:
    WindowNotifier() { winId(); }
    void addWidget(QWidget *widget) { widgets.append(widget); }
    void removeWidget(QWidget *widget) { widgets.removeAll(widget); }
    bool winEvent(MSG *message, long *result);

  private:
    QWidgetList widgets;
};

static bool resolveLibs()
{
  if(!pDwmIsCompositionEnabled) {
    QLibrary dwmLib(QString::fromAscii("dwmapi"));
    pDwmIsCompositionEnabled =(PtrDwmIsCompositionEnabled)dwmLib.resolve("DwmIsCompositionEnabled");
    pDwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)dwmLib.resolve("DwmExtendFrameIntoClientArea");
    pDwmEnableBlurBehindWindow = (PtrDwmEnableBlurBehindWindow)dwmLib.resolve("DwmEnableBlurBehindWindow");
    pDwmGetColorizationColor = (PtrDwmGetColorizationColor)dwmLib.resolve("DwmGetColorizationColor");
  }

  return (pDwmIsCompositionEnabled != 0);
}

#endif

bool QtWin::isCompositionEnabled()
{
#ifdef Q_WS_WIN
  if(resolveLibs()) {
    HRESULT hr = S_OK;
    BOOL isEnabled = false;

    hr = pDwmIsCompositionEnabled(&isEnabled);
    if(SUCCEEDED(hr)) {
      return isEnabled;
    }
  }
#endif

  return false;
}

bool QtWin::enableBlurBehindWindow(QWidget *widget, bool enable)
{
  bool result = false;

#ifdef Q_WS_WIN
  if(resolveLibs()) {
    DWM_BLURBEHIND bb = {0};
    HRESULT hr = S_OK;
    bb.fEnable = enable;
    bb.dwFlags = DWM_BB_ENABLE;
    bb.hRgnBlur = NULL;

    widget->setAttribute(Qt::WA_TranslucentBackground, enable);
    widget->setAttribute(Qt::WA_NoSystemBackground, enable);

    hr = pDwmEnableBlurBehindWindow(widget->winId(), &bb);
    if(SUCCEEDED(hr)) {
      result = true;
      windowNotifier()->addWidget(widget);
    }
  }
#endif

  return result;
}

bool QtWin::extendFrameIntoClientArea(QWidget *widget, int left, int top, int right, int bottom)
{
  bool result = false;

#ifdef Q_WS_WIN
  if(resolveLibs()) {
    QLibrary dwmLib(QString::fromAscii("dwmapi"));
    HRESULT hr = S_OK;
    MARGINS m = {left, right, top, bottom};

    hr = pDwmExtendFrameIntoClientArea(widget->winId(), &m);
    if(SUCCEEDED(hr)) {
      result = true;
      windowNotifier()->addWidget(widget);
    }

    widget->setAttribute(Qt::WA_TranslucentBackground, result);
  }
#endif

  return result;
}

QColor QtWin::colorizatinColor()
{
  QColor resultColor = QApplication::palette().window().color();

#ifdef Q_WS_WIN
  if(resolveLibs()) {
    DWORD color = 0;
    BOOL opaque = FALSE;
    QLibrary dwmLib(QString::fromAscii("dwmapi"));
    HRESULT hr = S_OK;
    hr = pDwmGetColorizationColor(&color, &opaque);

    if(SUCCEEDED(hr)) {
      resultColor = QColor(color);
    }
  }
#endif

  return resultColor;
}

#ifdef Q_WS_WIN
WindowNotifier *QtWin::windowNotifier()
{
  static WindowNotifier *windowNotifierInstance = 0;

  if(!windowNotifierInstance) {
    windowNotifierInstance = new WindowNotifier;
  }

  return windowNotifierInstance;
}

bool WindowNotifier::winEvent(MSG *message, long *result)
{
  if(message && message->message == WM_DWMCOMPOSITIONCHANGED) {
    bool compositionEnabled = QtWin::isCompositionEnabled();

    foreach(QWidget *widget, widgets) {
      if(widget) {
        widget->setAttribute(Qt::WA_NoSystemBackground, compositionEnabled);
      }

      widget->update();
    }
  }

  return QWidget::winEvent(message, result);
}
#endif
