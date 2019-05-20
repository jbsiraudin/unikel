#ifndef LIGHTKELVINLET_H
#define LIGHTKELVINLET_H

#include "src/point3.h"

struct lightTranslateKelvinLet {
    // physical parameters:
    point3d center;
    point3d pos;
    double epsilon , epsilon2 , epsilon3;
    double a , b; // elasticity parameters : a = 0.25 / (M_PI * muShearModulus);  b = a / (4 - 4*nuPoissonRatio);

    point3d translation_f;

    lightTranslateKelvinLet() : epsilon(10.0) , a(1.0) , b(0.5) {}

    void setElasticityParameters( double muShearModulus , double nuPoissonRatio = 0.5 ) { // advised : muShearModulus as a function of the Statel's size and nuPoissonRatio to 0.5 (incompressible)
        a = 0.25 / (M_PI * muShearModulus);
        b = a / (4 - 4*nuPoissonRatio);
    }

    void setMonoScale( double i_epsilon ) {
        epsilon = i_epsilon;
    }
    void setBiScale( double i_epsilon1 , double i_epsilon2 ) { // advised: i_epsilon(i+1) = 1.1 * i_epsilon(i)
        epsilon = i_epsilon1;
        epsilon2 = i_epsilon2;
    }
    void setTriScale( double i_epsilon1 , double i_epsilon2 , double i_epsilon3 ) { // advised: i_epsilon(i+1) = 1.1 * i_epsilon(i)
        epsilon = i_epsilon1;
        epsilon2 = i_epsilon2;
        epsilon3 = i_epsilon3;
    }

    point3d computeVelocityTranslateMonoscale( point3d const & x ) const {
        return computeMonoScaleVelocity_forTranslation(x,epsilon);
    }

    point3d computeVelocityTranslateMonoscaleWithoutTip( point3d const & x ) const {
        double w = 1.0;
        return ((2.0/(3*a - 2*b)) / (w/epsilon)) * computeMonoScaleVelocity_forTranslation(x,epsilon);
    }

    point3d computeVelocityTranslateBiscale( point3d const & x ) const {
        return computeMonoScaleVelocity_forTranslation(x,epsilon) - computeMonoScaleVelocity_forTranslation(x,epsilon2);
    }

    point3d computeVelocityTranslateBiscaleWithoutTip( point3d const & x ) const {
        double w = 1.0 , w2 = -1.0;
        return ((2.0/(3*a - 2*b)) / (w/epsilon + w2/epsilon2)) *
                (computeMonoScaleVelocity_forTranslation(x,epsilon) - computeMonoScaleVelocity_forTranslation(x,epsilon2));
    }

    point3d computeVelocityTranslateTriscale( point3d const & x ) const {
        return computeMonoScaleVelocity_forTranslation(x,epsilon)
                - ((epsilon3*epsilon3 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forTranslation(x,epsilon2)
                + ((epsilon2*epsilon2 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forTranslation(x,epsilon3);
    }

    point3d computeVelocityTranslateTriscaleWithoutTip( point3d const & x ) const {
        double w = 1.0 , w2 = - ((epsilon3*epsilon3 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) , w3 = ((epsilon2*epsilon2 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2));
        return ((2.0/(3*a - 2*b)) / (w/epsilon + w2/epsilon2 + w3/epsilon3)) *
                (computeMonoScaleVelocity_forTranslation(x,epsilon)
                 + w2 * computeMonoScaleVelocity_forTranslation(x,epsilon2)
                 + w3 * computeMonoScaleVelocity_forTranslation(x,epsilon3));
    }

private:
    point3d computeMonoScaleVelocity_forTranslation( point3d const & x , double i_epsilon ) const {
        point3d const & r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + i_epsilon * i_epsilon );
        point3d result = ((a-b)/r_epsilon)*translation_f + (b * point3d::dot(r,translation_f)/(r_epsilon*r_epsilon*r_epsilon))*r + (a * i_epsilon*i_epsilon / (2 * r_epsilon*r_epsilon*r_epsilon))*translation_f;
        return result;
    }
};

struct lightScaleKelvinLet {
    enum KelvinLetState {
        INACTIVE,
        ACTIVE,
        GRABBED
    };

    KelvinLetState state;

    // physical parameters:
    point3d center;
    point3d pos;
    double epsilon , epsilon2 , epsilon3;
    double a , b; // elasticity parameters : a = 0.25 / (M_PI * muShearModulus);  b = a / (4 - 4*nuPoissonRatio);

    double scaling_s;

    lightScaleKelvinLet() : state(ACTIVE) , epsilon(10.0) , a(1.0) , b(0.5) {}

    void setElasticityParameters( double muShearModulus , double nuPoissonRatio = 0.5 ) { // advised : muShearModulus as a function of the Statel's size and nuPoissonRatio to 0.5 (incompressible)
        a = 0.25 / (M_PI * muShearModulus);
        b = a / (4 - 4*nuPoissonRatio);
    }

    void setMonoScale( double i_epsilon ) {
        epsilon = i_epsilon;
    }
    void setBiScale( double i_epsilon1 , double i_epsilon2 ) { // advised: i_epsilon(i+1) = 1.1 * i_epsilon(i)
        epsilon = i_epsilon1;
        epsilon2 = i_epsilon2;
    }
    void setTriScale( double i_epsilon1 , double i_epsilon2 , double i_epsilon3 ) { // advised: i_epsilon(i+1) = 1.1 * i_epsilon(i)
        epsilon = i_epsilon1;
        epsilon2 = i_epsilon2;
        epsilon3 = i_epsilon3;
    }

    point3d computeVelocityScaleMonoscale( point3d const & x ) const {
        return computeMonoScaleVelocity_forScale(x,epsilon);
    }

    point3d computeVelocityScaleBiscale( point3d const & x ) const {
        return computeMonoScaleVelocity_forScale(x,epsilon) - computeMonoScaleVelocity_forScale(x,epsilon2);
    }

    point3d computeVelocityScaleTriscale( point3d const & x ) const {
        return computeMonoScaleVelocity_forScale(x,epsilon)
                - ((epsilon3*epsilon3 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forScale(x,epsilon2)
                + ((epsilon2*epsilon2 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forScale(x,epsilon3);
    }

private:
    point3d computeMonoScaleVelocity_forScale( point3d const & x , double i_epsilon ) const {
        point3d const & r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + i_epsilon * i_epsilon );
        return (2.0*b-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*i_epsilon*i_epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*(scaling_s-1)*r;
    }
};

struct lightTwistKelvinLet {
    enum KelvinLetState {
        INACTIVE,
        ACTIVE,
        GRABBED
    };

    KelvinLetState state;

    // physical parameters:
    point3d center;
    point3d pos;
    double epsilon , epsilon2 , epsilon3;
    double a , b; // elasticity parameters : a = 0.25 / (M_PI * muShearModulus);  b = a / (4 - 4*nuPoissonRatio);

    point3d twisting_q;

    lightTwistKelvinLet() : state(ACTIVE) , epsilon(10.0) , a(1.0) , b(0.5) {}

    void setElasticityParameters( double muShearModulus , double nuPoissonRatio = 0.5 ) { // advised : muShearModulus as a function of the Statel's size and nuPoissonRatio to 0.5 (incompressible)
        a = 0.25 / (M_PI * muShearModulus);
        b = a / (4 - 4*nuPoissonRatio);
    }

    void setMonoScale( double i_epsilon ) {
        epsilon = i_epsilon;
    }
    void setBiScale( double i_epsilon1 , double i_epsilon2 ) { // advised: i_epsilon(i+1) = 1.1 * i_epsilon(i)
        epsilon = i_epsilon1;
        epsilon2 = i_epsilon2;
    }
    void setTriScale( double i_epsilon1 , double i_epsilon2 , double i_epsilon3 ) { // advised: i_epsilon(i+1) = 1.1 * i_epsilon(i)
        epsilon = i_epsilon1;
        epsilon2 = i_epsilon2;
        epsilon3 = i_epsilon3;
    }

    point3d computeVelocityTwistMonoscale( point3d const & x ) const {
        return computeMonoScaleVelocity_forTwist(x,epsilon);
    }

    point3d computeVelocityTwistBiscale( point3d const & x ) const {
        return computeMonoScaleVelocity_forTwist(x,epsilon) - computeMonoScaleVelocity_forTwist(x,epsilon2);
    }

    point3d computeVelocityTwistTriscale( point3d const & x ) const {
        return computeMonoScaleVelocity_forTwist(x,epsilon)
                - ((epsilon3*epsilon3 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forTwist(x,epsilon2)
                + ((epsilon2*epsilon2 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forTwist(x,epsilon3);
    }

private:
    point3d computeMonoScaleVelocity_forTwist( point3d const & x , double i_epsilon ) const {
        point3d const & r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + i_epsilon * i_epsilon );
        return (-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*i_epsilon*i_epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*point3d::cross(twisting_q ,r);
    }
};

#endif // LIGHTKELVINLET_H
