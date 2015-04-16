//Copyright (C) 2014-2015  Corwin Hansen
#include "main.h"//main header


int main(int argc, char *argv[]){
	renderer = NULL;//set values to null
	window = NULL;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){//start SDL and get any error if it dosen't.
		printf("Could not load SDL: %s\n", SDL_GetError());//print out error
		exit(EXIT_FAILURE);//exit
	}




	// load support for PNG image format
	int flags = IMG_INIT_PNG;//flag for PNG image loading
	int initted = IMG_Init(flags);//get initialised flags
	if ((initted&flags) != flags) {//if it caused error
		printf("Could not initialised png support: %s\n", IMG_GetError());//print error message
		exit(EXIT_FAILURE);//exit
	}


	//initialise ttf
	if (TTF_Init() == -1) {//if it causes error
		printf("could not initialised ttf support: %s\n", TTF_GetError());//print error message
		exit(EXIT_FAILURE);//exit
	}


	//filter events
	SDL_SetEventFilter(EventFilter, NULL);

	
	//quit SDL at exit
	atexit(Quit);



	Resize();//get display


	//Create window
	window = SDL_CreateWindow(
		WINDOW_NAME,                  // window title
		SDL_WINDOWPOS_CENTERED,           // initial x position
		SDL_WINDOWPOS_CENTERED,           // initial y position
		width,                               // width, in pixels
		height,                               // height, in pixels
		WINDOWFLAGS        //use these flags
		);


	// Check that the window was made
	if (window == NULL) {
		// In the event that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);//exit
	}


	//Create renderer
	renderer = SDL_CreateRenderer(window, -1, 0);


	// Check that the renderer was made
	if (renderer == NULL) {
		// In the event that the window could not be made...
		printf("Could not create renderer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);//exit
	}


	//initialise and load stuff
	delay = DELAY;//10 ms delay
	somethingwentwrong = GetTextTexture(font_64, "somethingwentwrong", 0, 0, 0);//image to display if something went wrong

	//loading message
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
	SDL_RenderClear(renderer);//clear screen
	SDL_Texture *loading = GetTextTexture(font_16, "Loading", 0, 0, 0);//loading message
	DrawText(loading, 0.5, 0.5, NULL, 1);//draw loading message
	SDL_RenderPresent(renderer);//present loading message
	SDL_DestroyTexture(loading);//don't need this texture

	//set initial values
	ix = 0.5 * ws;//image width and height
	iy = 0.5 * hs;
	pointa.p.x = PAX;//triangle default
	pointa.p.y = PAY;
	pointb.p.x = PBX;
	pointb.p.y = PBY;
	pointc.p.x = PCX;
	pointc.p.y = PCY;
	linea.l.priority = 0;//priority from high to low
	lineb.l.priority = 1;
	linec.l.priority = 2;
	anglea.a.priority = 3;
	angleb.a.priority = 4;
	anglec.a.priority = 5;
	lineh.l.priority = 6;
	pointa.p.known = 1;//only trust points at start
	pointb.p.known = 1;
	pointc.p.known = 1;
	linea.l.known = 0;
	lineb.l.known = 0;
	linec.l.known = 0;
	anglea.a.known = 0;
	angleb.a.known = 0;
	anglec.a.known = 0;
	lineh.l.known = 0;


	//load textures
	pen = GetTexture("pen.png");//get pen texture
	Background = NULL;//no background
	if (argc == 2){//if there is second argument
		SDL_Surface* surface = IMG_Load(argv[1]);//load surface
		if (surface == NULL){//if it could not be loaded
			printf("could not load image: %s\n", IMG_GetError());//error message
		}
		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);//get texture from loaded image
		if (texture == NULL){//if it could not be converted to texture
			printf("could not create texture: %s\n", SDL_GetError());//error message
		}
		SDL_FreeSurface(surface);//free surface
		Background = texture;//set texture
	}

	Resize();//reload display




	//main loop
	while (1) {
		while (SDL_PollEvent(&event)) {//for each event
			switch (event.type) {//for each event type
			case SDL_QUIT://quit everything
				Quit();//quit everything
				break;//get out
			case SDL_MOUSEBUTTONDOWN://when clicking down
				Clicked(event.button.x, event.button.y);//run clicked function 
				break;//get out
			case SDL_MOUSEMOTION://when mouse moved
				MouseX = (double)(event.button.x) / maxside;//set x and y position of mouse from square normalised
				MouseY = (double)(event.button.y) / maxside;
				if (event.button.button == SDL_BUTTON_LEFT || event.button.which == SDL_TOUCH_MOUSEID){//if mouse is pressed down
					Draged();//run draged function 
				}
				break;//get out
			case SDL_WINDOWEVENT://when window was changed
				Resize();//resize stuff
				break;//get out
			default://for everything else
				//ignore event
				break;//get out
			}
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
		SDL_RenderClear(renderer);//clear screen
		DrawBase();//draw background image

		Value *Left = &pointc, *Right = &pointc, *Top = &pointc, *Bottom = &pointc;//right, left, top and bottom point
		scale = 1;//scale up or down

		//calculate top, bottom, left and right
		if (pointa.p.x < Left->p.x)Left = &pointa;//get left
		if (pointb.p.x < Left->p.x)Left = &pointb;
		if (pointa.p.x > Right->p.x)Right = &pointa;//get right
		if (pointb.p.x > Right->p.x)Right = &pointb;
		if (pointa.p.y < Top->p.y)Top = &pointa;//get top
		if (pointb.p.y < Top->p.y)Top = &pointb;
		if (pointa.p.y > Bottom->p.y)Bottom = &pointa;//get bottom
		if (pointb.p.y > Bottom->p.y)Bottom = &pointb;
		//calculate scale 
		if ((hs * 0.70) / (Bottom->p.y - Top->p.y) < (ws * 0.90) / (Right->p.x - Left->p.x)){//if height scale is bigger then width
			scale = (hs * 0.70) / (Bottom->p.y - Top->p.y);//calculate scale
		}
		else{//if width scale is bigger
			scale =(ws * 0.90) / (Right->p.x - Left->p.x);//calculate scale
		}
		//calculate shifts
		xshift = (ws*0.5) - ((Right->p.x * scale + Left->p.x * scale)*0.5);//get shift to get triangle to center
		yshift = (hs*0.75) - (Bottom->p.y * scale);//get shift to get triangle 1/4 up

		tax = pointa.p.x * scale + xshift;//set x and y position for a, b and c
		tay = pointa.p.y * scale + yshift;
		tbx = pointb.p.x * scale + xshift;
		tby = pointb.p.y * scale + yshift;
		tcx = pointc.p.x * scale + xshift;
		tcy = pointc.p.y * scale + yshift;

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);//draw red for height
		SDL_RenderDrawLine(renderer, tcx * maxside, tay * maxside, tcx * maxside, tcy * maxside);//height line
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);//draw blue for height
		SDL_RenderDrawLine(renderer, tax * maxside, tay * maxside, tcx * maxside, tay * maxside);//base extension line
		SDL_RenderDrawLine(renderer, tbx * maxside, tby * maxside, tcx * maxside, tby * maxside);//base extansion line

		if (tay > tcy){//if c is above a
			if (tax > tbx){//if a is right of b
				lAx = tax + 0.02;//calculate point label A and B
				lAy = tay + 0.02;
				lBx = tbx - 0.02;
				lBy = tby + 0.02;
				lax = (tbx + tcx) / 2 - 0.03125 * (tby - tcy);//calculate line label a and b
				lay = (tby + tcy) / 2 + 0.03125 * (tbx - tcx);
				lbx = (tax + tcx) / 2 + 0.03125 * (tay - tcy);
				lby = (tay + tcy) / 2 - 0.03125 * (tax - tcx);
			}
			else{//if a is left of b
				lAx = tax - 0.02;//calculate point label A and B
				lAy = tay + 0.02;
				lBx = tbx + 0.02;
				lBy = tby + 0.02;
				lax = (tbx + tcx) / 2 + 0.03125 * (tby - tcy);//calculate line label a and b
				lay = (tby + tcy) / 2 - 0.03125 * (tbx - tcx);
				lbx = (tax + tcx) / 2 - 0.03125 * (tay - tcy);
				lby = (tay + tcy) / 2 + 0.03125 * (tax - tcx);
			}
			lCx = tcx;//calculate point label C
			lCy = tcy - 0.0225;
			lcx = (tax + tbx) / 2;//calculate line label c
			lcy = (tay + tby) / 2 + 0.02;

		}
		else{//if c is below a
			if (tax > tbx){//if a is right of b
				lAx = tax + 0.02;//calculate point label A and B
				lAy = tay - 0.02;
				lBx = tbx - 0.02;
				lBy = tby - 0.02;
				lax = (tbx + tcx) / 2 + 0.03125 * (tby - tcy);//calculate line label a and b
				lay = (tby + tcy) / 2 - 0.03125 * (tbx - tcx);
				lbx = (tax + tcx) / 2 - 0.03125 * (tay - tcy);
				lby = (tay + tcy) / 2 + 0.03125 * (tax - tcx);
			}
			else{//if a is left of b
				lAx = tax - 0.02;//calculate point label A and B
				lAy = tay - 0.02;
				lBx = tbx + 0.02;
				lBy = tby - 0.02;
				lax = (tbx + tcx) / 2 - 0.03125 * (tby - tcy);//calculate line label a and b
				lay = (tby + tcy) / 2 + 0.03125 * (tbx - tcx);
				lbx = (tax + tcx) / 2 + 0.03125 * (tay - tcy);
				lby = (tay + tcy) / 2 - 0.03125 * (tax - tcx);
			}
			lCx = tcx;//calculate point label C
			lCy = tcy + 0.0225;
			lcx = (tax + tbx) / 2;//calculate line label c
			lcy = (tay + tby) / 2 - 0.03;

		}
		lhx = tcx + 0.02;//calculate line label h
		lhy = (tay + tcy)/2 + 0.02;

		DrawText(Text_A, lAx, lAy, NULL, 1);//draw labels on the triangle
		DrawText(Text_B, lBx, lBy, NULL, 1);
		DrawText(Text_C, lCx, lCy, NULL, 1);
		DrawText(Text_a, lax, lay, NULL, 1);
		DrawText(Text_b, lbx, lby, NULL, 1);
		DrawText(Text_c, lcx, lcy, NULL, 1);
		DrawText(Text_h, lhx, lhy, NULL, 1);

		DrawTriangle(tax, tay, tbx, tby, tcx, tcy);//draw triangle




		SDL_RenderPresent(renderer);//present rendered
		if (difftime(time(NULL), lasttime) >= 1){//if 1 second passed
			fps = frame - lastframe;//set fps
			lasttime = time(NULL);//set new lasttime
			lastframe = frame;
#ifdef FPS
			printf("%d fps\n", fps);
#endif
		}
		frame++;//increment frame

		SDL_Delay(delay);//delay some time
	}

	exit(EXIT_SUCCESS);//if it somehow reaches here
}

















int EventFilter(void* userdata, SDL_Event* e){//event filter
	switch (e->type) {//for each event type
	case SDL_QUIT://quit everything
		exit(EXIT_SUCCESS);//exit
		return 0;//delete that event
		break;//get out

	case SDL_APP_TERMINATING://quit everything
		exit(EXIT_SUCCESS);//exit
		return 0;//delete that event
		break;//get out

	case SDL_APP_LOWMEMORY://if low in memory
		//can't do anything
		return 0;//delete that event
		break;//get out

	case SDL_APP_WILLENTERBACKGROUND://if entering background
		delay = DELAY_SLOW;//lower fps
		return 0;//delete that event
		break;//get out

	case SDL_APP_DIDENTERBACKGROUND://if in background
		//don't do anything
		return 0;//delete that event
		break;//get out

	case SDL_APP_WILLENTERFOREGROUND://if entering forgeound
		delay = DELAY;//higher fps
		return 0;//delete that event
		break;//get out

	case SDL_APP_DIDENTERFOREGROUND://if in forground
		//don't do anything
		return 0;//delete that event
		break;//get out

	default://for everything else
		return 1;//don't delete event
		break;//get out
	}
	return 1;//just in case something wiard happens
}




















void Quit(void){//quit everything
	SDL_DestroyTexture(Text_A);//destroy textures
	SDL_DestroyTexture(Text_B);
	SDL_DestroyTexture(Text_C);
	SDL_DestroyTexture(Text_a);
	SDL_DestroyTexture(Text_b);
	SDL_DestroyTexture(Text_c);
	SDL_DestroyTexture(Text_h);







	TTF_CloseFont(font_4);//close all fonts
	TTF_CloseFont(font_6);
	TTF_CloseFont(font_8);
	TTF_CloseFont(font_16);
	TTF_CloseFont(font_24);
	TTF_CloseFont(font_32);
	TTF_CloseFont(font_46);
	TTF_CloseFont(font_64);
	SDL_DestroyRenderer(renderer);//destroy renderer
	SDL_DestroyTexture(somethingwentwrong);//destroy something went wrong texture
	SDL_DestroyWindow(window);//destroy window
	IMG_Quit();//quit SDL_Image
	TTF_Quit();//quit SDL_TTF
	SDL_Quit();//quit SDL
	
	return;//exit function if it didn't exit for some reason
}

















void GetDisplay(void){//get display
	SDL_DisplayMode display;//display mode 
	if (window == NULL){
		//get desplay mode of the desktop if there is no window yet
		if (SDL_GetDesktopDisplayMode(0, &display) != 0) {
			printf("Could not get display mode: %s\n", SDL_GetError());//take care of errors
			exit(EXIT_FAILURE);//exit
		}
		width = WIDTH;//set display width and height
		height = HEIGHT;
	}
	else{
		//get desplay mode of the window if there is window
		SDL_GetWindowSize(window, &width, &height);//get width and height
	}

	if (width > height){//get smallest of 2 sides
		maxside = height;
	}
	else {
		maxside = width;
	}

	ws = (double)width / (double)maxside;//get width and height 
	hs = (double)height / (double)maxside;

	return;//exit function
}




















void Clicked(long int x, long int y){//x and y positions clicked
	MouseX = (double)(x) / maxside;//set x and y position of mouse from square normalised
	MouseY = (double)(y) / maxside;
	lmx = MouseX;//set last mouse click position 
	lmy = MouseY;
	return;//exit function
}



















void Draged(void){
	if (hypot(tcx - MouseX, tcy - MouseY) < CLICK_RANGE){//if mouse is within the range of c
		pointc.p.x -= (tcx - MouseX)/scale;//move x and y
		pointc.p.y -= (tcy - MouseY)/scale;

		pointa.p.known = 1;//only trust points
		Calculate();//recalculate triangle
	}
	else if (hypot(tbx - MouseX, tby - MouseY) < CLICK_RANGE){//if mouse is within the range of b
		pointb.p.x -= (tbx - MouseX) / scale;//move x

		pointa.p.known = 1;//only trust points
		Calculate();//recalculate triangle
	}
	else if (hypot(tax - MouseX, tay - MouseY) < CLICK_RANGE){//if mouse is within the range of a
		pointa.p.x -= (tax - MouseX) / scale;//move x
		
		pointa.p.known = 1;//only trust points
		Calculate();//recalculate triangle
	}
	else if (MouseY < 0.75 * hs){//if in right range
		double lastx = ix;
		double lasty = iy;
		ix = lastx - (lmx - MouseX); //set image x and y
		iy = lasty - (lmy - MouseY);
		lmx = MouseX;//set last mouse moved position 
		lmy = MouseY;
	}
	return;//exit function
}























SDL_Texture* GetTexture(const char *file){//make texture from this file
	char image[256] = RESOURCES;//folder path
	strcat(image, file);//append path
	SDL_Surface* surface = IMG_Load(image);//load surface
	if (surface == NULL){//if it could not be loaded
		printf("could not load image: %s\n", IMG_GetError());//error message
		return somethingwentwrong;//something went wrong
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);//get texture from loaded image
	if (texture == NULL){//if it could not be converted to texture
		printf("could not create texture: %s\n", SDL_GetError());//error message
		return somethingwentwrong;//something went wrong
	}
	SDL_FreeSurface(surface);//free surface
	return texture;//return texture
}

























TTF_Font* GetFont(const char *file, int size){//get font from file
	char fontfile[256] = RESOURCES;//folder path
	strcat(fontfile, FONTS);//append fonts path
	strcat(fontfile, file);//append path
	TTF_Font *font = TTF_OpenFont(fontfile, size);//get font
	if (font == NULL){//if it could not be loaded
		printf("could not load font: %s\n", TTF_GetError());//error message
		exit(EXIT_FAILURE);//exit
	}
	return font;//return font
}



























SDL_Texture* GetTextTexture(TTF_Font* font, const char* text, int r, int g, int b){//make texture from font text and rgb
	SDL_Color color;//set color
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = 255;
	char texturetext[256];//text to display for texture
	strcpy(texturetext, text);//copy text
	strtok(texturetext, "\n");//remove newline
	SDL_Surface* surface = TTF_RenderText_Blended(font, texturetext, color);//rendere text as surface
	if (surface == NULL){//if it could not be loaded
		printf("could not load text: %s\n", TTF_GetError());//error message
		exit(EXIT_FAILURE);//exit
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);//get texture from loaded image
	if (texture == NULL){//if it could not be converted to texture
		printf("could not create texture: %s\n", SDL_GetError());//error message
		exit(EXIT_FAILURE);//exit
	}
	SDL_FreeSurface(surface);//free surface
	return texture;//return created texture
}

























void Resize(void){//recalculate numbers related to size and load texts
	GetDisplay();//get display



	TTF_CloseFont(font_4);//close all fonts
	TTF_CloseFont(font_6);
	TTF_CloseFont(font_8);
	TTF_CloseFont(font_16);
	TTF_CloseFont(font_24);
	TTF_CloseFont(font_32);
	TTF_CloseFont(font_46);
	TTF_CloseFont(font_64);

	//load fonts on different sizes (1/n of maxside)
	font_4 = GetFont(FONT, maxside / 4);
	font_6 = GetFont(FONT, maxside / 6);
	font_8 = GetFont(FONT, maxside / 8);
	font_16 = GetFont(FONT, maxside / 16);
	font_24 = GetFont(FONT, maxside / 24);
	font_32 = GetFont(FONT, maxside / 32);
	font_46 = GetFont(FONT, maxside / 46);
	font_64 = GetFont(FONT, maxside / 64);


	if (renderer != NULL){//if there is a renderer
		SDL_DestroyTexture(Text_A);//destroy textures
		SDL_DestroyTexture(Text_B);
		SDL_DestroyTexture(Text_C);
		SDL_DestroyTexture(Text_a);
		SDL_DestroyTexture(Text_b);
		SDL_DestroyTexture(Text_c);
		SDL_DestroyTexture(Text_h);

		Text_A = GetTextTexture(font_32, "A", 0, 0, 0);//create new textures of the labels
		Text_B = GetTextTexture(font_32, "B", 0, 0, 0);
		Text_C = GetTextTexture(font_32, "C", 0, 0, 0);
		Text_a = GetTextTexture(font_32, "a", 0, 0, 0);
		Text_b = GetTextTexture(font_32, "b", 0, 0, 0);
		Text_c = GetTextTexture(font_32, "c", 0, 0, 0);
		Text_h = GetTextTexture(font_32, "h", 0, 0, 0);

	}
}























void DrawBase(void){//draw basic stuff
	if (Background != NULL){//if there is a background
		SDL_Rect dest;
		int w, h, access;//value to fill up
		long format;
		SDL_QueryTexture(Background, &format, &access, &w, &h);//get text box size
		dest.w = (int)w * scale;//set width and height
		dest.h = (int)h * scale;
		dest.x = (int)(ix * maxside);//set x and y
		dest.y = (int)(iy * maxside);

		dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
		dest.y = dest.y - dest.h / 2;

		SDL_RenderCopy(renderer, Background, NULL, &dest);//draw texture
	}
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
	SDL_Rect rect;//rectangle to draw
	rect.w = (int)(ws * maxside);//set width and height
	rect.h = (int)(hs * 0.25 * maxside);
	rect.x = 0;//set x and y
	rect.y = (int)(0.75 * maxside);
	SDL_RenderFillRect(renderer, &rect);//draw rectangle to mask bottom
}



























void DrawText(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int center){//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size
	if (texture == NULL) {//if texture passed dosen't exist
		texture = somethingwentwrong;//set texture to something went wrong
	}
	SDL_Rect dest;
	int w, h, access;//value to fill up
	long format;
	SDL_QueryTexture(texture, &format, &access, &w, &h);//get text box size
	dest.w = (int) w;//set width and height
	dest.h = (int) h;
	dest.x = (int)(x * maxside);//set x and y
	dest.y = (int)(y * maxside);

	if (center){
		dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
		dest.y = dest.y - dest.h / 2;
	}

	SDL_RenderCopy(renderer, texture, rect, &dest);//draw texture

}



























void DrawIMG(SDL_Texture *texture, double x, double y, SDL_Rect *rect, double w, double h, int center){//draw rect of texture at x and y position at scale from maxside normalised. Null rect for whole texture. set center to 1 to center to x and y
	if (texture == NULL) {//if texture passed dosen't exist
		texture = somethingwentwrong;//set texture to something went wrong
	}	SDL_Rect dest;
	dest.w = (int) (maxside * w);//set width and height
	dest.h = (int) (maxside * h);
	dest.x = (int)(x * maxside);//set x and y
	dest.y = (int)(y * maxside);

	if (center){
		dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
		dest.y = dest.y - dest.h / 2;
	}

	SDL_RenderCopy(renderer, texture, rect, &dest);//draw texture

}

















void DrawTriangle(double ax, double ay, double bx, double by, double cx, double cy){//draw triangle for those points with 1 as maxside
	DrawLine(ax, ay, bx, by);//draw line from a to b
	DrawLine(bx, by, cx, cy);//draw line from b to c
	DrawLine(cx, cy, ax, ay);//draw line from c to a
}
















void DrawLine(double ax, double ay, double bx, double by){//draw line for those points with 1 as maxside
	int i;//counter
	for (i = 0; i < hypot(ax - bx, ay - by)*maxside; i++){//for each dot on line
		double x = ax - ((ax - bx) / (hypot(ax - bx, ay - by)*maxside))*i;//set x
		double y = ay - ((ay - by) / (hypot(ax - bx, ay - by)*maxside))*i;//set y
		DrawIMG(pen, x, y, NULL, PEN_SIZE, PEN_SIZE, 1);//draw point centerd at full size
	}
}


















void GetKnown(void){//recalculate known points
	if (pointa.p.known || pointb.p.known || pointc.p.known){//if one of the points are known
		pointa.p.known = 1;//only trust points
		pointb.p.known = 1;
		pointc.p.known = 1;
		linea.l.known = 0;
		lineb.l.known = 0;
		linec.l.known = 0;
		anglea.a.known = 0;
		angleb.a.known = 0;
		anglec.a.known = 0;
		lineh.l.known = 0;
	}


}





















void Calculate(void){//calculate values in the triangle
	GetKnown();
	if (pointa.p.known && pointb.p.known && pointc.p.known){//if points are known
		linea.l.l = hypot(pointb.p.x - pointc.p.x, pointb.p.y - pointc.p.y);//get lines
		lineb.l.l = hypot(pointa.p.x - pointc.p.x, pointa.p.y - pointc.p.y);
		linec.l.l = hypot(pointb.p.x - pointa.p.x, pointb.p.y - pointa.p.y);
		lineh.l.l = fabs(pointc.p.y - pointa.p.y);//get height
	}

}












































































