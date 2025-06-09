#include "../include/algorithm/ChanConvexHull.hpp"
#include "../include/algorithm/GrahamConvexHull.hpp"
#include "../include/algorithm/JarvisConvexHull.hpp"
#include "../include/algorithm/PointSet.hpp"
#include "../include/graphics/Button.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

const sf::Color BACKGROUND_COLOR(51, 52, 70);
const sf::Color BORDER_COLOR(40, 42, 54);
const sf::Color REGULAR_POINT_COLOR(173, 216, 230);
const sf::Color HULL_POINT_COLOR(255, 184, 77);
const sf::Color START_POINT_COLOR(50, 205, 50);
const sf::Color HULL_LINE_COLOR(140, 205, 235);
const sf::Color MESH_COLOR(100, 149, 237);
const sf::Color TEXT_COLOR(248, 248, 242);

class ConvexHullVisualizer {
public:
  ConvexHullVisualizer()
      : nextButton("New points", font, {20, SF_HEIGHT - 70}, {200, 50}),
        toggleMeshButton("Toggle mesh", font, {240, SF_HEIGHT - 70},
                         {200, 50}) {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(SF_WIDTH, SF_HEIGHT), "Convex Hull",
                  sf::Style::Default, settings);
    window.setFramerateLimit(60);
    paddingArea.setSize(
        {SF_WIDTH - 2 * SF_PADDING, SF_HEIGHT - 2 * SF_PADDING});
    paddingArea.setPosition(SF_PADDING, SF_PADDING);
    paddingArea.setFillColor(sf::Color::Transparent);
    paddingArea.setOutlineColor(BORDER_COLOR);
    paddingArea.setOutlineThickness(2);
    if (!font.loadFromFile("../res/AgaveNerdFontMono-Bold.ttf")) {
      throw std::runtime_error("Failed to load font!");
    }
  }

  void run() {
    if (!initialize())
      return;
    mainLoop();
  }

private:
  sf::RenderWindow window;
  sf::Font font;
  Button nextButton;
  Button toggleMeshButton;
  PointSet pointSet;
  std::unique_ptr<ConvexHullBase> hullAlgorithm;
  std::vector<sf::CircleShape> points;
  std::vector<sf::Text> pointLabels;
  std::vector<sf::VertexArray> partitionMeshes;
  sf::VertexArray hullLines;
  sf::RectangleShape paddingArea;
  bool showMesh = false;
  bool showHull = true;

  bool initialize() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(SF_WIDTH, SF_HEIGHT), "Convex Hull - SFML",
                  sf::Style::Default, settings);
    window.setFramerateLimit(60);

    if (!font.loadFromFile("../res/AgaveNerdFontMono-Bold.ttf")) {
      std::cerr << "Failed to load font!" << std::endl;
      return false;
    }

    setupUI();
    generateNewSet();
    return true;
  }

  void setupUI() {
    nextButton = {"New points", font, {20, SF_HEIGHT - 70}, {200, 50}};
    toggleMeshButton = {"Toggle mesh", font, {240, SF_HEIGHT - 70}, {200, 50}};

    paddingArea.setSize(
        {SF_WIDTH - 2 * SF_PADDING, SF_HEIGHT - 2 * SF_PADDING});
    paddingArea.setPosition(SF_PADDING, SF_PADDING);
    paddingArea.setFillColor(sf::Color::Transparent);
    paddingArea.setOutlineColor(BORDER_COLOR);
    paddingArea.setOutlineThickness(2);
  }

  void generateNewSet() {
    // pointSet.generate_points(6);
    // TODO : DEBUG
    // pointSet = PointSet({{736.28, 153.004},
    //                      {1782.49, 388.164},
    //                      {992.796, 498.01},
    //                      {899.033, 841.504},
    //                      {1181.24, 803.516},
    //                      {833.415, 418.588}});
    // pointSet = PointSet({{604.808, 881.046},
    //                      {182.465, 739.731},
    //                      {101.264, 896.107},
    //                      {1560.1, 553.974},
    //                      {1538, 666.174},
    //                      {703.238, 443.074}});
    // pointSet =
    //     PointSet({{240.452, 259.459}, {509.175, 269.044}, {824.281, 259.915},
    //               {257.724, 678.07},  {944.414, 409.202}, {281.558, 842.998},
    //               {1417.59, 216.899}, {1469.59, 852.766}, {1211.91, 796.047},
    //               {941.111, 716.141}, {1561.49, 249.277}, {1526.12, 296.132},
    //               {1332.52, 790.75},  {1269.63, 899.828}, {118.543, 622.576},
    //               {759.669, 692.79},  {1411.79, 667.195}, {862.367, 831.84},
    //               {246.334, 677.638}, {1796.67, 684.981}, {1298.75, 826.136},
    //               {656.631, 283.319}, {1437.41, 732.968}, {1422.6, 404.89},
    //               {764.51, 167.628},  {970.997, 752.238}, {425.118, 862.272},
    //               {1480.36, 667.363}, {178.496, 128.064}, {881.097, 140.504},
    //               {1701.36, 171.9},   {1368.94, 875.067}, {557.874, 717.462},
    //               {685.26, 891.958}});
    pointSet.generate_points();
    initializeHullAlgorithm();
    createVisualElements();
  }

  void initializeHullAlgorithm() {
    hullAlgorithm = std::make_unique<ChanConvexHull>(pointSet.get_set());
    // hullAlgorithm = std::make_unique<GrahamConvexHull>(pointSet.get_set());
    // hullAlgorithm = std::make_unique<JarvisConvexHull>(pointSet.get_set());
  }

  void createVisualElements() {
    createPoints();
    createPartitionMeshes();
    createHullLines();
  }

  void createPoints() {
    points.clear();
    size_t i = 0;
    for (const auto &point : pointSet.get_set()) {
      sf::CircleShape circle(SF_POINT_RADIUS);
      circle.setPointCount(32);
      if (point == hullAlgorithm->getHull()[0]) {
        circle.setFillColor(START_POINT_COLOR);
      } else {
        circle.setFillColor(REGULAR_POINT_COLOR);
      }
      circle.setOutlineColor(sf::Color::Black);
      circle.setOutlineThickness(2);
      circle.setPosition(point.x_ - SF_POINT_RADIUS,
                         point.y_ - SF_POINT_RADIUS);
      points.push_back(std::move(circle));
    }
  }

  void createPartitionMeshes() {
    if (auto chanHull = dynamic_cast<ChanConvexHull *>(hullAlgorithm.get())) {
      partitionMeshes.clear();
      size_t partitions = chanHull->getPartitionsNumber();
      partitionMeshes.resize(partitions);

      const sf::Color startColor(100, 230, 255);
      const sf::Color endColor(180, 120, 255);

      for (size_t i = 0; i < partitions; ++i) {
        const auto &polygon = chanHull->getGrahamPartition(i);
        partitionMeshes[i] = sf::VertexArray(sf::LineStrip, polygon.size() + 1);
        float ratio =
            partitions > 1 ? static_cast<float>(i) / (partitions - 1) : 0.5f;
        sf::Color meshColor(
            static_cast<sf::Uint8>(startColor.r +
                                   ratio * (endColor.r - startColor.r)),
            static_cast<sf::Uint8>(startColor.g +
                                   ratio * (endColor.g - startColor.g)),
            static_cast<sf::Uint8>(startColor.b +
                                   ratio * (endColor.b - startColor.b)));

        for (size_t j = 0; j < polygon.size(); ++j) {
          partitionMeshes[i][j] = {sf::Vector2f(polygon[j].x_, polygon[j].y_),
                                   meshColor};
        }
        if (!polygon.empty()) {
          partitionMeshes[i][polygon.size()] = partitionMeshes[i][0];
        }
      }
    }
  }

  void createHullLines() {
    pointLabels.clear();
    const auto &hullPoints = hullAlgorithm->getHull();
    hullLines = sf::VertexArray(sf::LineStrip, hullPoints.size() + 1);

    for (size_t i = 0; i < hullPoints.size(); ++i) {
      hullLines[i] = {sf::Vector2f(hullPoints[i].x_, hullPoints[i].y_),
                      HULL_LINE_COLOR};
    }
    if (!hullPoints.empty()) {
      hullLines[hullPoints.size()] = hullLines[0];
    }

    for (size_t i = 0; i < hullPoints.size(); ++i) {
      sf::Text label;
      label.setFont(font);
      label.setCharacterSize(16);
      label.setString(std::to_string(i));
      label.setFillColor(TEXT_COLOR);

      // Position the label near the point
      label.setPosition(hullPoints[i].x_ + SF_POINT_RADIUS + 2,
                        hullPoints[i].y_ - SF_POINT_RADIUS);

      pointLabels.push_back(std::move(label));
    }
  }

  void handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::MouseButtonReleased &&
          event.mouseButton.button == sf::Mouse::Left) {
        handleMouseClick(event);
      }
    }
  }

  void handleMouseClick(const sf::Event &event) {
    sf::Vector2f mousePos =
        window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

    if (nextButton.contains(mousePos)) {
      generateNewSet();
    } else if (toggleMeshButton.contains(mousePos)) {
      showMesh = !showMesh;
      showHull = !showHull;
    }
  }

  void render() {
    window.clear(BACKGROUND_COLOR);
    // window.draw(paddingArea);

    for (const auto &point : points) {
      window.draw(point);
    }

    for (const auto &label : pointLabels) {
      window.draw(label);
    }

    if (showMesh) {
      for (const auto &mesh : partitionMeshes) {
        window.draw(mesh);
      }
    }

    if (showHull) {
      window.draw(hullLines);
    }

    nextButton.draw(window);
    toggleMeshButton.draw(window);
    window.display();
  }

  void mainLoop() {
    while (window.isOpen()) {
      handleEvents();
      render();
    }
  }
};

int main() {
  ConvexHullVisualizer visualizer;
  visualizer.run();
  return EXIT_SUCCESS;
}
