#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "stdoutredirector.h"

class QPlainTextEdit;

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void readData();
    void onTimeout();

private:
    StdoutRedirector *redirector;
    QPlainTextEdit *edit;

};

#endif // WIDGET_H
