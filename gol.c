#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <time.h>

/*=======================*/
/*DEFINE YOUR PARAMETERS HERE*/
#define BLOCK_NUM 42 //chose a number that HEIGHT%BLOCK_NUM==0 and add 2
#define DENCITY 30 /*percentage scale from 1 to 100*/
/*=======================*/

#define HEIGHT 1000 
#define WIDTH 1000	
#define BLOCK_TOTAL BLOCK_NUM*BLOCK_NUM
#define BLOCK_WIDTH WIDTH/(BLOCK_NUM-2)  
#define BLOCK_HEIGHT HEIGHT/(BLOCK_NUM-2)  

typedef struct{
	int x,y;
	bool alive;
}block;

block currentState[BLOCK_TOTAL]={0}; 
block nextState[BLOCK_TOTAL]={0};

void Initialise_Fields(block *first, block *second)
{
	for(int i=1; i<BLOCK_NUM-1; i++)
	{
		for(int k=1; k<BLOCK_NUM-1; k++)
		{
			(first+(k+i*BLOCK_NUM))->x=(i-1)*(BLOCK_WIDTH);
			(first+(k+i*BLOCK_NUM))->y=(k-1)*(BLOCK_HEIGHT);
			if(rand()%100<DENCITY) (first+(k+i*BLOCK_NUM))->alive=true;
			(second+(k+i*BLOCK_NUM))->x=(i-1)*(BLOCK_WIDTH);
			(second+(k+i*BLOCK_NUM))->y=(k-1)*(BLOCK_HEIGHT);
			if(rand()%100<DENCITY) (second+(k+i*BLOCK_NUM))->alive=true;
			
		}
	}
}

#define CURR_INDEX 	(curr+(k+i*BLOCK_NUM))
#define NEXT_INDEX	(next+(k+i*BLOCK_NUM))
/*SUMMING THE NUMBER OF NEIGHBOURS AROUND THE 3X3 BLOCK*/
#define NEIGHBOURS (curr+(k-1+(i+1)*BLOCK_NUM))->alive    \
	+ (curr+(k+1+i*BLOCK_NUM))->alive  	  			  	  \
	+ (curr+(k-1+i*BLOCK_NUM))->alive        			  \
	+ (curr+(k+(i+1)*BLOCK_NUM))->alive     			  \
	+ (curr+(k+(i-1)*BLOCK_NUM))->alive     			  \
	+ (curr+(k+1+(i+1)*BLOCK_NUM))->alive   			  \
	+ (curr+(k+1+(i-1)*BLOCK_NUM))->alive   			  \
	+ (curr+(k-1+(i-1)*BLOCK_NUM))->alive

#define CASE1 (Neighbours==3 || Neighbours==2) && CURR_INDEX->alive==true
#define CASE2 Neighbours==3 && CURR_INDEX->alive==false

void update_and_render(block *curr, block *next, SDL_Renderer *renderer)
{
	SDL_Rect dst;
	dst.h=BLOCK_HEIGHT;
	dst.w=BLOCK_WIDTH;
	for(int i=1; i<BLOCK_NUM-1; i++)
	{	
		for(int k=1; k<BLOCK_NUM-1; k++)
		{
			int Neighbours = NEIGHBOURS;
			dst.x=NEXT_INDEX->x;
			dst.y=NEXT_INDEX->y;

			if((CASE1) || (CASE2)) 
			{
				NEXT_INDEX->alive=true;
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
			else 
			{
				NEXT_INDEX->alive=false;
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
			}
			SDL_RenderFillRect(renderer, &dst);	
		}
	}
	SDL_RenderPresent(renderer);
}

void render_State(block *next, SDL_Renderer *renderer)
{
	SDL_Rect dst;
	dst.w=BLOCK_WIDTH;
	dst.h=BLOCK_HEIGHT;
	
	for(int i=1; i<BLOCK_NUM-1; i++)
	{
		for(int k=1; k<BLOCK_NUM-1; k++)
		{
			dst.x=NEXT_INDEX->x;
			dst.y=NEXT_INDEX->y;
			if(NEXT_INDEX->alive==true)
			{
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderFillRect(renderer, &dst);
			} 
			else 
			{
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
				SDL_RenderFillRect(renderer, &dst);
			}
			SDL_RenderCopy(renderer, NULL, NULL, &dst);
		}
	}
	SDL_RenderPresent(renderer);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	return;
}

int getRefreshRate(SDL_Window *window) 
{
	int displayIndex = SDL_GetWindowDisplayIndex(window);                                      
    SDL_DisplayMode mode; 
    SDL_GetDisplayMode(displayIndex, 0, &mode);
    return mode.refresh_rate;
}     

void clear_Renderer(SDL_Renderer *renderer)
{
	SDL_RenderClear(renderer);
}

int main()
{
	if(SDL_Init(SDL_INIT_EVERYTHING)<0)
	{
		fprintf(stderr, "Error Could Not Initiate SDL: %s\n", SDL_GetError());
		exit(1);
	}

	#define CENTERED SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED	

	SDL_Window *window=SDL_CreateWindow("Game Of Life", CENTERED, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
	if(window==NULL)
	{
		fprintf(stderr, "Error Could Not Create Window %s\n", SDL_GetError());
		exit(1);
	}

	SDL_Renderer *renderer=SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(renderer==NULL)
	{
		fprintf(stderr, "Error Could Not Create Renderer %s\n", SDL_GetError());
		exit(1);
	}

	srand(time(NULL));
	block *curr = &currentState[0];
	block *next = &nextState[0];
	Initialise_Fields(curr, next);
	
	Uint32 frameCapTime = SDL_GetTicks(),timer = SDL_GetTicks();
	int mouseX, mouseY;
	bool golRunning = true, pause = false;

	while(golRunning)
	{
		SDL_Event event;
		SDL_GetMouseState(&mouseX, &mouseY);
		while(SDL_PollEvent(&event))
		{
			switch(event.type){
				case SDL_QUIT:
					golRunning=false;
				case SDL_MOUSEBUTTONDOWN:
				  //printf("mouse is at %d %d\n", mouseX/(WIDTH/(BLOCK_NUM-2)+1, mouseY/(WIDTH/(BLOCK_NUM-2)+1);
					SDL_PumpEvents();
					mouseY=mouseY/(HEIGHT/(BLOCK_NUM-2));
					mouseX=mouseX/(WIDTH/(BLOCK_NUM-2));
					(curr+(mouseY+1+(mouseX+1)*BLOCK_NUM))->alive=true;	
					render_State(curr, renderer);
					clear_Renderer(renderer);
				case SDL_KEYDOWN:
					/*pausing the game by pressing Space*/
					if(event.key.keysym.scancode==SDL_SCANCODE_SPACE)
					{
					
						if(!pause) 
						{
							pause=true;
							printf("GOL Paused\n");
						}
						else 
						{
							pause=false;
							printf("GOL Unpaused\n");
						}
						break;
					}	
			}
			break;	
		}

		if(SDL_GetTicks()-timer>100 && !pause)
		{
			update_and_render(curr, next, renderer);
			clear_Renderer(renderer);	
			timer=SDL_GetTicks();
			/*swap the two arrays*/
			block *Swap_Helper=curr;
			curr = next;
			next = Swap_Helper;
		}

		//delay to reduce cpu usage
		SDL_Delay(1);

		if(frameCapTime<1000/getRefreshRate(window))
		{
			SDL_Delay(1000/getRefreshRate(window)-frameCapTime);
			frameCapTime=SDL_GetTicks();
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0;
}
