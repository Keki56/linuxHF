#ifndef MAP_H
#define MAP_H

class Map
{
private:
    int size;
public:
    struct Wall {
        double top;
        double width;

        Wall(double top, double width): top(top), width(width) {}
    };

   // Wall wall;

    Map();

    double getDistance(double localPlayerPos, double remotePlayerPos);


};

#endif // MAP_H
