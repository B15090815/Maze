#include "aseach.h"
#include<QDebug>
Astar::Astar(vector< vector<int> > &maze)
{
    this->maze = maze;
}

//计算G值
int Astar::calcG(Point *point)
{
    int parentG = point->parent==NULL ? 0:point->parent->G; //如果是初始节点，则其父节点是空
    return parentG+Cost;
}

int Astar::calcH(Point *point,Point *endpos)
{
    //用曼哈顿距离计算H
    return ( abs(endpos->x - point->x) + abs( endpos->y - point->y ) )* Cost;

}

int Astar::calcF(Point *point)
{
    return point->G + point->H;
}

Point* Astar::getLeastFpoint()
{
    if( !openList.empty())
    {
        auto resPoint = openList.front();
        for(auto &point:openList)
            if(point->F < resPoint->F)
                resPoint = point;
        return resPoint;
    }
    return NULL;
}

Point *Astar::findPath(Point &startPoint,Point &endPoint)
{
    openList.push_back(new Point(startPoint.x,startPoint.y));
    while(!openList.empty())
    {
        Point *curPoint=getLeastFpoint(); //找到F值最小的点
        openList.remove(curPoint); //从开启列表中删除
        closeList.push_back(curPoint); //放到关闭列表
        //1,找到当前周围四个格中可以通过的格子
        auto surroundPoints = getSurroundPoints(curPoint);
        for(auto &target:surroundPoints)
        {
            //2,对某一个格子，如果它不在开启列表中，加入到开启列表，设置当前格为其父节点，计算F G H
            if(!isInList(openList,target))
            {
                target->parent=curPoint;
                target->G=calcG(target);
                target->H=calcH(target,&endPoint);
                target->F=calcF(target);
                openList.push_back(target);
            }
            //3，对某一个格子，它在开启列表中，计算G值, 如果比原来的大, 就什么都不做, 否则设置它的父节点为当前点,并更新G和F
            else
            {
                int tempG=calcG(target);
                if(tempG < target->G)
                {
                    target->parent=curPoint;
                    target->G=tempG;
                    target->F=calcF(target);
//                    qDebug()<<"okkkk";
                }
            }
            Point *resPoint=isInList(openList,&endPoint);
            if(resPoint)
                return resPoint;    //返回列表里的节点指针，不要用原来传入的endpoint指针，因为发生了深拷贝
        }
    }

    return NULL;
}

list<Point *> Astar::GetPath(Point &startPoint,Point &endPoint)
{
    Point *result=findPath(startPoint,endPoint);
    list<Point *> path;
    //返回路径，如果没找到路径，返回空链表

    while(result)
    {
        path.push_front(result);
        result=result->parent;
    }

    return path;
}

Point *Astar::isInList(const list<Point *> &plist , const Point *point) const
{
    //判断某个节点是否在列表中，这里不能比较指针，因为每次加入列表是新开辟的节点，只能比较坐标
    for(auto p:plist)
        if(p->x==point->x && p->y==point->y)
            return p;
    return NULL;
}

bool Astar::isCanreach(const Point *point,const Point *target) const
{
    if(target->x<0 || (target->x > maze.size()-1)
        ||target->y <0 || target->y > maze[0].size()-1
        ||maze[target->x][target->y]==1
        ||(target->x == point->x && target->y == point->y)
        ||isInList(closeList,target)){

            return false;
        } //如果点与当前节点重合、超出地图、是障碍物、或者在关闭列表中，返回false

    else
    {
        return true;
    }
}

vector<Point *> Astar::getSurroundPoints(const Point *point) const
{
    vector<Point *> surroundPoints;

    int x = point->x;
    int y = point->y;
    if( isCanreach( point,new Point(x-1,y)) ) //up
        surroundPoints.push_back(new Point(x-1,y));

    if( isCanreach( point,new Point(x+1,y) ) ) //down
        surroundPoints.push_back(new Point(x+1,y));

    if( isCanreach( point,new Point(x,y-1)) )  //left
        surroundPoints.push_back(new Point(x,y-1));

    if( isCanreach( point,new Point(x,y+1)) )  //right
        surroundPoints.push_back(new Point(x,y+1));

    return surroundPoints;
}
