/****************************************************************************
** Copyright (c) 2013 Debao Zhang <hello@debao.me>
** All right reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#include "stdoutredirector.h"
#include <QUuid>

#ifdef Q_OS_WIN
#include <private/qwindowspipereader_p.h>
#include <io.h>
#include <fcntl.h>
#endif

#ifdef Q_OS_WIN
static void createWinPipe(HANDLE &hRead, HANDLE &hWrite)
{
    QString pipeName;
    while (1) {
        //Try to find one unused pipeName
        pipeName = QString::fromLatin1("\\\\.\\pipe\\stdoutredirector-%1").arg(QUuid::createUuid().toString());
        SECURITY_ATTRIBUTES attributes = {sizeof(SECURITY_ATTRIBUTES), 0, false};
        hRead = ::CreateNamedPipe((wchar_t*)pipeName.utf16(), PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
                                  PIPE_TYPE_BYTE | PIPE_WAIT, 1, 0, 1024 * 1024, 0, &attributes);
        if (hRead != INVALID_HANDLE_VALUE)
            break;
    }

    SECURITY_ATTRIBUTES attributes = {sizeof(SECURITY_ATTRIBUTES), 0, true};
    hWrite = ::CreateFile((wchar_t*)pipeName.utf16(), GENERIC_WRITE,
                        0, &attributes, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    ::ConnectNamedPipe(hRead, NULL);
}
#endif

StdoutRedirector::StdoutRedirector(QObject *parent) :
    QObject(parent)
{
#ifdef Q_OS_WIN
    createWinPipe(hRead, hWrite);
    ::SetStdHandle(STD_OUTPUT_HANDLE, hWrite);

    int fd = _open_osfhandle((intptr_t)hWrite, _O_WRONLY|_O_TEXT);
    _dup2(fd, 1);
    _close(fd);

    pipeReader = new QWindowsPipeReader(this);
    pipeReader->setHandle(hRead);
    pipeReader->startAsyncRead();
    connect(pipeReader, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
#endif
}

StdoutRedirector::~StdoutRedirector()
{
#ifdef Q_OS_WIN
    pipeReader->stop();
    ::CloseHandle(hRead);
    ::CloseHandle(hWrite);
#endif
}

qint64 StdoutRedirector::bytesAvailable() const
{
#ifdef Q_OS_WIN
    return pipeReader->bytesAvailable();
#endif
    return -1;
}

QByteArray StdoutRedirector::read(qint64 maxlen)
{
    QByteArray result(int(maxlen), Qt::Uninitialized);
#ifdef Q_OS_WIN
    qint64 readBytes = pipeReader->read(result.data(), result.size());
    if (readBytes <= 0)
        result.clear();
    else
        result.resize(int(readBytes));
#endif
    return result;
}
