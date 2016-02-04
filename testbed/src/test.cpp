#include <SDL.h>
#include <iostream>
#include <ctime>
#include "test.h"
#include "renderer.h"

void ShowHelp()
{
    std::cout << "./a.out <test> [obj_file]" << std::endl;
    std::cout << "tests:" << std::endl;
    TestMap * testMap = Test::GetTestMap();
    for(TestMap::iterator itr = testMap->begin(); itr != testMap->end(); ++itr)
    {
        std::cout << " " << itr->first << std::endl;
    }
}

int main(int argc, char* argv[])
{
	if(argc <= 1)
	{
        ShowHelp();
		return 1;
	}
    
    TestMap * testMap = Test::GetTestMap();
    std::string key(argv[1]);
    
    TestMap::iterator search = testMap->find(key);
    if(search == testMap->end())
    {
        ShowHelp();
        return 1;
    }
    
    if(search->second->Init(argc, argv) == false)
    {
        return 0;
    }
    
    SDL_Window* displayWindow;
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Testbed", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GL_CreateContext(displayWindow);
    
    Renderer ARenderer(800,600);

    SDL_Event event;    
    bool run = true;
    const double fps = 60.f;
    const float delta = 1.f/fps;
    double timeBank = delta;
    clock_t start,end;
    start = clock();
    
    while (run)
    {
        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                run = false;
            }
        }
                
        end = clock();
        float seconds = (float)(end - start) / CLOCKS_PER_SEC;
        timeBank += seconds;
        start = end;
        
        if(timeBank >= delta)
        {
            while(timeBank >= delta)
            {
                timeBank -= delta;
                ARenderer.Clear();
                run &= search->second->Tick(ARenderer, (float)delta);
                ARenderer.Flush();
            }

            SDL_GL_SwapWindow(displayWindow);
            SDL_Delay((unsigned int) delta);
        }
    }
    
    SDL_Quit();
    
    
	return 1;
}
