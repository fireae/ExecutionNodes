#include <CImg.h>
#include <execution_nodes/execution_nodes.h>
#include <execution_nodes/node.h>
#include <execution_nodes/node_registry.h>
#include <iostream>
#include <random>

using namespace execution_nodes;
using namespace cimg_library;

class LoadImageNode : public Node {

public:
  LoadImageNode(const NodeDefinition &definition, const ConnectorPtr &connector)
      : Node(definition, connector),
        imageFilePath_(getSetting<std::string>("imageFilePath")) {}

  void execute() {
    CImg<unsigned char> image(imageFilePath_.c_str());
    setOutput("image", image);
  }

private:
  std::string imageFilePath_;
};

class GaussianBlurNode : public Node {

public:
  GaussianBlurNode(const NodeDefinition &definition,
                   const ConnectorPtr &connector)
      : execution_nodes::Node(definition, connector),
        sigma_(getSetting<float>("sigma")) {}

  void execute() {
    CImg<unsigned char> image;
    getInput("image", image);
    image.blur(sigma_, true, true);
    setOutput("blurred", image);
  }

private:
  float sigma_;
};

class DisplayImageNode : public Node {

public:
  DisplayImageNode(const NodeDefinition &definition,
                   const ConnectorPtr &connector)
      : Node(definition, connector),
        windowName_(getSetting<std::string>("windowName")) {}

  void execute() {
    CImg<unsigned char> image;
    getInput("image", image);
    CImgDisplay display(image, windowName_.c_str());

    while (!display.is_closed()) {
      display.wait();
    }
  }

private:
  std::string windowName_;
};

class BinarizeImageNode : public Node {

public:
  BinarizeImageNode(const NodeDefinition &definition,
                    const ConnectorPtr &connector)
      : Node(definition, connector), threshold_(getSetting<int>("threshold")) {}

  void execute() {

    CImg<unsigned char> image;
    getInput("image", image);

    // initialize a binary image version of img
    CImg<unsigned char> binary(image._width, image._height, 1, 1, 0);

    cimg_forXY(image, x, y) {
      int gray_value = image(x, y, 0, 0);
      if (gray_value > threshold_) {
        binary(x, y, 0, 0) = 255;
      } else {
        binary(x, y, 0, 0) = 0;
      }
    }
    setOutput("binary", binary);
  }

private:
  int threshold_;
};

static const NodeRegistry registry = {
    REGISTER(LoadImageNode),
    REGISTER(GaussianBlurNode),
    REGISTER(DisplayImageNode),
    REGISTER(BinarizeImageNode),
};

int main() {

  std::string filePath =
      "C:/Users/Sebastian/source/repos/ExecutionNodes/examples/img_proc.json";
  auto graphDef = loadGraphDefFromJson(filePath);
  Graph graph(graphDef, registry);
  graph.execute();

}