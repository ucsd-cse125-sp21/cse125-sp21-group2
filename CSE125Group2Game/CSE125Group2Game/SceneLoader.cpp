#include "SceneLoader.h"

#include "Utils.h"

/**
 * Constructs and populates the scene graph from the scene file passed in.
 *
 * This constructor should only be called once.
 *
 * @param fileName The path to the scene file to be loaded.
 */
SceneGraph SceneLoader::LoadFromFile(const std::string& fileName,
                                     MeshLoader& loader,
                                     TextureLoader& tloader) {
  // Open file
  std::ifstream file(fileName);

  if (!file) {
    CRASH("Failed to open scene file " + fileName + "!");
  }

  // Create json and populate it from file
  nlohmann::json j;
  file >> j;

  /* Required: Name
   * Defaults: parent_name = root
               pos & rot = 0
               scale = 1
               obj_file = empty object
  */

  SceneGraph graph;

  int count = 0;
  for (nlohmann::json& object : j["scene"]) {
    count++;

    // Get the name of the object
    nlohmann::json& name = object["name"];

    // If the name is null, no name was provided
    if (name.is_null()) {
      std::cout << "Error: object " << count << " is missing a name."
                << std::endl;
      continue;
    } else if (graph.mNameMapping.find(name) != graph.mNameMapping.end()) {
      // If the name already exists, throw an error
      std::cout << "Error: object " << count
                << " has a duplicate name: " << name << std::endl;
      continue;
    }

    // Only set up scene graph if this is the client code
    // Get the name of the parent node
    nlohmann::json& parentName = object["parent_name"];

    SceneGraphNode* parentNode = graph.getRoot();

    // Set the parent if defined
    if (!parentName.is_null() && parentName != "root") {
      std::cout << "Here" << std::endl;
      auto value = graph.getByName(parentName);

      // If the parent node hasn't been added yet, throw and error
      if (value == nullptr) {
        std::cout << "Error: parent of " << name << " does not exist."
                  << std::endl;
        continue;
      }

      parentNode = value;
    }

    // Assign the xPos
    nlohmann::json& xPosS = object["xPos"];
    float xPos = 0;
    if (!xPosS.is_null()) xPos = xPosS;

    // Assign the yPos
    nlohmann::json& yPosS = object["yPos"];
    float yPos = 0;
    if (!yPosS.is_null()) yPos = yPosS;

    // Assign the zPos
    nlohmann::json& zPosS = object["zPos"];
    float zPos = 0;
    if (!zPosS.is_null()) zPos = zPosS;

    // Assign the xRot
    nlohmann::json& xRotS = object["xRot"];
    float xRot = 0;
    if (!xRotS.is_null()) xRot = xRotS;

    // Assign the yRot
    nlohmann::json& yRotS = object["yRot"];
    float yRot = 0;
    if (!yRotS.is_null()) yRot = yRotS;

    // Assign the zRot
    nlohmann::json& zRotS = object["zRot"];
    float zRot = 0;
    if (!zRotS.is_null()) zRot = zRotS;

    // Assign the xScale
    nlohmann::json& xScaleS = object["xScale"];
    float xScale = 1;
    if (!xScaleS.is_null()) xScale = xScaleS;

    // Assign the yScale
    nlohmann::json& yScaleS = object["yScale"];
    float yScale = 1;
    if (!yScaleS.is_null()) yScale = yScaleS;

    // Assign the zScale
    nlohmann::json& zScaleS = object["zScale"];
    float zScale = 1;
    if (!zScaleS.is_null()) zScale = zScaleS;

    // Assign the bbRight
    nlohmann::json& bbLengthS = object["bb_length"];
    float bbLength = 0;
    if (!bbLengthS.is_null()) bbLength = bbLengthS;

    // Assign the bbTop
    nlohmann::json& bbHeightS = object["bb_height"];
    float bbHeight = 0;
    if (!bbHeightS.is_null()) bbHeight = bbHeightS;

    // Assign the bbBottom
    nlohmann::json& bbWidthS = object["bb_width"];
    float bbWidth = 0;
    if (!bbWidthS.is_null()) bbWidth = bbWidthS;

    Transform* transform =
        new Transform(glm::vec3(xPos, yPos, zPos), glm::vec3(xRot, yRot, zRot),
                      glm::vec3(xScale, yScale, zScale),
                      glm::vec3(bbLength, bbHeight, bbWidth));

    // Only assign mesh if this is client code
    // Assign the mesh
    nlohmann::json& objectFile = object["obj_file"];
    std::string test = objectFile;
    Model* mesh = NULL;

    // Object is a cube
    if (!objectFile.is_null() &&
        (objectFile == "cube" || objectFile == "Cube")) {
      mesh = Model::Cube(transform, loader);
    } else if (!objectFile.is_null()) {
      // Object is a obj file
      mesh = new Model(ASSET(test), transform, loader, tloader);
    }

    GameObject* obj = new GameObject(transform, ((std::string)name).data(), 1);

    // add node into the graph
    SceneGraphNode* node = new SceneGraphNode(parentNode, obj, mesh);
    graph.mNameMapping.insert(std::make_pair(name, node));
  }

  return graph;
}
