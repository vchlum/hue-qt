#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QColor>

//https://gist.github.com/tobi08151405/7b0a8151c9df1a41a87c1559dac1243a

class ColorWheel : public QWidget
{
    Q_OBJECT
    public:
        explicit ColorWheel(QWidget *parent = 0);
        QSize sizeHint() const override;

    signals:

    public slots:

    protected:
        void paintEvent(QPaintEvent *e);
        void resizeEvent(QResizeEvent *e);

    private:
        QColor* color;
        int margin;
        int h;
        int s;
        int v;
        int radius;

    private slots:
};

class TemperatureBox : public QWidget
{
    Q_OBJECT
    public:
        explicit TemperatureBox(QWidget *parent = 0);
        QSize sizeHint() const override;

    signals:

    public slots:


    protected:
        void paintEvent(QPaintEvent *e);
        void resizeEvent(QResizeEvent *e);

    private:
        QColor* color;
        int margin;
        int h;
        int s;
        int v;
        int radius;

    private slots:
};

class ColorPicker : public QWidget
{
    Q_OBJECT
    public:
        explicit ColorPicker(QWidget *parent = 0);
        QSize sizeHint() const override;

    signals:

    public slots:

    protected:

    private:
        QVBoxLayout* main_layout;

    private slots:

};

#endif // COLORWHEEL_H