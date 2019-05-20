#ifndef FIELDADVECTOR_H
#define FIELDADVECTOR_H

#include "point3.h"

struct FieldAdvector{
    template< class field_struct_t >
    point3d explicitEuler( point3d const & x , field_struct_t const & field , unsigned int nSteps ) const {
        point3d advectedTrajectory(0,0,0);
        double timeStep = 1.0 / (double)(nSteps);
        for( unsigned int s = 0 ; s < nSteps ; ++s ){
            advectedTrajectory += timeStep * field.computeVelocity(x+advectedTrajectory);
        }
        return advectedTrajectory;
    }



    template< class field_struct_t >
    point3d RungeKutta_RK4( point3d const & x , field_struct_t const & field , unsigned int nSteps ) const {
        point3d advectedTrajectory(0,0,0);
        double timeStep = 1.0 / (double)(nSteps);
        for( unsigned int s = 0 ; s < nSteps ; ++s ){
            point3d xN = x + advectedTrajectory;
            point3d k1 = field.computeVelocity(xN);
            point3d k2 = field.computeVelocity(xN + (timeStep/2)*k1);
            point3d k3 = field.computeVelocity(xN + (timeStep/2)*k2);
            point3d k4 = field.computeVelocity(xN + (timeStep)*k3);
            advectedTrajectory += (timeStep/6) * (k1 + 2*k2 + 2*k3 + k4);
        }
        return advectedTrajectory;
    }

    template< class field_struct_t >
    point3d RungeKutta_RK4( point3d const & x , field_struct_t const & field , double timeStart , double timeEnd , unsigned int nSteps ) const {
        point3d advectedTrajectory(0,0,0);
        double timeStep = (timeEnd - timeStart) / (double)(nSteps);
        for( unsigned int s = 0 ; s < nSteps ; ++s ){
            point3d xCurrent = x + advectedTrajectory;
            double timeCurrent = timeStart  +  s * timeStep;
            point3d k1 = field.computeVelocity(xCurrent  ,  timeCurrent);
            point3d k2 = field.computeVelocity(xCurrent + (timeStep/2)*k1  ,  timeCurrent + timeStep/2);
            point3d k3 = field.computeVelocity(xCurrent + (timeStep/2)*k2  ,  timeCurrent + timeStep/2);
            point3d k4 = field.computeVelocity(xCurrent + (timeStep)*k3  ,  timeCurrent + timeStep);
            advectedTrajectory += (timeStep/6) * (k1 + 2*k2 + 2*k3 + k4);
        }
        return advectedTrajectory;
    }
};


#endif // FIELDADVECTOR_H
