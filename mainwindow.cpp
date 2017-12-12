#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include<iostream>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include <QMouseEvent>
#include<time.h>
#include<QTime>
using namespace std;
typedef struct point{
    int i;      //路径横坐标
    int j;      //路径纵坐标
    int dr;     //方向
}point;  //定义栈和存放最短路径的数组
point *Stack,*Path;

point startpos={-1,-1,-1},endpos={-1,-1,-1};

int **maze;

//int maze[4][4]={
//    {0,1,1,1},
//    {1,0,1,1},
//    {1,1,1,1},
//    {1,1,1,0}
//};
//int maze[8][8]={
//    {0,0,1,0,0,0,1,0},
//    {0,1,1,0,0,0,1,0},
//    {1,0,0,0,1,1,0,0},
//    {0,1,1,1,0,0,0,0},
//    {0,0,0,1,0,0,0,0},
//    {0,1,0,0,0,1,0,0},
//    {0,1,1,1,0,1,1,0},
//    {1,0,0,0,0,0,0,0},
//};

int top;
int countpath;
int minLen;
float rate = 0.6;

MainWindow::MainWindow(QWidget *parent) : 
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    scene(new QGraphicsScene())
{
    ui->setupUi(this);
    connect(ui->generate_maze,QPushButton::clicked,this,draw);
    connect(ui->generate_path,QPushButton::clicked,this,drawpath1);
    connect(ui->generate_multi_path,QPushButton::clicked,this,drawpath2);
    connect(ui->generate_shortpath,QPushButton::clicked,this,drawpath3);
    connect(ui->col_input,QLineEdit::textChanged,this,initSE);
    connect(ui->row_input,QLineEdit::textChanged,this,initSE);
    connect(ui->refresh,QPushButton::clicked,this,refresh);
    connect(ui->wall_rate,QLineEdit::textChanged,this,setRate);
    ui->show_maze->setScene(scene);
    scene->setSceneRect(0,0,500,500);

    W = scene->width();
    H = scene->height();
    isClear = false;
    itemLen = 0;
    col = 0;
    row = 0;
    numLine = 0;
    wallrate = 0.5;


}

MainWindow::~MainWindow()
{
    delete ui;
    delete scene;
}


void MainWindow::getRC()
{
    int c = ui->col_input->text().toInt();
    int r = ui->row_input->text().toInt();
    wallrate = ui->wall_rate->text().toFloat();
    if(c != col || r != row)
    {
        //先释放内存
        if(maze != NULL)
        {
            for(int i=0;i<row;i++)
                delete [] maze[i];
            delete [] maze;
        }
        if(Stack !=NULL)
        {
            delete [] Stack;
        }
        if(Path !=NULL)
        {
            delete [] Path;
        }

        isClear = true;
        col = c;
        row = r;

        //重新构造迷宫矩阵
        maze = new int* [row];
        for(int i=0;i<row;i++){
            maze[i] = new int[col];
            for(int j=0;j<col;j++)
                maze[i][j] = randNum(wallrate);
        }

        Stack = new point[row*col];
        Path = new point[row*col];
        selectSE = 1;
    }
 }

void MainWindow::initSE()
{
    QString c = ui->col_input->text();
    QString r = ui->row_input->text();
    ui->exitp->setText(r+","+c);
    ui->entrance->setText("(1,1)");
    ui->wall_rate->setText("0.5");
}

void MainWindow::setSe()
{
    QMessageBox message;
    QFont font;
    font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
    font.setPointSize(14);
    message.setFont(font);

    if(numLine>0){
        if(selectSE==0){
            setIsdraw();
            return;
        }

        int i1,j1,i2,j2;
        QString entrance = ui->entrance->text();
        QString exitp = ui->exitp->text();
        QRegExp regx("[,， ]+");
        QStringList qlist;
        qlist = entrance.split(regx);
        if(qlist.length()>2){
            message.setText("入口坐标输入格式有误！");
            message.show();
            message.exec();
            return;
        }
        i1 = qlist[0].toInt() -1;
        j1= qlist[1].toInt() -1;
        qlist = exitp.split(regx);
        if(qlist.length()>2){
            message.setText("出口坐标输入格式有误！");
            message.show();
            message.exec();
            return;
        }
        i2 = qlist[0].toInt() -1;
        j2 = qlist[1].toInt() -1;

        if(i1 >=row || j1 >=col){
            message.setText("入口坐标超出范围！");
            message.show();
            message.exec();
            return;
        }
        if(i2 >=row || j2 >=col){
            message.setText("出口坐标超出范围！");
            message.show();
            message.exec();
            return;
        }

        if(maze[i1][j1]){
            message.setText("入口位置是墙，请重新选择入口！");
            message.show();
            message.exec();
            return;
        }

        if(maze[i2][j2]){
            message.setText("出口位置是墙，请重新选择出口！");
            message.show();
            message.exec();
            return;
        }

        if(i1 != startpos.i || j1 != startpos.j || i2 != endpos.i || j2 != endpos.j){
            startpos.i = i1;
            startpos.j = j1;
            endpos.i = i2;
            endpos.j = j2;
            setIsdraw();
        }

    }else{
        message.setText("请先构建迷宫！");
        message.show();
        message.exec();
        setUndraw();
        return;
    }
}

void MainWindow::draw()
{
    getRC();

    if(isClear)
    {
        if(hasItems())
        {
            scene->clear();
        }

        QGraphicsLineItem *fl;
        QGraphicsRectItem *fr;
        qreal w = W / col;
        qreal h = H / row;
        qreal step = 0;
        for(int i=0;i<=row;i++){
            fl = new QGraphicsLineItem(0,step,W,step);
            scene->addItem(fl);
            step = step + h;
        }

        step = 0;
        for(int i=0;i<=col;i++){
            fl = new QGraphicsLineItem(step,0,step,H);
            scene->addItem(fl);
            step = step + w;
        }
        numLine = scene->items().length();

        for(int i=0;i<row;i++)
            for(int j=0;j<col;j++){
                if(maze[i][j]==1){
                    fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
                    fr->setBrush(Qt::gray);
                    scene->addItem(fr);
                }
            }
        isClear = false;
        itemLen = scene->items().length(); //墙和线格的Item总数
    }
}

void MainWindow::findpath(int mode)
{

    QTime *t = new QTime();
    t->start();
    top = -1;
    countpath = 0;
    minLen = INT_MAX;
    haspath = false;

    //清空显示板
    if(!(ui->result->toPlainText() == ""))
        ui->result->clear();

    int i,j,dr,k;
    bool find;
    top++;

    int si,sj,ei,ej;
    si = startpos.i;
    sj = startpos.j;
    ei = endpos.i;
    ej = endpos.j;

    Stack[top] = startpos;
    maze[si][sj] = -1;

    int derection = 0;
    if(ei>si && ej>sj){
        derection=0;
    } else if(ei<si && ej<sj){
        derection=1;
    }else if(si>ei && sj<ej){
        derection=2;
    }else if(si<ei && sj>ej){
        derection=3;
    }


    while(top>-1){
        i = Stack[top].i;
        j = Stack[top].j;
        dr = Stack[top].dr;
        if(i==ei && j==ej){
            countpath++;
            haspath = true;

            if(mode == 2 ){
                //mode2表示查找多条路径
                QString str;
                str = "第"+ QString::number(countpath) + "路径,长度为:" + QString::number(top+1);
                ui->result->append(str);
                str = "路径为:";
                for(k=0;k<top;k++){
                    str += '(' + QString::number(Stack[k].i+1) + ',' + QString::number(Stack[k].j+1) + ')' + "->";
                }
                str += '(' + QString::number(Stack[k].i+1) + ',' + QString::number(Stack[k].j+1) + ')';
                ui->result->append(str);
//                if(countpath > 20) //多条路径查询，多余20条就不查了，直接跳出,对于墙稀疏的迷宫路径很多
//                    break;
            }

            if(top+1<minLen){       //比较输出最短路径
                for(k=0;k<=top;k++)
                    Path[k]=Stack[k];
                minLen=top+1;
                //mode1搜索一条路径，找到一条路径就跳出
                if(mode == 1){
                    break;
                }
            }
            maze[Stack[top].i][Stack[top].j] = 0;
            top--;
            i = Stack[top].i;
            j = Stack[top].j;
            dr = Stack[top].dr;
        }

        find = false;
        if(derection==0){//下 右
            while(dr<4){
                dr++;
                switch (dr){
                case 0:
                    if(Stack[top].i < row-1){
                        i=Stack[top].i+1;   //下
                        j=Stack[top].j;
                    }
                    break;
                case 1:
                    if(Stack[top].j<col-1){
                        i=Stack[top].i;
                        j=Stack[top].j+1; //右
                    }
                    break;
                case 2:
                    if(Stack[top].i>0){
                        i=Stack[top].i-1; //上
                        j=Stack[top].j;
                    }
                    break;
                case 3:
                    if(Stack[top].j>0){
                        i=Stack[top].i;
                        j=Stack[top].j-1;   //左
                    }
                    break;
                }
                //若找到一个方向可行，就跳出
                if(maze[i][j]==0){/*maze[i][j]==0*/
                    find = true;
                    break;
                }
            }
        }else if(derection==1){//左上
            while(dr<4){
            dr++;
            switch (dr){
            case 0:
                if(Stack[top].j>0){
                    i=Stack[top].i;
                    j=Stack[top].j-1;   //左
                }
                break;
            case 1:
                if(Stack[top].i>0){
                    i=Stack[top].i-1; //上
                    j=Stack[top].j;
                }
                break;
            case 2:
                if(Stack[top].j<col-1){
                    i=Stack[top].i;
                    j=Stack[top].j+1; //右
                }
                break;
            case 3:
                if(Stack[top].i < row-1){
                    i=Stack[top].i+1;   //下
                    j=Stack[top].j;
                }
                break;
            }

            if(maze[i][j]==0){/*maze[i][j]==0*/
                find = true;
                break;
            }

        }
        }else if(derection==2){//右上
            while(dr<4){
            dr++;
            switch (dr){
            case 0:
                if(Stack[top].i>0){
                    i=Stack[top].i-1; //上
                    j=Stack[top].j;
                }
                break;
            case 1:
                if(Stack[top].j<col-1){
                    i=Stack[top].i;
                    j=Stack[top].j+1; //右
                }
                break;
            case 2:
                if(Stack[top].i < row-1){
                    i=Stack[top].i+1;   //下
                    j=Stack[top].j;
                }
                break;
            case 3:
                if(Stack[top].j>0){
                    i=Stack[top].i;
                    j=Stack[top].j-1;   //左
                }
                break;
            }

            if(maze[i][j]==0){/*maze[i][j]==0*/
                find = true;
                break;
            }

        }
        }else{//左下
            while(dr<4){
            dr++;
            switch (dr){
            case 0:
                if(Stack[top].i < row-1){
                    i=Stack[top].i+1;   //下
                    j=Stack[top].j;
                }
                break;
            case 1:
                if(Stack[top].j>0){
                    i=Stack[top].i;
                    j=Stack[top].j-1;   //左
                }
                break;
            case 2:
                if(Stack[top].i>0){
                    i=Stack[top].i-1; //上
                    j=Stack[top].j;
                }
                break;
            case 3:
                if(Stack[top].j<col-1){
                    i=Stack[top].i;
                    j=Stack[top].j+1; //右
                }
                break;
            }

            if(maze[i][j]==0){/*maze[i][j]==0*/
                find = true;
                break;
            }

        }
        }


        if(find){
            Stack[top].dr = dr;
            top++;
            Stack[top].i = i;
            Stack[top].j = j;
            Stack[top].dr = -1;
            maze[i][j] = -1;
        }else{
            maze[Stack[top].i][Stack[top].j]=0;   //让该位置变为其他路径的可走结点
            top--;
        }

    }

//    qDebug()<<t->elapsed()/1000.0<<"s";

    if(haspath){
        QString str;
        if(mode==1){
            str ="这条路径长度:" + QString::number(minLen);
        }else{
            str ="最短路径:" + QString::number(minLen);
        }

        ui->result->append(str);
        str = "路径为:";
        for(k=0;k<minLen-1;k++){
            str += '(' + QString::number(Path[k].i+1) + ',' + QString::number(Path[k].j+1) + ')' + "->";
        }
        str += '(' + QString::number(Path[k].i+1) + ',' + QString::number(Path[k].j+1) + ')';
        ui->result->append(str);
        str = "执行的时间为：" + QString::number(t->elapsed()/1000.0) + "s";
        ui->result->append(str);
    }
    delete t;

    Renewmaze();
}
//找到一条路径
void MainWindow::drawpath1()
{

    setSe();
    if(isDraw[0]){
        int rc;
        if((rc = scene->items().length() - itemLen) >0 )  /*scene->items().length() != itemLen*/
        {
            while(rc>0)
            {
                scene->removeItem(scene->items().first());
                rc--;
            }

        }
        findpath(1);
        if(!haspath)
        {
            QMessageBox message;
            QFont font;
            font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
            font.setPointSize(14);
            message.setFont(font);
            message.setText("没有可行的路径！");
            message.show();
            message.exec();
            return;
        }
        qreal w = W / col;
        qreal h = H / row;
        QGraphicsRectItem *fr;
        int i,j;
        i = Path[0].i;
        j = Path[0].j;
        fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
        fr->setBrush(Qt::red);
        scene->addItem(fr);
        for(int k=1;k<minLen-1;k++){
            i = Path[k].i;
            j = Path[k].j;
            fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
            fr->setBrush(Qt::green);
            scene->addItem(fr);
        }
        i = Path[minLen-1].i;
        j = Path[minLen-1].j;
        fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
        fr->setBrush(Qt::cyan);
        scene->addItem(fr);
        isDraw[0] = false;

//        qDebug()<<"findpath1";
    }
}
//多条路径
void MainWindow::drawpath2()
{
    setSe();
    if(isDraw[1]){
        int rc;
        if((rc = scene->items().length() - itemLen) >0 )  /*scene->items().length() != itemLen*/
        {
            while(rc>0)
            {
                scene->removeItem(scene->items().first());
                rc--;
            }
        }

        findpath(2);
        if(!haspath)
        {

            QMessageBox message;
            QFont font;
            font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
            font.setPointSize(14);
            message.setFont(font);
            message.setText("没有可行的路径！");
            message.show();
            message.exec();
            return;
        }
        qreal w = W / col;
        qreal h = H / row;
        QGraphicsRectItem *fr;
        int i,j;
        i = Path[0].i;
        j = Path[0].j;
        fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
        fr->setBrush(Qt::red);
        scene->addItem(fr);
        for(int k=1;k<minLen-1;k++){
            i = Path[k].i;
            j = Path[k].j;
            fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
            fr->setBrush(Qt::green);
            scene->addItem(fr);
        }
        i = Path[minLen-1].i;
        j = Path[minLen-1].j;
        fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
        fr->setBrush(Qt::cyan);
        scene->addItem(fr);
        isDraw[1] = false;
    }
}
//最短路径
void MainWindow::drawpath3()
{
    setSe();
    if(isDraw[2]){
        int rc;
        if((rc = scene->items().length() - itemLen) >0 )  /*scene->items().length() != itemLen*/
        {
            while(rc>0)
            {
                scene->removeItem(scene->items().first());
                rc--;
            }

        }

        QTime *t = new QTime();
        t->start();
        vector< vector<int> > mazz(row,vector<int>(col));
        for(int si=0;si<row;si++)
            for(int sj=0;sj<col;sj++)
                mazz[si][sj] = maze[si][sj];

        Astar astar(mazz);
        Point start(startpos.i,startpos.j);
        Point endd(endpos.i,endpos.j);
        list<Point *> path=astar.GetPath(start,endd);


        if(path.size()!=0)
        {
            int kp = 0;
            for(auto &p:path)
            {
                Path[kp].i = p->x;
                Path[kp].j = p->y;
                kp++;
            }
            kp--;
            mazz.clear();
            path.clear();
            if(!(ui->result->toPlainText() == ""))
                ui->result->clear();
            QString str;
            str ="最短路径长度:" + QString::number(kp+1);
            ui->result->append(str);
            str = "路径为:";
            qreal w = W / col;
            qreal h = H / row;
            QGraphicsRectItem *fr;
            int i,j;
            i = Path[0].i;
            j = Path[0].j;

            str += '(' + QString::number(i+1) + ',' + QString::number(j+1) + ')' + "->";

            fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
            fr->setBrush(Qt::red);
            scene->addItem(fr);

            for(int k=1;k<kp;k++){
                i = Path[k].i;
                j = Path[k].j;
                str += '(' + QString::number(i+1) + ',' + QString::number(j+1) + ')' + "->";
                fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
                fr->setBrush(Qt::green);
                scene->addItem(fr);
            }

            i = Path[kp].i;
            j = Path[kp].j;

            str += '(' + QString::number(i+1) + ',' + QString::number(j+1) + ')';
            ui->result->append(str);
            str = "搜索时间为：" + QString::number(t->elapsed()/1000.0) + "s";
            ui->result->append(str);
            fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
            fr->setBrush(Qt::cyan);
            scene->addItem(fr);

            isDraw[2] = false;
            selectSE = 1;
        }
        else{
            QMessageBox message;
            QFont font;
            font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
            font.setPointSize(14);
            message.setFont(font);
            message.setText("没有可行的路径！");
            message.show();
            message.exec();
            selectSE = 1;
        }

    }
}

void MainWindow::Renewmaze()
{
    //修正maze矩阵
    selectSE = 1;
    for(int i=0;i<row;i++)
        for(int j=0;j<col;j++)
            if(maze[i][j]==-1)
                maze[i][j] = 0;
}

void MainWindow::refresh()
{
    if(hasItems()){
        int numWall = itemLen - numLine;
        while (numWall>0) {
            scene->removeItem(scene->items().first());
            numWall--;
        }
    }else{
        QMessageBox message;
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font.setPointSize(14);
        message.setFont(font);
        message.setText("请先构建迷宫！");
        message.show();
        message.exec();
        return;
    }

    //清空显示板
    if(!(ui->result->toPlainText() == ""))
        ui->result->clear();

    startpos.i = -1;
    startpos.j = -1;
    endpos.i = -1;
    endpos.j = -1;

    qreal w = W / col;
    qreal h = H / row;
    QGraphicsRectItem *fr;

    for(int i=0;i<row;i++)
        for(int j=0;j<col;j++){
            if((maze[i][j] = randNum(wallrate)) ==1){
                fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
                fr->setBrush(Qt::gray);
                scene->addItem(fr);
            }
        }
    itemLen = scene->items().length();//重新获取墙和布线的总数
    selectSE = 1;
    setUndraw();
}

void MainWindow::setIsdraw()
{
    for(int i=0;i<3;i++)
        isDraw[i] = true;
}

void MainWindow::setUndraw()
{
    for(int i=0;i<3;i++)
        isDraw[i] = false;
}

bool MainWindow::hasItems()
{
    return !scene->items().length()==0;
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(selectSE){
        int x = e->x();
        int y = e->y();
        int xx = ui->show_maze->x();
        int yy = ui->show_maze->y();
        qreal w = W / col;
        qreal h = H / row;

        if( x>xx+5 && x<xx+W+5 && y>yy+5 && y<yy+H+5){
            int rc = scene->items().length() - itemLen;
            while (rc>0 && selectSE==1) {
                scene->removeItem(scene->items().first());
                rc--;
            }
            x = x - xx - 5;
            y = y - yy - 10;
            int i = floor(y/h); //row
            int j = floor(x/w); //col
            QGraphicsRectItem *fr;
            fr = new QGraphicsRectItem(QRectF(w*j,i*h,w,h));
            if(selectSE==1){
                if(maze[i][j] == 1){
                    QMessageBox message;
                    QFont font;
                    font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
                    font.setPointSize(14);
                    message.setFont(font);
                    message.setText("该处是墙，请重新选择入口！");
                    message.show();
                    message.exec();
                    return;
                }
                fr->setBrush(Qt::red);
                ui->entrance->setText(QString::number(i+1)+","+QString::number(j+1));
                startpos.i = i;
                startpos.j = j;
                selectSE++;
            }

            else if(selectSE==2){
                if(maze[i][j] == 1){
                    QMessageBox message;
                    QFont font;
                    font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
                    font.setPointSize(14);
                    message.setFont(font);
                    message.setText("该处是墙，请重新选择出口！");
                    message.show();
                    message.exec();
                    return;
                }
                fr->setBrush(Qt::cyan);
                ui->exitp->setText(QString::number(i+1)+","+QString::number(j+1));
                endpos.i = i;
                endpos.j = j;
                selectSE = 0;
            }
            scene->addItem(fr);
        }
    }
}

int MainWindow::randNum(float rate)
{
    if(rand()%100 < int(100*rate))
        return 1;
    else {
        return 0;
    }
}

void MainWindow::setRate()
{
    wallrate = ui->wall_rate->text().toFloat();
    srand(unsigned(time(NULL)));
}
