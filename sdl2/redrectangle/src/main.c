#include <psp2/kernel/processmgr.h>
#include <SDL2/SDL.h>

//Screen dimension constants
enum {
  SCREEN_WIDTH  = 960,
  SCREEN_HEIGHT = 544
};

SDL_Window    * gWindow   = NULL;
SDL_Renderer  * gRenderer = NULL;

SDL_Rect fillRect = { SCREEN_WIDTH  / 4, 
		      SCREEN_HEIGHT / 4, 
		      SCREEN_WIDTH  / 2, 
		      SCREEN_HEIGHT / 2 
};



int main(int argc, char *argv[]) 
{
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
      return -1;

  if ((gWindow = SDL_CreateWindow( "RedRectangle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN)) == NULL)
    return -1;

  if ((gRenderer = SDL_CreateRenderer( gWindow, -1, 0)) == NULL)
      return -1;

  SDL_SetRenderDrawColor( gRenderer, 255,0,0,255);
  SDL_RenderFillRect( gRenderer, &fillRect );
  SDL_RenderPresent( gRenderer );
  SDL_Delay(4000);
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
  gRenderer = NULL;
  
  SDL_Quit();
  sceKernelExitProcess(0);
  return 0;
}
