#ifndef ASEACH_H
#define ASEACH_H
#include<cstdlib>
#include<list>
#include<vector>

using namespace std;
const int Cost = 10;

typedef struct Point
{
    int x,y; //点坐标，x代表横排，y代表竖列
    int F,G,H; //F=G+H
    Point *parent; //parent的坐标，这里用指针
    Point(int _x,int _y):x(_x),y(_y),F(0),G(0),H(0),parent(NULL){}  //变量初始化
}Point;

class Astar
{
private:
    Point *findPath(Point &startPoint,Point &endPoint);
    vector<Point *> getSurroundPoints(const Point *point) const;
    bool isCanreach(const Point *point,const Point *target) const; //判断某点是否可以用于下一步判断
    Point *isInList(const list<Point *> &list,const Point *point) const; //判断开启/关闭列表中是否包含某点
    Point *getLeastFpoint(); //从开启列表中返回F值最小的节点
    //计算FGH值
    int calcG(Point *point);
    int calcH(Point *point,Point *end);
    int calcF(Point *point);
private:
    vector< vector<int> > maze;
    list<Point *> openList;  //开启列表
    list<Point *> closeList; //关闭列表

public:
    Astar(vector< vector<int> > &maze);
    list<Point *> GetPath(Point &startPoint,Point &endPoint);
};

#endif // ASEACH_H
