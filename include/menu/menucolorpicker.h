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
        void colorPicked(QColor color);

    public slots:

    protected:
        void paintEvent(QPaintEvent *e);
        void resizeEvent(QResizeEvent *e);
        void mousePressEvent(QMouseEvent *e);

    private:
        QColor* color;
        int margin;
        double h;
        double s;
        double v;
        double x;
        double y;
        double radius;
        QColor mapColor(int _x, int _y);

    private slots:
};

class TemperatureBox : public QWidget
{
    Q_OBJECT
    public:
        explicit TemperatureBox(QWidget *parent = 0);
        QSize sizeHint() const override;

    signals:
        void mirekPicked(int mirek);

    public slots:


    protected:
        void paintEvent(QPaintEvent *e);
        void resizeEvent(QResizeEvent *e);
        void mousePressEvent(QMouseEvent *e);

    private:
        QColor* color;
        int mirek_maximum;
        int mirek_minimum;
        int mirek_size;
        int margin;
        double h;
        double s;
        double v;
        double radius;

    private slots:
};

class ColorPicker : public QWidget
{
    Q_OBJECT
    public:
        explicit ColorPicker(QString id = "", bool use_color = false, bool use_temperature = false, QWidget *parent = 0);
        QSize sizeHint() const override;

    signals:
        void colorPicked(QString id, QColor color);
        void mirekPicked(QString id, int mirek);

    public slots:

    protected:

    private:
        QString identifier;
        QVBoxLayout* main_layout;

    private slots:

};

#endif // COLORWHEEL_H