//Modified by Chloe McPherson, Floyd (Richie) Richardson, Hsin Miao (Daniel) Lee
//ME 557: Computer Graphics
//Final Project: Flying Spaceship Adventure
//Last modified on 12/18/15
//
// main_Final_spaceship.cpp
//  
//
// Created from 07_Spotlight by Rafael Radkowski on 5/28/15.
//  Copyright (c) 2015 -. All rights reserved.
//
//

// stl include
#include <iostream>
#include <string>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


// glfw includes
#include <GLFW/glfw3.h>

// include local files
#include "controls.h"
#include "HCI557Common.h"
#include "CoordSystem.h"
#include "Texture.h"
#include "Sphere3D.h"
#include "GLObjects3D.h"
#include "GLObjectObjInt.h"
#include "RayIntersectionTest.h"

#include <ctime>

using namespace std;


// The handle to the window object
GLFWwindow*         window;

// Define some of the global variables we're using for this sample
GLuint program;

/* A trackball to move and rotate the camera view */
extern Trackball trackball;

int bump_map = 0;

GLAppearance* apperance_0;
GLAppearance* apperance_1;
GLAppearance* apperance_2;

// The interpolated spaceship
GLObjectObjInt* spaceship;
GLObjectObjInt* spaceship_result;

// to transform the sphere
glm::mat4 g_transform_spaceship;
glm::mat4 g_transform_spaceship_result;
glm::mat4 g_view_result;

// to swap between navigation with interpolation and without interpolation.
int    g_with_interpolation = 1;

int    g_swap_trackball_navigation = 1;

//at present, g_delta represents a change in location.  Eventually, it should
//    represent a change in velocity.  Something like .4/15 units per second per press.
float g_delta = 0.4;

// This is the callback we'll be registering with GLFW for keyboard handling.
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    bool move = false;
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

//1 and 2 swaps
    else if (key == 49 && action == GLFW_PRESS) // 1
    {
        g_with_interpolation++;
        g_with_interpolation = g_with_interpolation%2;
        spaceship->enableInterpolation((bool)g_with_interpolation);
        spaceship_result->enableInterpolation((bool)g_with_interpolation);
    }
    else if (key == 50 && action == GLFW_PRESS) // 2
    {
        g_swap_trackball_navigation++;
        g_swap_trackball_navigation = g_swap_trackball_navigation%2;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////
    // Spaceship Translation
    if( (key == 87 && action == GLFW_REPEAT) || (key == 87 && action == GLFW_PRESS) ) // key w
    {
        //cout <<  "key w pressed" << endl;
//  change position for the spacecraft
        
	g_transform_spaceship=  g_transform_spaceship* glm::translate(glm::vec3( 0.0, 0.0f, g_delta));
        spaceship->setMatrix(g_transform_spaceship);
		
		  //cout <<  "key w pressed" << endl;

		
//move should always be true
        move = true;
    }
    else if((key == 83 && action == GLFW_REPEAT) || (key == 83 && action == GLFW_PRESS)) // key s
    {
        //cout <<  "key s pressed" << endl;
 
//   change position for the spacecraft
        g_transform_spaceship=  g_transform_spaceship* glm::translate(glm::vec3( 0.0, 0.0f, -g_delta));
        spaceship->setMatrix(g_transform_spaceship);
//move should always be true
        move = true;
    }
	
    ////////////////////////////////////////////////////////////////////////////////////////////

	// Spaceship Rotation
    if((key == 65 && action == GLFW_REPEAT)||( key == 65 && action == GLFW_PRESS)) // key a
    {
        //cout <<  "key a pressed" << endl;
        g_transform_spaceship =  g_transform_spaceship * glm::rotate(0.04f, glm::vec3(0.0f, 1.0f, 0.0f));
        spaceship->setMatrix(g_transform_spaceship);
        move = true;
    }
    
    else if((key == 68 && action == GLFW_REPEAT) || (key == 68 && action == GLFW_REPEAT)) // key d
    {
        //cout <<  "key d pressed" << endl;
        g_transform_spaceship =  g_transform_spaceship * glm::rotate(-0.04f, glm::vec3(0.0f, 1.0f, 0.0f));
        spaceship->setMatrix(g_transform_spaceship);
        move = true;
    }

    if((key == 81 && action == GLFW_REPEAT)||( key == 81 && action == GLFW_PRESS)) // key q
    {
        //cout <<  "key q pressed" << endl;
        g_transform_spaceship =  g_transform_spaceship * glm::rotate(0.04f, glm::vec3(1.0f, 0.0f, 0.0f));
        spaceship->setMatrix(g_transform_spaceship);
        move = true;
    }
    
    else if((key == 69 && action == GLFW_REPEAT) || (key == 69 && action == GLFW_REPEAT)) // key e
    {
        //cout <<  "key e pressed" << endl;
        g_transform_spaceship =  g_transform_spaceship* glm::rotate(-0.04f, glm::vec3(1.0f, 0.0f, 0.0f));
	    spaceship->setMatrix(g_transform_spaceship);
        move = true;
    }

    if(move)
    {
            g_transform_spaceship_result=g_transform_spaceship;
            spaceship_result->setMatrix(g_transform_spaceship_result);        
    }
    
   //cout << key << endl;
}


int main(int argc, const char * argv[])
{
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// Init glfw, create a window, and init glew
    
    // Init the GLFW Window
    window = initWindow();
    
    
    // Init the glew api
    initGlew();
    SetCameraManipulator(CameraTypes::CAMERA_MANIPULATOR);

	// coordinate system
    CoordSystem* cs = new CoordSystem(200.0);
	
	//Appearance for spheres and spaceship
	GLAppearance* apperance_0 = new GLAppearance ("../../data/shaders/spherical_mapping.vs", "../../data/shaders/spherical_mapping.fs");
	GLAppearance* apperance_1 = new GLAppearance ("../../data/shaders/spherical_mapping.vs", "../../data/shaders/spherical_mapping.fs");
	GLAppearance* apperance_2 = new GLAppearance ("../../data/shaders/spherical_mapping.vs", "../../data/shaders/spherical_mapping.fs");
	GLAppearance* apperance_spaceship= new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");
	GLAppearance* apperance_spaceship2 = new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");
	
	//Light Sources
	GLDirectLightSource  light_source;  
	light_source._lightPos = glm::vec4(-190.0f, -190.0f, -190.0f, 1.0);
	light_source._ambient_intensity = 0.2;
	light_source._specular_intensity = 0.0;
	light_source._diffuse_intensity = 12.2;
	light_source._attenuation_coeff = 0.025;
	
	GLSpotLightSource spot_light;
	spot_light._lightPos = glm::vec4(30.0, 0.0, 15.0, 0.0);
	spot_light._ambient_intensity = 0.0;
	spot_light._specular_intensity = 0.0;
	spot_light._diffuse_intensity = 4.0;
	spot_light._cone_angle = 12; // in degrees
	spot_light._cone_direction = glm::vec3(30, 4, -20);
	
	GLDirectLightSource light_source_ship;
	light_source_ship._lightPos = glm::vec4(-17.f, -1.0f, 1.0f, 1.0);
	light_source_ship._ambient_intensity = 0.4;
	light_source_ship._specular_intensity = 0.0;
	light_source_ship._diffuse_intensity = 2.2;
	light_source_ship._attenuation_coeff = 0.025;
    
	//Applying light sources to objects
	apperance_0->addLightSource(light_source);
	apperance_1->addLightSource(light_source);
	apperance_2->addLightSource(light_source);
	apperance_spaceship->addLightSource(light_source_ship);
	apperance_spaceship->addLightSource(light_source);
	apperance_spaceship->addLightSource(spot_light);
	
	////////////////////////////////////////
	////Creating materials
	
	////First sphere material (planet)
	GLMaterial material_0;
	material_0._diffuse_material = glm::vec3(0.6, 0.0, 0.0);
    material_0._ambient_material = glm::vec3(0.6, 0.0, 0.0);
    material_0._specular_material = glm::vec3(1.0, 1.0, 1.0);
    material_0._shininess = 2.0;
    material_0._transparency = 1.0;
		
	////Second sphere material (moon)
	GLMaterial material_1;
	material_1._diffuse_material = glm::vec3(0.6, 1.0, 0.9);
    material_1._ambient_material = glm::vec3(0.6, 0.5, 1.0);
    material_1._specular_material = glm::vec3(1.0, 1.0, 1.0);
    material_1._shininess = 12.0;
    material_1._transparency = 2.0;
	
	////Third sphere material (sun)
	GLMaterial material_2;
    material_2._diffuse_material = glm::vec3(0.8, 0.8, 0.00);
    material_2._ambient_material = glm::vec3(1.0, 1.0, 0.0);
    material_2._specular_material = glm::vec3(1.0, 1.0, 1.0);
    material_2._shininess = 1.0;
    material_2._transparency = 1.0;
	
	/////Set Material to each sphere
	apperance_0->setMaterial(material_0);
	apperance_1->setMaterial(material_1);
	apperance_2->setMaterial(material_2);
	
	////////////////////////////////////////////////////////////////////////////////////////
	////Adding textures for planet, moon, sun
	//planet
	GLTexture* texture_0 = new GLTexture();
    texture_0->loadAndCreateTexture("../../data/textures/alienjupiter-black.bmp");
    apperance_0->setTexture(texture_0);

	//moon
	GLTexture* texture_1 = new GLTexture();
    texture_1->loadAndCreateTexture("../../data/textures/alienjupiter-black.bmp");
    apperance_1->setTexture(texture_1);

	//sun
	GLTexture* texture_2 = new GLTexture();
    texture_2->loadAndCreateTexture("../../data/textures/alienjupiter-black.bmp");
    apperance_2->setTexture(texture_2);
		
	////Finalize appearance objects
	apperance_0->finalize();
	apperance_1->finalize();
	apperance_2->finalize();
	
	////Create sphere geometry for planet
	GLSphere3D* sphere_planet = new GLSphere3D(-20.0, -15.0, -20.0, 20.0, 40, 20);
	sphere_planet->setApperance(*apperance_0);
	sphere_planet->init();
	apperance_0->updateLightSources();
	
	////Create sphere geometry for moon
	GLSphere3D* sphere_moon = new GLSphere3D(-15.0, 30.0, -10.0, 4.0, 40, 20);
	sphere_moon->setApperance(*apperance_1);
	sphere_moon->init();
	apperance_1->updateLightSources();
	
	////Create sphere geometry for sun
	GLSphere3D* sphere_sun = new GLSphere3D(190.0, 190.0, 190.0, 10.0, 40, 20);
	sphere_sun->setApperance(*apperance_2);
	sphere_sun->init();
	apperance_2->updateLightSources();
	

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

    // Create a material object for spaceship
    GLMaterial material_spaceship;
    material_spaceship._diffuse_material = glm::vec3(0.3, 1.0, 1.0);
    material_spaceship._ambient_material = glm::vec3(1.0, 0.6, 0.9);
    material_spaceship._specular_material = glm::vec3(1.0, 1.0, 1.0);
    material_spaceship._shininess = 3.5;
    material_spaceship._transparency = 1.0;
    
    // Add the material to the apperance object
    apperance_spaceship->setMaterial(material_spaceship);

    // Create a spaceship
    spaceship = new GLObjectObjInt("../../data/SciFi_Fighter_MK/SciFi_Fighter_MK-OBJ.obj");
    spaceship->setApperance(*apperance_spaceship);
    spaceship->init();
    
	//Make the spaceship smaller
	g_transform_spaceship= glm::scale(glm::vec3(1.0, 1.0, 1.0));
	spaceship->setMatrix(g_transform_spaceship);
	
    //Move the spaceship up
    g_transform_spaceship= glm::translate(glm::vec3(0.0, 0.0f, 10.0f));
    spaceship->setMatrix(g_transform_spaceship);
    
 	
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// Create spaceship appearance
    
    // add the light to this apperance object
    apperance_spaceship2 ->addLightSource(light_source);
    
    // Create a material object
    GLMaterial material_spaceship2 ;
    material_spaceship2._diffuse_material = glm::vec3(1.0, 1.0, 0.0);
    material_spaceship2._ambient_material = glm::vec3(1.0, 1.0, 0.0);
    material_spaceship2._specular_material = glm::vec3(1.0, 1.0, 1.0);
    material_spaceship2._shininess = 2.0;
    material_spaceship2._transparency = 1.0;
    
    // Add the material to the apperance object
    apperance_spaceship2->setMaterial(material_spaceship2);
	
	//Adding spaceship file from Turbo Squid
    spaceship_result = new GLObjectObjInt("../../data/SciFi_Fighter_MK/SciFi_Fighter_MK-OBJ.obj");
    spaceship_result->setApperance(*apperance_spaceship);
    spaceship_result->init();
    
	//Make the spaceship_result smaller
	g_transform_spaceship_result= glm::scale(glm::vec3(1.0, 1.0, 1.0));
	spaceship_result->setMatrix(g_transform_spaceship_result);
    
    g_transform_spaceship_result = glm::translate(glm::vec3(0.0, 0.0f, 5.0f));
    spaceship_result->setMatrix(g_transform_spaceship_result);

	//Finalize the appearances
	//apperance_0->finalize();
	//apperance_1->finalize();
	//apperance_2->finalize();
    apperance_spaceship->finalize();
	apperance_spaceship2->finalize();
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// Main render loop
    
    // Set up our green background color
    static const GLfloat clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const GLfloat clear_depth[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    // This sets the camera to a new location
    // the first parameter is the eye position, the second the center location, and the third the up vector. 
    SetViewAsLookAt(glm::vec3(12.0f, 65.0f, 12.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    
    // Enable depth test
    // ignore this line, it allows us to keep the distance value after we proejct each object to a 2d canvas.
    glEnable(GL_DEPTH_TEST);
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// Blending
    
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// Here we set a new keyboard callback
    
    // Set the keyboard callback so that when we press ESC, it knows what to do.
    glfwSetKeyCallback(window, keyboard_callback);
    
    
    glm::mat4 camera_transformation = glm::lookAt(glm::vec3(1.5f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 object_transformation = glm::mat4();
    glm::mat4 camera_matrix = glm::mat4();
    glm::mat4 camera_delta = glm::rotate(20.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// Main render loop
    
    // This is our render loop. As long as our window remains open (ESC is not pressed), we'll continue to render things.
    while(!glfwWindowShouldClose(window))
    {
        
        // Clear the entire buffer with our green color (sets the background to be green).
        glClearBufferfv(GL_COLOR , 0, clear_color);
        glClearBufferfv(GL_DEPTH , 0, clear_depth);
        
    
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //// This renders the objects

        // draw the objects
        cs->draw();
        
		//loadedModel1->draw();
        //spaceship->draw();
        spaceship_result->draw();
		sphere_planet->draw();
		sphere_moon->draw();
        sphere_sun->draw();
		
        // Camera manipulation
        if(g_swap_trackball_navigation == 0)
        {
            // This code attaches the virtual camera just behind the object.
            // Read the location from the object on the ground
            object_transformation = spaceship_result->getModelMatrix();
            
            // Add the camera and a camera delta
            camera_matrix = camera_delta *  camera_transformation * glm::inverse(object_transformation);
            
            // set the view matrix
           //SetViewAsLookAt(glm::vec3(camera_matrix[1][1],camera_matrix[2][1],camera_matrix[3][1]),glm::vec3(camera_matrix[1][2],camera_matrix[2][2],camera_matrix[3][2]),glm::vec3(camera_matrix[1][3],camera_matrix[2][3],camera_matrix[3][3]));
        }
        else
        {
            // Set the trackball location
            SetTrackballLocation(GetCurrentCameraMatrix(), GetCurrentCameraTranslation());
//this should make the spaceship rotate with the trackball
            //spaceship_result->setMatrix(GetCurrentCameraMatrix()*glm::inverse(camera_delta*camera_transformation));
        }
        
       
        //// This renders the objects
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        
        // Swap the buffers so that what we drew will appear on the screen.
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    
    
    delete cs;


}