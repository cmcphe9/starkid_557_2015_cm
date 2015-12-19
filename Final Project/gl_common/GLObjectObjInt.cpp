#include "GLObjectObjInt.h"

#include <algorithm>


GLObjectObjInt::GLObjectObjInt(string filename):
_file_and_path(filename)
{
    
    _file_ok = false;
    _file_ok =load_obj(filename.c_str(), _vertices, _normals, _elements);

    _program = 0;

    _position_initialized = false;
    _target_position  = glm::vec3(0.0, 0.0, 0.0);
	_current_position = _target_position;
    
    _with_interpolation = true;
   
}

GLObjectObjInt::GLObjectObjInt()
{

}


GLObjectObjInt::~GLObjectObjInt()
{
    
    // Program clean up when the window gets closed.
    glDeleteVertexArrays(1, _vaoID);
    glDeleteProgram(_program);
}



/*!
 Init the geometry object
 */
void GLObjectObjInt::init(void)
{
    if(_file_ok)
    {
        initShader();
        initVBO();
    }
}




/*!
 Set a model matrix to move the object around
 @param matrix - the model matrix for this object.
 */
void GLObjectObjInt::setMatrix(glm::mat4& matrix)
{
    /////////////////////////////////////////////
    // With interpolation
    if(_with_interpolation)
    {
        // with interpolation, remember the target location that you like to find
        _target_matrix = matrix;
        _target_quat = glm::quat_cast(matrix);
        _target_position[0] = _target_matrix[3][0];
        _target_position[1] = _target_matrix[3][1];
        _target_position[2] = _target_matrix[3][2];
    
    
        // Just for initialization. Move your object immediately to the location
        if(!_position_initialized)
        {
            _current_quat = glm::quat_cast(matrix);
            _current_position = _target_position;
        
            // update the matrix.
            _modelMatrix =  matrix;
            glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, &_modelMatrix[0][0]);
            _position_initialized = true;
        }
    }
    else
    /////////////////////////////////////////////
    // Without interpolation
    {
        _modelMatrix =  matrix;
        glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, &_modelMatrix[0][0]);
    }

}



/*!
 Interpolation function for the matrix /
 the position of the object
 */
void GLObjectObjInt::interpolateMat(void)
{
    // Calculate the distance between the target position and the current position.
    glm::vec3 temp = _target_position - _current_position;
    float distance = sqrt(dot(temp, temp));
    
    
    glm::quat temp_quad;
    temp_quad.x = _target_quat.x - _current_quat.x;
    temp_quad.y = _target_quat.y - _current_quat.y;
    temp_quad.z = _target_quat.z - _current_quat.z;
    temp_quad.w = _target_quat.w - _current_quat.w;
    
    // Calculate the distance between the target angle and the current angle.
    float delta_angle = sqrt( (_target_quat.x - _current_quat.x)*(_target_quat.x - _current_quat.x)  +
                         (_target_quat.y - _current_quat.y)*(_target_quat.y - _current_quat.y)  +
                         (_target_quat.z - _current_quat.z)*(_target_quat.z - _current_quat.z)  +
                        (_target_quat.w - _current_quat.w)*(_target_quat.w - _current_quat.w));
    
    // If the distance is too large, find the next step
    if (distance > 0.01 || delta_angle > 0.01) {
        
        // Linear interpolation of the position
        _current_position = _current_position + temp *  glm::vec3(0.08);
        
        // Linear interpolation of the rotation using slerp
        _current_quat = glm::slerp(_current_quat, _target_quat, 0.25f);
        
        // convert the quaternion to a matrix
        _target_matrix = glm::mat4_cast(_current_quat);
        
        // write the position back.
        _target_matrix[3][0] = _current_position[0];
        _target_matrix[3][1] = _current_position[1];
        _target_matrix[3][2] = _current_position[2];
        
        // update the model matrix.
        _modelMatrix = _target_matrix;
        glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, &_modelMatrix[0][0]);
        
    }
}


/*!
 Enable interpolation.
 */
void GLObjectObjInt::enableInterpolation(bool enable)
{
    _with_interpolation = enable;
}




bool GLObjectObjInt::extractNextFace1(string& in, string& out, int& pointIdx0, int& pointIdx1, int& pointIdx2 )
{
    GLuint a,b,c;
    
    
    // remove the first space
    int idv1 = in.find_first_of(" ");
    string v1(in.substr(idv1+1, in.length()-idv1)); // remove it
    
    sscanf(v1.c_str(), " %d %d %d", &a, &b, &c);
    
    return true;
}




bool GLObjectObjInt::extractNextFace3(string& in, string& out, int& pointIdx, int& texIdx, int& normalIdx )
{
    GLuint a,b,c;
    
    // first
    int idv1 = in.find_first_of(" ");
    string v1(in.substr(idv1+1, in.length()-idv1));
    
    sscanf(v1.c_str(), " %d/%d/%d", &a, &b, &c);
    
    int idv2 = v1.find_first_of(" ");
    if(idv2 != -1)
    {
        out = v1.substr(idv2, v1.length()-idv2); // with blank
    }
    else
    {
        out = "";
    }
    
    /*
    int idx = v1.find_first_of("/");
    istringstream c1(v1.substr(0,idx));
    
    
    c1 >> a;
    
    // second
    string v2 = v1.substr(idx+1, v1.length()-idx);
    
    int idv2 = v2.find_first_of("/");
    istringstream c2(v2.substr(0,idv2));
    c2 >> b;
    
    
    // third
    string v3 = v2.substr(idv2+1, v2.length()-idv2);
    
    int idv3 = v3.find_first_of(" ");
    istringstream c3(v3.substr(0,idv3));
    
    c3 >> c;
    
    if(idv3 != -1)
        out = v3.substr(idv3, in.length()-idv3); // with blank
    else
        out = "";
     */

    pointIdx = a;
    texIdx = b;
    normalIdx = c;
    
    return true;

}




bool GLObjectObjInt::load_obj(const char* filename, vector<glm::vec3> &vertices, vector<glm::vec3> &normals, vector<GLuint> &elements)
{
    ifstream in(filename, ios::in);
    if (!in)
    {
        cerr << "Cannot open " << filename << endl;
        return false;
    }
    

    
    vector<glm::vec3> temp_normals;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec3> temp_index_normal2point;
    vector<glm::vec3> temp_index_triangle;
    vector<glm::vec3> temp_index_textures;
    
    string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;// v.w = 1.0f;
            temp_vertices.push_back(v);
            _vertex_colors.push_back(glm::vec4(1.0,0.0,0.0,1.0));
            
        }
        else if (line.substr(0,2) == "f ")
        {
            int idx = line.find_first_of("/");
            if(idx == -1)
            {
                istringstream s(line.substr(2));
                GLushort a,b,c;
                s >> a; s >> b; s >> c;
                cout << a << " : " << b << " : " << c << endl;;
                a--; b--; c--;
            
                elements.push_back(a); elements.push_back(b); elements.push_back(c);
            }
            else
            {
                
                size_t n = std::count(line.begin(), line.end(), '/');
                if(n == 0)
                {
                    string new_line;
                    int p0, p1, p2;
                    extractNextFace1(line, new_line, p0, p1, p2 );
                    
                    temp_index_triangle.push_back(glm::vec3(p0, p1, p2));
                    temp_index_normal2point.push_back(glm::vec3(p0, p1, p2)); // same normal vectors.
                    
                }
                else if(n == 3)
                {
                    string new_line;
                    int p0, p1, p2;
                    extractNextFace1(line, new_line, p0, p1, p2 );
                    
                    temp_index_triangle.push_back(glm::vec3(p0, p1, p2));
                    temp_index_normal2point.push_back(glm::vec3(p0, p1, p2)); // same normal vectors.

                
                }
                else if(n==6)
                {
                    string new_line;
                    int p0, t0,  n0;
                    int p1, t1,  n1;
                    int p2, t2,  n2;
                    int p3, t3,  n3;
                    
                    // first
                    extractNextFace3(line, new_line, p0, t0, n0 );
                    
                    // second
                    extractNextFace3(new_line, new_line, p1, t1, n1 );
                    
                    // third
                    extractNextFace3(new_line, new_line, p2, t2, n2 );
                    
                    // Quads and not triangles
                    if(new_line.length() > 2)
                    {
                        // fourth
                        extractNextFace3(new_line, new_line, p3, t3, n3 );
                    }

                    // remember the normal vectors
                    temp_index_triangle.push_back(glm::vec3(p0, p1, p2));
                    temp_index_normal2point.push_back(glm::vec3(n0, n1, n2));
                    temp_index_textures.push_back(glm::vec3(t0, t1, t2));
                    
                    elements.push_back(p0);
                    elements.push_back(p1);
                    elements.push_back(p2);
                    
                }
                
               
            }
        }
        else if (line.substr(0,3) == "vn ")
        {
            istringstream s(line.substr(3));
            glm::vec3 n; s >> n.x; s >> n.y; s >> n.z;
            temp_normals.push_back(n);
        }
        else if (line.substr(0,7) == "mtllib ")
        {
            istringstream s(line.substr(7));
            s >> _material_file;
            /* ignoring this line */
        }
        else if (line.substr(0,2) == "g ")
        {
            istringstream s(line.substr(2));
            s >> _model_name;
            /* ignoring this line */
        }
        else if (line[0] == '#')
        {
            /* ignoring this line */
        }
        else
        {
            /* ignoring this line */
        }
    }
    
    
    /////////////////////////////////////////////
    // assign normals to points and points to triangles.
    
    vertices.clear();
    normals.clear();
    for(int i=0; i<temp_index_triangle.size(); i++)
    {
        glm::vec3 pointIdx = temp_index_triangle[i];
        glm::vec3 normalIdx = temp_index_normal2point[i];
        
        // the three points of one triangle
        // -1 since obj does not use the index 0
        vertices.push_back(temp_vertices[ pointIdx.x-1 ]);
        vertices.push_back(temp_vertices[ pointIdx.y-1 ]);
        vertices.push_back(temp_vertices[ pointIdx.z-1 ]);
        
        normals.push_back(temp_normals[normalIdx.x-1]);
        normals.push_back(temp_normals[normalIdx.y-1]);
        normals.push_back(temp_normals[normalIdx.z-1]);
        
    }
    
    
    
    
    
    
    
    if(normals.size() > 0) return true; // loaded normal vectors
    
    normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
    for (int i = 0; i < elements.size(); i+=3)
    {
        GLushort ia = elements[i];
        GLushort ib = elements[i+1];
        GLushort ic = elements[i+2];
        glm::vec3 normal = glm::normalize(glm::cross(
                                                     glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
                                                     glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
        normals[ia] = normals[ib] = normals[ic] = normal;
        
        
       // _vertex_colors.push_back(glm::vec4(1.0,0.0,0.0,1.0));
      //  _vertex_colors.push_back(glm::vec4(1.0,0.0,0.0,1.0));
       // _vertex_colors.push_back(glm::vec4(1.0,0.0,0.0,1.0));
    }
    
    return true;
}


/*!
 Create the vertex buffer object for this element
 */
void GLObjectObjInt::initVBO(void)
{
    _num_vertices = _vertices.size();
    
    // create memory for the vertices, etc.
    float* vertices = new float[_num_vertices * 3];
    float* normals = new float[_normals.size() * 3];
    int* indices = new int[_elements.size()];
    
    // Copy all vertices
    for(int i=0; i<_vertices.size() ; i++)
    {
        glm::vec3 t = _vertices[i];
        for (int j=0; j<3; j++) {
            vertices[(i*3)+j] = t[j];
        }
    }
    
    // copy all normals
    for(int i=0; i<_normals.size() ; i++)
    {
        glm::vec3 n = _normals[i];
        for (int j=0; j<3; j++) {
            normals[(i*3)+j] = n[j];
        }
    }

    
    
    
    glGenVertexArrays(1, _vaoID); // Create our Vertex Array Object
    glBindVertexArray(_vaoID[0]); // Bind our Vertex Array Object so we can use it
    
    
    glGenBuffers(2, _vboID); // Generate our Vertex Buffer Object
    
    // vertices
     int locPos = glGetAttribLocation(_program, "in_Position");
    glBindBuffer(GL_ARRAY_BUFFER, _vboID[0]); // Bind our Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, _num_vertices * 3 * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
    
    glVertexAttribPointer((GLuint)locPos, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(locPos); //
    
    
     // normals
    int locNorm = glGetAttribLocation(_program, "in_Normal");
    glBindBuffer(GL_ARRAY_BUFFER, _vboID[1]); // Bind our Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, _normals.size() * 3 * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
    
    glVertexAttribPointer((GLuint)locNorm, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(locNorm); //
    
    // Index buffer array.
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboID[2]);
   // glBufferData(GL_ELEMENT_ARRAY_BUFFER, _elements.size() * sizeof(unsigned int), &_elements[0], GL_STATIC_DRAW);

    
    glBindVertexArray(0); // Disable our Vertex Buffer Object

}


/*
 Inits the shader program for this object
 */
void GLObjectObjInt::initShader(void)
{
    
    if(!_apperance.exists())return;
    
    // This loads the shader program from a file
    _program = _apperance.getProgram();
    
    
    glUseProgram(_program);
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Vertex information / names
    
    glBindAttribLocation(_program, 0, "in_Position");
    glBindAttribLocation(_program, 1, "in_Normal");

    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Define the model view matrix.
    _modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // Create our model matrix which will halve the size of our model
    addModelViewMatrixToProgram(_program);
    
    
    
    glUseProgram(0);
    
}




/*!
 Draw the objects
 */
void GLObjectObjInt::draw(void)
{
    
    glUseProgram(_program);

    // Bind the buffer and switch it to an active buffer
    glBindVertexArray(_vaoID[0]);
    
    
    // this changes the camera location
    glm::mat4 rotated_view =  rotatedViewMatrix();
    glUniformMatrix4fv(_viewMatrixLocation, 1, GL_FALSE, &rotated_view[0][0]); // send the view matrix to our shader
    glUniformMatrix4fv(_inverseViewMatrixLocation, 1, GL_FALSE, &invRotatedViewMatrix()[0][0]);
    glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, &_modelMatrix[0][0]); //

    
   // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    // Draw the triangles
    glDrawArrays(GL_TRIANGLES, 0, _num_vertices);
    
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementbuffer);
    //glDrawElements( GL_TRIANGLES, _elements.size(), GL_UNSIGNED_INT,(void*)0 );
    
    
    // Unbind our Vertex Array Object
    glBindVertexArray(0);
    
}



/*!
Returns the number of vertices
*/
int GLObjectObjInt::size(void)
{
    return _num_vertices;
}


/*!
To update the vertices.
This function takes a vector of vertices and replaces the current vector.
*/
void GLObjectObjInt::updateVertices(float* vertices)
{

    glBindVertexArray(_vaoID[0]); // Bind our Vertex Array Object so we can use it
    
    // vertices
    int locPos = glGetAttribLocation(_program, "in_Position");
    glBindBuffer(GL_ARRAY_BUFFER, _vboID[0]); // Bind our Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, _num_vertices * 3 * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer((GLuint)locPos, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(locPos); //

}