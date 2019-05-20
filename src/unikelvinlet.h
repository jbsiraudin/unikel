#ifndef KEYPOINTS_H
#define KEYPOINTS_H

#include "lightkelvinlet.h"

struct uniKelvinlet {
    enum KelvinLetState {
        INACTIVE,
        ACTIVE,
        GRABBED
    };

    KelvinLetState state;
    float charge; // E [0, 1] : definie the charge inside the vector

    lightTranslateKelvinLet kT;
};

#endif // KEYPOINTS_H
