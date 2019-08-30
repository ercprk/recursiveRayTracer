#define NUM_OPENGL_LIGHTS 8

#include "MyGLCanvas.h"

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) 
: Fl_Gl_Window(x, y, w, h, l)
{
	mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	
	rotVec = glm::vec3(0.0f, 0.0f, 0.0f);
	eyePosition = glm::vec3(2.0f, 2.0f, 2.0f);
    pixels = NULL;

    objType = SHAPE_CUBE;
    cube = new Cube();
    cylinder = new Cylinder();
    cone = new Cone();
    sphere = new Sphere();
    shape = cube;

	isectOnly = 1;
    recurseDepth = 0;
	scale = 1.0f;

    rayTracer = NULL;
    parser = NULL;
    flatSceneTree = NULL;
	camera = new Camera();
	camera->orientLookAt(eyePosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

MyGLCanvas::~MyGLCanvas()
{
	if (camera != NULL)
    {
		delete camera;
	}

	if (pixels != NULL)
    {
		delete[] pixels;
        std::cout << "[  MyGLCanvas  ] Pixels deallocated" << std::endl;
	}

    if (rayTracer != NULL)
    {
        delete rayTracer;
    }

    if (flatSceneTree != NULL)
    {
        delete flatSceneTree;
    }

    if (parser != NULL)
    {
        delete parser;
    }
    std::cout << "[  MyGLCanvas  ] Destructor called" << std::endl;
}

void
MyGLCanvas::loadSceneFile(const char* filenamePath)
{
	if (parser != NULL)
    {
		delete parser;
	}

    if (rayTracer != NULL)
    {
        delete rayTracer;
    }

    if (flatSceneTree != NULL)
    {
        delete flatSceneTree;
    }

	parser = new SceneParser(filenamePath);

	bool success = parser->parse();
	cout << "success? " << success << endl;

	if (success == false)
    {
		delete parser;
		parser = NULL;
	} 
    else
    {
		SceneCameraData cameraData;
		parser->getCameraData(cameraData);

		camera->reset();
		camera->setViewAngle(cameraData.heightAngle);

		if (cameraData.isDir == true)
        {
			camera->orientLookVec(cameraData.pos, cameraData.look, 
                cameraData.up);
		}
		else
        {
			camera->orientLookAt(cameraData.pos, cameraData.lookAt, 
                cameraData.up);
		}

        /****************** FLATTENING PARSER ********************/
        flatSceneTree = new FlatSceneTree();
        flatSceneTree->flattenSceneTree(parser->getRootNode());

        rayTracer = new RayTracer(flatSceneTree, parser, cube, 
            cylinder, cone, sphere);
	}
}

void MyGLCanvas::draw()
{
	if (!valid())
    { //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("Establishing GL context\n");

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (parser == NULL)
    {
		return;
	}

	if (pixels == NULL)
    {
		return;
	}

	//this just draws the "pixels" to the screen
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(w(), h(), GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

int
MyGLCanvas::handle(int e)
{
	//printf("Event was %s (%d)\n", fl_eventnames[e], e);
	switch (e)
    {
        case FL_KEYUP:
            printf("Keyboard event: key pressed: %c\n", Fl::event_key());
            break;
        case FL_MOUSEWHEEL:
            break;
	}

	return Fl_Gl_Window::handle(e);
}

void
MyGLCanvas::resize(int x, int y, int w, int h)
{
	Fl_Gl_Window::resize(x, y, w, h);

	if (camera != NULL)
    {
		camera->setScreenSize(w, h);
	}

	puts("Resize called");
}

void
MyGLCanvas::updateCamera(int width, int height)
{
	float xy_aspect;
	xy_aspect = (float)width / (float)height;

	camera->setScreenSize(width, height);
}

void
MyGLCanvas::updateRayTracer(int recurseDepth)
{
    rayTracer->setRecurseDepth(recurseDepth);
}

//Given the pixel (x, y) position, set its color to (r, g, b)
void
MyGLCanvas::setpixel(GLubyte* buf, int x, int y, int r, int g, int b)
{
	int pixelWidth = camera->getScreenWidth();
	buf[(y*pixelWidth + x) * 3 + 0] = (GLubyte)r;
	buf[(y*pixelWidth + x) * 3 + 1] = (GLubyte)g;
	buf[(y*pixelWidth + x) * 3 + 2] = (GLubyte)b;
}

void MyGLCanvas::renderScene()
{
    int pixelWidth, pixelHeight, i, j, num_lights;
    double t;
    glm::vec3 eye, pix, ray; // point denotes intersection point
    glm::mat4 film2WorldMat;
    FlatSceneNode* closestObj;
    SceneColor intensity;

	std::cout << "Render button clicked" << std::endl;

	if (parser == NULL)
    {
		std::cout << "No scene loaded yet" << std::endl;
		return;
	}

	pixelWidth = w();
	pixelHeight = h();

	updateCamera(pixelWidth, pixelHeight);
    updateRayTracer(recurseDepth);

	if (pixels != NULL)
    {
		delete[] pixels;
        std::cout << "Pixels deallocated" << std::endl;
	}

	pixels = new GLubyte[pixelWidth * pixelHeight * 3];
	memset(pixels, 0, pixelWidth * pixelHeight * 3);
    std::cout << "Pixels allocated" << std::endl;

    // Compute matrix to transform film pixel coordinate into world coordinate
    film2WorldMat = camera->getInvTranslationMat() 
        * camera->getInvRotationMat() * camera->getInvScalingMat();

    // Eye point in world coordinate
    eye = camera->getEyePoint();

	for (i = 0; i < pixelWidth; i++)
    {
		for (j = 0; j < pixelHeight; j++)
        {
            //std::cout << i << " " << j << std::endl;
            pix = glm::vec3(((2.0 * (double)i) / ((double)pixelWidth - 1.0)) 
                - 1.0, ((2.0 * (double)j) / ((double)pixelHeight - 1.0)) 
                - 1.0, -1.0);
            pix = glm::vec3(film2WorldMat * glm::vec4(pix, 1.0));
            ray = glm::normalize(pix - eye);

            t = rayTracer->rayCast(eye, ray, closestObj);

            if (t > 0.0)
            {
                if (isectOnly)
                {
                    setpixel(pixels, i, j, 200, 200, 200);
                }
                else
                {
                    intensity = rayTracer->rayTrace(eye, ray, t, 
                        closestObj, 0);
                    setpixel(pixels, i, j, intensity.r * 255.0, 
                        intensity.g * 255.0, intensity.b * 255.0);
                }
            }
		}
	}

	std::cout << "Render complete" << std::endl;
	redraw();
}

