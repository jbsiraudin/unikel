#ifndef UNIKELVINLET_H
#define UNIKELVINLET_H

#include "src/point3.h"

struct uniKelvinLet{
    // physical parameters:
    point3d center;
    point3d end;

    point3d translation_f;

    double scaling_s;
    double twisting_q;
    point3d twisting_v;

    double epsilon;
    double a , b; // elasticity parameters : a = 0.25 / (M_PI * muShearModulus);  b = a / (4 - 4*nuPoissonRatio);

    uniKelvinLet() : epsilon(0.1) , a(1.0) , b(0.3) {}

    void printInfo() {
        std::cout << scaling_s << std::endl;
    }

    void setCenterEnd(point3d inCenter, point3d inEnd) {
        center = inCenter;
        end = inEnd;
        translation_f = end - center;
    }

    void setElasticityParameters( double muShearModulus , double nuPoissonRatio = 0.5 ) { // advised : muShearModulus as a function of the Statel's size and nuPoissonRatio to 0.5 (incompressible)
        a = 0.25 / (M_PI * muShearModulus);
        b = a / (4 - 4*nuPoissonRatio);
    }

    point3d computeVelocity( point3d x ) const {
        point3d r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + epsilon * epsilon );
        point3d resultTranslate = ((a-b)/r_epsilon)*translation_f + (b * point3d::dot(r,translation_f)/(r_epsilon*r_epsilon*r_epsilon))*r + (a * epsilon*epsilon / (2 * r_epsilon*r_epsilon*r_epsilon))*translation_f;
        point3d resultScale = (2.0*b-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*epsilon*epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*(-scaling_s)*r;
        point3d resultTwist = (-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*epsilon*epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*point3d::cross(translation_f.direction()*twisting_q, r);
        //return ((2.0/(3*a - 2*b)) / (1.0/epsilon)) * resultTranslate;
        return (resultTranslate + resultScale + resultTwist);
    }

    point3d RungeKutta_RK4( point3d const & x , unsigned int nSteps ) const {
        point3d advectedTrajectory(0,0,0);
        double timeStep = 1.0 / (double)(nSteps);
        for( unsigned int s = 0 ; s < nSteps ; ++s ){
            point3d xN = x + advectedTrajectory;
            point3d k1 = computeVelocity(xN);
            point3d k2 = computeVelocity(xN + (timeStep/2)*k1);
            point3d k3 = computeVelocity(xN + (timeStep/2)*k2);
            point3d k4 = computeVelocity(xN + (timeStep)*k3);
            advectedTrajectory += (timeStep/6) * (k1 + 2*k2 + 2*k3 + k4);
        }
        return advectedTrajectory;
    }


    point3d computeVelocityConst( point3d const & x ) const {
        point3d r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + epsilon * epsilon );
        point3d resultTranslate = ((a-b)/r_epsilon)*translation_f + (b * point3d::dot(r,translation_f)/(r_epsilon*r_epsilon*r_epsilon))*r + (a * epsilon*epsilon / (2 * r_epsilon*r_epsilon*r_epsilon))*translation_f;
        //point3d resultScale = (2.0*b-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*epsilon*epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*(scaling_s-1)*r;
        //point3d resultTwist = (-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*epsilon*epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*point3d::cross(translation_f.direction()*twisting_q, r);

        return resultTranslate;
    }

    point3d computeCombinedVelocity( point3d const & x ) const {
        point3d resultTranslate = x + 0.1*computeVelocityTranslate(x);
        point3d resultScale = resultTranslate + 0.1*computeVelocityScale(resultTranslate);
        point3d resultTwist = computeVelocityTwist(resultScale);
        return resultScale;
    }

    point3d computeVelocityTranslate( point3d const & x ) const {
        point3d const & r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + epsilon * epsilon );
        point3d result = ((a-b)/r_epsilon)*translation_f + (b * point3d::dot(r,translation_f)/(r_epsilon*r_epsilon*r_epsilon))*r + (a * epsilon*epsilon / (2 * r_epsilon*r_epsilon*r_epsilon))*translation_f;
        return result;
    }

    point3d computeVelocityScale( point3d const & x ) const {
        point3d const & r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + epsilon * epsilon );
        return (2.0*b-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*epsilon*epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*(scaling_s-1)*r;
    }

    point3d computeVelocityTwist( point3d const & x ) const {
        point3d const & r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + epsilon * epsilon );
        return (-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*epsilon*epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*point3d::cross(translation_f.direction()*twisting_q, r);
    }
};


#endif // UNIKELVINLET_H

