#include <psp2/kernel/processmgr.h>
#include <SDL3/SDL.h>

//Screen dimension constants
enum {
  SCREEN_WIDTH  = 960,
  SCREEN_HEIGHT = 544
};

SDL_Window    * gWindow   = NULL;
SDL_Renderer  * gRenderer = NULL;

SDL_FRect fillRect = { SCREEN_WIDTH  / 4,
		      SCREEN_HEIGHT / 4,
		      SCREEN_WIDTH  / 2,
		      SCREEN_HEIGHT / 2
};



int main(int argc, char *argv[])
{
  if( !SDL_Init( SDL_INIT_VIDEO ) )
      return -1;

  if ((gWindow = SDL_CreateWindow( "RedRectangle", SCREEN_WIDTH, SCREEN_HEIGHT, 0)) == NULL)
    return -1;

  if ((gRenderer = SDL_CreateRenderer( gWindow, 0)) == NULL)
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
  return 0;
}
