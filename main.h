//Copyright (C) 2014-2015  Corwin Hansen
#ifndef MAIN_H//include protection
	#define MAIN_H


#include <stdio.h>//input and output
#include <stdlib.h>//standard stuff
#include <math.h>//all the math functions
#include <string.h>//string related things
#include <time.h>//time on system
#include "SDL.h"//include sdl
#include "SDL_ttf.h"//include sdl_ttf
#include "SDL_image.h"//include sdl_image


#define FULLLSCREEN //define if fullscreen

#ifdef FULLLSCREEN 
#define WINDOWFLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS        //fullscreen borderless window
#define WIDTH display.w //width and height of window
#define HEIGHT display.h
#else
#define WINDOWFLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE        //resizable window
#define WIDTH display.w/2 //width and height of window
#define HEIGHT display.h/2;
#endif

#define PEN_SIZE 0.0078125

#define WINDOW_NAME "Trigonometry"
#define DELAY 10 //default delay
#define DELAY_SLOW 100 //dealy while in background

//paths
#define RESOURCES "resources/" //media resources path. 
#define FONTS "fonts/" //fonts path. Inside media resources path.
#define FONT "OpenSans-Regular.ttf" //font path

//defalut points
#define PAX 0.0
#define PAY 1.0
#define PBX 1.0
#define PBY 1.0
#define PCX 0.5
#define PCY (1.0 - (0.5*sqrt(3.0)))



SDL_Window *window;//sdl window
SDL_Renderer *renderer;//sdl renderer
SDL_Event event;//event

int width, height, maxside;//width and hight and smallest of 2 side of window
double ws, hs;//width and height scaled to maxside
time_t lasttime;//last time mesured
unsigned long int lastframe;//last frame recorded
unsigned long int frame;//number of frames from start
unsigned long int fps;//frames per second

SDL_Texture *somethingwentwrong;//image to display if something goes wrong




SDL_Texture *testimg;//image to test if it works
SDL_Texture *pen;//pen to draw with



typedef struct point{
	double x, y;//x and y position
}Point;

typedef struct line{
	double l;//length
}Line;

typedef struct angle{
	double a;//angle
}Angle;

typedef union value{//value
	Point p;//point
	Line l;//line
	Angle a;//angle
}Value;

Value pointa, pointb, pointc;//point a, b and c of the triangle to do calculations on
Value linea, lineb, linec;//line a, b and c of triangle to calculate on
Value anglea, angleb, anglec;//angle a, b and c of triangle to calculate on
Value lineh;//height of triangle

Value *Selected;//selected object
Value *Edit[3];//edited value in order







uint32_t delay;//amount of delay for each frame
double MouseX, MouseY;//x and y position of mouse / touch normalised


//fonts for different sizes (1/n of maxside)
TTF_Font* font_4;
TTF_Font* font_6;
TTF_Font* font_8;
TTF_Font* font_16;
TTF_Font* font_24;
TTF_Font* font_32;
TTF_Font* font_46;
TTF_Font* font_64;


int EventFilter(void* userdata, SDL_Event* e);//event filter
void Quit(void);//quit everything
void GetDisplay(void);//get display
void Clicked(long int x, long int y);//x and y positions clicked
void Resize(void);//recalculate numbers related to size
SDL_Texture* GetTexture(const char *file);//make texture from this file
TTF_Font* GetFont(const char *file, int size);//get font from file
SDL_Texture* GetTextTexture(TTF_Font* font, const char* text, int r, int g, int b);//make texture from font text and rgb
void DrawBase(void);//draw basic stuff
void DrawText(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int center);//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size
void DrawIMG(SDL_Texture *texture, double x, double y, SDL_Rect *rect, double w, double h, int center);//draw rect of texture at x and y position normalised at scale from maxside. Null rect for whole texture. set center to 1 to center to x and y
void DrawTriangle(double ax, double ay, double bx, double by, double cx, double cy);//draw triangle for those points with 1 as maxside
void DrawLine(double ax, double ay, double bx, double by);//draw line for those points with 1 as maxside

#endif