#pragma once

/*
Copyright(c) 2021 Gareth Francis

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <vsg/viewer/Viewer.h>

#include <vsgvr/VR.h>

namespace vsgvr {
class VSGVR_DECLSPEC VRViewer final : public vsg::Inherit<vsg::Viewer, VRViewer> {
public:
  VRViewer(vsg::ref_ptr<vsgvr::VRContext> ctx,
           vsg::ref_ptr<vsg::WindowTraits> windowTraits);
  VRViewer(const VRViewer &) = delete;
  Viewer &operator=(const VRViewer &) = delete;

  // TODO: Could make base poll virtual, or use a Window subclass for events?
  // bool pollVREvents(bool dicardPreviewEvents = true);
  // void handleEvents() override;

  void update() override;
  void present() override;

  std::vector<vsg::ref_ptr<vsg::CommandGraph>>
  createCommandGraphsForView(vsg::ref_ptr<vsg::Node> vsg_scene);

private:
  void addWindow(vsg::ref_ptr<vsg::Window>) override;
  void createDesktopWindow(vsg::ref_ptr<vsg::WindowTraits> windowTraits);

  const VkFormat hmdImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
  std::vector<vsgvr::HMDImage> hmdImages;
  std::vector<vsg::ref_ptr<vsg::View>> views;
  std::vector<vsg::ref_ptr<vsg::Camera>> m_hmdCameras;
  vsg::ref_ptr<vsg::CommandGraph> hmdCommandGraph;

  vsg::ref_ptr<vsgvr::VRContext> m_ctx;
  vsg::ref_ptr<vsg::Window> m_desktopWindow;
  vsg::ref_ptr<vsg::Camera> m_desktopCamera;
  std::list<std::string> vrRequiredInstanceExtensions;
  std::list<std::string> vrRequiredDeviceExtensions;

  vsg::ref_ptr<vsg::Camera> createCameraForScene(vsg::ref_ptr<vsg::Node> scene,
                                                 const VkExtent2D &extent);
  vsg::ref_ptr<vsg::RenderGraph>
  createHmdRenderGraph(vsg::Device *device, vsg::Context &context,
                       const VkExtent2D &extent, HMDImage &img,
                       VkClearColorValue &clearColour);
  void submitVRFrames();
};
} // namespace vsgvr
