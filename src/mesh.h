#ifndef MESH_H
#define MESH_H

#include <vector>
#include "point3.h"

struct Vertex{
    point3d p;
    Vertex() {}
    Vertex(double x , double y , double z) : p(x,y,z) {}
    double & operator [] (unsigned int c) { return p[c]; }
    double operator [] (unsigned int c) const { return p[c]; }
};

struct Triangle{
    unsigned int corners[3];
    unsigned int & operator [] (unsigned int c) { return corners[c]; }
    unsigned int operator [] (unsigned int c) const { return corners[c]; }
    unsigned int size() const { return 3 ; }
};

struct Mesh{
    std::vector< Vertex > vertices;
    std::vector< Triangle > triangles;
    point3d center;

    BBOXd boundingBox;
    void updateBoundingBox() {
        boundingBox.clear();
        for( unsigned int v = 0 ; v < vertices.size() ; ++v )
            boundingBox.add( vertices[v] );

        center = (boundingBox.BB + boundingBox.bb)/2.0;
    }
};


#endif // MESH_H
