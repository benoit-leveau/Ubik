#include <memory>

#include "options.hpp"
#include "scene.hpp"
#include "renderer.hpp"
#include "integrator.hpp"
#include "sampler.hpp"
#include "OptionParser.h"
#include "logging.hpp"
#include "signal.hpp"
#include "adaptativesampler.hpp"
#include "fixedsampler.hpp"

int main(int argc, char**argv)
{
    const std::string usage = "usage: %prog [OPTION]... [FILE]...";
    const std::string version = "%prog 1.0\nCopyright (C) 2015 Benoit Leveau\n";

    optparse::OptionParser parser = optparse::OptionParser()
        .usage(usage)
        .version(version);

    optparse::OptionGroup group = optparse::OptionGroup(parser, "Global Options");
    group.add_option("--width").action("store").type("int").set_default(640).help("Output width. (default: %default)");
    group.add_option("--height").action("store").type("int").set_default(480).help("Output height. (default: %default)");
    group.add_option("-t", "--threads").action("store").type("int").set_default(0).help("Sets the number of threads. (default: %default - use all available cores)");
    group.add_option("-v", "--verbosity").action("store").type("int").set_default(3).help("Verbosity. (default: %default)");
    parser.add_option_group(group);

    // Add choice for Sampler + options for each sampler
    char const* const samplers[] = { "fixed", "adaptative" };
    parser.add_option("--sampler").choices(&samplers[0], &samplers[2]).set_default(samplers[0]).help("Sets the sampler algorithm. default: %default");
    FixedSampler::createOptions(parser);
    AdaptativeSampler::createOptions(parser);

    // Add choice for Integrator + options for each integrator
    char const* const integrators[] = { "smallpt", "pathtracer", "image" };
    parser.add_option("--integrator").choices(&integrators[0], &integrators[3]).set_default(integrators[0]).help("Sets the integrator algorithm. default: %default");
    // no options
    
    // Add choice for Renderer + options for each renderer
    char const* const renderers[] = { "tiled", "interactive" };
    parser.add_option("--renderer").choices(&renderers[0], &renderers[2]).set_default(renderers[0]).help("Sets the renderer algorithm. default: %default");
    optparse::OptionGroup group_tiled = optparse::OptionGroup(parser, "Tiled Renderer Options");
    group_tiled.add_option("--show-window").action("store_true").help("Show a render window.");
    group_tiled.add_option("--bucket-size").action("store").type("int").set_default(64).help("Sets the bucket size. default: %default");
    char const* const modes[] = { "topleft", "topright", "bottomleft", "bottomright", "spiral" };
    group_tiled.add_option("-m", "--mode").choices(&modes[0], &modes[5]).set_default("spiral").help("Sets the bucket order mode. default: %default");
    group_tiled.add_option("-o", "--output").action("store").type("string");
    parser.add_option_group(group_tiled);

    optparse::Values& parse_options = parser.parse_args(argc, argv);

    // setup ctrl-c/interruption/sigint handler
    install_handler();

    Logger logger(int(parse_options.get("verbosity")));
    logger.log_system_info(INFO);

    // render options
    Options options(int(parse_options.get("width")),
                    int(parse_options.get("height")),
                    std::string(parse_options.get("sampler")),
                    std::string(parse_options.get("integrator")),
                    std::string(parse_options.get("renderer")),
                    bool(parse_options.get("show_window")),
                    int(parse_options.get("bucket_size")),
                    std::string(parse_options.get("mode")),
                    size_t(parse_options.get("min_samples")),
                    size_t(parse_options.get("max_samples")),
                    size_t(parse_options.get("samples")),
                    double(parse_options.get("threshold")),
                    int(parse_options.get("threads")),
                    std::string(parse_options.get("output")));

    // read/construct scene
    auto scene = std::make_shared<Scene>(options, logger);
    scene->load();

    // construct integrator
    auto integrator = Integrator::create_integrator(options, scene, logger);

    // construct sampler
    std::shared_ptr<Sampler> sampler = nullptr;
    if (options.renderer != "interactive")
        sampler = Sampler::create_sampler(options, integrator, logger);

    auto renderer = Renderer::create_renderer(scene, integrator, sampler, options, logger);
    renderer->run();

	return 0;
}

