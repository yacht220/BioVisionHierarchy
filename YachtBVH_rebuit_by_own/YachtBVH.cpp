//×¢Òâ£¬ÐèÒªÏÈ#include <gl/glut.h>£¬ÔÙ#include <gl/gl.h>
#include <gl/glut.h>  
#include <gl/glu.h>
#include <gl/gl.h>

#include "math3d.h"
#include "resource.h"

#include "loadAnimation.h"
#include "Skeleton.h"
#include "Voice.h"

///////////////////////////////////////////////////////////////////////////////
// These values need to be available globally
    // Light values and coordinates
    GLfloat  ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat  diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat	 lightPos[] = { -50.0f, 50.0f, 50.0f, 0.0f };

	// Transformation matrix to project shadow
    M3DMatrix44f shadowMat;

	//´´½¨¹Ç÷À
	t_Bone m_Skeleton;

	bool rFlag = FALSE, lFlag = FALSE;//±êÊ¶ÓïÒô¿ØÖÆÖÐµÄ×óÓÒ×ª¶¯

	int rLoop = 0, lLoop = 0;//±êÊ¶ÒòÓïÒô¿ØÖÆÒýÆðµÄ×ª¶¯´ÎÊý
///////////////////////////////////////////////////////////
// Draw a ground
void DrawGround(void)
{
	glBegin(GL_QUADS);
	  glNormal3f(0.0f, 1.0f, 0.0f);
	  glColor3f(0,1,0);
      glVertex3f(-150.0f, 0.0f, -150.0f);
      glVertex3f(150.0f, 0.0f, -150.0f);
	  glVertex3f(150.0f, 0.0f, 150.0f);
	  glVertex3f(-150.0f, 0.0f, 150.0f);
    glEnd();
}


/////////'/////////////////////////////////////////////////////////////////////
// Function:	drawSkeleton
// Purpose:		Actually draws the Skeleton it is recurqive
// Arguments:	None
///////////////////////////////.///////////////m////////////////+//////////////
void drawSkeleton(t_Bone *rootBone,int nShadow)
{
/// Local Variables //////////////////////////////////////////////////////////
    int loop;
	t_Bone *curBone;
////////////////////////////////////////+//////////////////////////////////////
	if(nShadow == 0)
		glColor3f(1,0,0);
	else
		glColor3f(0,0,0);


	curBone = rootBone->children;//Í¬Ò»¸ö¸¸¹Ç÷ÀÏÂ¿ÉÒÔÓÐ¶à¸ö×Ó¹Ç÷À£¬curBone++Ïàµ±ÓÚÈÃÖ¸ÕëcurBoneÖ¸ÏòÁíÒ»¸ö×Ó¹Ç÷À
	for (loop = 0; loop < rootBone->childCnt; loop++)
	{
		glPushMatrix();

		glTranslatef(curBone->trans.x, curBone->trans.y, curBone->trans.z);

		// Set observer's orientation and position
		glRotatef(curBone->rot.z, 0.0f, 0.0f, 1.0f);
		glRotatef(curBone->rot.x, 1.0f, 0.0f, 0.0f);
		glRotatef(curBone->rot.y, 0.0f, 1.0f, 0.0f);
        
		//»­Ò»¸öÊµÐÄÇòÌå±íÊ¾Ò»¸ö¹Ç÷À
		glutSolidSphere(3.0f, 10, 10);

		// CHECK IF THIS BONE HAS CHILDREN, IF SO RECURSIVE CALL/ÕâÑù²ÅÄÜ±£Ö¤ËùÓÐµÄ×Ó¹Ç÷À¶¼»­Íê
		if (curBone->childCnt > 0)
			drawSkeleton(curBone,nShadow); 
		//Ìø³öµÝ¹é£¬»Øµ½ÉÏÒ»¼¶¼ÌÐøÖ´ÐÐ£¬ÒÀ´ÎÀàÍÆ

		glPopMatrix();

		curBone++;
	}
}
//// drawSkeleton /////////////////////////////////////////////////////////////



// Called to draw scene
void RenderScene(void)
{
    /// Local Variables ///////////////////////////////////////////////////////////
	int flag=0;//±êÊ¶²»Í¬µÄÓïÒôÃüÁî
    ///////////////////////////////////////////////////////////////////////////////

	if (m_Skeleton.rot.y  > 360.0f) m_Skeleton.rot.y  -= 360.0f;
    if (m_Skeleton.rot.x   > 360.0f) m_Skeleton.rot.x   -= 360.0f;
    if (m_Skeleton.rot.z > 360.0f) m_Skeleton.rot.z -= 360.0f;
	
	/////////////////////////////////////////////////////////////////
	///////Voice/////////////////////////////////////////////////
	ProcessRecoEvent(NULL,&flag);
	switch(flag)
	{
		case 1://right flag
            rFlag = TRUE;
			break;
		case 2://left flag
            lFlag = TRUE;
			break;
		case 3://run flag
			DestroySkeleton(&m_Skeleton);//Çå¿Õ¹Ç÷À´æ´¢¿Õ¼ä
			LoadBVH("..\\YachtBVH_rebuit_by_own\\BVH\\Run.bvh",&m_Skeleton);//¼ÓÔØbvhÊý¾ÝÎÄ¼þ
			break;
		case 4://walk flag
			DestroySkeleton(&m_Skeleton);//Çå¿Õ¹Ç÷À´æ´¢¿Õ¼ä
			LoadBVH("..\\YachtBVH_rebuit_by_own\\BVH\\Walk-Normal.bvh",&m_Skeleton);//¼ÓÔØbvhÊý¾ÝÎÄ¼þ
			break;
		case 5://fight flag
			DestroySkeleton(&m_Skeleton);//Çå¿Õ¹Ç÷À´æ´¢¿Õ¼ä
			LoadBVH("..\\YachtBVH_rebuit_by_own\\BVH\\DOUBLE PUNCH 1.bvh",&m_Skeleton);//¼ÓÔØbvhÊý¾ÝÎÄ¼þ
			break;
		case 6://stand flag
			DestroySkeleton(&m_Skeleton);//Çå¿Õ¹Ç÷À´æ´¢¿Õ¼ä
			LoadBVH("..\\YachtBVH_rebuit_by_own\\BVH\\T.bvh",&m_Skeleton);//¼ÓÔØbvhÊý¾ÝÎÄ¼þ
			break;
		case 7://walk slowly flag
			DestroySkeleton(&m_Skeleton);//Çå¿Õ¹Ç÷À´æ´¢¿Õ¼ä
			LoadBVH("..\\YachtBVH_rebuit_by_own\\BVH\\Walk-Slow.bvh",&m_Skeleton);//¼ÓÔØbvhÊý¾ÝÎÄ¼þ
			break;
		case 8://dance flag
			DestroySkeleton(&m_Skeleton);//Çå¿Õ¹Ç÷À´æ´¢¿Õ¼ä
			LoadBVH("..\\YachtBVH_rebuit_by_own\\BVH\\ballet1.bvh",&m_Skeleton);//¼ÓÔØbvhÊý¾ÝÎÄ¼þ
			break;
		case 9://run slowly flag
			DestroySkeleton(&m_Skeleton);//Çå¿Õ¹Ç÷À´æ´¢¿Õ¼ä
			LoadBVH("..\\YachtBVH_rebuit_by_own\\BVH\\Jog.bvh",&m_Skeleton);//¼ÓÔØbvhÊý¾ÝÎÄ¼þ
			break;
	}
	///////////////////////////////////////////////////////////////


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Draw a ground///////////////
	DrawGround();

	//ÓïÒô¿ØÖÆ×óÓÒÃüÁîÒýÆðµÄÖð²½×ª¶¯

	if(rLoop == 45)
	{
		rFlag = FALSE;
		rLoop = 0;
	}
	if(lLoop == 45)
	{
		lFlag = FALSE;
		lLoop = 0;
	}	
	
	if(rFlag == TRUE)
	{		
		m_Skeleton.rot.y -=2.0f;
		rLoop++;
	}	
	if(lFlag == TRUE)
	{
		m_Skeleton.rot.y += 2.0f;
		lLoop++;
	}

    glPushMatrix();
	
	//Set light position
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

	// Get ready to draw the shadow
    // First disable lighting and save the projection state
    glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
    glPushMatrix();

	// Multiply by shadow projection matrix
    glMultMatrixf((GLfloat *)shadowMat);

	glTranslatef(m_Skeleton.trans.x, m_Skeleton.trans.y, m_Skeleton.trans.z);

	// Set root skeleton's orientation and position
    glRotatef(m_Skeleton.rot.z, 0.0f, 0.0f, 1.0f);
	glRotatef(m_Skeleton.rot.x, 1.0f, 0.0f, 0.0f);
    glRotatef(m_Skeleton.rot.y, 0.0f, 1.0f, 0.0f);

	drawSkeleton(&m_Skeleton,1);

	// Restore the projection to normal
    glPopMatrix();
	// Restore lighting state variables
    glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

    glPushMatrix();

	glTranslatef(m_Skeleton.trans.x, m_Skeleton.trans.y, m_Skeleton.trans.z);

	// Set root skeleton's orientation and position
    glRotatef(m_Skeleton.rot.z, 0.0f, 0.0f, 1.0f);
	glRotatef(m_Skeleton.rot.x, 1.0f, 0.0f, 0.0f);
    glRotatef(m_Skeleton.rot.y, 0.0f, 1.0f, 0.0f);

	drawSkeleton(&m_Skeleton,0);

    glPopMatrix();

	glPopMatrix();

    glFinish();

	// Show the image
	glutSwapBuffers();

	//Ôö¼ÓÒ»Ö¡
	BoneAdvanceFrame(&m_Skeleton ,1);
}


// This function does any needed initialization on the rendering
// context. 
void SetupRC()
{
	// Any three points on the ground (counter clockwise order)
    M3DVector3f points[3] = {{ -30.0f, 0.0f, -20.0f },
                            { -30.0f, 0.0f, 20.0f },
                            { 40.0f, 0.0f, 20.0f }};
///////////////////////////////////////////////////////////////////////////////
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

     // Setup light parameters
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientLight);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	
	// Mostly use material tracking
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

	// Get the plane equation from three points on the ground
    M3DVector4f vPlaneEquation;
    m3dGetPlaneEquation(vPlaneEquation, points[0], points[1], points[2]);

    // Calculate projection matrix to draw shadow on the ground
    m3dMakePlanarShadowMatrix(shadowMat, vPlaneEquation, lightPos);
}


void TimerFunc(int value)
{

    glutPostRedisplay();
    glutTimerFunc(30, TimerFunc, 1);
}

void ChangeSize(int width, int height)
{
// Local Variables /////////////////////////////////////////////////////////
    GLfloat aspect;
///////////////////////////////'/////////////////////////////////////////////

    glViewport(0, 0, width, height);

    aspect = (GLfloat)width/(GLfloat)height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(30.0, aspect,1, 2000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glTranslatef(0,-5,-500);
	glRotatef(30,1,0,0);	
}


int main(int argc, char* argv[])
{
	//Voice////////////////////////////////////////////////
	//ÒªÊ¹ÓÃCoInitialize()»òÕßCoInitializeEx()ÕâÁ½¸öº¯ÊýµÄÆäÖÐÒ»¸öÀ´´´½¨COM»òÕßDCOM¶ÔÏó
	::CoInitialize(NULL);

	InitSAPI(NULL);
	///////////////////////////////////////////////////////
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
	glutCreateWindow("YachtBVH");
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
    glutTimerFunc(250, TimerFunc, 1);
	SetupRC();
	glutMainLoop();
    //Voice////////////////////////////////////////////////
	//ÒªÊ¹ÓÃ¶ÔÓ¦µÄUnInitialize()»òÕßUnInitializeEx()À´ÊÍ·ÅCOM»òÕßDCOM½Ó¿Ú
	::CoUninitialize();
	/////////////////////////////////////////////////////
	return 0;
}
