#include <memory>

#include "options.hpp"
#include "scene.hpp"
#include "renderer.hpp"
#include "OptionParser.h"
#include "logging.hpp"
#include <signal.h>

void signal_handler(int arg)
{
    if( SIGINT == arg )
    {
        std::cout << "\nInterrupted by user!\n";
        ::exit( EXIT_SUCCESS );
    }
}

int main(int argc, char**argv)
{
    const std::string usage = "usage: %prog [OPTION]... [FILE]...";
    const std::string version = "%prog 1.0\nCopyright (C) 2015 Benoit Leveau\n";
    
    optparse::OptionParser parser = optparse::OptionParser()
        .usage(usage)
        .version(version);

    parser.add_option("--interactive").action("store_true").help("Interactive mode");

    optparse::OptionGroup group = optparse::OptionGroup(parser, "Global Options");
    group.add_option("--width").action("store").type("int").set_default(640).help("Output width. (default: %default)");
    group.add_option("--height").action("store").type("int").set_default(480).help("Output height. (default: %default)");
    group.add_option("-t", "--threads").action("store").type("int").set_default(0).help("Sets the number of threads. (default: %default - use all available cores)");
    group.add_option("-v", "--verbosity").action("store").type("int").set_default(3).help("Verbosity. (default: %default)");
    parser.add_option_group(group);
    
    optparse::OptionGroup group_tiled = optparse::OptionGroup(parser, "Tiled Renderer Options");
    group_tiled.add_option("--show-window").action("store_true").help("Show a render window.");
    group_tiled.add_option("--bucket-size").action("store").type("int").set_default(64).help("Sets the bucket size. default: %default");
    char const* const modes[] = { "topleft", "topright", "bottomleft", "bottomright", "spiral" };
    group_tiled.add_option("-m", "--mode").choices(&modes[0], &modes[5]).set_default("spiral").help("Sets the bucket order mode. default: %default");
    group_tiled.add_option("-o", "--output").action("store").type("string");
    
    group_tiled.add_option("--min-samples").action("store").type("int").set_default(0).help("Sets the minimum sampling. default: %default");
    group_tiled.add_option("--max-samples").action("store").type("int").set_default(1).help("Sets the maximum sampling. default: %default");
    parser.add_option_group(group_tiled);
    
    optparse::Values& parse_options = parser.parse_args(argc, argv);

    // setup ctrl-c/interruption/sigint handler
    ::signal(SIGINT, signal_handler);

    Logger logger(int(parse_options.get("verbosity")));
    logger.log_system_info(INFO);

    // render options
    Options options(int(parse_options.get("width")),
                    int(parse_options.get("height")),
                    bool(parse_options.get("interactive")),
                    bool(parse_options.get("show_window")),
                    int(parse_options.get("bucket_size")),
                    std::string(parse_options.get("mode")),
                    true, // path tracer
                    true, // fixed sampling,
                    int(parse_options.get("min_samples")),
                    int(parse_options.get("max_samples")),
                    int(parse_options.get("threads")),
                    std::string(parse_options.get("output")));

    // read/construct scene 
    auto scene = std::make_shared<Scene>();
    scene->frame = 0;
    
    auto renderer = Renderer::create_renderer(scene, options, logger);
    renderer->run();
    
	return 0;
}

