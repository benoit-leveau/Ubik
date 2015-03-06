// #include <iostream>
#include <vector>

#include "options.hpp"
#include "exrdriver.hpp"
#include "displaydriver.hpp"
#include "scene.hpp"
#include "renderer/renderer.hpp"


int main(int, char**argv)
{
    std::vector<OutputDriver *> output_list;

    // render options
    Options options(atoi(argv[1]), // width
                    atoi(argv[2]), // height
                    bool(atoi(argv[3])), // interactive
                    bool(atoi(argv[4])), // show_window
                    atoi(argv[5]), // bucketsize
                    bool(atoi(argv[6])), // spiral mode
                    true, // path tracer
                    true, // fixed sampling
                    1, // min_samples
                    atoi(argv[7]), // max_samples
                    atoi(argv[8]), // nbthreads
                    std::string(argv[9]));

    // read/construct scene 
    Scene *scene = new Scene();
    scene->frame = 0;

    // create display driver
    DisplayDriver *display = NULL;
    
    if (options.show_window || options.interactive)
    {
        display = new DisplayDriver(options.width, options.height);
        output_list.push_back(display);
    }

    if (!options.interactive)
    {
        // create EXR driver
        EXRDriver *exr_output = new EXRDriver(options.width, options.height, options.bucketsize, options.output_file);
        output_list.push_back(exr_output);
    }

    Renderer *renderer = Renderer::create_renderer(scene, options);
    renderer->run(output_list);

    // close any output
    for(auto &output : output_list)
        output->close();
    
    if (display/* && !options.interactive*/){
        // when there is a display driver, wait for user interaction to quit
        display->loop();
    }

    // Clean data
    for(auto &output : output_list)
        delete output;
    delete renderer;
    delete scene;
	
	return 0;
}

