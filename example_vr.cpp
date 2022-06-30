#include <vsg/all.h>

#include <vsgvr/VRViewer.h>
#include <vsgvr/openvr/OpenVRContext.h>

int main(int argc, char **argv) {
  try
  {
    // set up defaults and read command line arguments to override them
    vsg::CommandLine arguments(&argc, argv);

    // set up vsg::Options to pass in filepaths and ReaderWriter's and other IO
    // realted options to use when reading and writing files.
    auto options = vsg::Options::create();
    arguments.read(options);

    vsg::Paths searchPaths = vsg::getEnvPaths("VSG_FILE_PATH");
    vsg::Path path = vsg::filePath(vsg::executableFilePath());
    vsg::Path dataPath = path;
    dataPath.append(VSGVR_INSTALL_REL_DATADIR);
    searchPaths.push_back(path);
    searchPaths.push_back(dataPath);
    vsg::Path filename = vsg::findFile("world.vsgt", searchPaths);
    if (argc > 1) {
        filename = arguments[1];
        if (!vsg::fileExists(filename)) {
            std::cerr << "Could not find file '" << filename << "'" << std::endl;
            return EXIT_FAILURE;
        }
    }
    if (arguments.errors()) {
        arguments.writeErrorMessages(std::cerr);
        return EXIT_FAILURE;
    }

    // load the scene graph
    vsg::ref_ptr<vsg::Group> vsg_scene =
        vsg::read_cast<vsg::Group>(filename, options);
    if (!vsg_scene) {
        std::cerr << "Could not read the scene graph" << std::endl;
        return EXIT_FAILURE;
    }

    // Initialise vr, and add nodes to the scene graph for each tracked device
    // TODO: If controllers are off when program starts they won't be added later
    auto vr = vsgvr::OpenVRContext::create();
    auto controllerNodeLeft = vsg::read_cast<vsg::Node>(vsg::findFile("controller.vsgt", searchPaths));
    auto controllerNodeRight = vsg::read_cast<vsg::Node>(vsg::findFile("controller2.vsgt", searchPaths));
    vsgvr::createDeviceNodes(vr, vsg_scene, controllerNodeLeft, controllerNodeRight);

    // Create the VR Viewer
    // This viewer creates its own desktop window internally along
    // with cameras, event handlers, etc
    // The provided window traits are a template - The viewer will configure
    // any additional settings as needed (such as disabling v-sync)
    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->windowTitle = "VSGVR Example";
    windowTraits->debugLayer = arguments.read({"--debug", "-d"});
    windowTraits->apiDumpLayer = arguments.read({"--api", "-a"});
    if (arguments.read({"--window", "-w"}, windowTraits->width,
                      windowTraits->height)) {
      windowTraits->fullscreen = false;
    }

    auto viewer = vsgvr::VRViewer::create(vr, windowTraits);

    // add close handler to respond the close window button and pressing escape
    viewer->addEventHandler(vsg::CloseHandler::create(viewer));

    // add the CommandGraph to render the scene
    auto commandGraphs = viewer->createCommandGraphsForView(vsg_scene);
    viewer->assignRecordAndSubmitTaskAndPresentation(commandGraphs);

    // compile all Vulkan objects and transfer image, vertex and primitive data to
    // GPU
    viewer->compile();

    // Render loop
    while (viewer->advanceToNextFrame()) {
      viewer->handleEvents();
      viewer->update();
      viewer->recordAndSubmit();
      viewer->present();
    }

    return EXIT_SUCCESS;
  }
  catch( const vsg::Exception& e )
  {
    std::cerr << "VSG Exception: " << e.message << std::endl;
    return EXIT_FAILURE;
  }
}
