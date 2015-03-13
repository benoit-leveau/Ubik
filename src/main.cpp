#include <memory>

#include "options.hpp"
#include "scene.hpp"
#include "renderer.hpp"
#include "OptionParser.h"


int main(int argc, char**argv)
{
    const std::string usage = "usage: %prog [OPTION]... DIR [FILE]...";
    const std::string version = "%prog 1.0\nCopyright (C) 2015 Benoit Leveau\n";
    
    optparse::OptionParser parser = optparse::OptionParser()
        .usage(usage)
        .version(version);

    parser.add_option("--interactive").action("store_true").help("Interactive mode");

    optparse::OptionGroup group = optparse::OptionGroup(parser, "Global Options", "Sets the image size.");
    group.add_option("-w", "--width").action("store").type("int").set_default(640).help("default: %default");
    group.add_option("-h", "--height").action("store").type("int").set_default(480).help("default: %default");
    group.add_option("-t", "--threads").action("store").type("int").set_default(8).help("Sets the number of threads. default: %default");
    parser.add_option_group(group);
    
    optparse::OptionGroup group_tiled = optparse::OptionGroup(parser, "Tiled Renderer Options", "Options for the Tiled Renderer mode.");
    group_tiled.add_option("--show-window").action("store_true").help("Show a render window.");
    group_tiled.add_option("--bucket-size").action("int").set_default(64).help("Sets the bucket size. Defaults to %default.");
    char const* const modes[] = { "spiral", "topleft" };
    group_tiled.add_option("-m", "--mode").choices(&modes[0], &modes[1]).set_default("spiral").help("Sets the image mode.");
    group_tiled.add_option("-o", "--output").action("store").type("string");
    
    group_tiled.add_option("--min-samples").action("store").type("int").set_default(0).help("Sets the minimum sampling. default: %default");
    group_tiled.add_option("--max-samples").action("store").type("int").set_default(1).help("Sets the maximum sampling. default: %default");
    parser.add_option_group(group_tiled);
    
    optparse::Values& parse_options = parser.parse_args(argc, argv);

    std::cout << "width: " << int(parse_options.get("width")) << std::endl;
    std::cout << "height: " << int(parse_options.get("height")) << std::endl;
    std::cout << "interactive: " << bool(parse_options.get("interactive")) << std::endl;
    std::cout << "bucket size: " << int(parse_options.get("bucket_size")) << std::endl;

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
                    int(parse_options.get("nb_threads")),
                    std::string(parse_options.get("output")));

    // read/construct scene 
    auto scene = std::make_shared<Scene>();
    scene->frame = 0;
    
    auto renderer = Renderer::create_renderer(scene, options);
    renderer->run();
    
	return 0;
}

