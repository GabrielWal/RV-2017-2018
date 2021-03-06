#include "vector.h"
#include "texture.h"
#include "mesh.h"
#include "utilities.h"
#include <iostream>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>
#define GL_GLEXT_PROTOTYPES
#endif

using namespace std;

// Some global variables:
int windowW = 1366;
int windowH = 768;
float focale = 65.0f;
float near = 0.1f;
float far = 100.0f;
float zoom = 1.0f;
// Rotations around X and Y axis
GLint oldX = 0;
GLint oldY = 0;
GLboolean boutonClick = false;
// Mesh creation
Mesh mesh;


// callback functions
GLvoid affichage() { mesh.affichage(); }
GLvoid souris(int bouton, int etat, int x, int y);
GLvoid deplacementSouris(int x, int y);
GLvoid redimensionner(int w, int h);
GLvoid clavier(unsigned char touche, int x, int y);
void specialKeys( int key, int x, int y );

int main(int argc, char *argv[]) {
   if (argc > 1) {
      if (mesh.readOBJ(argv[1]) == false) {
         cout << "Problem reading OBJ file" << endl;
      }
   }
   else {
      if (mesh.readOBJ("DNA.obj") == false) {
         cout << "Problem reading OBJ file" << endl;
      }
   }
   // Initialisation de GLUT
   glutInit(&argc, argv);
   // Choix du mode d'affichage (ici RVB)
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   // Position initiale de la fenetre GLUT
   glutInitWindowPosition(200, 200);
   // Taille initiale de la fenetre GLUT
   glutInitWindowSize(windowW, windowH);
   // Creation de la fenetre GLUT
   glutCreateWindow("Load OBJ");
   
   // With help from NeHe tutorial
   // Enable Texture Mapping
   glEnable(GL_TEXTURE_2D);
   glClearDepth(1.0f);
   // Depth Buffer Setup
   glEnable(GL_DEPTH_TEST);
   // The Type Of Depth Testing To Do
   glDepthFunc(GL_LEQUAL);
   // Enable lighting
   glEnable(GL_LIGHTING);
   // Enable light number 1
   glEnable(GL_LIGHT0);

   // Texture loading
   if (mesh.getTextureFiles().size() > 0 && exist(mesh.getTextureFiles()[0]))
   {
      GLuint textureID = 0;
      Texture T = Texture(mesh.getTextureFiles()[0].c_str(), false);
      glGenTextures(1, &textureID);
      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, T.getTexData());
   }
   //glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!! Not working in VM
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

   // Really Nice Perspective
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 

   // Définition de la couleur d'effacement du framebuffer
   glClearColor(0.0f,0.0f,0.0f,0.0f);


   // Définition des fonctions de callbacks
   glutDisplayFunc(affichage);
   glutMouseFunc(souris);
   glutMotionFunc(deplacementSouris);
   glutKeyboardFunc(clavier);
   glutReshapeFunc(redimensionner);
   glutSpecialFunc(specialKeys);

   // Lancement de la boucle infinie GLUT
   glutMainLoop();

   return 0;
}

// Callback de redimensionnement de la fenêtre
GLvoid redimensionner(int w, int h) {
   // Garde les valeurs
   windowW = w;
   windowH = h;
   // eviter une division par 0
   if(windowH==0)
      windowH = 1;
   
   float ratio = (float)windowW / (float)windowH;
   std::cout << "Ratio : " << ratio << std::endl;
   
   // Projection
   glMatrixMode(GL_PROJECTION);
   
   // Resetting matrix
   glLoadIdentity();
   
   // Viewport
   glViewport(0, 0, windowW, windowH);
   
   // Mise en place de la perspective
   gluPerspective(focale, ratio, near, far);
   
   // Placement de la caméra
   gluLookAt(0, 0, 2, 0, 0, 0, 0, 1, 0);
   
   // Retourne a la pile modelview
   glMatrixMode(GL_MODELVIEW);
}

GLvoid clavier(unsigned char touche, int x, int y) {
   switch(touche) {
      case '+' : // zoom in
         mesh.setZoom(mesh.getZoom()+0.1);
         glutPostRedisplay();
         break;
      case '-' : // zoom in
         mesh.setZoom(mesh.getZoom()-0.1);
         glutPostRedisplay();
         break;
      case 'q' : // quit
      case 27 :
         exit(0);
         break;
   }
   glutPostRedisplay();
}

void specialKeys( int key, int x, int y ) {
   //  Right arrow - increase rotation by 5 degree
   if (key == GLUT_KEY_RIGHT) {
      mesh.setAngleY(mesh.getAngleY()+5);
   }
    
   //  Left arrow - decrease rotation by 5 degree
   else if (key == GLUT_KEY_LEFT) {
      mesh.setAngleY(mesh.getAngleY()-5);
   }
    
   else if (key == GLUT_KEY_UP) {
      mesh.setAngleX(mesh.getAngleX()+5);
   }
    
   else if (key == GLUT_KEY_DOWN) {
      mesh.setAngleX(mesh.getAngleX()-5);
   }
    
   //  Request display update
   glutPostRedisplay();
}

// Mouse functions
GLvoid souris(int bouton, int etat, int x, int y){
   // Test pour voir si le bouton gauche de la souris est appuyé
   if(bouton == GLUT_LEFT_BUTTON && etat == GLUT_DOWN) {
      boutonClick = true;
      oldX = x;
      oldY = y;
   }
   
   // si on relache le bouton gauche
   if(bouton == GLUT_LEFT_BUTTON && etat == GLUT_UP) {
      boutonClick = false;
   }

   if(bouton==3) { // scroll up
      mesh.setZoom(mesh.getZoom()+0.01);
      glutPostRedisplay();
   }
   if(bouton==4) { // scroll down
      mesh.setZoom(mesh.getZoom()-0.01);
      glutPostRedisplay();
   }
}

GLvoid deplacementSouris(int x, int y) {
   // si le bouton gauche est appuye et qu'on se deplace
   // alors on doit modifier les angles de rotations du cube
   // en fonction de la derniere position de la souris 
   // et de sa position actuelle
   if(boutonClick) {
      mesh.setAngleX(mesh.getAngleX() + (x-oldX)*0.5);
      mesh.setAngleY(mesh.getAngleY() + (y-oldY)*0.5);
      // Appeler le re-affichage de la scene OpenGL
      glutPostRedisplay();
   }
   
   // Mise a jour des anciennes positions de la souris en X et Y
   oldX = x;
   oldY = y;
}
