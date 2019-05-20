#ifndef KELVINLET_H
#define KELVINLET_H


#include "src/point3.h"

struct KelvinLet{
    enum KelvinLetScale {
        MONOSCALE,
        BISCALE,
        TRISCALE,
        NBOFSCALES
    };
    enum KelvinLetState {
        INACTIVE,
        ACTIVE,
        GRABBED
    };
    enum KelvinLetMode {
        TRANSLATE,
        TWIST,
        SCALE,
        PINCH
    };

    // physical parameters:
    point3d center;
    double epsilon , epsilon2 , epsilon3;
    KelvinLetScale scale;
    double a , b; // elasticity parameters : a = 0.25 / (M_PI * muShearModulus);  b = a / (4 - 4*nuPoissonRatio);

    KelvinLetMode mode;
    point3d translation_f;
    double scaling_s;
    point3d twisting_q;

    // related to interaction:
    point3d pos;
    KelvinLetState state;
    double editingBrushDepth;
    bool enforceTipDisplacement;

    KelvinLet() : state(ACTIVE) , epsilon(10.0) , a(1.0) , b(0.5) , editingBrushDepth(-1) , enforceTipDisplacement(false) {}

    void setElasticityParameters( double muShearModulus , double nuPoissonRatio = 0.5 ) { // advised : muShearModulus as a function of the Statel's size and nuPoissonRatio to 0.5 (incompressible)
        a = 0.25 / (M_PI * muShearModulus);
        b = a / (4 - 4*nuPoissonRatio);
    }

    void setMonoScale( double i_epsilon ) {
        scale = MONOSCALE;
        epsilon = i_epsilon;
    }
    void setBiScale( double i_epsilon1 , double i_epsilon2 ) { // advised: i_epsilon(i+1) = 1.1 * i_epsilon(i)
        scale = BISCALE;
        epsilon = i_epsilon1;
        epsilon2 = i_epsilon2;
    }
    void setTriScale( double i_epsilon1 , double i_epsilon2 , double i_epsilon3 ) { // advised: i_epsilon(i+1) = 1.1 * i_epsilon(i)
        scale = TRISCALE;
        epsilon = i_epsilon1;
        epsilon2 = i_epsilon2;
        epsilon3 = i_epsilon3;
    }

    void toggleEnforcementOfTipDisplacement(bool e) {
        enforceTipDisplacement = e;
    }

    point3d computeVelocity( point3d const & x ) const {
        if(mode == TRANSLATE) {
            if(scale == MONOSCALE) {
                if(!enforceTipDisplacement)
                    return computeMonoScaleVelocity_forTranslation(x,epsilon);
                double w = 1.0;
                return ((2.0/(3*a - 2*b)) / (w/epsilon)) * computeMonoScaleVelocity_forTranslation(x,epsilon);
            }
            if(scale == BISCALE) {
                if(!enforceTipDisplacement)
                    return computeMonoScaleVelocity_forTranslation(x,epsilon) - computeMonoScaleVelocity_forTranslation(x,epsilon2);
                double w = 1.0 , w2 = -1.0;
                return ((2.0/(3*a - 2*b)) / (w/epsilon + w2/epsilon2)) *
                        (computeMonoScaleVelocity_forTranslation(x,epsilon) - computeMonoScaleVelocity_forTranslation(x,epsilon2));
            }
            if(scale == TRISCALE) {
                if(!enforceTipDisplacement)
                    return computeMonoScaleVelocity_forTranslation(x,epsilon)
                            - ((epsilon3*epsilon3 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forTranslation(x,epsilon2)
                            + ((epsilon2*epsilon2 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forTranslation(x,epsilon3);
                double w = 1.0 , w2 = - ((epsilon3*epsilon3 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) , w3 = ((epsilon2*epsilon2 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2));
                return ((2.0/(3*a - 2*b)) / (w/epsilon + w2/epsilon2 + w3/epsilon3)) *
                        (computeMonoScaleVelocity_forTranslation(x,epsilon)
                         + w2 * computeMonoScaleVelocity_forTranslation(x,epsilon2)
                         + w3 * computeMonoScaleVelocity_forTranslation(x,epsilon3));
            }
        }
        else if(mode == SCALE) {
            if(scale == MONOSCALE) {
                return computeMonoScaleVelocity_forScale(x,epsilon);
            }
            if(scale == BISCALE) {
                return computeMonoScaleVelocity_forScale(x,epsilon) - computeMonoScaleVelocity_forScale(x,epsilon2);
            }
            if(scale == TRISCALE) {
                return computeMonoScaleVelocity_forScale(x,epsilon)
                        - ((epsilon3*epsilon3 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forScale(x,epsilon2)
                        + ((epsilon2*epsilon2 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forScale(x,epsilon3);

            }
        }
        else if(mode == TWIST) {
            if(scale == MONOSCALE) {
                return computeMonoScaleVelocity_forTwist(x,epsilon);
            }
            if(scale == BISCALE) {
                return computeMonoScaleVelocity_forTwist(x,epsilon) - computeMonoScaleVelocity_forTwist(x,epsilon2);
            }
            if(scale == TRISCALE) {
                return computeMonoScaleVelocity_forTwist(x,epsilon)
                        - ((epsilon3*epsilon3 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forTwist(x,epsilon2)
                        + ((epsilon2*epsilon2 - epsilon*epsilon)/(epsilon3*epsilon3 - epsilon2*epsilon2)) * computeMonoScaleVelocity_forTwist(x,epsilon3);

            }
        }
        return point3d(0,0,0); // default, should never be reached
    }


private:
    point3d computeMonoScaleVelocity_forTranslation( point3d const & x , double i_epsilon ) const {
        point3d const & r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + i_epsilon * i_epsilon );
        point3d result = ((a-b)/r_epsilon)*translation_f + (b * point3d::dot(r,translation_f)/(r_epsilon*r_epsilon*r_epsilon))*r + (a * i_epsilon*i_epsilon / (2 * r_epsilon*r_epsilon*r_epsilon))*translation_f;
        return result;
    }

    point3d computeMonoScaleVelocity_forScale( point3d const & x , double i_epsilon ) const {
        point3d const & r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + i_epsilon * i_epsilon );
        return (2.0*b-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*i_epsilon*i_epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*(scaling_s-1)*r;
    }
    point3d computeMonoScaleVelocity_forTwist( point3d const & x , double i_epsilon ) const {
        point3d const & r = x - center;
        double r_epsilon = sqrt( r.sqrnorm() + i_epsilon * i_epsilon );
        return (-a)*(1.0/(r_epsilon*r_epsilon*r_epsilon) + 3.0*i_epsilon*i_epsilon/(2.0*r_epsilon*r_epsilon*r_epsilon*r_epsilon*r_epsilon))*point3d::cross(twisting_q ,r);
    }
};



#endif // KELVINLET_H
