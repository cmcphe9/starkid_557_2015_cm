#include "TripleTexture.h"

//  Created by Floyd R, Fei XU, Minh Truong
TripleTexture::TripleTexture()
{

    _texture_1 = 0;
    _texture_2 = 0;
    _texture_3 = 0;

    _texture_blend_mode = 0;
    _textureIdx1 = -1;
    _textureIdx2 = -1;
    _textureIdx3 = -1;
    _textureBlendModelIdx = -1;

    _dirty = false;

}


TripleTexture::~TripleTexture()
{

}

/*!
 Load two bitmap images as textures from files.
 @param path_and_file_texture_1 - path and file of the first image.
 @param path_and_file_texture_1 - path and file of the second image.
 @return int - the texture id when the texture was sucessfully loaded.
 */
int TripleTexture::loadAndCreateTextures(string path_and_file_texture_1, string path_and_file_texture_2, string path_and_file_texture_3)
{

    unsigned int channels1;
    unsigned int width1;
    unsigned int height1;

    unsigned int channels2;
    unsigned int width2;
    unsigned int height2;

    unsigned int channels3;
    unsigned int width3;
    unsigned int height3;

    unsigned char* data1 = loadBitmapFile(path_and_file_texture_1, channels1, width1,  height1 );
    unsigned char* data2 = loadBitmapFile(path_and_file_texture_2, channels2, width2,  height2 );
    unsigned char* data3 = loadBitmapFile(path_and_file_texture_3, channels3, width3,  height3 );

    if(data1 == NULL || data2 == NULL || data3 == NULL)return -1;





    //**********************************************************************************************
    // Texture generation - background

    // Activate a texture unit
    glActiveTexture(GL_TEXTURE0);

    // Generate a texture, this function allocates the memory and
    // associates the texture with a variable.
    glGenTextures(1, &_texture_1 );

    // Set a texture as active texture.
    glBindTexture( GL_TEXTURE_2D, _texture_1 );

    // Change the parameters of your texture units.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );


    // Create a texture and load it to your graphics hardware. This texture is automatically associated
    // with texture 0 and the textuer variable "texture" / the active texture.
    if(channels1 == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_BGR, GL_UNSIGNED_BYTE, data1);
    else if(channels1 == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_BGRA, GL_UNSIGNED_BYTE, data1);

    //**********************************************************************************************
    // Create a midmap texture pyramid and load it to the graphics hardware.
    // Note, the MIN and MAG filter must be set to one of the available midmap filters.
    //gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );

    // Delete your loaded data
    free( data1 );




    //**********************************************************************************************
    // Texture light

    // Activate a texture unit
    glActiveTexture(GL_TEXTURE1);

    // Generate a texture, this function allocates the memory and
    // associates the texture with a variable.
    glGenTextures(1, &_texture_2 );

    // Set a texture as active texture.
    glBindTexture( GL_TEXTURE_2D, _texture_2 );

    // Change the parameters of your texture units.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );


    // Create a texture and load it to your graphics hardware. This texture is automatically associated
    // with texture 0 and the textuer variable "texture" / the active texture.
    if(channels2 == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_BGR, GL_UNSIGNED_BYTE, data2);
    else if(channels2 == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_BGRA, GL_UNSIGNED_BYTE, data2);

    //**********************************************************************************************
    // Create a midmap texture pyramid and load it to the graphics hardware.
    // Note, the MIN and MAG filter must be set to one of the available midmap filters.
    //gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );

    // Delete your loaded data
    free( data2 );

    //**********************************************************************************************
    // Extra Texture

    // Activate a texture unit
    glActiveTexture(GL_TEXTURE2);

    // Generate a texture, this function allocates the memory and
    // associates the texture with a variable.
    glGenTextures(1, &_texture_3 );

    // Set a texture as active texture.
    glBindTexture( GL_TEXTURE_2D, _texture_3 );

    // Change the parameters of your texture units.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );


    // Create a texture and load it to your graphics hardware. This texture is automatically associated
    // with texture 0 and the textuer variable "texture" / the active texture.
    if(channels3 == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_BGR, GL_UNSIGNED_BYTE, data3);
    else if(channels3 == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width3, height3, 0, GL_BGRA, GL_UNSIGNED_BYTE, data3);

    //**********************************************************************************************
    // Create a midmap texture pyramid and load it to the graphics hardware.
    // Note, the MIN and MAG filter must be set to one of the available midmap filters.
    //gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );

    // Delete your loaded data
    free( data3 );


    // Return the texture.
    return _texture_1;



}

/*!
 This sets the texture blend model
 @param mode - the values 0,1, or 2
 @return true, when a new mode was set, false when current and new mode are equal
 */
bool TripleTexture::setTextureBlendMode(int mode)
{
    if(mode < 0 || mode > 2)return false;
    if(_texture_blend_mode == mode)return false;

    // write the texture blend mode
    _texture_blend_mode = mode;

    _dirty = true;

    return true;

}




/*!
 Adds the variables of this object to the shader program
 */
//virtual
bool TripleTexture::addVariablesToProgram(GLuint program, int variable_index)
{
    if(program == -1)return false; // no program exits


    GLint params;
    glGetProgramiv( program, GL_LINK_STATUS, &params);
    if(params == GL_FALSE)
    {
        cerr << "[GLTexture] Program " << program << " has not been linked. Textures cannot be added." << endl;
        return false;
    }

    // enable the program
    glUseProgram(program);


    // get the location of a uniform variable. Note, the program must be linked at this position.
    // location of the texture in the glsl program
    _textureIdx1 = glGetUniformLocation(program, _glsl_names[0].c_str() );
    checkUniform(_textureIdx1, _glsl_names[0]);


    _textureIdx2 = glGetUniformLocation(program, _glsl_names[1].c_str() );
    checkUniform(_textureIdx2, _glsl_names[1]);

    _textureIdx3 = glGetUniformLocation(program, _glsl_names[2].c_str() );
    checkUniform(_textureIdx3, _glsl_names[2]);

    _textureBlendModelIdx = glGetUniformLocation(program, _glsl_names[3].c_str() );
    checkUniform(_textureBlendModelIdx, _glsl_names[3]);

    //****************************************************************************************************
    // Link the texture to the uniform variable and texture unit 0;

    /*
     glActiveTexture tells OpenGL which texture unit we want to use. GL_TEXTURE0 is the first texture unit, so we will just use that.
     */
    glActiveTexture(GL_TEXTURE0);


    //We use glBindTexture bind our texture into the active texture unit.
    glBindTexture(GL_TEXTURE_2D, _texture_1);

    /*
     Then we set the tex uniform of the shaders to the index of the texture unit. We used texture unit zero, so we set the tex uniform to the integer value 0.
     */
    glUniform1i(_textureIdx1, 0);




    /*
     glActiveTexture tells OpenGL which texture unit we want to use. GL_TEXTURE0 is the first texture unit, so we will just use that.
     */
    glActiveTexture(GL_TEXTURE1);


    //We use glBindTexture bind our texture into the active texture unit.
    glBindTexture(GL_TEXTURE_2D, _texture_2);

    /*
     Then we set the tex uniform of the shaders to the index of the texture unit. We used texture unit zero, so we set the tex uniform to the integer value 0.
     */
    glUniform1i(_textureIdx2, 1);


    /*
     glActiveTexture tells OpenGL which texture unit we want to use. GL_TEXTURE0 is the first texture unit, so we will just use that.
     */
    glActiveTexture(GL_TEXTURE2);


    //We use glBindTexture bind our texture into the active texture unit.
    glBindTexture(GL_TEXTURE_2D, _texture_3);

    /*
     Then we set the tex uniform of the shaders to the index of the texture unit. We used texture unit zero, so we set the tex uniform to the integer value 0.
     */
    glUniform1i(_textureIdx3, 2);


    // update the variable
    dirty(program);

    return true;

}


/*!
 The function indicates that the variables of this object require an update
 */
//virtual
bool TripleTexture::dirty(GLuint program)
{
    // enable the program
    glUseProgram(program);

    // write the texture blend mode
    glUniform1i(_textureBlendModelIdx, _texture_blend_mode);


    // disable the program
    glUseProgram(0);

    _dirty = false;

    return true;

}
