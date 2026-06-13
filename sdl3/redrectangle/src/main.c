#include <psp2/kernel/processmgr.h>
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) 
{
  if( !SDL_Init( SDL_INIT_VIDEO ) )
    return SDL_APP_FAILURE;

  if ((gWindow = SDL_CreateWindow( "RedRectangle", SCREEN_WIDTH, SCREEN_HEIGHT, 0)) == NULL)
    return SDL_APP_FAILURE;
  
  if ((gRenderer = SDL_CreateRenderer( gWindow, 0)) == NULL)
      return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
  SDL_SetRenderDrawColor( gRenderer, 255,0,0,255);
  SDL_RenderFillRect( gRenderer, &fillRect );
  SDL_RenderPresent( gRenderer );
  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  /* SDL will clean up the window/renderer for us. */
  gWindow = NULL;
  gRenderer = NULL;
}
