//  Jiale Feng, Geethanjali Jeevanatham, Chloe McPherson 2015-10-11
//  GLSphereExt.hpp
//  HCI557_Spotlight
//
#pragma once


// stl include
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// locals
#include "GLObject.h"
#include "GLSphere.h"
#include "Shaders.h"

#include "HCI557Datatypes.h"





class GLSphereDirect : public GLSphere
{
public:

    GLSphereDirect(float center_x, float center_y, float center_z, float radius, int rows = 10, int segments = 10 );
    ~GLSphereDirect();


protected:



    /*
     Inits the shader program for this object
     */
    virtual void initShader(void);


    // The lights object
	GLLightSource           _light_source0;
    GLSpotLightSource       _light_source1;



};
