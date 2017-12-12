#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "aseach.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
/*==================================*/
    void getRC();
    bool hasItems();
    void setSe();
    void findpath(int mode);
    void Renewmaze();
    void setIsdraw();
    void setUndraw();
    int randNum(float rate);

private:
    int row;
    int col;
    qreal W ;
    qreal H;
    qreal w;
    qreal h;
    int itemLen;
    int numLine;
    bool isClear;
    bool isDraw[3];
    bool haspath;
    int selectSE;
    float wallrate;

private slots:
    void draw();
    void drawpath1();
    void drawpath2();
    void drawpath3();
    void initSE();
    void refresh();
    void setRate();

protected:
    void mousePressEvent(QMouseEvent *e);


    friend class Astar;

};

#endif // MAINWINDOW_H
