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
#ifndef STDOUTREDIRECTOR_H
#define STDOUTREDIRECTOR_H

#include <QObject>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

class QWindowsPipeReader;

class StdoutRedirector : public QObject
{
    Q_OBJECT
public:
    explicit StdoutRedirector(QObject *parent = 0);
    ~StdoutRedirector();

    qint64 bytesAvailable() const;
    QByteArray read(qint64 maxlen);

signals:
    void readyRead();

private:
#ifdef Q_OS_WIN
    HANDLE hRead;
    HANDLE hWrite;
    QWindowsPipeReader *pipeReader;
#endif
    
};

#endif // STDOUTREDIRECTOR_H
