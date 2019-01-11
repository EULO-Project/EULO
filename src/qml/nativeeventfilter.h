#ifndef NATIVEEVENTFILTER_H
#define NATIVEEVENTFILTER_H
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <windowsx.h>
#include <objidl.h> // Fixes error C2504: 'IUnknown' : base class undefined
//#include <gdiplus.h>
//#include <GdiPlusColor.h>

#include <QDebug>
#include <QQuickWindow>

class NativeEventFilter: public QAbstractNativeEventFilter
{
public:



    HWND winId;
    bool maxmized;
    int desktop_width;
    int desktop_height;
    QQuickWindow *main_window;
    quint64 count = 0;

    bool from_maxmized = true;

    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE
    {




        MSG* msg = (MSG *)message;


        switch (msg->message)
        {
        case WM_NCCALCSIZE:
        {
            //this kills the window frame and title bar we added with WS_THICKFRAME and WS_CAPTION
            *result = 0;
            return true;
        }
        case WM_GETMINMAXINFO:
        {


            if (::IsZoomed(msg->hwnd)) {
                maxmized=true;
                from_maxmized = true;

                QObject *model=main_window->findChild<QObject*>("background");
                QVariant retValue;

                QMetaObject::invokeMethod(model,"setMargin",Qt::DirectConnection,
                                          Q_RETURN_ARG(QVariant,retValue),
                                          Q_ARG(QVariant,QVariant::fromValue(8)),
                                          Q_ARG(QVariant,QVariant::fromValue(0))
                                          );

            }
            else {
                maxmized=false;

                if(from_maxmized){

                    QObject *model=main_window->findChild<QObject*>("background");
                    QVariant retValue;

                    QMetaObject::invokeMethod(model,"setMargin",Qt::DirectConnection,
                                              Q_RETURN_ARG(QVariant,retValue),
                                              Q_ARG(QVariant,QVariant::fromValue(10)),
                                              Q_ARG(QVariant,QVariant::fromValue(0))
                                              );

                    from_maxmized=false;

                }

            }


            //       *result = ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
            return true;
        }
        case WM_NCHITTEST:
        {

            QObject *title=main_window->findChild<QObject*>("title");


            if(title->property("opened").toBool())
            {
                *result = HTCLIENT;
                return true;
            }


            HWND active_window= GetActiveWindow();
            int padding=0;


            //qDebug()<<"active_window:"<<active_window;

          //  if(active_window!=winId) return false; //GH BUG: qml中open新window 当window把底层窗体遮挡时，如果鼠标移动到下层窗体边缘，居然可以变更上层窗体大小。。



            RECT winrect;
            GetWindowRect(msg->hwnd, &winrect);

            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);

            int width = winrect.right - winrect.left;
            int height = winrect.bottom - winrect.top;

            if(maxmized)
                padding=6;
            else
                padding=8;


            //left border
            if (x >= winrect.left && x < winrect.left + padding)
            {
                *result = HTLEFT;
            }
            //right border
            if (x < winrect.right && x >= winrect.right - padding)
            {
                *result = HTRIGHT;
            }

            //bottom border
            if (y < winrect.bottom && y >= winrect.bottom - padding)
            {
                *result = HTBOTTOM;
            }
            //top border
            if (y >= winrect.top && y < winrect.top + padding)
            {
                *result = HTTOP;
            }


            //bottom left corner
            if (x >= winrect.left && x < winrect.left + padding &&
                    y < winrect.bottom && y >= winrect.bottom - padding)
            {
                *result = HTBOTTOMLEFT;
            }
            //bottom right corner
            if (x < winrect.right && x >= winrect.right - padding &&
                    y < winrect.bottom && y >= winrect.bottom - padding)
            {
                *result = HTBOTTOMRIGHT;
            }
            //top left corner
            if (x >= winrect.left && x < winrect.left + padding &&
                    y >= winrect.top && y < winrect.top + padding)
            {
                *result = HTTOPLEFT;
            }
            //top right corner
            if (x < winrect.right && x >= winrect.right - padding&&
                    y >= winrect.top && y < winrect.top + padding)
            {
                *result = HTTOPRIGHT;
            }


            //            qDebug()<<"winrect.top:"<<winrect.top;
            //            qDebug()<<"winrect.height:"<<winrect.bottom-winrect.top;
            //            qDebug()<<"window_height:"<<window_height;

            //            qDebug()<<"winrect.top:"<<winrect.top;

            //            qDebug()<<"y:"<<y;
            //            qDebug()<<"padding:"<<padding;

           // if(msg->hwnd != winId) return false; //Dialogs don't need a HTCAPTION

            // Here is for coin_type_btn
            if(x - winrect.left >= width - 100 - padding
                    && x - winrect.left <= width - padding - 28
                    && y >= winrect.top + padding + 39
                    && y <= winrect.top + 64 + padding)
            {
                *result = HTCLIENT;
                return true;
            }

            // Here is for NaviPanel
            if(x - winrect.left > width/2 - 333
                    && x - winrect.left < width/2 + 333
                    && y >= winrect.top + padding
                    && y < winrect.top + 74 + padding)
            {
                *result = HTCLIENT;
                return true;
            }

            // Here is for close min_mum buttons
            if((x > winrect.right - padding - 350
                && x < winrect.right - padding
                &&  y >= winrect.top + padding
                && y < winrect.top + 28 + padding ))
            {
                *result = HTCLIENT;
                return true;
            }

            if (y >= winrect.top + padding
                    && y < winrect.top + 74 + padding
                    && x > winrect.left + padding
                    && x < winrect.right - padding
                    //&& !(x > winrect.right - padding-105 && x < winrect.right - padding &&  y >= winrect.top + padding && y < winrect.top + 28 + padding )
                    )
            {
                *result = HTCAPTION;
            }


            if (0!=*result) return true;



            *result = HTCLIENT;
            return true;

        }

        default:
            return false;
        }
    }
};

#endif // NATIVEEVENTFILTER_H
