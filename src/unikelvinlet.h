#ifndef UNIKELVINLET_H
#define UNIKELVINLET_H

#include "src/point3.h"

struct uniKelvinLet{
    // physical parameters:
    point3d center;
    point3d pos;
    double epsilon;
    double a , b; // elasticity parameters : a = 0.25 / (M_PI * muShearModulus);  b = a / (4 - 4*nuPoissonRatio);

    point3d translation_f;
    double scaling_s;
    point3d twisting_q;

    uniKelvinLet() : epsilon(10.0) , a(1.0) , b(0.5) {}

    void setElasticityParameters( double muShearModulus , double nuPoissonRatio = 0.5 ) { // advised : muShearModulus as a function of the Statel's size and nuPoissonRatio to 0.5 (incompressible)
        a = 0.25 / (M_PI * muShearModulus);
        b = a / (4 - 4*nuPoissonRatio);
    }

    point3d computeNaiveVelocity( point3d const & x ) const {
        point3d const & r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + epsilon * epsilon );
        point3d resultTranslate = ((a-b)/r_epsilon)*translation_f + (b * point3d::dot(r,translation_f)/(r_epsilon*r_epsilon*r_epsilon))*r + (a * epsilon*epsilon / (2 * r_epsilon*r_epsilon*r_epsilon))*translation_f;
        point3d resultScale = (2.0*b-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*epsilon*epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*(scaling_s-1)*r;
        point3d resultTwist = (-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*epsilon*epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*point3d::cross(twisting_q ,r);
        return resultTranslate + resultScale + resultTwist;
    }

    point3d computeCombinedVelocity( point3d const & x ) const {
        point3d resultTranslate = computeVelocityTranslate(x);
        point3d resultScale = computeVelocityScale(resultTranslate);
        point3d resultTwist = computeVelocityTwist(resultScale);
        return resultTwist;
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
        return (-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*epsilon*epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*point3d::cross(twisting_q ,r);
    }
};


#endif // UNIKELVINLET_H

